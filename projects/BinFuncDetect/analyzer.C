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
                    ida[i].ida_not_rose.insert(start_va, nbytes);
                    output <<"!";
                } else if (rose_func_va!=ida_func_va[i]) {
                    /* Instruction assigned by ROSE and IDA to different function entry addresses. */
                    ida[i].rose_ida_diff.insert(start_va, nbytes);
                    output <<"!";
                } else {
                    /* Instruction assigned to same function by ROSE and IDA */
                    output <<" ";
                }
            } else if (rose_func_va) {
                /* Instruction assigned to function by ROSE but not IDA.  Don't show the "!" for padding bytes since we already
                 * know that IDA doesn't include them in the function. */
                ida[i].rose_not_ida.insert(start_va, nbytes);
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
                size_t size = node->get_raw_bytes().size();
                MyUnparser *unparser = dynamic_cast<MyUnparser*>(args.unparser);
                assert(unparser!=NULL);
                unparser->show_names(args.output, ida, node, rose_func_va, ida_func_va, va, size);
            }
            return enabled;
        }
    };

    template<class NodeType, class Args>
    class DiscontiguousNotifier: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const Args &args) {
            if (enabled) {
                MyUnparser *unparser = dynamic_cast<MyUnparser*>(args.unparser);
                NodeType *node = args.get_node();
                rose_addr_t va = node->get_address();
                if (unparser->nprinted++>0 && va!=unparser->next_address) {
                    if (va > unparser->next_address) {
                        rose_addr_t nskipped = va - unparser->next_address;
                        args.output <<"Skipping " <<nskipped <<" byte" <<(1==nskipped?"":"s") <<"\n";
                    } else {
                        rose_addr_t nskipped = unparser->next_address - va;
                        args.output <<"Backward " <<nskipped <<" byte" <<(1==nskipped?"":"s") <<"\n";
                    }
                }
                unparser->next_address = va + node->get_raw_bytes().size();
            }
            return enabled;
        }
    };

    /* Emits info about what other functions call this one whenever we hit an entry point of a function. */
    class FuncCallers: public UnparserCallback {
    public:
        FunctionCallGraph &cg;
        FuncCallers(FunctionCallGraph &cg): cg(cg) {}
        virtual bool operator()(bool enabled, const FunctionArgs &args) {
            if (enabled && AsmUnparser::ORGANIZED_BY_ADDRESS==args.unparser->get_organization()) {
                size_t ncallers = 0;
                Vertex called_v = args.func->get_cached_vertex();
                typename boost::graph_traits<FunctionCallGraph>::in_edge_iterator ei, eend;
                for (boost::tie(ei, eend)=in_edges(called_v, cg); ei!=eend; ++ei) {
                    SgAsmFunction *caller = get(boost::vertex_name, cg, source(*ei, cg));
                    if (0==ncallers++)
                        args.output <<StringUtility::addrToString(args.func->get_entry_va()) <<": Function called by";
                    args.output <<" 0x" <<std::hex <<caller->get_entry_va() <<std::dec;
                }
                if (0==ncallers)
                    args.output <<StringUtility::addrToString(args.func->get_entry_va()) <<": No known callers.";
                args.output <<"\n";
            }
            return enabled;
        }
    };

public:
    FuncName<SgAsmInstruction, AsmUnparser::UnparserCallback::InsnArgs> insn_func_name;
    FuncName<SgAsmStaticData,  AsmUnparser::UnparserCallback::StaticDataArgs> data_func_name;
    FuncCallers func_callers;
    DiscontiguousNotifier<SgAsmInstruction, AsmUnparser::UnparserCallback::InsnArgs> insn_skip;
    DiscontiguousNotifier<SgAsmStaticData,  AsmUnparser::UnparserCallback::StaticDataArgs> data_skip;
    rose_addr_t next_address;
    size_t nprinted;

    MyUnparser(FunctionCallGraph &cg, IdaInfo &ida)
        : insn_func_name(ida), data_func_name(ida), func_callers(cg), next_address(0), nprinted(0) {
        set_organization(AsmUnparser::ORGANIZED_BY_ADDRESS);
        insn_callbacks.pre
            .append(&insn_func_name)
            .after(&insnBlockSeparation, &insn_skip, 1);
        staticdata_callbacks.pre
            .append(&data_func_name)
            .after(&staticDataBlockSeparation, &data_skip, 1);
        function_callbacks.pre
            .before(&functionAttributes, &func_callers, 1);
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
               <<"Address     Hexadecimal data         CharData  BIR   ROSE     ";
        for (size_t i=0; i<insn_func_name.ida.size(); i++)
            output <<"IDA[" <<i <<"]     ";
        output <<"    Instruction etc.\n";
        output <<"----------- ------------------------|--------| --- {---------";
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
                } else if (0!=(func->get_reason() & SgAsmFunction::FUNC_INTERPAD)) {
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
    partitioner->set_search(SgAsmFunction::FUNC_DEFAULT | SgAsmFunction::FUNC_INTRABLOCK);
    //partitioner->set_debug(stderr);
    SgAsmBlock *gblock = partitioner->partition(interp, insns, map);
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
    unparser.unparse(std::cout, gblock);

    statistics(interp, insns, ida);
}

#endif /* HAVE_GCRYPT_H */
