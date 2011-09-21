#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gcrypt.h>
#include <fcntl.h>
#include <sys/mman.h>


#include "rose_getline.h"
#include "stringify.h"
#include "BinaryLoader.h"                       /* maps specimen into an address space */
#include "BinaryFunctionCall.h"                 /* function call graphs */

class IdaFile {
public:
    IdaFile(int id): id(id) {}

    struct Function {
        std::string name;                       /* name of function if known */
        rose_addr_t entry_va;                   /* entry address of function */
        size_t size;                            /* size of function in bytes (IDA functions are contiguous in memory) */
    };

    typedef std::map<rose_addr_t/*insn*/, std::set<rose_addr_t>/*functions*/> AddrMap;
    typedef std::map<rose_addr_t/*entry*/, Function> FuncMap;

    std::string name;                           /* Name of file from which info was parsed. */
    int id;                                     /* ID for printing IDA name, like "IDA[1]" */
    AddrMap address;                            /* Map from memory address to functions that "own" that address. */
    FuncMap function;                           /* List of functions by entry address. */

    ExtentMap rose_not_ida;                     /* Bytes that ROSE assigned to a function, but IDA didn't */
    ExtentMap ida_not_rose;                     /* Bytes that IDA assigned to a function, but ROSE didn't */
    ExtentMap rose_ida_diff;                    /* Bytes assigned by both ROSE and IDA, but to different function entry pts. */

    /* Parse a CSV file containing info from IDAPro, selecting only lines with the specified file hash string. */
    void parse(const std::string &csv_name, const std::string &exe_hash);
};
    

typedef std::vector<IdaFile> IdaInfo;           /* Information from multiple CSV files. */

void
IdaFile::parse(const std::string &csv_name, const std::string &exe_hash)
{
    name = csv_name;
    FILE *f = fopen(csv_name.c_str(), "r");
    assert(f!=NULL);

    char *line = NULL;
    size_t linesz = 0;
    static const size_t nfields = 8; // 0=>file_hash, 1=>entry_va, 2=>name, 3=>hash 4=>pichash 5=>bytes 6=>picbytes 7=>flags
    while (rose_getline(&line, &linesz, f)>0) {
        size_t fieldno=0;
        char *field[nfields];
        for (char *s=strtok(line, ","); s && fieldno<nfields; s=strtok(NULL, ","), fieldno++)
            field[fieldno] = s;
        assert(fieldno==nfields);
        if (strcmp(field[0], exe_hash.c_str()))
            continue;

        /* Function info */
        rose_addr_t func_entry = strtoull(field[1], NULL, 16);
        Function &f = function[func_entry];
        f.entry_va = func_entry;
        f.name = field[2];
        f.size = strlen(field[5])/2; // two hexadecimal digits per byte

        /* What addresses does this function own? */
        for (size_t i=0; i<f.size; i++)
            address[f.entry_va+i].insert(f.entry_va);
    }

    if (line)
        free(line);
    fclose(f);
}

/* We use our own instruction unparser that does a few additional things that ROSE's default unparser doesn't do.  The extra
 * things are each implemented as a callback and are described below. */
template<class FunctionCallGraph>
class MyUnparser: public AsmUnparser {
    typedef typename boost::graph_traits<FunctionCallGraph>::vertex_descriptor Vertex;

    /* Emits a blank line in the listing when two consecutive instructions are not adjacent in memory. */
    class InsnInterSpace: public UnparserCallback {
    public:
        rose_addr_t next_addr;
        InsnInterSpace(): next_addr(0) {}
        virtual bool operator()(bool enabled, const InsnArgs &args) {
            rose_addr_t cur_addr = args.insn->get_address();
            if (enabled && cur_addr!=next_addr && next_addr!=0) {
                if (cur_addr > next_addr) {
                    args.output <<"skipped " <<(cur_addr-next_addr) <<" byte" <<(1==cur_addr-next_addr?"":"s");
                } else {
                    args.output <<"back " <<(next_addr-cur_addr) <<" byte" <<(1==next_addr-cur_addr?"":"s");
                }
                args.output <<std::endl;
            }
            next_addr = cur_addr + args.insn->get_raw_bytes().size();
            return enabled;
        }
    };

    /* Emits info about a function when we hit the entry point of the function. */
    class InsnFuncEntry: public UnparserCallback {
    public:
        FunctionCallGraph &cg;
        InsnFuncEntry(FunctionCallGraph &cg): cg(cg) {}
        virtual bool operator()(bool enabled, const InsnArgs &args) {
            if (enabled) {
                SgAsmFunctionDeclaration *func = SageInterface::getEnclosingNode<SgAsmFunctionDeclaration>(args.insn);
                if (func && func->get_entry_va()==args.insn->get_address()) {
                    args.output <<"\n========== Function ===========\n"
                                <<func->reason_str(false) <<"\n";

                    if (func->get_can_return()) {
                        args.output <<"Can return to caller.\n";
                    } else {
                        args.output <<"Does not return to caller.\n";
                    }
                    args.output <<"Called by:";
                    Vertex called_v = func->get_cached_vertex();
                    typename boost::graph_traits<FunctionCallGraph>::in_edge_iterator ei, eend;
                    for (boost::tie(ei, eend)=in_edges(called_v, cg); ei!=eend; ++ei) {
                        SgAsmFunctionDeclaration *caller = get(boost::vertex_name, cg, source(*ei, cg));
                        args.output <<" 0x" <<std::hex <<caller->get_entry_va() <<std::dec;
                    }
                    args.output <<"\n";
                }
            }
            return enabled;
        }
    };

    /* Emits the name or address of the function to which the instruction belongs, inline with the instruction */
    class InsnFuncName: public UnparserCallback {
    public:
        IdaInfo &ida;
        InsnFuncName(IdaInfo &ida): ida(ida) {}
        virtual bool operator()(bool enabled, const InsnArgs &args) {
            if (enabled) {

                args.output <<"{ ";

                /* Name of (single) ROSE function containing this instruction. */
                SgAsmFunctionDeclaration *rose_func = SageInterface::getEnclosingNode<SgAsmFunctionDeclaration>(args.insn);
                rose_addr_t rose_func_va = 0;
                if (0!=(rose_func->get_reason() & SgAsmFunctionDeclaration::FUNC_LEFTOVERS))
                    rose_func = NULL;
                if (rose_func) {
                    char tmp[64];
                    rose_func_va = rose_func->get_entry_va();
                    sprintf(tmp, "%08"PRIx64, rose_func_va);
                    args.output <<tmp;
                } else {
                    args.output <<"        ";
                }

                /* Name of (multiple) IDA functions containing this instruction. */
                for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
                    IdaFile::AddrMap::iterator ai=ida_i->address.find(args.insn->get_address());
                    if (ai!=ida_i->address.end()) {
                        char tmp[64];
                        sprintf(tmp, "%08"PRIx64, *ai->second.begin());
                        args.output <<" " <<tmp;

                        if (!rose_func) {
                            /* Instruction assigned to function by IDA but not ROSE */
                            ida_i->ida_not_rose.insert(args.insn->get_address(), args.insn->get_raw_bytes().size());
                            args.output <<"!";
                        } else if (ai->second.find(rose_func_va)==ai->second.end()) {
                            /* Instruction assigned by ROSE and IDA to different function entry addresses. */
                            ida_i->rose_ida_diff.insert(args.insn->get_address(), args.insn->get_raw_bytes().size());
                            args.output <<"!";
                        } else {
                            /* Instruction assigned to same function by ROSE and IDA */
                            args.output <<" ";
                        }
                        args.output <<(ai->second.size()>1 ? "*" : " "); /* IDA assigned to more than one function? */
                    } else if (rose_func) {
                        /* Instruction assigned to function by ROSE but not IDA */
                        ida_i->rose_not_ida.insert(args.insn->get_address(), args.insn->get_raw_bytes().size());
                        args.output <<"         ! ";
                    } else {
                        /* Instruction assigned by neither ROSE nor IDA */
                        args.output <<"           ";
                    }
                }
                args.output <<"}";
            }
            return enabled;
        }
    };

    /* Emits block reason bits for the first instruction of every block, and white space for others. */
    class InsnBlockReasons: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args) {
            if (enabled) {
                SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(args.insn);
                assert(bb!=NULL);

                static size_t width = 0;
                if (0==width)
                    width = bb->reason_str(true, 0).size();
                    
                if (args.insn == bb->get_statementList().front()) {
                    args.output <<" " <<bb->reason_str(true) <<" ";
                } else {
                    args.output <<std::setw(width+2) <<" ";
                }
            }
            return enabled;
        }
    };

public:
    InsnInterSpace insn_inter_space;
    InsnFuncEntry insn_func_entry;
    InsnFuncName insn_func_name;
    InsnBlockReasons insn_block_reasons;

    MyUnparser(FunctionCallGraph &cg, IdaInfo &ida): insn_func_entry(cg), insn_func_name(ida) {
        //insn_callbacks.pre.replace(&insnRawBytes, &insnAddress); // print address w/o raw bytes
        insn_callbacks.pre.prepend(&insn_func_entry);
        insn_callbacks.pre.prepend(&insn_inter_space);
        insn_callbacks.pre.append(&insn_func_name);
        insn_callbacks.pre.append(&insn_block_reasons);
    }
};

static std::string
hash_file(const std::string &filename)
{
    std::string retval;

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd<0)
        return "";

    struct stat sb;
    if (fstat(fd, &sb)<0) {
        close(fd);
        return "";
    }

    char *content = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd); fd=-1;
    gcry_check_version(NULL);
    const size_t nbytes = gcry_md_get_algo_dlen(GCRY_MD_MD5);
    unsigned char file_binary_hash[nbytes];
    gcry_md_hash_buffer(GCRY_MD_MD5, file_binary_hash, content, sb.st_size);
    munmap(content, sb.st_size); content=NULL;

    for (size_t i=0; i<nbytes; i++) {
        retval += "0123456789abcdef"[(file_binary_hash[i] & 0xf0) >> 4];
        retval += "0123456789abcdef"[(file_binary_hash[i] & 0x0f)     ];
    }

    return retval;
}

static double
pchange(size_t n0, size_t n) {
    return 100.0*((double)n-n0)/n0;
}

static void
statistics(SgAsmInterpretation *interp, const Disassembler::InstructionMap &insns, IdaInfo &ida)
{
    ExtentMap emap;
    static int width=20;
    char tmp[128];
    int note=1;

    std::vector<SgAsmFunctionDeclaration*> rose_functions;
    struct T1: public AstSimpleProcessing {
        std::vector<SgAsmFunctionDeclaration*> &rose_functions;
        T1(std::vector<SgAsmFunctionDeclaration*> &rose_functions): rose_functions(rose_functions) {}
        void visit(SgNode *node) {
            SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
            if (func) {
                if (func->get_reason() & SgAsmFunctionDeclaration::FUNC_LEFTOVERS) {
                    /* not a function -- just a collection of otherwise unassigned instructions */
                } else if (SgAsmFunctionDeclaration::FUNC_INTERPAD==func->get_reason()) {
                    /* not a function -- used exclusivly as padding between functions */
                } else {
                    rose_functions.push_back(func);
                }
            }
        }
    };
    T1(rose_functions).traverse(interp, preorder);

    // Header
    fprintf(stderr, "==========\nStatistics\n==========\n");
    fprintf(stderr, "                                        %-*s", width, "ROSE");
    for (size_t i=0; i<ida.size(); i++) {
        sprintf(tmp, "IDA[%zu]", i+1);
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "----------------------------------------");
    for (size_t i=0; i<width+(width+1)*ida.size(); i++) fputc('-', stderr);
    fprintf(stderr, "-------------\n");
    
    // Number of bytes disassembled (including those not assigned to any function, and counting overlaps twice)
    size_t ndis0=0;
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
        ndis0 += ii->second->get_raw_bytes().size();
    fprintf(stderr, "Disassembled:                           %-*zu", width, ndis0);
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        size_t ndis = ida_i->address.size();
        sprintf(tmp, "%zu(%+0.1f%%)", ndis, pchange(ndis0, ndis));
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Bytes disassembled by IDA but not ROSE
    fprintf(stderr, "Disassembled by IDA but not ROSE:       %-*s", width, "N/A");
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        emap.clear();
        for (IdaFile::FuncMap::iterator fi=ida_i->function.begin(); fi!=ida_i->function.end(); ++fi)
            emap.insert(fi->second.entry_va, fi->second.size);
        for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
            emap.erase(ii->first, ii->second->get_raw_bytes().size());
        size_t n = emap.size();
        if (n>0) {
            printf("[NOTE %d] Bytes disassembled by IDA[%d] (%s) but not ROSE:\n", note, ida_i->id, ida_i->name.c_str());
            emap.dump_extents(stdout, "    ", "", false);
            sprintf(tmp, "%zu[%d]", n, note++);
        } else {
            sprintf(tmp, "%zu", n);
        }
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Number of bytes assigned to functions (other than FUNC_LEFTOVERS, and not counting overlaps twice)
    emap.clear();
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmFunctionDeclaration *func = SageInterface::getEnclosingNode<SgAsmFunctionDeclaration>(ii->second);
        if (func &&
            0==(func->get_reason() & SgAsmFunctionDeclaration::FUNC_LEFTOVERS) &&
            SgAsmFunctionDeclaration::FUNC_INTERPAD!=func->get_reason())
            emap.insert(ii->first, ii->second->get_raw_bytes().size());
    }
    size_t nassigned0 = emap.size();
    fprintf(stderr, "Assigned to functions (excl. padding):  %-*zu", width, nassigned0);
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        size_t nassigned = ida_i->address.size();
        sprintf(tmp, "%zu(%+0.1f%%)", nassigned, pchange(nassigned0, nassigned));
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Number of bytes assigned to padding
    emap.clear();
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmFunctionDeclaration *func = SageInterface::getEnclosingNode<SgAsmFunctionDeclaration>(ii->second);
        if (func &&
            0==(func->get_reason() & SgAsmFunctionDeclaration::FUNC_LEFTOVERS) &&
            SgAsmFunctionDeclaration::FUNC_INTERPAD==func->get_reason())
            emap.insert(ii->first, ii->second->get_raw_bytes().size());
    }
    size_t npadding0 = emap.size();
    fprintf(stderr, "Assigned to padding:                    %-*zu", width, npadding0);
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i)
        fprintf(stderr, " %-*s", width, "N/A");
    fprintf(stderr, " bytes\n");

    // Number of functions detected
    size_t nfunc0 = rose_functions.size();
    fprintf(stderr, "Functions detected (excl. padding):     %-*zu", width, nfunc0);
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        size_t nfunc = ida_i->function.size();
        sprintf(tmp, "%zu(%+0.1f%%)", nfunc, pchange(nfunc0, nfunc));
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, "\n");

    // Average number of bytes in each function
    double avefsize0 = nassigned0/nfunc0;
    fprintf(stderr, "Average function size (excl. padding):  %-*.2f", width, avefsize0);
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        double avefsize = ida_i->address.size() / ida_i->function.size();
        sprintf(tmp, "%0.2f(%+0.1f%%)", avefsize, 100*(avefsize-avefsize0)/avefsize0);
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Entry points detected by both ROSE and IDA
    fprintf(stderr, "Functions detected by ROSE and IDA:     %-*s", width, "N/A");
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        size_t n = 0;
        for (size_t i=0; i<rose_functions.size(); i++) {
            if (ida_i->function.find(rose_functions[i]->get_entry_va())!=ida_i->function.end())
                n++;
        }
        fprintf(stderr, " %-*zu", width, n);
    }
    fprintf(stderr, " entry points\n");

    // Entry points detected by ROSE but not IDA
    fprintf(stderr, "Functions detected by ROSE but not IDA: %-*s", width, "N/A");
    std::vector<double> avefsize_rni; // ave size of functions that are in ROSE but not IDA
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        std::set<rose_addr_t> entries;
        size_t nbytes = 0;
        for (size_t i=0; i<rose_functions.size(); i++) {
            if (ida_i->function.find(rose_functions[i]->get_entry_va())==ida_i->function.end()) {
                entries.insert(rose_functions[i]->get_entry_va());
                ExtentMap func_bytes;
                std::vector<SgAsmx86Instruction*> func_insns = SageInterface::querySubTree<SgAsmx86Instruction>(rose_functions[i]);
                for (size_t j=0; j<func_insns.size(); j++)
                    func_bytes.insert(func_insns[j]->get_address(), func_insns[j]->get_raw_bytes().size());
                nbytes += func_bytes.size();
            }
        }
        if (entries.empty()) {
            strcpy(tmp, "0");
            avefsize_rni.push_back(0.0);
        } else {
            printf("[NOTE %d] Function entries detected by ROSE but not IDA[%d] (%s):\n", note, ida_i->id, ida_i->name.c_str());
            for (std::set<rose_addr_t>::iterator ei=entries.begin(); ei!=entries.end(); ++ei)
                printf("  0x%08"PRIx64"\n", *ei);
            sprintf(tmp, "%zu[%d]", entries.size(), note++);
            avefsize_rni.push_back((double)nbytes/entries.size());
        }
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " entry points\n");
    fprintf(stderr, "          Average size (wrt all funcs): %-*s", width, "N/A");
    for (size_t i=0; i<avefsize_rni.size(); i++) {
        sprintf(tmp, "%0.2f(%+0.1f%%)", avefsize_rni[i], 100*(avefsize_rni[i]-avefsize0)/avefsize0);
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");
    
    // Entry points detected by IDA but not ROSE
    fprintf(stderr, "Functions detected by IDA but not ROSE: %-*s", width, "N/A");
    std::vector<double> avefsize_inr; // ave size of functions that are in IDA but not ROSE
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        std::set<rose_addr_t> entries;
        size_t nbytes = 0;
        for (IdaFile::FuncMap::iterator fi=ida_i->function.begin(); fi!=ida_i->function.end(); ++fi)
            entries.insert(fi->first);
        for (size_t i=0; i<rose_functions.size(); i++)
            entries.erase(rose_functions[i]->get_entry_va());
        if (entries.empty()) {
            strcpy(tmp, "0");
            avefsize_inr.push_back(0.0);
        } else {
            printf("[NOTE %d] Function entries detected by IDA[%d] (%s) but not ROSE:\n", note, ida_i->id, ida_i->name.c_str());
            for (std::set<rose_addr_t>::iterator ei=entries.begin(); ei!=entries.end(); ++ei) {
                printf("  0x%08"PRIx64"\n", *ei);
                nbytes += ida_i->function[*ei].size;
            }
            sprintf(tmp, "%zu[%d]", entries.size(), note++);
            avefsize_inr.push_back((double)nbytes/entries.size());
        }
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " entry points\n");
    fprintf(stderr, "          Average size (wrt all funcs): %-*s", width, "N/A");
    for (size_t i=0; i<avefsize_inr.size(); i++) {
        sprintf(tmp, "%0.2f(%+0.1f%%)", avefsize_inr[i], 100*(avefsize_inr[i]-avefsize0)/avefsize0);
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Num bytes assigned by ROSE that were not assigned by IDA
    fprintf(stderr, "Assigned by ROSE but not IDA:           %-*s", width, "N/A");
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        size_t n = ida_i->rose_not_ida.size();
        if (n>0) {
            printf("[NOTE %d] Bytes assigned to functions by ROSE but not IDA[%d] (%s):\n", note, ida_i->id, ida_i->name.c_str());
            ida_i->rose_not_ida.dump_extents(stdout, "    ", "", false);
            sprintf(tmp, "%zu[%d]", n, note++);
        } else {
            strcpy(tmp, "0");
        }
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Num bytes assigned by IDA that were not assigned by ROSE
    fprintf(stderr, "Assigned by IDA but not ROSE:           %-*s", width, "N/A");
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        size_t n = ida_i->ida_not_rose.size();
        if (n>0) {
            printf("[NOTE %d] Bytes assigned to functions by IDA[%d] (%s) but not ROSE:\n", note, ida_i->id, ida_i->name.c_str());
            ida_i->ida_not_rose.dump_extents(stdout, "    ", "", false);
            sprintf(tmp, "%zu[%d]", n, note++);
        } else {
            strcpy(tmp, "0");
        }
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Num bytes assigned to different (existing) functions in ROSE vs. IDA
    fprintf(stderr, "Assigned to differing entry points:     %-*s", width, "N/A");
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        size_t n = ida_i->rose_ida_diff.size();
        if (n>0) {
            printf("[NOTE %d] Bytes assigned to different functions by ROSE than IDA[%d] (%s)\n",
                   note, ida_i->id, ida_i->name.c_str());
            ida_i->rose_ida_diff.dump_extents(stdout, "    ", "", false);
            sprintf(tmp, "%zu[%d]", n, note++);
        } else {
            strcpy(tmp, "0");
        }
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Footer
    fprintf(stderr, "----------------------------------------");
    for (size_t i=0; i<width+(width+1)*ida.size(); i++) fputc('-', stderr);
    fprintf(stderr, "-------------\n");
    if (note>1)
        fprintf(stderr, "Notes (\"[#]\") referenced above can be found at the end of stdout.\n");
}


int
main(int argc, char *argv[])
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string file_hash = hash_file(argv[argc-1]);
    std::cerr <<"Analyzing binary file \"" <<argv[argc-1] <<"\"; md5sum=" <<file_hash <<"\n"
              <<"Output compares these algorithms:\n"
              <<"  [0] ROSE's recursive disassembler followed by instruction partitioning\n";
    assert(!file_hash.empty());

    IdaInfo ida;
    int argno = 1;
    while (argno<argc && strlen(argv[argno])>4 && !strcmp(argv[argno]+strlen(argv[argno])-4, ".csv")) {
        std::cerr <<"  [" <<argno <<"] IDA Pro info from " <<argv[argno] <<"\n";
        ida.push_back(IdaFile(ida.size()+1));
        ida.back().parse(argv[argno++], file_hash);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Parsing executable container...\n";
    int my_argc = 0;
    char *my_argv[argc+2];
    my_argv[my_argc++] = argv[0];
    my_argv[my_argc++] = strdup("-rose:read_executable_file_format_only"); // disable disassembly for now
    for (/*void*/; argno<argc; argno++)
        my_argv[my_argc++] = argv[argno];
    my_argv[my_argc] = NULL;
    SgProject *project = frontend(my_argc, my_argv);
    SgAsmInterpretation *interp = SageInterface::querySubTree<SgAsmInterpretation>(project).back();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Simulating program loader to map memory...\n";
    BinaryLoader *loader = BinaryLoader::lookup(interp)->clone();
    loader->set_perform_remap(true);
    loader->load(interp);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Running the recursive disassembler...\n";
    // Seed the disassembler work list with the entry addresses.
    Disassembler::AddressSet worklist;
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
        SgRVAList entry_rvalist = (*hi)->get_entry_rvas();
        for (size_t i=0; i<entry_rvalist.size(); i++) {
            rose_addr_t entry_va = (*hi)->get_base_va() + entry_rvalist[i].get_rva();
            worklist.insert(entry_va);
        }
    }

    // Initialize and run the disassembler
    MemoryMap *map = interp->get_map();
    assert(map!=NULL);
    Disassembler *disassembler = Disassembler::lookup(interp)->clone();
    disassembler->set_search(Disassembler::SEARCH_DEFAULT | Disassembler::SEARCH_DEADEND | Disassembler::SEARCH_UNKNOWN);
    Disassembler::BadMap bad;
    Disassembler::InstructionMap insns = disassembler->disassembleBuffer(map, worklist, NULL, &bad);
    if (!bad.empty()) {
        std::cerr <<"  Disassembly failed at " <<bad.size() <<" address" <<(1==bad.size()?"":"es") <<":\n";
        for (Disassembler::BadMap::const_iterator bmi=bad.begin(); bmi!=bad.end(); ++bmi)
            std::cerr <<"    " <<StringUtility::addrToString(bmi->first) <<": " <<bmi->second.mesg <<"\n";
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Partitioning instructions into functions...\n";
    Partitioner *partitioner = new Partitioner();
    partitioner->set_map(map);
    partitioner->set_search(SgAsmFunctionDeclaration::FUNC_DEFAULT | SgAsmFunctionDeclaration::FUNC_INTRABLOCK);
    //partitioner->set_debug(stderr);
    SgAsmBlock *gblock = partitioner->partition(interp, insns);
    interp->set_global_block(gblock);
    gblock->set_parent(interp);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Generating function call graph...\n";
    BinaryAnalysis::FunctionCall cg_analyzer;
    BinaryAnalysis::FunctionCall::Graph cg;
    cg_analyzer.build_cg_from_ast(interp, cg);
    cg_analyzer.cache_vertex_descriptors(cg);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Generating output...\n";
    MyUnparser<BinaryAnalysis::FunctionCall::Graph> unparser(cg, ida);
    for (Disassembler::InstructionMap::iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        unparser.unparse(std::cout, ii->second);
    }

    statistics(interp, insns, ida);
}
