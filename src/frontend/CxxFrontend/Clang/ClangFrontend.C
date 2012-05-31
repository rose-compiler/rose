
#include <iostream>

#include "ClangFrontend-private.h"

#include "rose_config.h"

#define DEBUG_VISITOR 0
#define DEBUG_SOURCE_LOCATION 0
#define USE_CLANG_HACK 0
#define DEBUG_SYMBOL_TABLE_LOOKUP 0
#define DEBUG_TRAVERSE_DECL 0
#define DEBUG_ARGS 0
#define DEBUG_ENUM_DECL 0
#define DEBUG_VISIT_STMT 0

extern bool roseInstallPrefix(std::string&);

int clang_main(int argc, char ** argv, SgSourceFile& sageFile) {
  // 0 - Analyse Cmd Line

    std::vector<std::string> inc_dirs_list;
    std::vector<std::string> define_list;
    std::vector<std::string> inc_list;
    std::string input_file;

    for (int i = 0; i < argc; i++) {
        std::string current_arg(argv[i]);
        if (current_arg.find("-I") == 0) {
            if (current_arg.length() > 2) {
                inc_dirs_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argc)
                    inc_dirs_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-D") == 0) {
            if (current_arg.length() > 2) {
                define_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argc)
                    define_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-c") == 0) {}
        else if (current_arg.find("-o") == 0) {
            if (current_arg.length() == 2) {
                i++;
                if (i >= argc) break;
            }
        }
        else {
            // TODO -include
#if DEBUG_ARGS
            std::cerr << "argv[" << i << "] = " << current_arg << " is neither define or include dir. Use it as input file."  << std::endl;
#endif
            input_file = current_arg;
        }
    }

    ClangToSageTranslator::Language language = ClangToSageTranslator::unknown;

    size_t last_period = input_file.find_last_of(".");
    std::string extention(input_file.substr(last_period + 1));

    if (extention == "c") {
        language = ClangToSageTranslator::C;
    }
    else if (extention == "C" || extention == "cxx" || extention == "cpp" || extention == "cc") {
        language = ClangToSageTranslator::CPLUSPLUS;
    }
    else if (extention == "objc") {
        language = ClangToSageTranslator::OBJC;
    }
    else if (extention == "cu") {
        language = ClangToSageTranslator::CUDA;
    }
    else if (extention == "ocl" || extention == "cl") {
        language = ClangToSageTranslator::OPENCL;
    }

    ROSE_ASSERT(language != ClangToSageTranslator::unknown);

    const char * cxx_config_include_dirs_array [] = CXX_INCLUDE_STRING;
    const char * c_config_include_dirs_array   [] = C_INCLUDE_STRING;

    std::vector<std::string> cxx_config_include_dirs (
                                                       cxx_config_include_dirs_array,
                                                       cxx_config_include_dirs_array + sizeof(cxx_config_include_dirs_array) / sizeof(const char*)
                                                     );
    std::vector<std::string> c_config_include_dirs   (
                                                       c_config_include_dirs_array,
                                                       c_config_include_dirs_array + sizeof(c_config_include_dirs_array) / sizeof(const char*)
                                                     );

    std::string rose_include_path;
    bool in_install_tree = roseInstallPrefix(rose_include_path);
    if (in_install_tree) {
        rose_include_path += "/include-staging/";
    }
    else {
        rose_include_path = std::string(ROSE_AUTOMAKE_TOP_BUILDDIR) + "/include-staging/";
    }

    std::vector<std::string>::iterator it;
    for (it = c_config_include_dirs.begin(); it != c_config_include_dirs.end(); it++)
        if (it->length() > 0 && (*it)[0] != '/')
            *it = rose_include_path + *it;
    for (it = cxx_config_include_dirs.begin(); it != cxx_config_include_dirs.end(); it++)
        if (it->length() > 0 && (*it)[0] != '/')
            *it = rose_include_path + *it;

    inc_dirs_list.push_back(rose_include_path + "clang/");


    // FIXME add ROSE path to gcc headers...
    switch (language) {
        case ClangToSageTranslator::C:
            inc_dirs_list.insert(inc_dirs_list.begin(), c_config_include_dirs.begin(), c_config_include_dirs.end());
            inc_list.push_back("clang-builtin-c.h");
            break;
        case ClangToSageTranslator::CPLUSPLUS:
            inc_dirs_list.insert(inc_dirs_list.begin(), cxx_config_include_dirs.begin(), cxx_config_include_dirs.end());
            inc_list.push_back("clang-builtin-cpp.hpp");
            break;
        case ClangToSageTranslator::CUDA:
            inc_dirs_list.insert(inc_dirs_list.begin(), cxx_config_include_dirs.begin(), cxx_config_include_dirs.end());
            inc_list.push_back("clang-builtin-cuda.hpp");
            break;
        case ClangToSageTranslator::OPENCL:
//          inc_dirs_list.insert(inc_dirs_list.begin(), c_config_include_dirs.begin(), c_config_include_dirs.end());
            // FIXME get the path right
            inc_list.push_back("clang-builtin-opencl.h");
            break;
        case ClangToSageTranslator::OBJC:
        default:
            ROSE_ASSERT(false);
    }

    // FIXME should be handle by Clang ?
    define_list.push_back("__I__=_Complex_I");

    unsigned cnt = define_list.size() + inc_dirs_list.size() + inc_list.size();
    char ** args = new char*[cnt];
    std::vector<std::string>::iterator it_str;
    unsigned i = 0;
    for (it_str = define_list.begin(); it_str != define_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i][0] = '-';
        args[i][1] = 'D';
        strcpy(&(args[i][2]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }
    for (it_str = inc_dirs_list.begin(); it_str != inc_dirs_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i][0] = '-';
        args[i][1] = 'I';
        strcpy(&(args[i][2]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }
    for (it_str = inc_list.begin(); it_str != inc_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 9];
        args[i][0] = '-'; args[i][1] = 'i'; args[i][2] = 'n'; args[i][3] = 'c';
        args[i][4] = 'l'; args[i][5] = 'u'; args[i][6] = 'd'; args[i][7] = 'e';
        strcpy(&(args[i][8]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }


  // 2 - Create a compiler instance

    clang::CompilerInstance * compiler_instance = new clang::CompilerInstance();

    clang::TextDiagnosticPrinter * diag_printer = new clang::TextDiagnosticPrinter(llvm::errs(), clang::DiagnosticOptions());
    compiler_instance->createDiagnostics(argc, argv, diag_printer, true, false);

    clang::CompilerInvocation * invocation = new clang::CompilerInvocation();
    clang::CompilerInvocation::CreateFromArgs(*invocation, args, &(args[cnt]), compiler_instance->getDiagnostics());
    compiler_instance->setInvocation(invocation);

    clang::LangOptions & lang_opts = compiler_instance->getLangOpts();

    switch (language) {
        case ClangToSageTranslator::C:
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_C, );
            break;
        case ClangToSageTranslator::CPLUSPLUS:
            lang_opts.CPlusPlus = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_CXX, );
            break;
        case ClangToSageTranslator::CUDA:
            lang_opts.CUDA = 1;
//          lang_opts.CPlusPlus = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_CUDA,   clang::LangStandard::lang_cuda);
            break;
        case ClangToSageTranslator::OPENCL:
            lang_opts.OpenCL = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_OpenCL, clang::LangStandard::lang_opencl);
            break;
        case ClangToSageTranslator::OBJC:
            ROSE_ASSERT(!"Objective-C is not supported by ROSE Compiler.");
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_, );
        default:
            ROSE_ASSERT(false);
    }

    clang::TargetOptions target_options;
    target_options.Triple = llvm::sys::getHostTriple();
    clang::TargetInfo * target_info = clang::TargetInfo::CreateTargetInfo(compiler_instance->getDiagnostics(), target_options);
    compiler_instance->setTarget(target_info);

    compiler_instance->createFileManager();
    compiler_instance->createSourceManager(compiler_instance->getFileManager());

    const clang::FileEntry * input_file_entry = compiler_instance->getFileManager().getFile(input_file);
    compiler_instance->getSourceManager().createMainFileID(input_file_entry);

    if (!compiler_instance->hasPreprocessor()) compiler_instance->createPreprocessor();

    if (!compiler_instance->hasASTContext()) compiler_instance->createASTContext();

    ClangToSageTranslator translator(compiler_instance, language);
    compiler_instance->setASTConsumer(&translator);   

    if (!compiler_instance->hasSema()) compiler_instance->createSema(clang::TU_Complete, NULL);

    ROSE_ASSERT (compiler_instance->hasDiagnostics());
    ROSE_ASSERT (compiler_instance->hasTarget());
    ROSE_ASSERT (compiler_instance->hasFileManager());
    ROSE_ASSERT (compiler_instance->hasSourceManager());
    ROSE_ASSERT (compiler_instance->hasPreprocessor());
    ROSE_ASSERT (compiler_instance->hasASTContext());
    ROSE_ASSERT (compiler_instance->hasSema());

  // 3 - Translate

    compiler_instance->getDiagnosticClient().BeginSourceFile(compiler_instance->getLangOpts(), &(compiler_instance->getPreprocessor()));
    clang::ParseAST(compiler_instance->getPreprocessor(), &translator, compiler_instance->getASTContext());
    compiler_instance->getDiagnosticClient().EndSourceFile();

    SgGlobal * global_scope = translator.getGlobalScope();

  // 4 - Attach to the file

    if (sageFile.get_globalScope() != NULL) SageInterface::deleteAST(sageFile.get_globalScope());

    sageFile.set_globalScope(global_scope);

    global_scope->set_parent(&sageFile);

    std::string file_name(input_file);

    Sg_File_Info * start_fi = new Sg_File_Info(file_name, 0, 0);
    Sg_File_Info * end_fi   = new Sg_File_Info(file_name, 0, 0);

    global_scope->set_startOfConstruct(start_fi);

    global_scope->set_endOfConstruct(end_fi);

  // 5 - Finish the AST (fixup phase)

    finishSageAST(translator);

    return 1;
}

void finishSageAST(ClangToSageTranslator & translator) {
    SgGlobal * global_scope = translator.getGlobalScope();

 // 1 - Label Statements: Move sub-statement after the label statement.

    std::vector<SgLabelStatement *> label_stmts = SageInterface::querySubTree<SgLabelStatement>(global_scope, V_SgLabelStatement);
    std::vector<SgLabelStatement *>::iterator label_stmt_it;
    for (label_stmt_it = label_stmts.begin(); label_stmt_it != label_stmts.end(); label_stmt_it++) {
        SgStatement * sub_stmt = (*label_stmt_it)->get_statement();
        if (!isSgNullStatement(sub_stmt)) {
            SgNullStatement * null_stmt = SageBuilder::buildNullStatement_nfi();
            translator.setCompilerGeneratedFileInfo(null_stmt);
            (*label_stmt_it)->set_statement(null_stmt);
            null_stmt->set_parent(*label_stmt_it);
            SageInterface::insertStatementAfter(*label_stmt_it, sub_stmt);
        }
    }

 // 2 - Place Preprocessor informations
}

SgGlobal * ClangToSageTranslator::getGlobalScope() { return p_global_scope; }

ClangToSageTranslator::ClangToSageTranslator(clang::CompilerInstance * compiler_instance, Language language_) :
    clang::ASTConsumer(),
    p_decl_translation_map(),
    p_stmt_translation_map(),
    p_type_translation_map(),
    p_global_scope(NULL),
    p_class_type_decl_first_see_in_type(),
    p_enum_type_decl_first_see_in_type(),
    p_compiler_instance(compiler_instance),
    p_sage_preprocessor_recorder(new SagePreprocessorRecord(&(p_compiler_instance->getSourceManager()))),
    language(language_)
{}

ClangToSageTranslator::~ClangToSageTranslator() {
    delete p_sage_preprocessor_recorder;
}

/* (protected) Helper methods */

void ClangToSageTranslator::applySourceRange(SgNode * node, clang::SourceRange source_range) {
    SgLocatedNode * located_node = isSgLocatedNode(node);
    SgInitializedName * init_name = isSgInitializedName(node);

#if DEBUG_SOURCE_LOCATION
    std::cerr << "Set File_Info for " << node << " of type " << node->class_name() << std::endl;
#endif

    if (located_node == NULL && init_name == NULL) {
        std::cerr << "Consistency error: try to apply a source range to a Sage node which is not a SgLocatedNode or a SgInitializedName." << std::endl;
        exit(-1);
    }
    else if (located_node != NULL) {
        Sg_File_Info * fi = located_node->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = located_node->get_endOfConstruct();
        if (fi != NULL) delete fi;
    }
    else if (init_name != NULL) {
        Sg_File_Info * fi = init_name->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = init_name->get_endOfConstruct();
        if (fi != NULL) delete fi;
    }

    Sg_File_Info * start_fi = NULL;
    Sg_File_Info * end_fi = NULL;

    if (source_range.isValid()) {
        clang::SourceLocation begin  = source_range.getBegin();
        clang::SourceLocation end    = source_range.getEnd();

        if (begin.isValid() && end.isValid()) {
            if (begin.isMacroID()) {
#if DEBUG_SOURCE_LOCATION
                std::cerr << "\tDump SourceLocation begin as it is a MacroID: ";
                begin.dump(p_compiler_instance->getSourceManager());
                std::cerr << std::endl;
#endif
                begin = p_compiler_instance->getSourceManager().getExpansionLoc(begin);
                ROSE_ASSERT(begin.isValid());
            }

            if (end.isMacroID()) {
#if DEBUG_SOURCE_LOCATION
                std::cerr << "\tDump SourceLocation end as it is a MacroID: ";
                end.dump(p_compiler_instance->getSourceManager());
                std::cerr << std::endl;
#endif
                end = p_compiler_instance->getSourceManager().getExpansionLoc(end);
                ROSE_ASSERT(end.isValid());
            }

            clang::FileID file_begin = p_compiler_instance->getSourceManager().getFileID(begin);
            clang::FileID file_end   = p_compiler_instance->getSourceManager().getFileID(end);

            bool inv_begin_line;
            bool inv_begin_col;
            bool inv_end_line;
            bool inv_end_col;

            unsigned ls = p_compiler_instance->getSourceManager().getSpellingLineNumber(begin, &inv_begin_line);
            unsigned cs = p_compiler_instance->getSourceManager().getSpellingColumnNumber(begin, &inv_begin_col);
            unsigned le = p_compiler_instance->getSourceManager().getSpellingLineNumber(end, &inv_end_line);
            unsigned ce = p_compiler_instance->getSourceManager().getSpellingColumnNumber(end, &inv_end_col);

            if (file_begin.isInvalid() || file_end.isInvalid() || inv_begin_line || inv_begin_col || inv_end_line || inv_end_col) {
                ROSE_ASSERT(!"Should not happen as everything have been check before...");
            }

            if (p_compiler_instance->getSourceManager().getFileEntryForID(file_begin) != NULL) {
                std::string file = p_compiler_instance->getSourceManager().getFileEntryForID(file_begin)->getName();

                start_fi = new Sg_File_Info(file, ls, cs);
                end_fi   = new Sg_File_Info(file, le, ce);
#if DEBUG_SOURCE_LOCATION
                std::cerr << "\tCreate FI for node in " << file << ":" << ls << ":" << cs << std::endl;
#endif
            }
#if DEBUG_SOURCE_LOCATION
            else {
                std::cerr << "\tDump SourceLocation for \"Invalid FileID\": " << std::endl << "\t";
                begin.dump(p_compiler_instance->getSourceManager());
                std::cerr << std::endl << "\t";
                end.dump(p_compiler_instance->getSourceManager());
                std::cerr << std::endl;
            }
#endif
        }
    }

    if (start_fi == NULL && end_fi == NULL) {
        start_fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        end_fi   = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();

        start_fi->setCompilerGenerated();
        end_fi->setCompilerGenerated();
#if DEBUG_SOURCE_LOCATION
        std::cerr << "Create FI for compiler generated node" << std::endl;
#endif
    }
    else if (start_fi == NULL || end_fi == NULL) {
        ROSE_ASSERT(!"start_fi == NULL xor end_fi == NULL");
    }

    if (located_node != NULL) {
        located_node->set_startOfConstruct(start_fi);
        located_node->set_endOfConstruct(end_fi);
    }
    else if (init_name != NULL) {
        init_name->set_startOfConstruct(start_fi);
        init_name->set_endOfConstruct(end_fi);
    }

}

void ClangToSageTranslator::setCompilerGeneratedFileInfo(SgNode * node, bool to_be_unparse) {
    Sg_File_Info * start_fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
    Sg_File_Info * end_fi   = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();

    start_fi->setCompilerGenerated();
    end_fi->setCompilerGenerated();

    if (to_be_unparse) {
        start_fi->setOutputInCodeGeneration();
        end_fi->setOutputInCodeGeneration();
    }

    ROSE_ASSERT(start_fi != NULL && end_fi != NULL);

#if DEBUG_SOURCE_LOCATION
    std::cerr << "Set File_Info for " << node << " of type " << node->class_name() << std::endl;
#endif

    SgLocatedNode * located_node = isSgLocatedNode(node);
    SgInitializedName * init_name = isSgInitializedName(node);

    if (located_node == NULL && init_name == NULL) {
        std::cerr << "Consistency error: try to set a Sage node which is not a SgLocatedNode or a SgInitializedName as compiler generated" << std::endl;
        exit(-1);
    }
    else if (located_node != NULL) {
        Sg_File_Info * fi = located_node->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = located_node->get_endOfConstruct();
        if (fi != NULL) delete fi;

        located_node->set_startOfConstruct(start_fi);
        located_node->set_endOfConstruct(end_fi);
    }
    else if (init_name != NULL) {
        Sg_File_Info * fi = init_name->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = init_name->get_endOfConstruct();
        if (fi != NULL) delete fi;

        init_name->set_startOfConstruct(start_fi);
        init_name->set_endOfConstruct(end_fi);
    }
}

SgSymbol * ClangToSageTranslator::GetSymbolFromSymbolTable(clang::NamedDecl * decl) {
    if (decl == NULL) return NULL;

    SgScopeStatement * scope = SageBuilder::topScopeStack();

    SgName name(decl->getNameAsString());

#if DEBUG_SYMBOL_TABLE_LOOKUP
    std::cerr << "Lookup symbol for: " << name << std::endl;
#endif

    if (name == "") {
        return NULL;
    }

    std::list<SgScopeStatement *>::reverse_iterator it;
    SgSymbol * sym = NULL;
    switch (decl->getKind()) {
        case clang::Decl::Typedef:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                 sym = (*it)->lookup_typedef_symbol(name);
                 it++;
            }
            break;
        }
        case clang::Decl::Var:
        case clang::Decl::ParmVar:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_variable_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::Function:
        {
            SgType * tmp_type = buildTypeFromQualifiedType(((clang::FunctionDecl *)decl)->getType());
            SgFunctionType * type = isSgFunctionType(tmp_type);
            ROSE_ASSERT(type);
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_function_symbol(name, type);
                it++;
            }
            break;
        }
        case clang::Decl::Field:
        {
            SgClassDeclaration * sg_class_decl = isSgClassDeclaration(Traverse(((clang::FieldDecl *)decl)->getParent()));
            ROSE_ASSERT(sg_class_decl != NULL);
            if (sg_class_decl->get_definingDeclaration() == NULL)
                std::cerr << "Runtime Error: cannot find the definition of the class/struct associate to the field: " << name << std::endl;
            else {
                scope = isSgClassDeclaration(sg_class_decl->get_definingDeclaration())->get_definition();
                // TODO: for C++, if 'scope' is in 'SageBuilder::ScopeStack': problem!!!
                //       It means that we are currently building the class
                while (scope != NULL && sym == NULL) {
                    sym = scope->lookup_variable_symbol(name);
                    scope = scope->get_scope();
                }
            }
            break;
        }
        case clang::Decl::CXXRecord:
        case clang::Decl::Record:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_class_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::Label:
        {
            // Should not be reach as we use Traverse to retrieve Label (they are "terminal" statements) (it avoids the problem of forward use of label: goto before declaration)
            name = SgName(((clang::LabelDecl *)decl)->getStmt()->getName());
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_label_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::EnumConstant:
        {
            name = SgName(((clang::EnumConstantDecl *)decl)->getName());
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_enum_field_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::Enum:
        {
            name = SgName(((clang::EnumDecl *)decl)->getName());
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_enum_symbol(name);
                it++;
            }
            break;
        }
        default:
            std::cerr << "Runtime Error: Unknown type of Decl. (" << decl->getDeclKindName() << ")" << std::endl;
    }

    return sym;
}

SgType * ClangToSageTranslator::buildTypeFromQualifiedType(const clang::QualType & qual_type) {
    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);

    ROSE_ASSERT(type != NULL); 

    if (qual_type.hasLocalQualifiers()) {
        SgModifierType * modified_type = new SgModifierType(type);
        SgTypeModifier & sg_modifer = modified_type->get_typeModifier();
        clang::Qualifiers qualifier = qual_type.getLocalQualifiers();

        if (qualifier.hasConst()) sg_modifer.get_constVolatileModifier().setConst();
        if (qualifier.hasVolatile()) sg_modifer.get_constVolatileModifier().setVolatile();
        if (qualifier.hasRestrict()) sg_modifer.setRestrict();
        
        if (qualifier.hasAddressSpace()) {
            unsigned addrspace = qualifier.getAddressSpace();
            switch (addrspace) {
                case clang::LangAS::opencl_global:
                    sg_modifer.setOpenclGlobal();
                    break;
                case clang::LangAS::opencl_local:
                    sg_modifer.setOpenclLocal();
                    break;
                case clang::LangAS::opencl_constant:
                    sg_modifer.setOpenclConstant();
                    break;
                default:
                    sg_modifer.setAddressSpace();
                    sg_modifer.set_address_space_value(addrspace);
            }
        }
/* TODO ObjC
        hasObjCGCAttr
        hasObjCLifetime
*/
        modified_type = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);

        return modified_type;
    }
    else {
        return type;
    }
}

/* Overload of ASTConsumer::HandleTranslationUnit, it is the "entry point" */

void ClangToSageTranslator::HandleTranslationUnit(clang::ASTContext & ast_context) {
    Traverse(ast_context.getTranslationUnitDecl());
}

/* Traverse({Decl|Stmt|Type} *) methods */

SgNode * ClangToSageTranslator::Traverse(clang::Decl * decl) {
    if (decl == NULL)
        return NULL;

    std::map<clang::Decl *, SgNode *>::iterator it = p_decl_translation_map.find(decl);
    if (it != p_decl_translation_map.end()) {
#if DEBUG_TRAVERSE_DECL
        std::cerr << "Traverse Decl : " << decl << " ";
        if (clang::NamedDecl::classof(decl)) {
            std::cerr << ": " << ((clang::NamedDecl *)decl)->getNameAsString() << ") ";
        }
        std::cerr << " already visited : node = " << it->second << std::endl;
#endif
        return it->second;
    }

    SgNode * result = NULL;
    bool ret_status = false;

    switch (decl->getKind()) {
        case clang::Decl::TranslationUnit:
            ret_status = VisitTranslationUnitDecl((clang::TranslationUnitDecl *)decl, &result);
            break;
        case clang::Decl::Typedef:
            ret_status = VisitTypedefDecl((clang::TypedefDecl *)decl, &result);
            break;
        case clang::Decl::Var:
            ret_status = VisitVarDecl((clang::VarDecl *)decl, &result);
            break;
        case clang::Decl::Function:
            ret_status = VisitFunctionDecl((clang::FunctionDecl *)decl, &result);
            break;
        case clang::Decl::ParmVar:
            ret_status = VisitParmVarDecl((clang::ParmVarDecl *)decl, &result);
            break;
        case clang::Decl::CXXRecord:
            std::cout << "Using C level for C++ construct: CXXRecord" << std::endl;
        case clang::Decl::Record:
            ret_status = VisitRecordDecl((clang::RecordDecl *)decl, &result);
            break;
        case clang::Decl::Field:
            ret_status = VisitFieldDecl((clang::FieldDecl *)decl, &result);
            break;
        case clang::Decl::Enum:
            ret_status = VisitEnumDecl((clang::EnumDecl *)decl, &result);
            break;
        case clang::Decl::EnumConstant:
            ret_status = VisitEnumConstantDecl((clang::EnumConstantDecl *)decl, &result);
            break;
        // TODO cases
        default:
            std::cerr << "Unknown declacaration kind: " << decl->getDeclKindName() << " !" << std::endl;
            ROSE_ASSERT(false);
    }

    ROSE_ASSERT(ret_status == false || result != NULL);

    p_decl_translation_map.insert(std::pair<clang::Decl *, SgNode *>(decl, result));

#if DEBUG_TRAVERSE_DECL
    std::cerr << "Traverse(clang::Decl : " << decl << " ";
    if (clang::NamedDecl::classof(decl)) {
        std::cerr << ": " << ((clang::NamedDecl *)decl)->getNameAsString() << ") ";
    }
    std::cerr << " visit done : node = " << result << std::endl;
#endif

    return ret_status ? result : NULL;
}

SgNode * ClangToSageTranslator::Traverse(clang::Stmt * stmt) {
    if (stmt == NULL)
        return NULL;

    std::map<clang::Stmt *, SgNode *>::iterator it = p_stmt_translation_map.find(stmt);
    if (it != p_stmt_translation_map.end())
        return it->second; 

    SgNode * result = NULL;
    bool ret_status = false;

    switch (stmt->getStmtClass()) {
        case clang::Stmt::InitListExprClass:
            ret_status = VisitInitListExpr((clang::InitListExpr *)stmt, &result);
            break;
        case clang::Stmt::DesignatedInitExprClass:
            ret_status = VisitDesignatedInitExpr((clang::DesignatedInitExpr *)stmt, &result);
            break;
        case clang::Stmt::IntegerLiteralClass:
            ret_status = VisitIntegerLiteral((clang::IntegerLiteral *)stmt, &result);
            break;
        case clang::Stmt::FloatingLiteralClass:
            ret_status = VisitFloatingLiteral((clang::FloatingLiteral *)stmt, &result);
            break;
        case clang::Stmt::ImaginaryLiteralClass:
            ret_status = VisitImaginaryLiteral((clang::ImaginaryLiteral *)stmt, &result);
            break;
        case clang::Stmt::CompoundLiteralExprClass:
            ret_status = VisitCompoundLiteralExpr((clang::CompoundLiteralExpr *)stmt, &result);
            break;
        case clang::Stmt::ImplicitCastExprClass:
            ret_status = VisitImplicitCastExpr((clang::ImplicitCastExpr *)stmt, &result);
            break;
        case clang::Stmt::CharacterLiteralClass:
            ret_status = VisitCharacterLiteral((clang::CharacterLiteral *)stmt, &result);
            break;
        case clang::Stmt::ParenExprClass:
            ret_status = VisitParenExpr((clang::ParenExpr *)stmt, &result);
            break;
        case clang::Stmt::PredefinedExprClass:
            ret_status = VisitPredefinedExpr((clang::PredefinedExpr *)stmt, &result);
            break;
        case clang::Stmt::StringLiteralClass:
            ret_status = VisitStringLiteral((clang::StringLiteral *)stmt, &result);
            break;
        case clang::Stmt::UnaryExprOrTypeTraitExprClass:
            ret_status = VisitUnaryExprOrTypeTraitExpr((clang::UnaryExprOrTypeTraitExpr *)stmt, &result);
            break;
        case clang::Stmt::ExtVectorElementExprClass:
            ret_status = VisitExtVectorElementExpr((clang::ExtVectorElementExpr *)stmt, &result);
            break;
        case clang::Stmt::BreakStmtClass:
            ret_status = VisitBreakStmt((clang::BreakStmt *)stmt, &result);
            break;
        case clang::Stmt::CompoundStmtClass:
            ret_status = VisitCompoundStmt((clang::CompoundStmt *)stmt, &result);
            break;
        case clang::Stmt::ContinueStmtClass:
            ret_status = VisitContinueStmt((clang::ContinueStmt *)stmt, &result);
            break;
        case clang::Stmt::DeclStmtClass:
            ret_status = VisitDeclStmt((clang::DeclStmt *)stmt, &result);
            break;
        case clang::Stmt::CallExprClass:
            ret_status = VisitCallExpr((clang::CallExpr *)stmt, &result);
            break;
        case clang::Stmt::CStyleCastExprClass:
            ret_status = VisitCStyleCastExpr((clang::CStyleCastExpr *)stmt, &result);
            break;
        case clang::Stmt::DeclRefExprClass:
            ret_status = VisitDeclRefExpr((clang::DeclRefExpr *)stmt, &result);
            break;
        case clang::Stmt::UnaryOperatorClass:
            ret_status = VisitUnaryOperator((clang::UnaryOperator *)stmt, &result);
            break;
        case clang::Stmt::VAArgExprClass:
            ret_status = VisitVAArgExpr((clang::VAArgExpr *)stmt, &result);
            break;
        case clang::Stmt::ForStmtClass:
            ret_status = VisitForStmt((clang::ForStmt *)stmt, &result);
            break;
        case clang::Stmt::IfStmtClass:
            ret_status = VisitIfStmt((clang::IfStmt *)stmt, &result);
            break;
        case clang::Stmt::DoStmtClass:
            ret_status = VisitDoStmt((clang::DoStmt *)stmt, &result);
            break;
        case clang::Stmt::ReturnStmtClass:
            ret_status = VisitReturnStmt((clang::ReturnStmt *)stmt, &result);
            break;
        case clang::Stmt::BinaryOperatorClass:
        case clang::Stmt::CompoundAssignOperatorClass:
            ret_status = VisitBinaryOperator((clang::BinaryOperator *)stmt, &result);
            break;
        case clang::Stmt::ConditionalOperatorClass:
            ret_status = VisitConditionalOperator((clang::ConditionalOperator *)stmt, &result);
            break;
        case clang::Stmt::ArraySubscriptExprClass:
            ret_status = VisitArraySubscriptExpr((clang::ArraySubscriptExpr *)stmt, &result);
            break;
        case clang::Stmt::MemberExprClass:
            ret_status = VisitMemberExpr((clang::MemberExpr *)stmt, &result);
            break;
        case clang::Stmt::LabelStmtClass:
            ret_status = VisitLabelStmt((clang::LabelStmt *)stmt, &result);
            break;
        case clang::Stmt::NullStmtClass:
            ret_status = VisitNullStmt((clang::NullStmt *)stmt, &result);
            break;
        case clang::Stmt::GotoStmtClass:
            ret_status = VisitGotoStmt((clang::GotoStmt *)stmt, &result);
            break;
        case clang::Stmt::WhileStmtClass:
            ret_status = VisitWhileStmt((clang::WhileStmt *)stmt, &result);
            break;
        case clang::Stmt::CaseStmtClass:
            ret_status = VisitCaseStmt((clang::CaseStmt *)stmt, &result);
            break;
        case clang::Stmt::DefaultStmtClass:
            ret_status = VisitDefaultStmt((clang::DefaultStmt *)stmt, &result);
            break;
        case clang::Stmt::SwitchStmtClass:
            ret_status = VisitSwitchStmt((clang::SwitchStmt *)stmt, &result);
            break;
//        case clang::Stmt::ImplicitValueInitExprClass: break; // FIXME
        // TODO
        default:
            std::cerr << "Unknown statement kind: " << stmt->getStmtClassName() << " !" << std::endl;
            ROSE_ASSERT(false);
    }

    ROSE_ASSERT(result != NULL);

    p_stmt_translation_map.insert(std::pair<clang::Stmt *, SgNode *>(stmt, result));

    return result;
}

SgNode * ClangToSageTranslator::Traverse(const clang::Type * type) {
    if (type == NULL)
        return NULL;

    std::map<const clang::Type *, SgNode *>::iterator it = p_type_translation_map.find(type);
    if (it != p_type_translation_map.end()) 
         return it->second;

    SgNode * result = NULL;
    bool ret_status = false;

    switch (type->getTypeClass()) {
        case clang::Type::Builtin:
            ret_status = VisitBuiltinType((clang::BuiltinType *)type, &result);
            break;
        case clang::Type::Complex:
            ret_status = VisitComplexType((clang::ComplexType *)type, &result);
            break;
        case clang::Type::Pointer:
            ret_status = VisitPointerType((clang::PointerType *)type, &result);
            break;
        case clang::Type::ConstantArray:
            ret_status = VisitConstantArrayType((clang::ConstantArrayType *)type, &result);
            break;
       case clang::Type::IncompleteArray:
            ret_status = VisitIncompleteArrayType((clang::IncompleteArrayType *)type, &result);
            break;
        case clang::Type::Paren:
            ret_status = VisitParenType((clang::ParenType *)type, &result);
            break;
        case clang::Type::FunctionProto:
            ret_status = VisitFunctionProtoType((clang::FunctionProtoType *)type, &result);
            break;
        case clang::Type::FunctionNoProto:
            ret_status = VisitFunctionNoProtoType((clang::FunctionNoProtoType *)type, &result);
            break;
        case clang::Type::Elaborated:
            ret_status = VisitElaboratedType((clang::ElaboratedType *)type, &result);
            break;
        case clang::Type::Record:
            ret_status = VisitRecordType((clang::RecordType *)type, &result);
            break;
        case clang::Type::Enum:
            ret_status = VisitEnumType((clang::EnumType *)type, &result);
            break;
        case clang::Type::Typedef:
            ret_status = VisitTypedefType((clang::TypedefType *)type, &result);
            break;
        case clang::Type::ExtVector:
        case clang::Type::Vector:
            ret_status = VisitVectorType((clang::VectorType *)type, &result);
            break;
        // TODO cases
        default:
            std::cerr << "Unknown type kind " << type->getTypeClassName() << " !" << std::endl;
            ROSE_ASSERT(false);
    }

    ROSE_ASSERT(result != NULL);

    p_type_translation_map.insert(std::pair<const clang::Type *, SgNode *>(type, result));

    return result;
}

SgNode * ClangToSageTranslator::TraverseForDeclContext(clang::DeclContext * decl_context) {
    return Traverse((clang::Decl*)decl_context);
}

/**********************/
/* Visit Declarations */
/**********************/

bool ClangToSageTranslator::VisitDecl(clang::Decl * decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitDecl" << std::endl;
#endif    
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the declaration..." << std::endl;
        return false;
    }

    if (!isSgGlobal(*node))
        applySourceRange(*node, decl->getSourceRange());

    // TODO attributes
/*
    std::cerr << "Attribute list for " << decl->getDeclKindName() << " (" << decl << "): ";
    clang::Decl::attr_iterator it;
    for (it = decl->attr_begin(); it != decl->attr_end(); it++) {
        std::cerr << (*it)->getKind() << ", ";
    }
    std::cerr << std::endl;

    if (clang::VarDecl::classof(decl)) {
        clang::VarDecl * var_decl = (clang::VarDecl *)decl;
        std::cerr << "Stoprage class for " << decl->getDeclKindName() << " (" << decl << "): " << var_decl->getStorageClass() << std::endl;
    }
*/
    return true;
}

bool ClangToSageTranslator::VisitRecordDecl(clang::RecordDecl * record_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitRecordDecl" << std::endl;
#endif

    // FIXME May have to check the symbol table first, because of out-of-order traversal of C++ classes (Could be done in CxxRecord class...)

    bool res = true;

    SgClassDeclaration * sg_class_decl = NULL;

  // Find previous declaration

    clang::RecordDecl * prev_record_decl = record_decl->getPreviousDeclaration();
    SgClassSymbol * sg_prev_class_sym = isSgClassSymbol(GetSymbolFromSymbolTable(prev_record_decl));
    SgClassDeclaration * sg_prev_class_decl = sg_prev_class_sym == NULL ? NULL : isSgClassDeclaration(sg_prev_class_sym->get_declaration());

    SgClassDeclaration * sg_first_class_decl = sg_prev_class_decl == NULL ? NULL : isSgClassDeclaration(sg_prev_class_decl->get_firstNondefiningDeclaration());
    SgClassDeclaration * sg_def_class_decl = sg_prev_class_decl == NULL ? NULL : isSgClassDeclaration(sg_prev_class_decl->get_definingDeclaration());

    ROSE_ASSERT(sg_first_class_decl != NULL || sg_def_class_decl == NULL);

    bool had_prev_decl = sg_first_class_decl != NULL;

  // Name

    SgName name(record_decl->getNameAsString());

  // Type of class

    SgClassDeclaration::class_types type_of_class;
    switch (record_decl->getTagKind()) {
        case clang::TTK_Struct:
            type_of_class = SgClassDeclaration::e_struct;
            break;
        case clang::TTK_Class:
            type_of_class = SgClassDeclaration::e_class;
            break;
        case clang::TTK_Union:
            type_of_class = SgClassDeclaration::e_union;
            break;
        default:
            std::cerr << "Runtime error: RecordDecl can only be a struct/class/union." << std::endl;
            res = false;
    }

  // Build declaration(s)

    sg_class_decl = new SgClassDeclaration(name, type_of_class, NULL, NULL);

    sg_class_decl->set_scope(SageBuilder::topScopeStack());
    sg_class_decl->set_parent(SageBuilder::topScopeStack());

    SgClassType * type = NULL;
    if (sg_first_class_decl != NULL) {
        type = sg_first_class_decl->get_type();
    }
    else {
        type = SgClassType::createType(sg_class_decl);
    }
    ROSE_ASSERT(type != NULL);
    sg_class_decl->set_type(type);

    if (record_decl->isAnonymousStructOrUnion()) sg_class_decl->set_isUnNamed(true);

    if (!had_prev_decl) {
        sg_first_class_decl = sg_class_decl;
        sg_first_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
        sg_first_class_decl->set_definingDeclaration(NULL);
        sg_first_class_decl->set_definition(NULL);
        sg_first_class_decl->setForward();
        if (!record_decl->field_empty()) {
            sg_def_class_decl = new SgClassDeclaration(name, type_of_class, type, NULL);
            sg_def_class_decl->set_scope(SageBuilder::topScopeStack());
            if (record_decl->isAnonymousStructOrUnion()) sg_def_class_decl->set_isUnNamed(true);
            sg_def_class_decl->set_parent(SageBuilder::topScopeStack());

            sg_class_decl = sg_def_class_decl; // we return thew defining decl

            sg_def_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
            sg_def_class_decl->set_definingDeclaration(sg_def_class_decl);

            sg_first_class_decl->set_definingDeclaration(sg_def_class_decl);
            setCompilerGeneratedFileInfo(sg_first_class_decl);
        }
    }
    else if (!record_decl->field_empty()) {
        if (sg_def_class_decl != NULL) {
            delete sg_class_decl;
            *node = sg_def_class_decl;
            return true;
        }
        sg_def_class_decl = sg_class_decl;
        sg_def_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
        sg_def_class_decl->set_definingDeclaration(sg_def_class_decl);
        sg_first_class_decl->set_definingDeclaration(sg_def_class_decl);
    }
    else // second (or more) non-defining declaration
        return false; // FIXME ROSE need only one non-defining declaration (SageBuilder don't let me build another one....)

  // Update symbol table

    if (!had_prev_decl) {
        SgScopeStatement * scope = SageBuilder::topScopeStack();
        SgClassSymbol * class_symbol = new SgClassSymbol(sg_first_class_decl);
        scope->insert_symbol(name, class_symbol);
    }

  // Build ClassDefinition

    if (!record_decl->field_empty()) {
        SgClassDefinition * sg_class_def = isSgClassDefinition(sg_def_class_decl->get_definition());
        if (sg_class_def == NULL) {
            sg_class_def = SageBuilder::buildClassDefinition_nfi(sg_def_class_decl);
        }
        sg_def_class_decl->set_definition(sg_class_def);

        ROSE_ASSERT(sg_class_def->get_symbol_table() != NULL);

        applySourceRange(sg_class_def, record_decl->getSourceRange());

        SageBuilder::pushScopeStack(sg_class_def);

        clang::RecordDecl::field_iterator it;
        for (it = record_decl->field_begin(); it != record_decl->field_end(); it++) {
            SgNode * tmp_field = Traverse(*it);
            SgDeclarationStatement * field_decl = isSgDeclarationStatement(tmp_field);
            ROSE_ASSERT(field_decl != NULL);
            sg_class_def->append_member(field_decl);
            field_decl->set_parent(sg_class_def);
        }

        SageBuilder::popScopeStack();
    }

    ROSE_ASSERT(sg_class_decl->get_definingDeclaration() == NULL || isSgClassDeclaration(sg_class_decl->get_definingDeclaration())->get_definition() != NULL);
    ROSE_ASSERT(sg_first_class_decl->get_definition() == NULL);
    ROSE_ASSERT(sg_def_class_decl == NULL || sg_def_class_decl->get_definition() != NULL);

    *node = sg_class_decl;

    return VisitDecl(record_decl, node) && res;
}

bool ClangToSageTranslator::VisitEnumDecl(clang::EnumDecl * enum_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitEnumDecl" << std::endl;
#endif
    bool res = true;

    SgName name(enum_decl->getNameAsString());


    clang::EnumDecl * prev_enum_decl = enum_decl->getPreviousDeclaration();
    SgEnumSymbol * sg_prev_enum_sym = isSgEnumSymbol(GetSymbolFromSymbolTable(prev_enum_decl));
    SgEnumDeclaration * sg_prev_enum_decl = sg_prev_enum_sym == NULL ? NULL : isSgEnumDeclaration(sg_prev_enum_sym->get_declaration());
    sg_prev_enum_decl = sg_prev_enum_decl == NULL ? NULL : isSgEnumDeclaration(sg_prev_enum_decl->get_definingDeclaration());

    SgEnumDeclaration * sg_enum_decl = SageBuilder::buildEnumDeclaration(name, SageBuilder::topScopeStack());
    *node = sg_enum_decl;

    if (sg_prev_enum_decl == NULL || sg_prev_enum_decl->get_enumerators().size() == 0) {
#if DEBUG_ENUM_DECL
      std::cerr << "ClangToSageTranslator::VisitEnumDecl parse enumerators "
                << "(prev_enum_decl " << prev_enum_decl
                << " | sg_prev_enum_decl = " << sg_prev_enum_decl
                << " | size = " << (sg_prev_enum_decl == NULL ? 0 : sg_prev_enum_decl->get_enumerators().size())
                << ")" << std::endl;
#endif
      clang::EnumDecl::enumerator_iterator it;
      for (it = enum_decl->enumerator_begin(); it != enum_decl->enumerator_end(); it++) {
          SgNode * tmp_enumerator = Traverse(*it);
          SgInitializedName * enumerator = isSgInitializedName(tmp_enumerator);

          ROSE_ASSERT(enumerator);

          enumerator->set_scope(SageBuilder::topScopeStack());
          sg_enum_decl->append_enumerator(enumerator);
      }
    }
    else {
      sg_enum_decl->set_definingDeclaration(sg_prev_enum_decl);
      sg_enum_decl->set_firstNondefiningDeclaration(sg_prev_enum_decl->get_firstNondefiningDeclaration());
    }

#if DEBUG_ENUM_DECL
    std::cerr << "ClangToSageTranslator::VisitEnumDecl: "
              << "create sg_enum_decl = " << sg_enum_decl
              << " with " <<  sg_enum_decl->get_enumerators().size() << " enumerators" << std::endl;
#endif

    return VisitDecl(enum_decl, node) && res;
}

bool ClangToSageTranslator::VisitTypedefDecl(clang::TypedefDecl * typedef_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitTypedefDecl" << std::endl;
#endif
    bool res = true;

    SgName name(typedef_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(typedef_decl->getUnderlyingType());

    SgTypedefDeclaration * sg_typedef_decl = SageBuilder::buildTypedefDeclaration_nfi(name, type, SageBuilder::topScopeStack());

    if (isSgClassType(type)) {
        std::map<SgClassType *, bool>::iterator bool_it = p_class_type_decl_first_see_in_type.find(isSgClassType(type));
        ROSE_ASSERT(bool_it != p_class_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            sg_typedef_decl->set_declaration(isSgNamedType(type)->get_declaration()->get_definingDeclaration());
            sg_typedef_decl->set_typedefBaseTypeContainsDefiningDeclaration(true);
        }
    }
    else if (isSgEnumType(type)) {
        std::map<SgEnumType *, bool>::iterator bool_it = p_enum_type_decl_first_see_in_type.find(isSgEnumType(type));
        ROSE_ASSERT(bool_it != p_enum_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            sg_typedef_decl->set_declaration(isSgEnumType(type)->get_declaration()->get_definingDeclaration());
            sg_typedef_decl->set_typedefBaseTypeContainsDefiningDeclaration(true);
        }
    }

    *node = sg_typedef_decl;

    return VisitDecl(typedef_decl, node) && res;
}

bool ClangToSageTranslator::VisitFieldDecl(clang::FieldDecl * field_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitFieldDecl" << std::endl;
#endif  
    bool res = true;
    
    SgName name(field_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(field_decl->getType());

    clang::Expr * init_expr = field_decl->getInClassInitializer();
    SgNode * tmp_init = Traverse(init_expr);
    SgExpression * expr = isSgExpression(tmp_init);
    // TODO expression list if aggregated initializer !
    if (tmp_init != NULL && expr == NULL) {
        std::cerr << "Runtime error: not a SgInitializer..." << std::endl;
        res = false;
    }
    SgInitializer * init = expr != NULL ? SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type()) : NULL;
    if (init != NULL)
        applySourceRange(init, init_expr->getSourceRange());

  // Cannot use 'SageBuilder::buildVariableDeclaration' because of anonymous field
    // *node = SageBuilder::buildVariableDeclaration(name, type, init, SageBuilder::topScopeStack());
  // Build it by hand...
    SgVariableDeclaration * var_decl = new SgVariableDeclaration(name, type, init);

    if (isSgClassType(type)) {
        std::map<SgClassType *, bool>::iterator bool_it = p_class_type_decl_first_see_in_type.find(isSgClassType(type));
        ROSE_ASSERT(bool_it != p_class_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            var_decl->set_baseTypeDefiningDeclaration(isSgNamedType(type)->get_declaration()->get_definingDeclaration());
            var_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
        }
    }
    else if (isSgEnumType(type)) {
        std::map<SgEnumType *, bool>::iterator bool_it = p_enum_type_decl_first_see_in_type.find(isSgEnumType(type));
        ROSE_ASSERT(bool_it != p_enum_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            var_decl->set_baseTypeDefiningDeclaration(isSgEnumType(type)->get_declaration()->get_definingDeclaration());
            var_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
        }
    }

    var_decl->set_firstNondefiningDeclaration(var_decl);
    var_decl->set_parent(SageBuilder::topScopeStack());

    ROSE_ASSERT(var_decl->get_variables().size() == 1);

    SgInitializedName * init_name = var_decl->get_variables()[0];
    ROSE_ASSERT(init_name != NULL);
    init_name->set_scope(SageBuilder::topScopeStack());

    applySourceRange(init_name, field_decl->getSourceRange());

    SgVariableDefinition * var_def = isSgVariableDefinition(init_name->get_declptr());
    ROSE_ASSERT(var_def != NULL);
    applySourceRange(var_def, field_decl->getSourceRange());

    SgVariableSymbol * var_symbol = new SgVariableSymbol(init_name);
    SageBuilder::topScopeStack()->insert_symbol(name, var_symbol);

    *node = var_decl;

    return VisitDecl(field_decl, node) && res; 
}

bool ClangToSageTranslator::VisitFunctionDecl(clang::FunctionDecl * function_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitFunctionDecl" << std::endl;
#endif
    bool res = true;

    // FIXME: There is something weird here when try to Traverse a function reference in a recursive function (when first Traverse is not complete)
    //        It seems that it tries to instantiate the decl inside the function...
    //        It may be faster to recode from scratch...
    //   If I am not wrong this have been fixed....

    SgName name(function_decl->getNameAsString());

    SgType * ret_type = buildTypeFromQualifiedType(function_decl->getResultType());

    SgFunctionParameterList * param_list = SageBuilder::buildFunctionParameterList_nfi();
      applySourceRange(param_list, function_decl->getSourceRange()); // FIXME find the good SourceRange (should be stored by Clang...)

    for (unsigned i = 0; i < function_decl->getNumParams(); i++) {
        SgNode * tmp_init_name = Traverse(function_decl->getParamDecl(i));
        SgInitializedName * init_name = isSgInitializedName(tmp_init_name);
        if (tmp_init_name != NULL && init_name == NULL) {
            std::cerr << "Runtime error: tmp_init_name != NULL && init_name == NULL" << std::endl;
            res = false;
            continue;
        }

        param_list->append_arg(init_name);
    }

    if (function_decl->isVariadic()) {
        SgName empty = "";
        SgType * ellipses_type = SgTypeEllipse::createType();
        param_list->append_arg(SageBuilder::buildInitializedName_nfi(empty, ellipses_type, NULL));
    }

    SgFunctionDeclaration * sg_function_decl;

    if (function_decl->isThisDeclarationADefinition()) {
        sg_function_decl = SageBuilder::buildDefiningFunctionDeclaration(name, ret_type, param_list, NULL);
        sg_function_decl->set_definingDeclaration(sg_function_decl);

        if (function_decl->isVariadic()) {
            sg_function_decl->hasEllipses();
        }

        if (!function_decl->hasBody()) {
            std::cerr << "Defining function declaration without body..." << std::endl;
            res = false;
        }
/*
        if (sg_function_decl->get_definition() != NULL) SageInterface::deleteAST(sg_function_decl->get_definition());

        SgFunctionDefinition * function_definition = new SgFunctionDefinition(sg_function_decl, NULL);

        SgInitializedNamePtrList & init_names = param_list->get_args();
        SgInitializedNamePtrList::iterator it;
        for (it = init_names.begin(); it != init_names.end(); it++) {
            (*it)->set_scope(function_definition);
            SgSymbolTable * st = function_definition->get_symbol_table();
            ROSE_ASSERT(st != NULL);
            SgVariableSymbol * tmp_sym  = new SgVariableSymbol(*it);
            st->insert((*it)->get_name(), tmp_sym);
        }
*/
        SgFunctionDefinition * function_definition = sg_function_decl->get_definition();

        if (sg_function_decl->get_definition()->get_body() != NULL)
            SageInterface::deleteAST(sg_function_decl->get_definition()->get_body());

        SageBuilder::pushScopeStack(function_definition);

        SgNode * tmp_body = Traverse(function_decl->getBody());
        SgBasicBlock * body = isSgBasicBlock(tmp_body);

        SageBuilder::popScopeStack();

        if (tmp_body != NULL && body == NULL) {
            std::cerr << "Runtime error: tmp_body != NULL && body == NULL" << std::endl;
            res = false;
        }
        else {
            function_definition->set_body(body);
            body->set_parent(function_definition);
            applySourceRange(function_definition, function_decl->getSourceRange());
        }

        sg_function_decl->set_definition(function_definition);
        function_definition->set_parent(sg_function_decl);

        SgFunctionDeclaration * first_decl;
        if (function_decl->getFirstDeclaration() == function_decl) {
            SgFunctionParameterList * param_list_ = SageBuilder::buildFunctionParameterList_nfi();
              setCompilerGeneratedFileInfo(param_list_);
            SgInitializedNamePtrList & init_names = param_list->get_args();
            SgInitializedNamePtrList::iterator it;
            for (it = init_names.begin(); it != init_names.end(); it++) {
                SgInitializedName * init_param = new SgInitializedName(**it);
                setCompilerGeneratedFileInfo(init_param);
                param_list_->append_arg(init_param);
            }

            first_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, ret_type, param_list_, NULL);
            setCompilerGeneratedFileInfo(first_decl);
            first_decl->set_parent(SageBuilder::topScopeStack());
            first_decl->set_firstNondefiningDeclaration(first_decl);
            if (function_decl->isVariadic()) first_decl->hasEllipses();
        }
        else {
            SgSymbol * tmp_symbol = GetSymbolFromSymbolTable(function_decl->getFirstDeclaration());
            SgFunctionSymbol * symbol = isSgFunctionSymbol(tmp_symbol);
            if (tmp_symbol != NULL && symbol == NULL) {
                std::cerr << "Runtime error: tmp_symbol != NULL && symbol == NULL" << std::endl;
                res = false;
            }
            if (symbol != NULL)
                first_decl = isSgFunctionDeclaration(symbol->get_declaration());
        }

        sg_function_decl->set_firstNondefiningDeclaration(first_decl);
        first_decl->set_definingDeclaration(sg_function_decl);
    }
    else {
        sg_function_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, ret_type, param_list, NULL);

        if (function_decl->isVariadic()) sg_function_decl->hasEllipses();

        SgInitializedNamePtrList & init_names = param_list->get_args();
        SgInitializedNamePtrList::iterator it;
        for (it = init_names.begin(); it != init_names.end(); it++) {
             (*it)->set_scope(SageBuilder::topScopeStack());
        }

        if (function_decl->getFirstDeclaration() != function_decl) {
            SgSymbol * tmp_symbol = GetSymbolFromSymbolTable(function_decl->getFirstDeclaration());
            SgFunctionSymbol * symbol = isSgFunctionSymbol(tmp_symbol);
            if (tmp_symbol != NULL && symbol == NULL) {
                std::cerr << "Runtime error: tmp_symbol != NULL && symbol == NULL" << std::endl;
                res = false;
            }
            SgFunctionDeclaration * first_decl = NULL;
            if (symbol != NULL) {
                first_decl = isSgFunctionDeclaration(symbol->get_declaration());
            }
            else {
                // FIXME Is it correct?
                SgNode * tmp_first_decl = Traverse(function_decl->getFirstDeclaration());
                first_decl = isSgFunctionDeclaration(tmp_first_decl);
                ROSE_ASSERT(first_decl != NULL);
                // ROSE_ASSERT(!"We should have see the first declaration already");
            }

            if (first_decl != NULL) {
                if (first_decl->get_firstNondefiningDeclaration() != NULL)
                    sg_function_decl->set_firstNondefiningDeclaration(first_decl->get_firstNondefiningDeclaration());
                else {
                    ROSE_ASSERT(first_decl->get_firstNondefiningDeclaration() != NULL);
                }
            }
            else {
                ROSE_ASSERT(!"First declaration not found!");
            }
        }
        else {
            sg_function_decl->set_firstNondefiningDeclaration(sg_function_decl);
        }
    }

    ROSE_ASSERT(sg_function_decl->get_firstNondefiningDeclaration() != NULL);
/* // TODO Fix problem with function symbols...
    SgSymbol * symbol = GetSymbolFromSymbolTable(function_decl);
    if (symbol == NULL) {
        SgFunctionSymbol * func_sym = new SgFunctionSymbol(isSgFunctionDeclaration(sg_function_decl->get_firstNondefiningDeclaration()));
        SageBuilder::topScopeStack()->insert_symbol(name, func_sym);        
    }
*/
//  ROSE_ASSERT(GetSymbolFromSymbolTable(function_decl) != NULL);

    *node = sg_function_decl;

    return VisitDecl(function_decl, node) && res;
}

bool ClangToSageTranslator::VisitVarDecl(clang::VarDecl * var_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitVarDecl" << std::endl;
#endif
     bool res = true;

  // Create the SAGE node: SgVariableDeclaration

    SgName name(var_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(var_decl->getType());

    clang::Expr * init_expr = var_decl->getInit();
    SgNode * tmp_init = Traverse(init_expr);
    SgExpression * expr = isSgExpression(tmp_init);
    if (tmp_init != NULL && expr == NULL) {
        std::cerr << "Runtime error: not a SgInitializer..." << std::endl; // TODO
        res = false;
    }
    SgExprListExp * expr_list_expr = isSgExprListExp(expr);

    SgInitializer * init = NULL;
    if (expr_list_expr != NULL)
        init = SageBuilder::buildAggregateInitializer(expr_list_expr, type);
    else if (expr != NULL)
        init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
    if (init != NULL)
        applySourceRange(init, init_expr->getSourceRange());

    SgVariableDeclaration * sg_var_decl = new SgVariableDeclaration(name, type, init); // scope: obtain from the scope stack.

    if (isSgClassType(type)) {
        std::map<SgClassType *, bool>::iterator bool_it = p_class_type_decl_first_see_in_type.find(isSgClassType(type));
        ROSE_ASSERT(bool_it != p_class_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            sg_var_decl->set_baseTypeDefiningDeclaration(isSgNamedType(type)->get_declaration()->get_definingDeclaration());
            sg_var_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
        }
    }
    else if (isSgEnumType(type)) {
        std::map<SgEnumType *, bool>::iterator bool_it = p_enum_type_decl_first_see_in_type.find(isSgEnumType(type));
        ROSE_ASSERT(bool_it != p_enum_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            sg_var_decl->set_baseTypeDefiningDeclaration(isSgEnumType(type)->get_declaration()->get_definingDeclaration());
            sg_var_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
        }
    }

    sg_var_decl->set_firstNondefiningDeclaration(sg_var_decl);
    sg_var_decl->set_parent(SageBuilder::topScopeStack());

    ROSE_ASSERT(sg_var_decl->get_variables().size() == 1);

    SgInitializedName * init_name = sg_var_decl->get_variables()[0];
    ROSE_ASSERT(init_name != NULL);
    init_name->set_scope(SageBuilder::topScopeStack());

    applySourceRange(init_name, var_decl->getSourceRange());

    SgVariableDefinition * var_def = isSgVariableDefinition(init_name->get_declptr());
    ROSE_ASSERT(var_def != NULL);
    applySourceRange(var_def, var_decl->getSourceRange());

    SgVariableSymbol * var_symbol = new SgVariableSymbol(init_name);
    SageBuilder::topScopeStack()->insert_symbol(name, var_symbol);

    *node = sg_var_decl;

    return VisitDecl(var_decl, node) && res;

}

bool ClangToSageTranslator::VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitParmVarDecl" << std::endl;
#endif
    bool res = true;

    SgName name(param_var_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(param_var_decl->getOriginalType());

    SgInitializer * init = NULL;

    if (param_var_decl->hasDefaultArg()) {
        SgNode * tmp_expr = Traverse(param_var_decl->getDefaultArg());
        SgExpression * expr = isSgExpression(tmp_expr);
        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
        }
        else {
            applySourceRange(expr, param_var_decl->getDefaultArgRange());
            init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
            applySourceRange(init, param_var_decl->getDefaultArgRange());
        }
    }

    *node = SageBuilder::buildInitializedName(name, type, init);

    return VisitDecl(param_var_decl, node) && res;
}

bool  ClangToSageTranslator::VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitEnumConstantDecl" << std::endl;
#endif
    bool res = true;

    SgName name(enum_constant_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(enum_constant_decl->getType());

    SgInitializer * init = NULL;

    if (enum_constant_decl->getInitExpr() != NULL) {
        SgNode * tmp_expr = Traverse(enum_constant_decl->getInitExpr());
        SgExpression * expr = isSgExpression(tmp_expr);
        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
        }
        else {
            init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
        }
    }

    SgInitializedName * init_name = SageBuilder::buildInitializedName(name, type, init);

    SgEnumFieldSymbol * symbol = new SgEnumFieldSymbol(init_name);

    SageBuilder::topScopeStack()->insert_symbol(name, symbol);

    *node = init_name;

    return VisitDecl(enum_constant_decl, node) && res;
}

bool ClangToSageTranslator::VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitTranslationUnitDecl" << std::endl;
#endif
    if (*node != NULL) {
        std::cerr << "Runtime error: The TranslationUnitDecl is already associated to a SAGE node." << std::endl;
        return false;
    }

  // Create the SAGE node: SgGlobal

    if (p_global_scope != NULL) {
        std::cerr << "Runtime error: Global Scope have already been set !" << std::endl;
        return false;
    }

    *node = p_global_scope = new SgGlobal();

    p_decl_translation_map.insert(std::pair<clang::Decl *, SgNode *>(translation_unit_decl, p_global_scope));

  // Traverse the children

    SageBuilder::pushScopeStack(*node);

    clang::DeclContext * decl_context = (clang::DeclContext *)translation_unit_decl; // useless but more clear

    bool res = true;
    clang::DeclContext::decl_iterator it;
    for (it = decl_context->decls_begin(); it != decl_context->decls_end(); it++) {
        if (*it == NULL) continue;
        SgNode * child = Traverse(*it);

        SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(child);
        if (decl_stmt == NULL && child != NULL) {
            std::cerr << "Runtime error: the node produce for a clang::Decl is not a SgDeclarationStatement !" << std::endl;
            std::cerr << "    class = " << child->class_name() << std::endl;
            res = false;
        }
        else if (child != NULL) {
            // FIXME This is a hack to avoid autonomous decl of unnamed type to being added to the global scope....
            SgClassDeclaration * class_decl = isSgClassDeclaration(child);
            if (class_decl != NULL && (class_decl->get_name() == "" || class_decl->get_isUnNamed())) continue;
            SgEnumDeclaration * enum_decl = isSgEnumDeclaration(child);
            if (enum_decl != NULL && (enum_decl->get_name() == "" || enum_decl->get_isUnNamed())) continue;

            p_global_scope->append_declaration(decl_stmt);
        }
    }

    SageBuilder::popScopeStack();

  // Traverse the class hierarchy

    return VisitDecl(translation_unit_decl, node) && res;
}

/********************/
/* Visit Statements */
/********************/

bool ClangToSageTranslator::VisitStmt(clang::Stmt * stmt, SgNode ** node) {
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the Statement..." << std::endl;
        return false;
    }

    // TODO Is there anything else todo?

    if (
        isSgLocatedNode(*node) != NULL &&
        (
            isSgLocatedNode(*node)->get_file_info() == NULL ||
            !(isSgLocatedNode(*node)->get_file_info()->isCompilerGenerated())
        )
    ) {
        applySourceRange(*node, stmt->getSourceRange());
    }

    return true;
}

bool ClangToSageTranslator::VisitBreakStmt(clang::BreakStmt * break_stmt, SgNode ** node) {
    *node = SageBuilder::buildBreakStmt();
    return VisitStmt(break_stmt, node);
}

bool ClangToSageTranslator::VisitCompoundStmt(clang::CompoundStmt * compound_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitCompoundStmt" << std::endl;
#endif

    bool res = true;

    SgBasicBlock * block = SageBuilder::buildBasicBlock();

    block->set_parent(SageBuilder::topScopeStack());

    SageBuilder::pushScopeStack(block);

    clang::CompoundStmt::body_iterator it;
    for (it = compound_stmt->body_begin(); it != compound_stmt->body_end(); it++) {
        SgNode * tmp_node = Traverse(*it);

#if DEBUG_VISIT_STMT
        if (tmp_node != NULL)
          std::cerr << "In VisitCompoundStmt : child is " << tmp_node->class_name() << std::endl;
        else
          std::cerr << "In VisitCompoundStmt : child is NULL" << std::endl;
#endif

        SgClassDeclaration * class_decl = isSgClassDeclaration(tmp_node);
        if (class_decl != NULL && (class_decl->get_name() == "" || class_decl->get_isUnNamed())) continue;
        SgEnumDeclaration * enum_decl = isSgEnumDeclaration(tmp_node);
        if (enum_decl != NULL && (enum_decl->get_name() == "" || enum_decl->get_isUnNamed())) continue;
#if DEBUG_VISIT_STMT
        else if (enum_decl != NULL)
          std::cerr << "enum_decl = " << enum_decl << " >> name: " << enum_decl->get_name() << std::endl;
#endif

        SgStatement * stmt  = isSgStatement(tmp_node);
        SgExpression * expr = isSgExpression(tmp_node);
        if (tmp_node != NULL && stmt == NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_node != NULL && stmt == NULL && expr == NULL" << std::endl;
            res = false;
        }
        else if (stmt != NULL) {
            block->append_statement(stmt);
        }
        else if (expr != NULL) {
            SgExprStatement * expr_stmt = SageBuilder::buildExprStatement(expr);
            block->append_statement(expr_stmt);
        }
    }

    SageBuilder::popScopeStack();

    *node = block;

    return VisitStmt(compound_stmt, node) && res;
}

bool ClangToSageTranslator::VisitContinueStmt(clang::ContinueStmt * continue_stmt, SgNode ** node) {
    *node = SageBuilder::buildContinueStmt();
    return VisitStmt(continue_stmt, node);
}

bool ClangToSageTranslator::VisitDeclStmt(clang::DeclStmt * decl_stmt, SgNode ** node) {
    bool res = true;

    if (decl_stmt->isSingleDecl()) {
        *node = Traverse(decl_stmt->getSingleDecl());
    }
    else {
        std::vector<SgNode *> tmp_decls;
        SgDeclarationStatement * decl;
        clang::DeclStmt::decl_iterator it;

        SgScopeStatement * scope = SageBuilder::topScopeStack();

        for (it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); it++)
            tmp_decls.push_back(Traverse(*it));
        for (unsigned i = 0; i < tmp_decls.size() - 1; i++) {
            decl = isSgDeclarationStatement(tmp_decls[i]);
            if (tmp_decls[i] != NULL && decl == NULL) {
                std::cerr << "Runtime error: tmp_decls[i] != NULL && decl == NULL" << std::endl;
                res = false;
                continue;
            }
            else {
              SgClassDeclaration * class_decl = isSgClassDeclaration(decl);
              if (class_decl != NULL && (class_decl->get_name() == "" || class_decl->get_isUnNamed())) continue;
              SgEnumDeclaration * enum_decl = isSgEnumDeclaration(decl);
              if (enum_decl != NULL && (enum_decl->get_name() == "" || enum_decl->get_isUnNamed())) continue;
            }
            scope->append_statement(decl);
            decl->set_parent(scope);
        }
        decl = isSgDeclarationStatement(tmp_decls[tmp_decls.size() - 1]);
        if (tmp_decls[tmp_decls.size() - 1] != NULL && decl == NULL) {
            std::cerr << "Runtime error: tmp_decls[tmp_decls.size() - 1] != NULL && decl == NULL" << std::endl;
            res = false;
        }
        *node = decl;
    }

    return res;
}

bool ClangToSageTranslator::VisitDoStmt(clang::DoStmt * do_stmt, SgNode ** node) {
    SgNode * tmp_cond = Traverse(do_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);

    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);

    ROSE_ASSERT(expr_stmt != NULL);

    SgDoWhileStmt * sg_do_stmt = SageBuilder::buildDoWhileStmt_nfi(expr_stmt, NULL);

    sg_do_stmt->set_condition(expr_stmt);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_do_stmt);

    SageBuilder::pushScopeStack(sg_do_stmt);

    SgNode * tmp_body = Traverse(do_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    SgExpression * expr = isSgExpression(tmp_body);
    if (expr != NULL) {
        body =  SageBuilder::buildExprStatement(expr);
        applySourceRange(body, do_stmt->getBody()->getSourceRange());
    }
    ROSE_ASSERT(body != NULL);

    body->set_parent(sg_do_stmt);

    SageBuilder::popScopeStack();

    sg_do_stmt->set_body(body);

    *node = sg_do_stmt;

    return VisitStmt(do_stmt, node); 
}

bool ClangToSageTranslator::VisitExpr(clang::Expr * expr, SgNode ** node) {
     // TODO Is there anything to be done? (maybe in relation with typing?)

     return VisitStmt(expr, node);
}

bool ClangToSageTranslator::VisitConditionalOperator(clang::ConditionalOperator * conditional_operator, SgNode ** node) {
    bool res = true;

    SgNode * tmp_cond  = Traverse(conditional_operator->getCond());
    SgExpression * cond_expr = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond_expr);
    SgNode * tmp_true  = Traverse(conditional_operator->getTrueExpr());
    SgExpression * true_expr = isSgExpression(tmp_true);
    ROSE_ASSERT(true_expr);
    SgNode * tmp_false = Traverse(conditional_operator->getFalseExpr());
    SgExpression * false_expr = isSgExpression(tmp_false);
    ROSE_ASSERT(false_expr);

    *node = SageBuilder::buildConditionalExp(cond_expr, true_expr, false_expr);

    return VisitExpr(conditional_operator, node) && res;
}

bool ClangToSageTranslator::VisitBinaryOperator(clang::BinaryOperator * binary_operator, SgNode ** node) {
    bool res = true;

    SgNode * tmp_lhs = Traverse(binary_operator->getLHS());
    SgExpression * lhs = isSgExpression(tmp_lhs);
    if (tmp_lhs != NULL && lhs == NULL) {
        std::cerr << "Runtime error: tmp_lhs != NULL && lhs == NULL" << std::endl;
        res = false;
    }

    SgNode * tmp_rhs = Traverse(binary_operator->getRHS());
    SgExpression * rhs = isSgExpression(tmp_rhs);
    if (tmp_rhs != NULL && rhs == NULL) {
        std::cerr << "Runtime error: tmp_rhs != NULL && rhs == NULL" << std::endl;
        res = false;
    }

    switch (binary_operator->getOpcode()) {
        case clang::BO_PtrMemD:   ROSE_ASSERT(!"clang::BO_PtrMemD:");//*node = SageBuilder::build(lhs, rhs); break;
        case clang::BO_PtrMemI:   ROSE_ASSERT(!"clang::BO_PtrMemI:");//*node = SageBuilder::build(lhs, rhs); break;
        case clang::BO_Mul:       *node = SageBuilder::buildMultiplyOp(lhs, rhs); break;
        case clang::BO_Div:       *node = SageBuilder::buildDivideOp(lhs, rhs); break;
        case clang::BO_Rem:       *node = SageBuilder::buildModOp(lhs, rhs); break;
        case clang::BO_Add:       *node = SageBuilder::buildAddOp(lhs, rhs); break;
        case clang::BO_Sub:       *node = SageBuilder::buildSubtractOp(lhs, rhs); break;
        case clang::BO_Shl:       *node = SageBuilder::buildLshiftOp(lhs, rhs); break;
        case clang::BO_Shr:       *node = SageBuilder::buildRshiftOp(lhs, rhs); break;
        case clang::BO_LT:        *node = SageBuilder::buildLessThanOp(lhs, rhs); break;
        case clang::BO_GT:        *node = SageBuilder::buildGreaterThanOp(lhs, rhs); break;
        case clang::BO_LE:        *node = SageBuilder::buildLessOrEqualOp(lhs, rhs); break;
        case clang::BO_GE:        *node = SageBuilder::buildGreaterOrEqualOp(lhs, rhs); break;
        case clang::BO_EQ:        *node = SageBuilder::buildEqualityOp(lhs, rhs); break;
        case clang::BO_NE:        *node = SageBuilder::buildNotEqualOp(lhs, rhs); break;
        case clang::BO_And:       *node = SageBuilder::buildBitAndOp(lhs, rhs); break;
        case clang::BO_Xor:       *node = SageBuilder::buildBitXorOp(lhs, rhs); break;
        case clang::BO_Or:        *node = SageBuilder::buildBitOrOp(lhs, rhs); break;
        case clang::BO_LAnd:      *node = SageBuilder::buildAndOp(lhs, rhs); break;
        case clang::BO_LOr:       *node = SageBuilder::buildOrOp(lhs, rhs); break;
        case clang::BO_Assign:    *node = SageBuilder::buildAssignOp(lhs, rhs); break;
        case clang::BO_MulAssign: *node = SageBuilder::buildMultAssignOp(lhs, rhs); break;
        case clang::BO_DivAssign: *node = SageBuilder::buildDivAssignOp(lhs, rhs); break;
        case clang::BO_RemAssign: *node = SageBuilder::buildModAssignOp(lhs, rhs); break;
        case clang::BO_AddAssign: *node = SageBuilder::buildPlusAssignOp(lhs, rhs); break;
        case clang::BO_SubAssign: *node = SageBuilder::buildMinusAssignOp(lhs, rhs); break;
        case clang::BO_ShlAssign: *node = SageBuilder::buildLshiftAssignOp(lhs, rhs); break;
        case clang::BO_ShrAssign: *node = SageBuilder::buildRshiftAssignOp(lhs, rhs); break;
        case clang::BO_AndAssign: *node = SageBuilder::buildAndAssignOp(lhs, rhs); break;
        case clang::BO_XorAssign: *node = SageBuilder::buildXorAssignOp(lhs, rhs); break;
        case clang::BO_OrAssign:  *node = SageBuilder::buildIorAssignOp(lhs, rhs); break;
        case clang::BO_Comma:     *node = SageBuilder::buildCommaOpExp(lhs, rhs); break;
        default:
            std::cerr << "Unknown opcode for binary operator: " << binary_operator->getOpcodeStr() << std::endl;
            res = false;
    }

    return VisitExpr(binary_operator, node) && res;
}

bool ClangToSageTranslator::VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr, SgNode ** node) {
    bool res = true;

    SgNode * tmp_base = Traverse(array_subscript_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);
    if (tmp_base != NULL && base == NULL) {
        std::cerr << "Runtime error: tmp_base != NULL && base == NULL" << std::endl;
        res = false;
    }

    SgNode * tmp_idx = Traverse(array_subscript_expr->getIdx());
    SgExpression * idx = isSgExpression(tmp_idx);
    if (tmp_idx != NULL && idx == NULL) {
        std::cerr << "Runtime error: tmp_idx != NULL && idx == NULL" << std::endl;
        res = false;
    }

    *node = SageBuilder::buildPntrArrRefExp(base, idx);

    return VisitExpr(array_subscript_expr, node) && res;
}

bool ClangToSageTranslator::VisitCallExpr(clang::CallExpr * call_expr, SgNode ** node) {
    bool res = true;

    SgNode * tmp_expr = Traverse(call_expr->getCallee());
    SgExpression * expr = isSgExpression(tmp_expr);
    if (tmp_expr != NULL && expr == NULL) {
        std::cerr << "Runtime error: tmp_expr != NULL && expr == NULLL" << std::endl;
        res = false;
    }

    SgExprListExp * param_list = SageBuilder::buildExprListExp_nfi();
        applySourceRange(param_list, call_expr->getSourceRange());

    clang::CallExpr::arg_iterator it;
    for (it = call_expr->arg_begin(); it != call_expr->arg_end(); ++it) {
        SgNode * tmp_expr = Traverse(*it);
        SgExpression * expr = isSgExpression(tmp_expr);
        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
            continue;
        }
        param_list->append_expression(expr);
    }

    *node = SageBuilder::buildFunctionCallExp_nfi(expr, param_list);

    return VisitExpr(call_expr, node) && res;
}

bool ClangToSageTranslator::VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast, SgNode ** node) {
    bool res = true;

    SgNode * tmp_expr = Traverse(c_style_cast->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);

    ROSE_ASSERT(expr);

    SgType * type = buildTypeFromQualifiedType(c_style_cast->getTypeAsWritten());

    *node = SageBuilder::buildCastExp(expr, type, SgCastExp::e_C_style_cast);

    return VisitExpr(c_style_cast, node) && res;
}

bool ClangToSageTranslator::VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr, SgNode ** node) {
    SgNode * tmp_expr = Traverse(implicit_cast_expr->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);
    
    ROSE_ASSERT(expr != NULL);
/*
    FIXME why not? I dont remember why i commented it... :)

    SgType * type = buildTypeFromQualifiedType(implicit_cast_expr->getType());
    SgCastExp * res = SageBuilder::buildCastExp(expr, type);
    setCompilerGeneratedFileInfo(res);

    *node = res;
*/

    *node = expr;

    return VisitExpr(implicit_cast_expr, node);
}

bool ClangToSageTranslator::VisitCharacterLiteral(clang::CharacterLiteral * character_literal, SgNode ** node) {
    *node = SageBuilder::buildCharVal(character_literal->getValue());

    return VisitExpr(character_literal, node);
}

bool ClangToSageTranslator::VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal, SgNode ** node) {
    SgNode * tmp_node = Traverse(compound_literal->getInitializer());
    SgExprListExp * expr = isSgExprListExp(tmp_node);

    ROSE_ASSERT(expr != NULL);

    SgType * type = buildTypeFromQualifiedType(compound_literal->getType());

    ROSE_ASSERT(type != NULL);

    *node = SageBuilder::buildCompoundInitializer_nfi(expr, type);

    return VisitExpr(compound_literal, node);
}

bool ClangToSageTranslator::VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr, SgNode ** node) {
    bool res = true;

    //SgNode * tmp_node = Traverse(decl_ref_expr->getDecl());
    // DONE: Do not use Traverse(...) as the declaration can not be complete (recursive functions)
    //       Instead use SymbolTable from ROSE as the symbol should be ready (cannot have a reference before the declaration)
    // FIXME: This fix will not work for C++ (methods/fields can be use before they are declared...)
    // FIXME: I feel like it could work now, we will see ....

    SgSymbol * sym = GetSymbolFromSymbolTable(decl_ref_expr->getDecl());

    if (sym == NULL) {
        SgNode * tmp_decl = Traverse(decl_ref_expr->getDecl());

        if (tmp_decl != NULL) {
            sym = GetSymbolFromSymbolTable(decl_ref_expr->getDecl());
        }
        // FIXME hack Traverse have added the symbol but we cannot find it (probably: problem with type and function lookup)
        if (sym == NULL && isSgFunctionDeclaration(tmp_decl) != NULL) {
            sym = new SgFunctionSymbol(isSgFunctionDeclaration(tmp_decl));
            sym->set_parent(tmp_decl);
        }
        
    }

    if (sym != NULL) { // Not else if it was NULL we have try to traverse it....
        SgVariableSymbol  * var_sym  = isSgVariableSymbol(sym);
        SgFunctionSymbol  * func_sym = isSgFunctionSymbol(sym);
        SgEnumFieldSymbol * enum_sym = isSgEnumFieldSymbol(sym);

        if (var_sym != NULL) {
            *node = SageBuilder::buildVarRefExp(var_sym);
        }
        else if (func_sym != NULL) {
            *node = SageBuilder::buildFunctionRefExp(func_sym);
        }
        else if (enum_sym != NULL) {
            SgEnumDeclaration * enum_decl = isSgEnumDeclaration(enum_sym->get_declaration()->get_parent());
            ROSE_ASSERT(enum_decl != NULL);
            SgName name = enum_sym->get_name();
            *node = SageBuilder::buildEnumVal_nfi(0, enum_decl, name);
        }
        else if (sym != NULL) {
            std::cerr << "Runtime error: Unknown type of symbol for a declaration reference." << std::endl;
            std::cerr << "    sym->class_name() = " << sym->class_name()  << std::endl;
            ROSE_ASSERT(false);
        }
    }
    else {
         std::cerr << "Runtime error: Cannot find the symbol for a declaration reference (even after trying to buil th declaration)" << std::endl;
         ROSE_ASSERT(false);
    }

    return VisitExpr(decl_ref_expr, node) && res;
}

bool ClangToSageTranslator::VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr, SgNode ** node) {
    SgInitializer * init = NULL;    
    {
        SgNode * tmp_expr = Traverse(designated_init_expr->getInit());
        SgExpression * expr = isSgExpression(tmp_expr);
        ROSE_ASSERT(expr != NULL);
        SgExprListExp * expr_list_exp = isSgExprListExp(expr);
        if (expr_list_exp != NULL) {
            // FIXME get the type right...
            init = SageBuilder::buildAggregateInitializer_nfi(expr_list_exp, NULL);
        }
        else {
            init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
        }
        ROSE_ASSERT(init != NULL);
        applySourceRange(init, designated_init_expr->getInit()->getSourceRange());
    }

    SgExprListExp * expr_list_exp = SageBuilder::buildExprListExp_nfi();
    clang::DesignatedInitExpr::designators_iterator it;
    for (it = designated_init_expr->designators_begin(); it != designated_init_expr->designators_end(); it++) {
        SgExpression * expr = NULL;
        if (it->isFieldDesignator()) {
            SgSymbol * symbol = GetSymbolFromSymbolTable(it->getField());
            SgVariableSymbol * var_sym = isSgVariableSymbol(symbol);
            ROSE_ASSERT(var_sym != NULL);
            expr = SageBuilder::buildVarRefExp_nfi(var_sym);
            applySourceRange(expr, it->getSourceRange());
        }
        else if (it->isArrayDesignator()) {
            SgNode * tmp_expr = Traverse(designated_init_expr->getArrayIndex(*it));
            expr = isSgExpression(tmp_expr);
            ROSE_ASSERT(expr != NULL);
        }
        else if (it->isArrayRangeDesignator()) {
            ROSE_ASSERT(!"I don't believe range designator initializer are supported by ROSE...");    
        }
        else ROSE_ASSERT(false);

        ROSE_ASSERT(expr != NULL);

        expr->set_parent(expr_list_exp);
        expr_list_exp->append_expression(expr);
    }

    applySourceRange(expr_list_exp, designated_init_expr->getDesignatorsSourceRange());

    SgDesignatedInitializer * design_init = new SgDesignatedInitializer(expr_list_exp, init);
    expr_list_exp->set_parent(design_init);
    init->set_parent(design_init);

    *node = design_init;

    return VisitExpr(designated_init_expr, node);
}

bool ClangToSageTranslator::VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr, SgNode ** node) {
    SgNode * tmp_base = Traverse(ext_vector_element_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);

    ROSE_ASSERT(base != NULL);

    SgType * type = buildTypeFromQualifiedType(ext_vector_element_expr->getType());

    clang::IdentifierInfo & ident_info = ext_vector_element_expr->getAccessor();
    std::string ident = ident_info.getName().str();

    SgScopeStatement * scope = SageBuilder::ScopeStack.front();
    SgGlobal * global = isSgGlobal(scope);
    ROSE_ASSERT(global != NULL);

  // Build Manually a SgVarRefExp to have the same Accessor (text version) TODO ExtVectorAccessor and ExtVectorType
    SgInitializedName * init_name = SageBuilder::buildInitializedName(ident, SageBuilder::buildVoidType(), NULL);
    setCompilerGeneratedFileInfo(init_name);
    init_name->set_scope(global);
    SgVariableSymbol * var_symbol = new SgVariableSymbol(init_name);
    SgVarRefExp * pseudo_field = new SgVarRefExp(var_symbol);
    setCompilerGeneratedFileInfo(pseudo_field, true);
    init_name->set_parent(pseudo_field);

    SgExpression * res = NULL;
    if (ext_vector_element_expr->isArrow())
        res = SageBuilder::buildArrowExp(base, pseudo_field);
    else
        res = SageBuilder::buildDotExp(base, pseudo_field);

    ROSE_ASSERT(res != NULL);

    *node = res;

   return VisitExpr(ext_vector_element_expr, node);
}

bool ClangToSageTranslator::VisitFloatingLiteral(clang::FloatingLiteral * floating_literal, SgNode ** node) {
    unsigned int precision =  llvm::APFloat::semanticsPrecision(floating_literal->getValue().getSemantics());
    if (precision == 24)
        *node = SageBuilder::buildFloatVal(floating_literal->getValue().convertToFloat());
    else if (precision == 53)
        *node = SageBuilder::buildDoubleVal(floating_literal->getValue().convertToDouble());
    else
        ROSE_ASSERT(!"In VisitFloatingLiteral: Unsupported float size");

    return VisitExpr(floating_literal, node);
}

bool ClangToSageTranslator::VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal, SgNode ** node) {
    SgNode * tmp_imag_val = Traverse(imaginary_literal->getSubExpr());
    SgValueExp * imag_val = isSgValueExp(tmp_imag_val);
    ROSE_ASSERT(imag_val != NULL);

    SgComplexVal * comp_val = new SgComplexVal(NULL, imag_val, imag_val->get_type(), "");

    *node = comp_val;

    return VisitExpr(imaginary_literal, node);
}

bool ClangToSageTranslator::VisitInitListExpr(clang::InitListExpr * init_list_expr, SgNode ** node) {
    // We use the syntactic version of the initializer if it exists
    if (init_list_expr->getSyntacticForm() != NULL) return VisitInitListExpr(init_list_expr->getSyntacticForm(), node);

    SgExprListExp * expr_list_expr = SageBuilder::buildExprListExp_nfi();

    clang::InitListExpr::iterator it;
    for (it = init_list_expr->begin(); it != init_list_expr->end(); it++) {
        SgNode * tmp_expr = Traverse(*it);
        SgExpression * expr = isSgExpression(tmp_expr);
        ROSE_ASSERT(expr != NULL);
        expr_list_expr->append_expression(expr);
    }

    *node = expr_list_expr;

    return VisitExpr(init_list_expr, node);
}

bool ClangToSageTranslator::VisitIntegerLiteral(clang::IntegerLiteral * integer_literal, SgNode ** node) {
    *node = SageBuilder::buildIntVal(integer_literal->getValue().getSExtValue());

    return VisitExpr(integer_literal, node);
}

bool ClangToSageTranslator::VisitMemberExpr(clang::MemberExpr * member_expr, SgNode ** node) {
    bool res = true;

    SgNode * tmp_base = Traverse(member_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);
    ROSE_ASSERT(base != NULL);

    SgSymbol * sym = GetSymbolFromSymbolTable(member_expr->getMemberDecl());

    SgVariableSymbol * var_sym  = isSgVariableSymbol(sym);
    SgMemberFunctionSymbol * func_sym = isSgMemberFunctionSymbol(sym);

    SgExpression * sg_member_expr = NULL;

    bool successful_cast = var_sym || func_sym;
    if (sym != NULL && !successful_cast) {
        std::cerr << "Runtime error: Unknown type of symbol for a member reference." << std::endl;
        std::cerr << "    sym->class_name() = " << sym->class_name()  << std::endl;
        res = false;
    }
    else if (var_sym != NULL) {
        sg_member_expr = SageBuilder::buildVarRefExp(var_sym);
    }
    else if (func_sym != NULL) { // C++
        sg_member_expr = SageBuilder::buildMemberFunctionRefExp_nfi(func_sym, false, false); // FIXME 2nd and 3rd params ?
    }

    ROSE_ASSERT(sg_member_expr != NULL);

    // TODO (C++) member_expr->getQualifier() : for 'a->Base::foo'

    if (member_expr->isArrow())
        *node = SageBuilder::buildArrowExp(base, sg_member_expr);
    else
        *node = SageBuilder::buildDotExp(base, sg_member_expr);

    return VisitExpr(member_expr, node) && res;
}

bool ClangToSageTranslator::VisitParenExpr(clang::ParenExpr * paren_expr, SgNode ** node) {
    bool res = true;

    SgNode * tmp_subexpr = Traverse(paren_expr->getSubExpr());
    SgExpression * subexpr = isSgExpression(tmp_subexpr);
    if (tmp_subexpr != NULL && subexpr == NULL) {
        std::cerr << "Runtime error: tmp_subexpr != NULL && subexpr == NULL" << std::endl;
        res = false;
    }

    // bypass ParenExpr, their is nothing equivalent in SageIII
    *node = subexpr;

    return VisitExpr(paren_expr, node) && res;
}

bool ClangToSageTranslator::VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr, SgNode ** node) {
    // FIXME It's get tricky here: PredefinedExpr represent compiler generateed variables
    //    I choose to attach those variables on demand in the function definition scope 

  // Traverse the scope's stack to find the last function definition:

    SgFunctionDefinition * func_def = NULL;
    std::list<SgScopeStatement *>::reverse_iterator it = SageBuilder::ScopeStack.rbegin();
    while (it != SageBuilder::ScopeStack.rend() && func_def == NULL) {
        func_def = isSgFunctionDefinition(*it);
        it++;
    }
    ROSE_ASSERT(func_def != NULL);

  // Determine the name of the variable

    SgName name;

    switch (predefined_expr->getIdentType()) {
        case clang::PredefinedExpr::Func:
            name = "__func__";
            break;
        case clang::PredefinedExpr::Function:
            name = "__FUNCTION__";
            break;
        case clang::PredefinedExpr::PrettyFunction:
            name = "__PRETTY_FUNCTION__";
            break;
        case clang::PredefinedExpr::PrettyFunctionNoVirtual:
            ROSE_ASSERT(false);
            break;
    }

  // Retrieve the associate symbol if it exists

    SgVariableSymbol * symbol = func_def->lookup_variable_symbol(name);

  // Else, build a compiler generated initialized name for this variable in the function defintion scope.

    if (symbol == NULL) {
        SgInitializedName * init_name = SageBuilder::buildInitializedName_nfi(name, SageBuilder::buildPointerType(SageBuilder::buildCharType()), NULL);

        init_name->set_parent(func_def);
        init_name->set_scope(func_def);

        Sg_File_Info * start_fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        start_fi->setCompilerGenerated();
        init_name->set_startOfConstruct(start_fi);

        Sg_File_Info * end_fi   = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        end_fi->setCompilerGenerated();
        init_name->set_endOfConstruct(end_fi);

        symbol = new SgVariableSymbol(init_name);

        func_def->insert_symbol(name, symbol);
    }
    ROSE_ASSERT(symbol != NULL);

  // Finally build the variable reference

    *node = SageBuilder::buildVarRefExp_nfi(symbol);

    return true;
}

bool ClangToSageTranslator::VisitStringLiteral(clang::StringLiteral * string_literal, SgNode ** node) {
    std::string tmp = string_literal->getString().str();
    const char * raw_str = tmp.c_str();

    unsigned i = 0;
    unsigned l = 0;
    while (raw_str[i] != '\0') {
        if (
            raw_str[i] == '\\' ||
            raw_str[i] == '\n' ||
            raw_str[i] == '\r' ||
            raw_str[i] == '"')
        {
            l++;
        }
        l++;
        i++;
    }
    l++;

    char * str = (char *)malloc(l * sizeof(char));
    i = 0;
    unsigned cnt = 0;

    while (raw_str[i] != '\0') {
        switch (raw_str[i]) {
            case '\\':
                str[cnt++] = '\\';
                str[cnt++] = '\\';
                break;
            case '\n':
                str[cnt++] = '\\';
                str[cnt++] = 'n';
                break;
            case '\r':
                str[cnt++] = '\\';
                str[cnt++] = 'r';
                break;
            case '"':
                str[cnt++] = '\\';
                str[cnt++] = '"';
                break;
            default:
                str[cnt++] = raw_str[i];
        }
        i++;
    }
    str[cnt] = '\0';

    ROSE_ASSERT(l==cnt+1);

    *node = SageBuilder::buildStringVal(str);

    return VisitExpr(string_literal, node);
}

bool ClangToSageTranslator::VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr, SgNode ** node) {
    bool res = true;

    SgExpression * expr = NULL;
    SgType * type = NULL;

    if (unary_expr_or_type_trait_expr->isArgumentType()) {
        type = buildTypeFromQualifiedType(unary_expr_or_type_trait_expr->getArgumentType());
    }
    else {
        SgNode * tmp_expr = Traverse(unary_expr_or_type_trait_expr->getArgumentExpr());
        expr = isSgExpression(tmp_expr);

        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
        }
    }

    switch (unary_expr_or_type_trait_expr->getKind()) {
        case clang::UETT_SizeOf:
            if (type != NULL) *node = SageBuilder::buildSizeOfOp_nfi(type);
            else if (expr != NULL) *node = SageBuilder::buildSizeOfOp_nfi(expr);
            else res = false;
            break;
        case clang::UETT_AlignOf:
            ROSE_ASSERT(!"C/C++  - AlignOf is not supported!");
        case clang::UETT_VecStep:
            ROSE_ASSERT(!"OpenCL - VecStep is not supported!");
    }

    return VisitStmt(unary_expr_or_type_trait_expr, node) && res;
}

bool ClangToSageTranslator::VisitUnaryOperator(clang::UnaryOperator * unary_operator, SgNode ** node) {
    bool res = true;

    SgNode * tmp_subexpr = Traverse(unary_operator->getSubExpr());
    SgExpression * subexpr = isSgExpression(tmp_subexpr);
    if (tmp_subexpr != NULL && subexpr == NULL) {
        std::cerr << "Runtime error: tmp_subexpr != NULL && subexpr == NULL" << std::endl;
        res = false;
    }

    switch (unary_operator->getOpcode()) {
        case clang::UO_PostInc:
            *node = SageBuilder::buildPlusPlusOp(subexpr, SgUnaryOp::postfix);
            break;
        case clang::UO_PostDec:
            *node = SageBuilder::buildMinusMinusOp(subexpr, SgUnaryOp::postfix);
            break;
        case clang::UO_PreInc:
            *node = SageBuilder::buildPlusPlusOp(subexpr, SgUnaryOp::prefix);
            break;
        case clang::UO_PreDec:
            *node = SageBuilder::buildMinusMinusOp(subexpr, SgUnaryOp::prefix);
            break;
        case clang::UO_AddrOf:
            *node = SageBuilder::buildAddressOfOp(subexpr);
            break;
        case clang::UO_Deref:
            *node = SageBuilder::buildPointerDerefExp(subexpr);
            break;
        case clang::UO_Plus:
            *node = SageBuilder::buildUnaryAddOp(subexpr);
            break;
        case clang::UO_Minus:
            *node = SageBuilder::buildMinusOp(subexpr);
            break;
        case clang::UO_Not:
            *node = SageBuilder::buildNotOp(subexpr);
            break;
        case clang::UO_LNot:
            *node = SageBuilder::buildBitComplementOp(subexpr);
            break;
        case clang::UO_Real:
            *node = SageBuilder::buildImagPartOp(subexpr);
            break;
        case clang::UO_Imag:
            *node = SageBuilder::buildRealPartOp(subexpr);
            break;
        case clang::UO_Extension:
            *node = subexpr;
            break;
        default:
            std::cerr << "Runtime error: Unknown unary operator." << std::endl;
            res = false;
    }

    return VisitExpr(unary_operator, node) && res;
}

bool ClangToSageTranslator::VisitVAArgExpr(clang::VAArgExpr * va_arg_expr, SgNode ** node) {
    SgNode * tmp_expr = Traverse(va_arg_expr->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);
    ROSE_ASSERT(expr != NULL);

    *node = SageBuilder::buildVarArgOp_nfi(expr, expr->get_type());

    return VisitExpr(va_arg_expr, node);
}

bool ClangToSageTranslator::VisitForStmt(clang::ForStmt * for_stmt, SgNode ** node) {
    bool res = true;

    SgForStatement * sg_for_stmt = SageBuilder::buildForStatement_nfi((SgForInitStatement *)NULL, NULL, NULL, NULL);

    SageBuilder::pushScopeStack(sg_for_stmt);

  // Initialization

    SgForInitStatement * for_init_stmt = NULL;

    {
        SgStatementPtrList for_init_stmt_list;
        SgNode * tmp_init = Traverse(for_stmt->getInit());
        SgStatement * init_stmt = isSgStatement(tmp_init);
        SgExpression * init_expr = isSgExpression(tmp_init);
        if (tmp_init != NULL && init_stmt == NULL && init_expr == NULL) {
            std::cerr << "Runtime error: tmp_init != NULL && init_stmt == NULL && init_expr == NULL (" << tmp_init->class_name() << ")" << std::endl;
            res = false;
        }
        else if (init_expr != NULL) {
            init_stmt = SageBuilder::buildExprStatement(init_expr);
            applySourceRange(init_stmt, for_stmt->getInit()->getSourceRange());
        }
        if (init_stmt != NULL)
            for_init_stmt_list.push_back(init_stmt);
        for_init_stmt = SageBuilder::buildForInitStatement_nfi(for_init_stmt_list);
        if (for_stmt->getInit() != NULL)
            applySourceRange(for_init_stmt, for_stmt->getInit()->getSourceRange());
        else
            setCompilerGeneratedFileInfo(for_init_stmt, true);
    }

  // Condition

    SgStatement * cond_stmt = NULL;

    {
        SgNode * tmp_cond = Traverse(for_stmt->getCond());
        SgExpression * cond = isSgExpression(tmp_cond);
        if (tmp_cond != NULL && cond == NULL) {
            std::cerr << "Runtime error: tmp_cond != NULL && cond == NULL" << std::endl;
            res = false;
        }
        if (cond != NULL) { 
            cond_stmt = SageBuilder::buildExprStatement(cond);
            applySourceRange(cond_stmt, for_stmt->getCond()->getSourceRange());
        }
        else {
            cond_stmt = SageBuilder::buildNullStatement_nfi();
            setCompilerGeneratedFileInfo(cond_stmt);
        }
    }

  // Increment

    SgExpression * inc = NULL;

    {
        SgNode * tmp_inc  = Traverse(for_stmt->getInc());
        inc = isSgExpression(tmp_inc);
        if (tmp_inc != NULL && inc == NULL) {
            std::cerr << "Runtime error: tmp_inc != NULL && inc == NULL" << std::endl;
            res = false;
        }
        if (inc == NULL) {
            inc = SageBuilder::buildNullExpression_nfi();
            setCompilerGeneratedFileInfo(inc);
        }
    }

  // Body

    SgStatement * body = NULL;

    {
        SgNode * tmp_body = Traverse(for_stmt->getBody());
        body = isSgStatement(tmp_body);
        if (body == NULL) {
            SgExpression * body_expr = isSgExpression(tmp_body);
            if (body_expr != NULL) {
                body = SageBuilder::buildExprStatement(body_expr);
                applySourceRange(body, for_stmt->getBody()->getSourceRange());
            }
        }
        if (tmp_body != NULL && body == NULL) {
            std::cerr << "Runtime error: tmp_body != NULL && body == NULL" << std::endl;
            res = false;
        }
        if (body == NULL) {
            body = SageBuilder::buildNullStatement_nfi();
            setCompilerGeneratedFileInfo(body);
        }
    }

    SageBuilder::popScopeStack();

  // Attach sub trees to the for statement

    for_init_stmt->set_parent(sg_for_stmt);
    if (sg_for_stmt->get_for_init_stmt() != NULL)
        SageInterface::deleteAST(sg_for_stmt->get_for_init_stmt());
    sg_for_stmt->set_for_init_stmt(for_init_stmt);

    if (cond_stmt != NULL) {
        cond_stmt->set_parent(sg_for_stmt);
        sg_for_stmt->set_test(cond_stmt);
    }

    if (inc != NULL) {
        inc->set_parent(sg_for_stmt);
        sg_for_stmt->set_increment(inc);
    }

    if (body != NULL) {
        body->set_parent(sg_for_stmt);
        sg_for_stmt->set_loop_body(body);
    }

    *node = sg_for_stmt;

    return VisitStmt(for_stmt, node) && res;
}

bool ClangToSageTranslator::VisitGotoStmt(clang::GotoStmt * goto_stmt, SgNode ** node) {
    bool res = true;
/*
    SgSymbol * tmp_sym = GetSymbolFromSymbolTable(goto_stmt->getLabel());
    SgLabelSymbol * sym = isSgLabelSymbol(tmp_sym);
    if (sym == NULL) {
        std::cerr << "Runtime error: Cannot find the symbol for the label: \"" << goto_stmt->getLabel()->getStmt()->getName() << "\"." << std::endl;
        res = false;
    }
    else {
        *node = SageBuilder::buildGotoStatement(sym->get_declaration());
    }
*/

    SgNode * tmp_label = Traverse(goto_stmt->getLabel()->getStmt());
    SgLabelStatement * label_stmt = isSgLabelStatement(tmp_label);
    if (label_stmt == NULL) {
        std::cerr << "Runtime Error: Cannot find the label: \"" << goto_stmt->getLabel()->getStmt()->getName() << "\"." << std::endl;
        res = false;
    }
    else {
        *node = SageBuilder::buildGotoStatement(label_stmt);
    }

    return VisitStmt(goto_stmt, node) && res;
}

bool ClangToSageTranslator::VisitIfStmt(clang::IfStmt * if_stmt, SgNode ** node) {
    bool res = true;

    // TODO if_stmt->getConditionVariable() appears when a variable is declared in the condition...

    *node = SageBuilder::buildIfStmt_nfi(NULL, NULL, NULL);

    SageBuilder::pushScopeStack(isSgScopeStatement(*node));

    SgNode * tmp_cond = Traverse(if_stmt->getCond());
    SgExpression * cond_expr = isSgExpression(tmp_cond);
    SgStatement * cond_stmt = SageBuilder::buildExprStatement(cond_expr);
    applySourceRange(cond_stmt, if_stmt->getCond()->getSourceRange());

    SgNode * tmp_then = Traverse(if_stmt->getThen());
    SgStatement * then_stmt = isSgStatement(tmp_then);
    if (then_stmt == NULL) {
        SgExpression * then_expr = isSgExpression(tmp_then);
        ROSE_ASSERT(then_expr != NULL);
        then_stmt = SageBuilder::buildExprStatement(then_expr);
    }
    applySourceRange(then_stmt, if_stmt->getThen()->getSourceRange());

    SgNode * tmp_else = Traverse(if_stmt->getElse());
    SgStatement * else_stmt = isSgStatement(tmp_else);
    if (else_stmt == NULL) {
        SgExpression * else_expr = isSgExpression(tmp_else);
        if (else_expr != NULL)
            else_stmt = SageBuilder::buildExprStatement(else_expr);
    }
    if (else_stmt != NULL) applySourceRange(else_stmt, if_stmt->getElse()->getSourceRange());

    SageBuilder::popScopeStack();

    cond_stmt->set_parent(*node);
    isSgIfStmt(*node)->set_conditional(cond_stmt);

    then_stmt->set_parent(*node);
    isSgIfStmt(*node)->set_true_body(then_stmt);
    if (else_stmt != NULL) {
      else_stmt->set_parent(*node);
      isSgIfStmt(*node)->set_false_body(else_stmt);
    }

    return VisitStmt(if_stmt, node) && res;
}

bool ClangToSageTranslator::VisitLabelStmt(clang::LabelStmt * label_stmt, SgNode ** node) {
    bool res = true;

    SgName name(label_stmt->getName());

    SgNode * tmp_sub_stmt = Traverse(label_stmt->getSubStmt());
    SgStatement * sg_sub_stmt = isSgStatement(tmp_sub_stmt);
    if (sg_sub_stmt == NULL) {
        SgExpression * sg_sub_expr = isSgExpression(tmp_sub_stmt);
        ROSE_ASSERT(sg_sub_expr != NULL);
        sg_sub_stmt = SageBuilder::buildExprStatement(sg_sub_expr);
    }

    ROSE_ASSERT(sg_sub_stmt != NULL);

    *node = SageBuilder::buildLabelStatement_nfi(name, sg_sub_stmt, SageBuilder::topScopeStack());

    SgLabelStatement * sg_label_stmt = isSgLabelStatement(*node);
    SgFunctionDefinition * label_scope = NULL;
    std::list<SgScopeStatement *>::reverse_iterator it = SageBuilder::ScopeStack.rbegin();
    while (it != SageBuilder::ScopeStack.rend() && label_scope == NULL) {
        label_scope = isSgFunctionDefinition(*it);
        it++;
    }
    if (label_scope == NULL) {
         std::cerr << "Runtime error: Cannot find a surrounding function definition for the label statement: \"" << name << "\"." << std::endl;
         res = false;
    }
    else {
        sg_label_stmt->set_scope(label_scope);
        SgLabelSymbol* label_sym = new SgLabelSymbol(sg_label_stmt);
        label_scope->insert_symbol(label_sym->get_name(), label_sym);
    }

    return VisitStmt(label_stmt, node) && res;
}

bool ClangToSageTranslator::VisitNullStmt(clang::NullStmt * null_stmt, SgNode ** node) {
    *node = SageBuilder::buildNullStatement_nfi();
    return VisitStmt(null_stmt, node);
}

bool ClangToSageTranslator::VisitReturnStmt(clang::ReturnStmt * return_stmt, SgNode ** node) {
    bool res = true;

    SgNode * tmp_expr = Traverse(return_stmt->getRetValue());
    SgExpression * expr = isSgExpression(tmp_expr);
    if (tmp_expr != NULL && expr == NULL) {
        std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
        res = false;
    }
    *node = SageBuilder::buildReturnStmt(expr);

    return VisitStmt(return_stmt, node) && res;
}

bool ClangToSageTranslator::VisitCaseStmt(clang::CaseStmt * case_stmt, SgNode ** node) {
    SgNode * tmp_stmt = Traverse(case_stmt->getSubStmt());
    SgStatement * stmt = isSgStatement(tmp_stmt);
    SgExpression * expr = isSgExpression(tmp_stmt);
    if (expr != NULL) {
        stmt = SageBuilder::buildExprStatement(expr);
        applySourceRange(stmt, case_stmt->getSubStmt()->getSourceRange());
    }
    ROSE_ASSERT(stmt != NULL);

    SgNode * tmp_lhs = Traverse(case_stmt->getLHS());
    SgExpression * lhs = isSgExpression(tmp_lhs);
    ROSE_ASSERT(lhs != NULL);

/*  FIXME GNU extension not-handled by ROSE
    SgNode * tmp_rhs = Traverse(case_stmt->getRHS());
    SgExpression * rhs = isSgExpression(tmp_rhs);
    ROSE_ASSERT(rhs != NULL);
*/
    ROSE_ASSERT(case_stmt->getRHS() == NULL);

    *node = SageBuilder::buildCaseOptionStmt_nfi(lhs, stmt);

    return VisitStmt(case_stmt, node);
}

bool ClangToSageTranslator::VisitDefaultStmt(clang::DefaultStmt * default_stmt, SgNode ** node) {
    SgNode * tmp_stmt = Traverse(default_stmt->getSubStmt());
    SgStatement * stmt = isSgStatement(tmp_stmt);

    *node = SageBuilder::buildDefaultOptionStmt_nfi(stmt);

    return VisitStmt(default_stmt, node);
}

bool ClangToSageTranslator::VisitSwitchStmt(clang::SwitchStmt * switch_stmt, SgNode ** node) {
    SgNode * tmp_cond = Traverse(switch_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);
    
    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);
        applySourceRange(expr_stmt, switch_stmt->getCond()->getSourceRange());

    SgSwitchStatement * sg_switch_stmt = SageBuilder::buildSwitchStatement_nfi(expr_stmt, NULL);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_switch_stmt);

    SageBuilder::pushScopeStack(sg_switch_stmt);

    SgNode * tmp_body = Traverse(switch_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    ROSE_ASSERT(body != NULL);

    SageBuilder::popScopeStack();

    sg_switch_stmt->set_body(body);

    *node = sg_switch_stmt;

    return VisitStmt(switch_stmt, node);
}

bool ClangToSageTranslator::VisitWhileStmt(clang::WhileStmt * while_stmt, SgNode ** node) {
    SgNode * tmp_cond = Traverse(while_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);

    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);

    SgWhileStmt * sg_while_stmt = SageBuilder::buildWhileStmt_nfi(expr_stmt, NULL);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_while_stmt);

    SageBuilder::pushScopeStack(sg_while_stmt);

    SgNode * tmp_body = Traverse(while_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    SgExpression * expr = isSgExpression(tmp_body);
    if (expr != NULL) {
        body =  SageBuilder::buildExprStatement(expr);
        applySourceRange(body, while_stmt->getBody()->getSourceRange());
    }
    ROSE_ASSERT(body != NULL);

    body->set_parent(sg_while_stmt);

    SageBuilder::popScopeStack();

    sg_while_stmt->set_body(body);

    *node = sg_while_stmt;

    return VisitStmt(while_stmt, node);
}

/***************/
/* Visit Types */
/***************/

bool ClangToSageTranslator::VisitType(clang::Type * type, SgNode ** node) {
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the type..." << std::endl;
        return false;
    }
/*
    std::cerr << "Dump type " << type->getTypeClassName() << "(" << type << "): ";
    type->dump();
    std::cerr << std::endl;
*/
    // TODO

    return true;
}

bool ClangToSageTranslator::VisitConstantArrayType(clang::ConstantArrayType * constant_array_type, SgNode ** node) {
    SgType * type = buildTypeFromQualifiedType(constant_array_type->getElementType());

    // TODO clang::ArrayType::ArraySizeModifier

    SgExpression * expr = SageBuilder::buildIntVal(constant_array_type->getSize().getSExtValue());

    *node = SageBuilder::buildArrayType(type, expr);

    return VisitType(constant_array_type, node);
}

bool ClangToSageTranslator::VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type, SgNode ** node) {
    SgType * type = buildTypeFromQualifiedType(incomplete_array_type->getElementType());

    // TODO clang::ArrayType::ArraySizeModifier

    *node = SageBuilder::buildArrayType(type);

    return VisitType(incomplete_array_type, node);
}

bool ClangToSageTranslator::VisitAttributedType(clang::AttributedType * attributed_type, SgNode ** node) {
    SgType * type = buildTypeFromQualifiedType(attributed_type->getModifiedType());

    SgModifierType * modified_type = SgModifierType::createType(type);
    SgTypeModifier & sg_modifer = modified_type->get_typeModifier();

    switch (attributed_type->getAttrKind()) {
        case clang::AttributedType::attr_noreturn:             sg_modifer.setGnuAttributeNoReturn();      break;
        case clang::AttributedType::attr_cdecl:                sg_modifer.setGnuAttributeCdecl();         break;
        case clang::AttributedType::attr_stdcall:              sg_modifer.setGnuAttributeStdcall();       break;

        case clang::AttributedType::attr_address_space:
            std::cerr << "Unsupported attribute attr_address_space" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_regparm:
            std::cerr << "Unsupported attribute attr_regparm" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_vector_size:
            std::cerr << "Unsupported attribute attr_vector_size" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_neon_vector_type:
            std::cerr << "Unsupported attribute attr_neon_vector_type" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_neon_polyvector_type:
            std::cerr << "Unsupported attribute attr_neon_polyvector_type" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_objc_gc:
            std::cerr << "Unsupported attribute attr_objc_gc" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_objc_ownership:
            std::cerr << "Unsupported attribute attr_objc_ownership" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_pcs:
            std::cerr << "Unsupported attribute attr_pcs" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_fastcall:
            std::cerr << "Unsupported attribute attr_fastcall" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_thiscall:
            std::cerr << "Unsupported attribute attr_thiscall" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_pascal:
            std::cerr << "Unsupported attribute attr_pascal" << std::endl; ROSE_ASSERT(false);
        default:
            std::cerr << "Unknown attribute" << std::endl; ROSE_ASSERT(false);
    } 

    *node = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);;

    return VisitType(attributed_type, node);
}

bool ClangToSageTranslator::VisitBuiltinType(clang::BuiltinType * builtin_type, SgNode ** node) {
    switch (builtin_type->getKind()) {
        case clang::BuiltinType::Void:       *node = SageBuilder::buildVoidType();             break;
        case clang::BuiltinType::Bool:       *node = SageBuilder::buildBoolType();             break;
        case clang::BuiltinType::Short:      *node = SageBuilder::buildShortType();            break;
        case clang::BuiltinType::Int:        *node = SageBuilder::buildIntType();              break;
        case clang::BuiltinType::Long:       *node = SageBuilder::buildLongType();             break;
        case clang::BuiltinType::LongLong:   *node = SageBuilder::buildLongLongType();         break;
        case clang::BuiltinType::Float:      *node = SageBuilder::buildFloatType();            break;
        case clang::BuiltinType::Double:     *node = SageBuilder::buildDoubleType();           break;
        case clang::BuiltinType::LongDouble: *node = SageBuilder::buildLongDoubleType();       break;

        case clang::BuiltinType::Char_S:     *node = SageBuilder::buildCharType();             break;

        case clang::BuiltinType::UInt:       *node = SageBuilder::buildUnsignedIntType();      break;
        case clang::BuiltinType::UChar:      *node = SageBuilder::buildUnsignedCharType();     break;
        case clang::BuiltinType::SChar:      *node = SageBuilder::buildSignedCharType();       break;
        case clang::BuiltinType::UShort:     *node = SageBuilder::buildUnsignedShortType();    break;
        case clang::BuiltinType::ULong:      *node = SageBuilder::buildUnsignedLongType();     break;
        case clang::BuiltinType::ULongLong:  *node = SageBuilder::buildUnsignedLongLongType(); break;
/*
        case clang::BuiltinType::NullPtr:    *node = SageBuilder::build(); break;
*/
        // TODO ROSE type ?
        case clang::BuiltinType::UInt128:    *node = SageBuilder::buildUnsignedLongLongType(); break;
        case clang::BuiltinType::Int128:     *node = SageBuilder::buildLongLongType();         break;
 
        case clang::BuiltinType::Char_U:    std::cerr << "Char_U    -> "; break;
        case clang::BuiltinType::WChar_U:   std::cerr << "WChar_U   -> "; break;
        case clang::BuiltinType::Char16:    std::cerr << "Char16    -> "; break;
        case clang::BuiltinType::Char32:    std::cerr << "Char32    -> "; break;
        case clang::BuiltinType::WChar_S:   std::cerr << "WChar_S   -> "; break;


        case clang::BuiltinType::ObjCId:
        case clang::BuiltinType::ObjCClass:
        case clang::BuiltinType::ObjCSel:
        case clang::BuiltinType::Dependent:
        case clang::BuiltinType::Overload:
        case clang::BuiltinType::BoundMember:
        case clang::BuiltinType::UnknownAny:
        default:
            std::cerr << "Unknown builtin type: " << builtin_type->getName(p_compiler_instance->getLangOpts()) << " !" << std::endl;
            exit(-1);
    }

    return VisitType(builtin_type, node);
}

bool ClangToSageTranslator::VisitComplexType(clang::ComplexType * complex_type, SgNode ** node) {
    bool res = true;

    SgType * type = buildTypeFromQualifiedType(complex_type->getElementType());

    *node = SageBuilder::buildComplexType(type);

    return VisitType(complex_type, node) && res;
}

bool ClangToSageTranslator::VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type, SgNode ** node) {
    bool res = true;

    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();

    SgType * ret_type = buildTypeFromQualifiedType(function_no_proto_type->getResultType()); 

    *node = SageBuilder::buildFunctionType(ret_type, param_type_list);

    return VisitType(function_no_proto_type, node) && res;
}

bool ClangToSageTranslator::VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type, SgNode ** node) {
    bool res = true;
    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();
    for (unsigned i = 0; i < function_proto_type->getNumArgs(); i++) {
        SgType * param_type = buildTypeFromQualifiedType(function_proto_type->getArgType(i));

        param_type_list->append_argument(param_type);
    }

    if (function_proto_type->isVariadic()) {
        param_type_list->append_argument(SgTypeEllipse::createType());
    }

    SgType * ret_type = buildTypeFromQualifiedType(function_proto_type->getResultType());

    SgFunctionType * func_type = SageBuilder::buildFunctionType(ret_type, param_type_list);
    if (function_proto_type->isVariadic()) func_type->set_has_ellipses(1);

    *node = func_type;

    return VisitType(function_proto_type, node) && res;
}

bool ClangToSageTranslator::VisitParenType(clang::ParenType * paren_type, SgNode ** node) {
    *node = buildTypeFromQualifiedType(paren_type->getInnerType());

    return VisitType(paren_type, node);
}

bool ClangToSageTranslator::VisitPointerType(clang::PointerType * pointer_type, SgNode ** node) {
    SgType * type = buildTypeFromQualifiedType(pointer_type->getPointeeType());

    *node = SageBuilder::buildPointerType(type);

    return VisitType(pointer_type, node);
}

bool ClangToSageTranslator::VisitEnumType(clang::EnumType * enum_type, SgNode ** node) {
    SgSymbol * sym = GetSymbolFromSymbolTable(enum_type->getDecl());

    SgEnumSymbol * enum_sym = isSgEnumSymbol(sym);

    if (enum_sym == NULL) {
        SgNode * tmp_decl = Traverse(enum_type->getDecl());
        SgEnumDeclaration * sg_decl = isSgEnumDeclaration(tmp_decl);

        ROSE_ASSERT(sg_decl != NULL);

        *node = sg_decl->get_type();
    }
    else {
        *node = enum_sym->get_type();
    }

    if (isSgEnumType(*node) != NULL) {
        if (enum_sym == NULL) {
            p_enum_type_decl_first_see_in_type.insert(std::pair<SgEnumType *, bool>(isSgEnumType(*node), true));
        }
        else
            p_enum_type_decl_first_see_in_type.insert(std::pair<SgEnumType *, bool>(isSgEnumType(*node), false));
    }

    return VisitType(enum_type, node);
}

bool ClangToSageTranslator::VisitRecordType(clang::RecordType * record_type, SgNode ** node) {
    SgSymbol * sym = GetSymbolFromSymbolTable(record_type->getDecl());

    SgClassSymbol * class_sym = isSgClassSymbol(sym);

    if (class_sym == NULL) {
        SgNode * tmp_decl = Traverse(record_type->getDecl());
        SgClassDeclaration * sg_decl = isSgClassDeclaration(tmp_decl);

        ROSE_ASSERT(sg_decl != NULL);

        *node = sg_decl->get_type();
    }
    else {
        *node = class_sym->get_type();
    }

    if (isSgClassType(*node) != NULL) {
        if (class_sym == NULL) {
            p_class_type_decl_first_see_in_type.insert(std::pair<SgClassType *, bool>(isSgClassType(*node), true));
            isSgNamedType(*node)->set_autonomous_declaration(true);
        }
        else
            p_class_type_decl_first_see_in_type.insert(std::pair<SgClassType *, bool>(isSgClassType(*node), false));
    }

    return VisitType(record_type, node);
}

bool ClangToSageTranslator::VisitTypedefType(clang::TypedefType * typedef_type, SgNode ** node) {
    bool res = true;

    SgSymbol * sym = GetSymbolFromSymbolTable(typedef_type->getDecl());
    SgTypedefSymbol * tdef_sym = isSgTypedefSymbol(sym);

    if (tdef_sym == NULL) {
        std::cerr << "Runtime Error: Cannot find a typedef symbol for the TypedefType." << std::endl;
        res = false;
    }

    *node = tdef_sym->get_type();

   return VisitType(typedef_type, node) && res;
}

bool ClangToSageTranslator::VisitElaboratedType(clang::ElaboratedType * elaborated_type, SgNode ** node) {
    SgType * type = buildTypeFromQualifiedType(elaborated_type->getNamedType());

    // FIXME clang::ElaboratedType contains the "sugar" of a type reference (eg, "struct A" or "M::N::A"), it should be pass down to ROSE

    *node = type;

    return VisitType(elaborated_type, node);
}

bool ClangToSageTranslator::VisitVectorType(clang::VectorType * vector_type, SgNode ** node) {
    SgType * type = buildTypeFromQualifiedType(vector_type->getElementType());

    SgModifierType * modified_type = new SgModifierType(type);
    SgTypeModifier & sg_modifer = modified_type->get_typeModifier();

    sg_modifer.setVectorType();
    sg_modifer.set_vector_size(vector_type->getNumElements());

    *node = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);

    return VisitType(vector_type, node);
}

std::pair<Sg_File_Info *, PreprocessingInfo *> ClangToSageTranslator::preprocessor_top() {
    return p_sage_preprocessor_recorder->top();
}

bool ClangToSageTranslator::preprocessor_pop() {
    return p_sage_preprocessor_recorder->pop();
}

// struct NextPreprocessorToInsert

NextPreprocessorToInsert::NextPreprocessorToInsert(ClangToSageTranslator & translator_) :
  cursor(NULL),
  candidat(NULL),
  next_to_insert(NULL),
  translator(translator_)
{}

NextPreprocessorToInsert * NextPreprocessorToInsert::next() {
    if (!translator.preprocessor_pop()) return NULL;

    NextPreprocessorToInsert * res = new NextPreprocessorToInsert(translator);

    std::pair<Sg_File_Info *, PreprocessingInfo *> next = translator.preprocessor_top();
    res->cursor = next.first;
    res->next_to_insert = next.second;
    res->candidat = candidat;
}

// class

NextPreprocessorToInsert * PreprocessorInserter::evaluateInheritedAttribute(SgNode * astNode, NextPreprocessorToInsert * inheritedValue) {
    SgLocatedNode * loc_node = isSgLocatedNode(astNode);
    if (loc_node == NULL) return inheritedValue;

    Sg_File_Info * current_pos = loc_node->get_startOfConstruct();

    bool passed_cursor = *current_pos > *(inheritedValue->cursor);

    if (passed_cursor) {
        // TODO insert on inheritedValue->candidat
        return inheritedValue->next();
    }

    
}

// class SagePreprocessorRecord

SagePreprocessorRecord::SagePreprocessorRecord(clang::SourceManager * source_manager) :
  p_source_manager(source_manager),
  p_preprocessor_record_list()
{}

void SagePreprocessorRecord::InclusionDirective(clang::SourceLocation HashLoc, const clang::Token & IncludeTok, llvm::StringRef FileName, bool IsAngled,
                                                const clang::FileEntry * File, clang::SourceLocation EndLoc, llvm::StringRef SearchPath, llvm::StringRef RelativePath) {
    std::cerr << "InclusionDirective" << std::endl;

    bool inv_begin_line;
    bool inv_begin_col;

    unsigned ls = p_source_manager->getSpellingLineNumber(HashLoc, &inv_begin_line);
    unsigned cs = p_source_manager->getSpellingColumnNumber(HashLoc, &inv_begin_col);

    std::string file = p_source_manager->getFileEntryForID(p_source_manager->getFileID(HashLoc))->getName();

    std::cerr << "    In file  : " << file << std::endl;
    std::cerr << "    From     : " << ls << ":" << cs << std::endl;
    std::cerr << "    Included : " << FileName.str() << std::endl;
    std::cerr << "    Is angled: " << (IsAngled ? "T" : "F") << std::endl;

    Sg_File_Info * file_info = new Sg_File_Info(file, ls, cs);
    PreprocessingInfo * preproc_info = new PreprocessingInfo(
                                                  PreprocessingInfo::CpreprocessorIncludeDeclaration,
                                                  FileName.str(),
                                                  file,
                                                  ls,
                                                  cs,
                                                  0,
                                                  PreprocessingInfo::before
                                           );

    p_preprocessor_record_list.push_back(std::pair<Sg_File_Info *, PreprocessingInfo *>(file_info, preproc_info));
}

void SagePreprocessorRecord::EndOfMainFile() {
    std::cerr << "EndOfMainFile" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Ident(clang::SourceLocation Loc, const std::string & str) {
    std::cerr << "Ident" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaComment(clang::SourceLocation Loc, const clang::IdentifierInfo * Kind, const std::string & Str) {
    std::cerr << "PragmaComment" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaMessage(clang::SourceLocation Loc, llvm::StringRef Str) {
    std::cerr << "PragmaMessage" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaDiagnosticPush(clang::SourceLocation Loc, llvm::StringRef Namespace) {
    std::cerr << "PragmaDiagnosticPush" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaDiagnosticPop(clang::SourceLocation Loc, llvm::StringRef Namespace) {
    std::cerr << "PragmaDiagnosticPop" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaDiagnostic(clang::SourceLocation Loc, llvm::StringRef Namespace, clang::diag::Mapping mapping, llvm::StringRef Str) {
    std::cerr << "PragmaDiagnostic" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::MacroExpands(const clang::Token & MacroNameTok, const clang::MacroInfo * MI, clang::SourceRange Range) {
    std::cerr << "MacroExpands" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::MacroDefined(const clang::Token & MacroNameTok, const clang::MacroInfo * MI) {
    std::cerr << "" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::MacroUndefined(const clang::Token & MacroNameTok, const clang::MacroInfo * MI) {
    std::cerr << "MacroUndefined" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Defined(const clang::Token & MacroNameTok) {
    std::cerr << "Defined" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::SourceRangeSkipped(clang::SourceRange Range) {
    std::cerr << "SourceRangeSkipped" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::If(clang::SourceRange Range) {
    std::cerr << "If" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Elif(clang::SourceRange Range) {
    std::cerr << "Elif" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Ifdef(const clang::Token & MacroNameTok) {
    std::cerr << "Ifdef" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Ifndef(const clang::Token & MacroNameTok) {
    std::cerr << "Ifndef" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Else() {
    std::cerr << "Else" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Endif() {
    std::cerr << "Endif" << std::endl;
    ROSE_ASSERT(false);
}

std::pair<Sg_File_Info *, PreprocessingInfo *> SagePreprocessorRecord::top() {
    return p_preprocessor_record_list.front();
}

bool SagePreprocessorRecord::pop() {
    p_preprocessor_record_list.erase(p_preprocessor_record_list.begin());
    return !p_preprocessor_record_list.empty();
}

