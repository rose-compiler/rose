// Adds functions to the database.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "DwarfLineMapper.h"
#include "BinaryFunctionCall.h"
#include "BinaryReturnValueUsed.h"
#include "rose.h"

#include "vectorCompression.h"

using namespace rose;
using namespace CloneDetection;
using namespace rose::BinaryAnalysis;

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE SPECIMEN\n"
              <<"  This command parses and disassembles the specimen executable, selects functions that match the specified\n"
              <<"  criteria, and adds those functions to the database if they are not already present in the database. The ID\n"
              <<"  number for the specimen is emitted on standard output.\n"
              <<"\n"
              <<"    --[no-]link\n"
              <<"            Perform dynamic linking and also consider functions that exist in the linked-in libraries. The\n"
              <<"            default is to not perform dynamic linking.\n"
              <<"    --[no-]save-ast\n"
              <<"            The --save-ast switch causes a binary version of the AST to be saved in the database.  This\n"
              <<"            AST will be read by other specimen-processing commands rather than reparsing the specimen.\n"
              <<"            The default is to not save the AST in the database because not all necessary binary information\n"
              <<"            is saved (MemoryMaps for instance, are not part of the AST).\n"
              <<"    --signature-components=by_category|total_for_variant|operand_total|ops_for_variant|specific_op|\n"
              <<"                           operand_pair|apply_log\n"
              <<"            Select which, if any, properties should be counted and/or how they should be counted. By default\n"
              <<"            no properties are counted. By default the instructions are counted by operation kind, but one can\n"
              <<"            optionally choose to count by instruction category.\n"
              <<"    --save-instructions\n"
              <<"            Save instruction mappping to the database. Only needed for the lsh clone detection.\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    SPECIMENS\n"
              <<"            Zero or more binary specimen names.\n";
    exit(exit_status);
}

struct Switches {
    Switches(): link(false), save_ast(false) {}
    bool link, save_ast;
};

static struct InstructionSelector: SgAsmFunction::NodeSelector {
    virtual bool operator()(SgNode *node) { return isSgAsmInstruction(node)!=NULL; }
} iselector;

static struct DataSelector: SgAsmFunction::NodeSelector {
    virtual bool operator()(SgNode *node) { return isSgAsmStaticData(node)!=NULL; }
} dselector;

// Returns true if the file looks like text
static bool
is_text_file(FILE *f)
{
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
static void
save_source_files(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id)
{
    std::cerr <<argv0 <<": saving source code listings for each function\n";
    SqlDatabase::StatementPtr stmt1 = tx->statement("select files.id, files.name"
                                                    " from semantic_files as files"
                                                    " left join semantic_sources as sources"
                                                    " on files.id = sources.file_id"
                                                    " where sources.file_id is null");
    SqlDatabase::StatementPtr stmt2 = tx->statement("select count(*) from semantic_sources where file_id = ?");
    SqlDatabase::StatementPtr stmt3 = tx->statement("insert into semantic_sources"
                                                    // 0       1        2     3
                                                    "(file_id, linenum, line, cmd) values (?,?,?,?)");

    char *line = NULL;
    size_t linesz = 0;
    for (SqlDatabase::Statement::iterator c1=stmt1->begin(); c1!=stmt1->end(); ++c1) {
        int file_id = c1.get<int>(0);
        std::string file_name = c1.get<std::string>(1);
        FILE *f = fopen(file_name.c_str(), "r");
        if (is_text_file(f)) {
            stmt2->bind(0, file_id);
            if (stmt2->execute_int()<=0) {
                size_t line_num = 0;
                ssize_t nread;
                while ((nread=getline(&line, &linesz, f))>0) {
                    while (nread>0 && isspace(line[nread-1]))
                        line[--nread] = '\0';
                    stmt3->bind(0, file_id);
                    stmt3->bind(1, ++line_num);
                    stmt3->bind(2, line);
                    stmt3->bind(3, cmd_id);
                    stmt3->execute();
                }
            }
        }
        if (f)
            fclose(f);
    }
    if (line)
        free(line);
}

// Rewrite a call graph by removing dynamic linked function thunks.  If we were using ROSE's experimental Graph2 stuff
// instead of the Boost Graph Library's (BGL) adjacency_list, we could modify the graph in place efficiently.  But with
// adjacency_list it's more efficient to create a whole new graph. [Robb P. Matzke 2013-05-16]
static CG
rewrite_call_graph(CG &src)
{
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
static void
save_call_graph(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id, SgAsmInterpretation *interp, FilesTable &files,
                const std::vector<SgAsmFunction*> &selected_functions)
{
    std::cerr <<argv0 <<": saving call graph\n";

    IdFunctionMap existing_ids = existing_functions(tx, files, selected_functions);
    std::map<SgAsmFunction*, int/*id*/> existing_funcs;
    for (IdFunctionMap::iterator i=existing_ids.begin(); i!=existing_ids.end(); ++i)
        existing_funcs[i->second] = i->first;

    // Generate the full call graph, then rewrite the call graph so calls to thunks for dynamically-linked functions look
    // like they're calls directly to the dynamically linked function.
    CG cg1 = BinaryAnalysis::FunctionCall().build_cg_from_ast<CG>(interp);
    CG cg2 = rewrite_call_graph(cg1);

    // Filter out vertices (and their incident edges) if the vertex is a function which is not part of the database (has no
    // function ID).
    struct CGFilter: BinaryAnalysis::FunctionCall::VertexFilter {
        const std::map<SgAsmFunction*, int> &existing;
        CGFilter(const std::map<SgAsmFunction*, int> &existing): existing(existing) {}
        virtual bool operator()(BinaryAnalysis::FunctionCall*, SgAsmFunction *vertex) ROSE_OVERRIDE {
            return existing.find(vertex)!=existing.end();
        }
    } vertex_filter(existing_funcs);
    BinaryAnalysis::FunctionCall copier;
    copier.set_vertex_filter(&vertex_filter);
    CG cg = copier.copy<CG>(cg2);

    // Delete from the database all call graph edges that correspond to one of our specimen files.
    std::set<int> file_ids;
    for (IdFunctionMap::iterator fi=existing_ids.begin(); fi!=existing_ids.end(); ++fi) {
        std::string filename = filename_for_function(fi->second);
        file_ids.insert(files.id(filename));
    }
    std::string sql = "delete from semantic_cg where file_id in (";
    for (std::set<int>::iterator i=file_ids.begin(); i!=file_ids.end(); ++i)
        sql += (i==file_ids.begin()?"":", ") + StringUtility::numberToString(*i);
    sql += ")";
    tx->execute(sql);

    // Add the new call graph edges into to the database
    SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_cg"
                                                   // 0      1       2        3
                                                   "(caller, callee, file_id, cmd) values (?, ?, ?, ?)");
    boost::graph_traits<CG>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(cg); ei!=ei_end; ++ei) {
        CG_Vertex caller_v = source(*ei, cg);
        CG_Vertex callee_v = target(*ei, cg);
        SgAsmFunction *caller = get(boost::vertex_name, cg, caller_v);
        SgAsmFunction *callee = get(boost::vertex_name, cg, callee_v);
        assert(existing_funcs.find(caller)!=existing_funcs.end());
        assert(existing_funcs.find(callee)!=existing_funcs.end());
        int caller_id = existing_funcs[caller];
        int callee_id = existing_funcs[callee];
        stmt->bind(0, caller_id);
        stmt->bind(1, callee_id);
        std::string filename = filename_for_function(caller);
        stmt->bind(2, files.id(filename));
        stmt->bind(3, cmd_id);
        stmt->execute();
    }
};

int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    Switches opt;
    int argno = 1;

    bool save_instructions = false;

    std::vector<std::string> signature_components;

    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        std::cout << argv[argno] << std::endl;
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--link")) {
            opt.link = true;
        } else if (!strcmp(argv[argno], "--no-link")) {
            opt.link = false;
        } else if (!strcmp(argv[argno], "--save-ast")) {
            opt.save_ast = true;
        } else if (!strcmp(argv[argno], "--no-save-ast")) {
            opt.save_ast = false;
        } else if (!strcmp(argv[argno], "--save-instructions")) {
            save_instructions = true;
        } else if (!strncmp(argv[argno], "--signature-components=", 23)) {
            static const char *comp_opts[7] = {"by_category", "total_for_variant", "operand_total", "ops_for_variant",
                                               "specific_op", "operand_pair", "apply_log"};
            bool isValid = false;
            for (int i=0; i<7 && !isValid; ++i)
                isValid = 0==strcmp(argv[argno]+23, comp_opts[i]);
            if (!isValid) {
                std::cerr <<argv0 <<": invalid argument for --signature-components: " <<argv[argno]+23 <<"\n";
                exit(1);
            }
            signature_components.push_back(argv[argno]+23);
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+2!=argc)
        usage(1);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno++])->transaction();
    int64_t cmd_id = start_command(tx, argc, argv, "adding functions");
    std::string specimen_name = StringUtility::getAbsolutePathFromRelativePath(argv[argno++], true);
    FilesTable files(tx);

    // Parse the binary specimen
    int specimen_id = files.insert(specimen_name);
    SgAsmInterpretation *interp = open_specimen(specimen_name, argv0, opt.link);
    SgBinaryComposite *binfile = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
    assert(interp!=NULL && binfile!=NULL);

    // Figure out what functions need to be added to the database.
    std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(interp);
    std::cerr <<argv0 <<": " <<all_functions.size() <<" function" <<(1==all_functions.size()?"":"s") <<" found\n";
    IdFunctionMap functions_to_add = missing_functions(tx, files, all_functions);
    std::cerr <<argv0 <<": adding " <<functions_to_add.size() <<" function" <<(1==functions_to_add.size()?"":"s")
              <<" to the database\n";

    // Figure out how many places each function is called and how many times a return value is used.
    ReturnValueUsed::Results used_retvals = ReturnValueUsed::analyze(interp);

    // Get source code location info for all instructions and update the FilesTable
    BinaryAnalysis::DwarfLineMapper* dlm;

    if (save_instructions) {
        dlm = new BinaryAnalysis::DwarfLineMapper(binfile);
        dlm->fix_holes();
        std::vector<SgAsmInstruction*> all_insns = SageInterface::querySubTree<SgAsmInstruction>(binfile);
        for (std::vector<SgAsmInstruction*>::iterator ii=all_insns.begin(); ii!=all_insns.end(); ++ii) {
            BinaryAnalysis::DwarfLineMapper::SrcInfo srcinfo = dlm->addr2src((*ii)->get_address());
            if (srcinfo.file_id>=0)
                files.insert(Sg_File_Info::getFilenameFromID(srcinfo.file_id));
        }
    }

    files.save(tx); // needs to be saved before we write foriegn keys into the semantic_functions table

    // Process each function
    SqlDatabase::StatementPtr stmt1 = tx->statement("insert into semantic_functions"
                                                    // 0   1         2     3        4            5
                                                    " (id, entry_va, name, file_id, specimen_id, ninsns,"
                                                    // 6      7      8     9       10          11
                                                    "  isize, dsize, size, digest, counts_b64, cmd,"
                                                    // 12         13
                                                    "  callsites, retvals_used)"
                                                    " values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    SqlDatabase::StatementPtr stmt2 = tx->statement("insert into semantic_instructions"
                                                    // 0        1     2         3        4
                                                    " (address, size, assembly, func_id, position,"
                                                    // 5            6         7
                                                    "  src_file_id, src_line, cmd) values (?, ?, ?, ?, ?, ?, ?, ?)");
    for (IdFunctionMap::iterator fi=functions_to_add.begin(); fi!=functions_to_add.end(); ++fi) {
        // Save function
        SgAsmFunction *func = fi->second;
        ReturnValueUsed::UsageCounts &retval_usage = used_retvals[func];
        AddressIntervalSet e_insns, e_data, e_total;
        size_t ninsns = func->get_extent(&e_insns, NULL, NULL, &iselector);
        func->get_extent(&e_data, NULL, NULL, &dselector);
        func->get_extent(&e_total);
        uint8_t digest[20];
        func->get_sha1(digest);
        std::string digest_str = Combinatorics::digest_to_string(std::vector<uint8_t>(digest, digest+20));

        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(func);
        SignatureVector vec;
        createVectorsForAllInstructions(vec, insns, signature_components);
        std::vector<uint8_t> compressedCounts = compressVector(vec.getBase(), SignatureVector::Size);

        stmt1->bind(0, fi->first);
        stmt1->bind(1, func->get_entry_va());
        stmt1->bind(2, func->get_name());
        stmt1->bind(3, files.id(filename_for_function(func)));
        stmt1->bind(4, specimen_id);
        stmt1->bind(5, ninsns);
        stmt1->bind(6, e_insns.size());
        stmt1->bind(7, e_data.size());
        stmt1->bind(8, e_total.size());
        stmt1->bind(9, digest_str);
        stmt1->bind(10, StringUtility::encode_base64(&compressedCounts[0], compressedCounts.size()));
        stmt1->bind(11, cmd_id);
        stmt1->bind(12, retval_usage.nUsed + retval_usage.nUnused);
        stmt1->bind(13, retval_usage.nUsed);
        stmt1->execute();

	// Save instructions
	for (size_t i=0; i<insns.size(); ++i) {
            int file_id  = -1;
            int line_num = -1;
            if (save_instructions) {
                BinaryAnalysis::DwarfLineMapper::SrcInfo srcinfo = dlm->addr2src(insns[i]->get_address());
                srcinfo.file_id < 0 ? -1 : files.id(Sg_File_Info::getFilenameFromID(srcinfo.file_id));
                line_num = srcinfo.line_num;
            }

            stmt2->bind(0, insns[i]->get_address());
            stmt2->bind(1, insns[i]->get_size());
            stmt2->bind(2, unparseInstruction(insns[i]));
            stmt2->bind(3, fi->first);
            stmt2->bind(4, i);
            stmt2->bind(5, file_id);
            stmt2->bind(6, line_num);
            stmt2->bind(7, cmd_id);
            stmt2->execute();
	}
    }

    // Save specimen information
    if (!functions_to_add.empty()) {
        save_source_files(tx, cmd_id);
        save_call_graph(tx, cmd_id, interp, files, all_functions);
        if (opt.save_ast) {
            std::cerr <<argv0 <<": saving AST\n";
            files.save_ast(tx, cmd_id, specimen_id, SageInterface::getProject());
        }
    }

    // Add binary files to the database and populate the semantic_specfiles table
    std::cerr <<argv0 <<": saving specimen-related binary files\n";
    struct T1: AstSimpleProcessing {
        SqlDatabase::TransactionPtr &tx;
        FilesTable &files;
        int64_t cmd_id;
        int specimen_id;
        T1(SqlDatabase::TransactionPtr &tx, FilesTable &files, int64_t cmd_id, int specimen_id)
            : tx(tx), files(files), cmd_id(cmd_id), specimen_id(specimen_id) {}
        void visit(SgNode *node) {
            if (SgAsmGenericFile *file = isSgAsmGenericFile(node)) {
                if (files.exists(file->get_name())) {
                    int file_id = files.id(file->get_name());
                    files.add_content(tx, cmd_id, file_id);
                    if (file_id!=specimen_id) {
                        tx->statement("insert into semantic_specfiles (specimen_id, file_id) values (?, ?)")
                            ->bind(0, specimen_id)
                            ->bind(1, file_id)
                            ->execute();
                    }
                }
            }
        }
    };
    T1(tx, files, cmd_id, specimen_id).traverse(SageInterface::getProject(), preorder);

    finish_command(tx, cmd_id,
                   "added "+StringUtility::numberToString(functions_to_add.size())+
                   " function"+(1==functions_to_add.size()?"":"s"));
    files.save(tx);
    tx->commit();
    std::cout <<"specimen " <<specimen_id <<"\n";
    return 0;
}
