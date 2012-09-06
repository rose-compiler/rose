#include "rose.h"

#ifndef ROSE_HAVE_GCRYPT_H
int main()
{
    fprintf(stderr, "This project requires <gcrypt.h>\n");
    return 1;
}
#else

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gcrypt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <boost/graph/graphviz.hpp>

#include "rose_getline.h"
#include "stringify.h"
#include "BinaryLoader.h"                       /* maps specimen into an address space */
#include "BinaryFunctionCall.h"                 /* function call graphs */
#include "BinaryCallingConvention.h"            /* for testing the calling convention analysis. */

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
class MyUnparser: public AsmUnparser {
    void show_names(std::ostream &output, IdaInfo &ida, SgNode *node,
                    rose_addr_t rose_func_va, const std::vector<rose_addr_t> &ida_func_va,
                    rose_addr_t start_va, size_t nbytes) {
        SgAsmBlock *blk = SageInterface::getEnclosingNode<SgAsmBlock>(node);
        output <<"{ ";

        if (rose_func_va) {
            char tmp[64];
            sprintf(tmp, "%08"PRIx64, rose_func_va);
            output <<tmp;
        } else {
            output <<"        ";
        }

        assert(ida_func_va.size()==ida.size());
        for (size_t i=0; i<ida_func_va.size(); i++) {
            if (ida_func_va[i]) {
                char tmp[64];
                sprintf(tmp, "%08"PRIx64, ida_func_va[i]);
                output <<" " <<tmp;

                if (!rose_func_va) {
                    /* Instruction/data assigned to function by IDA but not ROSE */
                    ida[i].ida_not_rose.insert(Extent(start_va, nbytes));
                    output <<"!";
                } else if (rose_func_va!=ida_func_va[i]) {
                    /* Instruction assigned by ROSE and IDA to different function entry addresses. */
                    ida[i].rose_ida_diff.insert(Extent(start_va, nbytes));
                    output <<"!";
                } else {
                    /* Instruction assigned to same function by ROSE and IDA */
                    output <<" ";
                }
            } else if (rose_func_va) {
                /* Instruction assigned to function by ROSE but not IDA.  Don't show the "!" for padding bytes since we already
                 * know that IDA doesn't include them in the function. */
                ida[i].rose_not_ida.insert(Extent(start_va, nbytes));
                if (blk && 0!=(blk->get_reason() & SgAsmBlock::BLK_PADDING)) {
                    output <<"          ";
                } else {
                    output <<"         !";
                }
            } else {
                /* Instruction assigned by neither ROSE nor IDA */
                output <<"          ";
            }
        }
                
        output <<"}";
    }

    /* Emits the address of the function to which the instruction or data node belongs, inline with the node, for ROSE and IDA */
    template<class NodeType, class Args>
    class FuncName: public UnparserCallback {
    public:
        IdaInfo &ida;
        FuncName(IdaInfo &ida): ida(ida) {}
        virtual bool operator()(bool enabled, const Args &args) {
            if (enabled) {
                NodeType *node = args.get_node();

                /* Name of (single) ROSE function containing this instruction. */
                SgAsmFunction *rose_func = SageInterface::getEnclosingNode<SgAsmFunction>(node);
                if (0!=(rose_func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS))
                    rose_func = NULL;
                rose_addr_t rose_func_va = rose_func ? rose_func->get_entry_va() : 0;

                /* Name of (multiple) IDA functions containing this instruction. */
                std::vector<rose_addr_t> ida_func_va;
                for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
                    IdaFile::AddrMap::iterator ai=ida_i->address.find(node->get_address());
                    if (ai!=ida_i->address.end()) {
                        ida_func_va.push_back(*ai->second.begin());
                    } else {
                        ida_func_va.push_back(0);
                    }
                }

                rose_addr_t va = node->get_address();
                size_t size = node->get_size();
                MyUnparser *unparser = dynamic_cast<MyUnparser*>(args.unparser);
                assert(unparser!=NULL);
                unparser->show_names(args.output, ida, node, rose_func_va, ida_func_va, va, size);
            }
            return enabled;
        }
    };

public:
    FuncName<SgAsmInstruction, AsmUnparser::UnparserCallback::InsnArgs> insn_func_name;
    FuncName<SgAsmStaticData,  AsmUnparser::UnparserCallback::StaticDataArgs> data_func_name;

    MyUnparser(BinaryAnalysis::ControlFlow::Graph &cfg, IdaInfo &ida, Disassembler *disassembler)
        : insn_func_name(ida), data_func_name(ida) {
        add_control_flow_graph(cfg);
        set_organization(AsmUnparser::ORGANIZED_BY_ADDRESS);
        insnBlockEntry.show_function = false; // don't show function addresses since our own show_names() does that.
        staticDataBlockEntry.show_function = false;
        staticDataDisassembler.init(disassembler);
        insn_callbacks.pre
            .append(&insn_func_name);
        staticdata_callbacks.pre
            .append(&data_func_name);
    }

    /* Augment parent class by printing a key and some column headings. */
    virtual size_t unparse(std::ostream &output, SgNode *ast) {
        output <<"The \"BIR\" column is \"block inclusion reasons\": why a block of instructions\n"
               <<"or data in a function?  Reasons are noted by the following letters:\n"
               <<SgAsmBlock::reason_key("    ")
               <<"Additional notes about block inclusion reasons:\n"
               <<"  * The \"left over blocks\" are those blocks that could not be assigned to any\n"
               <<"    function.  These are usually instructions that were disassembled by the\n"
               <<"    recursive disassembler using a more agressive and/or less precise analysis\n"
               <<"    than the partitioner.\n"
               <<"  * Intra-function blocks are areas of memory that exist inside an otherwise\n"
               <<"    contiguous function.  The partitioner can be configured to grab these blocks\n"
               <<"    as basic blocks or data blocks.  When it incorporates them as basic blocks,\n"
               <<"    a second CFG analysis is run to discover other blocks that might be\n"
               <<"    referenced by the new blocks.\n"
               <<"  * A \"CFG Head\" is a basic block that's permanently attached to the function\n"
               <<"    regardless of whether that block can be reached by following the function's\n"
               <<"    control flow graph from the function entry point.  These blocks usually\n"
               <<"    come from some kind of analysis run between the first and second CFG\n"
               <<"    analysis.  Blocks that are reachable from CFG heads that are not otherwise\n"
               <<"    reachable are also marked with a \"2\" to indicate they came from the\n"
               <<"    second CFG analysis.\n"
               <<"  * We are currently using the \"U\" (user defined) indicator for blocks that\n"
               <<"    come after the end of a function and before inter-function padding.  This\n"
               <<"    feature is experimental for now. See the Partitioner::PostFunctionBlocks\n"
               <<"    class.\n"
               <<"\n"
               <<"Bangs (\"!\") in the ROSE and IDA columns indicate differences between\n"
               <<"ROSE and IDA.  We don't show a bang for data blocks due to inter-function padding\n"
               <<"since we already know that ROSE can reliably include them in the function but\n"
               <<"IDA does not.\n"
               <<"\n"
               <<"\n"
               <<"\n"
               <<"Address     Hexadecimal data         CharData   BIR     ROSE     ";
        for (size_t i=0; i<insn_func_name.ida.size(); i++)
            output <<"IDA[" <<i <<"]     ";
        output <<"    Instruction etc.\n";
        output <<"----------- ------------------------|--------| ------ {---------";
        for (size_t i=0; i<insn_func_name.ida.size(); i++)
            output <<" ----------";
        output <<"}   -----------------------------------\n";

        return AsmUnparser::unparse(output, ast);
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

    std::vector<SgAsmFunction*> rose_functions;
    std::vector<SgAsmFunction*> padding_functions;
    struct T1: public AstSimpleProcessing {
        std::vector<SgAsmFunction*> &rose_functions;
        std::vector<SgAsmFunction*> &padding_functions;
        T1(std::vector<SgAsmFunction*> &rose_functions,
           std::vector<SgAsmFunction*> &padding_functions)
            : rose_functions(rose_functions), padding_functions(padding_functions) {}
        void visit(SgNode *node) {
            SgAsmFunction *func = isSgAsmFunction(node);
            if (func) {
                if (func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS) {
                    /* not a function -- just a collection of otherwise unassigned instructions */
                } else if (0!=(func->get_reason() & SgAsmFunction::FUNC_PADDING)) {
                    padding_functions.push_back(func);
                } else {
                    rose_functions.push_back(func);
                }
            }
        }
    };
    T1(rose_functions, padding_functions).traverse(interp, preorder);

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
    ExtentMap disassembled_bytes;
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
        disassembled_bytes.insert(Extent(ii->first, ii->second->get_size()));
    size_t ndis0 = disassembled_bytes.size();
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
        emap = disassembled_bytes;
        for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
            emap.erase(Extent(ii->first, ii->second->get_size()));
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

    // Number of bytes assigned to functions (other than FUNC_LEFTOVERS, and padding bytes, and not counting overlaps twice)
    struct NonPadding: public SgAsmFunction::NodeSelector {
        virtual bool operator()(SgNode *node) {
            SgAsmStaticData *datum = isSgAsmStaticData(node);
            SgAsmBlock *dblock = SageInterface::getEnclosingNode<SgAsmBlock>(datum);
            return !datum || !dblock || 0==(dblock->get_reason() & SgAsmBlock::BLK_PADDING);
        }
    } no_padding;
    emap.clear();
    for (std::vector<SgAsmFunction*>::iterator fi=rose_functions.begin(); fi!=rose_functions.end(); ++fi)
        (*fi)->get_extent(&emap, NULL, NULL, &no_padding);
    size_t nassigned0 = emap.size();
    fprintf(stderr, "Assigned to functions (excl. padding):  %-*zu", width, nassigned0);
    for (IdaInfo::iterator ida_i=ida.begin(); ida_i!=ida.end(); ++ida_i) {
        size_t nassigned = ida_i->address.size();
        sprintf(tmp, "%zu(%+0.1f%%)", nassigned, pchange(nassigned0, nassigned));
        fprintf(stderr, " %-*s", width, tmp);
    }
    fprintf(stderr, " bytes\n");

    // Number of bytes assigned to padding
    struct OnlyPadding: public NonPadding {
        virtual bool operator()(SgNode *node) {
            return !NonPadding::operator()(node);
        }
    } only_padding;
    emap.clear();
    for (std::vector<SgAsmFunction*>::iterator fi=rose_functions.begin(); fi!=rose_functions.end(); ++fi)
        (*fi)->get_extent(&emap, NULL, NULL, &only_padding);
    for (std::vector<SgAsmFunction*>::iterator fi=padding_functions.begin(); fi!=padding_functions.end(); ++fi)
        (*fi)->get_extent(&emap, NULL, NULL, NULL);
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
                    func_bytes.insert(Extent(func_insns[j]->get_address(), func_insns[j]->get_size()));
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

struct WritableRegion: public MemoryMap::Visitor {
    virtual bool operator()(const MemoryMap*, const Extent&, const MemoryMap::Segment &segment) {
        return 0 != (segment.get_mapperms() & MemoryMap::MM_PROT_WRITE);
    }
} writable_region;

/* Returns true for any anonymous memory region containing more than a certain size. */
static rose_addr_t large_anonymous_region_limit = 8192;
struct LargeAnonymousRegion: public MemoryMap::Visitor {
    virtual bool operator()(const MemoryMap*, const Extent &range, const MemoryMap::Segment &segment) {
        if (range.size()>large_anonymous_region_limit && segment.get_buffer()->is_zero()) {
            fprintf(stderr, "ignoring zero-mapped memory at va 0x%08"PRIx64" + 0x%08"PRIx64" = 0x%08"PRIx64"\n",
                    range.first(), range.size(), range.last()+1);
            return true;
        }
        return false;
    }
} large_anonymous_region;

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
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Simulating program loader to map memory...\n";
    BinaryLoader *loader = BinaryLoader::lookup(interp)->clone();
    //loader->set_debug(stderr);
    loader->set_perform_remap(true);
    loader->load(interp);
    MemoryMap *map = interp->get_map();
    assert(map!=NULL);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Building memory map for disassembly...\n";

    // Set the import section and each of the import address tables to read-only in the specified memory map (if they're in the
    // map), so that the partitioner thinks that the import data is constant.  The partitioner uses the read-only sections of
    // the map to initialize memory for the instruction semantics layer.  The more memory we can initialize, the more precise
    // the semantics will be that are used to build basic blocks and functions.  The ReadonlyImports needs to set the import
    // section and import address tables to read-only independently of each other because some PE executables put the IATs
    // outside the import section.  Even when the IATs are inside, setting the entire import section to read-only first keeps
    // the MemoryMap simpler.
    struct ReadonlyImports: public AstSimpleProcessing {
        MemoryMap *map;
        SgAsmGenericHeader *fhdr;
        ReadonlyImports(MemoryMap *map, SgAsmGenericHeader *fhdr): map(map), fhdr(fhdr) {}
        void visit(SgNode *node) {
            SgAsmPEImportSection *isec = isSgAsmPEImportSection(node);
            if (isec) {
                rose_addr_t addr = isec->get_mapped_actual_va();
                size_t size = isec->get_mapped_size();
                map->mprotect(Extent(addr, size), MemoryMap::MM_PROT_READ, true/*relax*/);
            }

            SgAsmPEImportDirectory *idir = isSgAsmPEImportDirectory(node);
            if (idir && idir->get_iat_rva().get_rva()!=0) {
                rose_addr_t iat_va = idir->get_iat_rva().get_va();
                size_t iat_sz = idir->get_iat_nalloc();
                map->mprotect(Extent(iat_va, iat_sz), MemoryMap::MM_PROT_READ, true/*relax*/);
            }
        }
    };

    // We must traverse the headers explicitly because they're not under the SgAsmInterpretation in the AST
    MemoryMap ro_map = *map;
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi)
        ReadonlyImports(&ro_map, *hi).traverse(*hi, preorder);
    ro_map.prune(writable_region);
    map->prune(large_anonymous_region);

    std::cerr <<"Disassembly map:\n";
    map->dump(stderr, "    ");
    std::cerr <<"Memory initialization map:\n";
    ro_map.dump(stderr, "    ");

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Running the recursive disassembler...\n";
    // Seed the disassembler work list with the entry addresses.
    Disassembler::AddressSet worklist;
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
        SgRVAList entry_rvalist = (*hi)->get_entry_rvas();
        for (size_t i=0; i<entry_rvalist.size(); i++) {
            rose_addr_t entry_va = (*hi)->get_base_va() + entry_rvalist[i].get_rva();
            worklist.insert(entry_va);
        }
    }

    // Initialize and run the disassembler
    Disassembler *disassembler = Disassembler::lookup(interp)->clone();
    disassembler->set_search(Disassembler::SEARCH_DEFAULT | Disassembler::SEARCH_DEADEND |
                             Disassembler::SEARCH_UNKNOWN | Disassembler::SEARCH_UNUSED);
    Disassembler::BadMap bad;
    Disassembler::InstructionMap insns = disassembler->disassembleBuffer(map, worklist, NULL, &bad);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Partitioning instructions into functions...\n";
    Partitioner *partitioner = new Partitioner();
    partitioner->set_search(SgAsmFunction::FUNC_DEFAULT | SgAsmFunction::FUNC_LEFTOVERS);
    partitioner->set_map(map, &ro_map);
    //partitioner->set_debug(stderr);
    SgAsmBlock *gblock = partitioner->partition(interp, insns);
    interp->set_global_block(gblock);
    gblock->set_parent(interp);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Evaluate how well the code criteria subsystem is working
#if 0
    Partitioner::RegionStats *mean=partitioner->get_aggregate_mean(), *variance=partitioner->get_aggregate_variance();
    assert(mean && variance);
    std::cerr <<"\n=== Aggregate mean ===\n" <<*mean <<"\n=== Aggregate variance ===\n" <<*variance <<"\n";
    Partitioner::CodeCriteria *cc = partitioner->new_code_criteria(mean, variance, 0.5);
    partitioner->set_code_criteria(cc);

    struct CodeDetector: public AstPrePostProcessing {
        Partitioner *partitioner;
        ExtentMap function_extent;

        CodeDetector(Partitioner *partitioner): partitioner(partitioner) {}
        
        void preOrderVisit(SgNode *node) {
            SgAsmInstruction *insn = isSgAsmInstruction(node);
            if (insn)
                function_extent.insert(Extent(insn->get_address(), insn->get_size()));
        }

        void postOrderVisit(SgNode *node) {
            SgAsmFunction *func = isSgAsmFunction(node);
            if (func) {
                std::ostringstream ss;
                double vote;
                bool iscode = partitioner->is_code(function_extent, &vote, &ss);
                func->set_comment(StringUtility::addrToString(func->get_entry_va()) +
                                  ": Code? "+ (iscode?"YES":"NO") + "  (" + StringUtility::numberToString(vote) + ")\n" +
                                  ss.str());
                function_extent.clear();
            }
        }
    } code_detector(partitioner);
    code_detector.traverse(gblock);

    struct DataDetector: public AstSimpleProcessing {
        Partitioner *partitioner;
        DataDetector(Partitioner *partitioner): partitioner(partitioner) {}
        void visit(SgNode *node) {
            SgAsmStaticData *data = isSgAsmStaticData(node);
            SgAsmBlock *blk = SageInterface::getEnclosingNode<SgAsmBlock>(data);
            if (data && blk && 0==(blk->get_reason() & (SgAsmBlock::BLK_PADDING|SgAsmBlock::BLK_JUMPTABLE))) {
                ExtentMap data_extent;
                data_extent.insert(Extent(data->get_address(), data->get_size()));
                double vote;
                std::ostringstream ss;
                bool iscode = partitioner->is_code(data_extent, &vote, &ss);
                data->set_comment(ss.str());
            }
        }
    } data_detector(partitioner);
    data_detector.traverse(gblock, preorder);
#endif

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Test the function calling convention analysis
#if 0
    struct CConvTester: public AstSimpleProcessing {
        void visit(SgNode *node) {
            SgAsmFunction *func = isSgAsmFunction(node);
            if (func)
                BinaryCallingConvention().analyze_callee(func);
        }
    } cconvTester;
    cconvTester.traverse(gblock, preorder);
#endif

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Generating control flow graph...\n";
    struct NoLeftovers: public BinaryAnalysis::ControlFlow::VertexFilter {
        virtual bool operator()(BinaryAnalysis::ControlFlow*, SgAsmBlock *blk) {
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(blk);
            return func && 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS);
        }
    } vertex_filter;
    BinaryAnalysis::ControlFlow cfg_analyzer;
    cfg_analyzer.set_vertex_filter(&vertex_filter);
    BinaryAnalysis::ControlFlow::Graph cfg;
    cfg_analyzer.build_cfg_from_ast(interp, cfg);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cerr <<"Generating output...\n";
#if 1
    MyUnparser unparser(cfg, ida, disassembler);
    unparser.unparse(std::cout, gblock);
#else
    AsmUnparser().unparse(std::cout, gblock);
#endif
    statistics(interp, insns, ida);

    return 0;
}

#endif /* HAVE_GCRYPT_H */
