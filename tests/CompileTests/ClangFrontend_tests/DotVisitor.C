
#include "DotVisitor.h"

#include "clang/Basic/FileManager.h"

DotVisitor::DotVisitor(const clang::SourceManager & src_mgr, const clang::LangOptions & lang_opts) :
    clang::RecursiveASTVisitor<DotVisitor>(),
    p_ast_nodes_map(),
    p_nodes_in_global_scope(),
    p_stmts_name(),
    p_decls_name(),
    p_types_name(),
    p_all_decls(),
    p_all_stmts(),
    p_all_types(),
    p_seen_nodes(),
    p_source_manager(src_mgr),
    p_lang_opts(lang_opts),
    p_empty_cnt(0),
    p_final_stage(false)
{}

DotVisitor::~DotVisitor() {}

bool DotVisitor::shouldVisitTemplateInstantiations() { return true ; }

void DotVisitor::toDot(std::ostream & out) {
    out << "digraph {" << std::endl;

    std::map<std::string, DotNode>::iterator it0;
    for (it0 = p_ast_nodes_map.begin(); it0 != p_ast_nodes_map.end(); it0++)
        it0->second.toDot(out, "\t");

#define SHOW_PSEUDO_GLOBAL_SCOPE 0
#if SHOW_PSEUDO_GLOBAL_SCOPE
    out << "\tglobal_scope_node [label=\"Global Scope\"];" << std::endl;

    std::set<std::string>::iterator it1;
    unsigned cnt = 0;
    for (it1 = p_nodes_in_global_scope.begin(); it1 != p_nodes_in_global_scope.end(); it1++) {
        out << "\t\t" << "global_scope_node -> " << *it1 << " [label=\"declaration[" << cnt << "]\", shape=\"box\"];" << std::endl;
        cnt++;
    }
#else /* SHOW_PSEUDO_GLOBAL_SCOPE */
    unsigned cnt;
#endif /* SHOW_PSEUDO_GLOBAL_SCOPE */

    out << std::endl;

    for (cnt = 0; cnt < p_empty_cnt; cnt++)
        out << "\t" << "empty_node_" << cnt << " [label=\"\", shape=\"diamond\"];" << std::endl;

    out << "}" << std::endl;
}

// Finalyse

void DotVisitor::finalyse() {
    p_final_stage = true;

    bool cont = true;
    while (cont) {
        std::map<std::string, clang::Decl *> all_decls(p_all_decls);
        std::map<std::string, clang::Stmt *> all_stmts(p_all_stmts);
        std::map<std::string, const clang::Type *> all_types(p_all_types);        

        cont = false;
        std::map<std::string, clang::Decl *>::const_iterator it0;
        for (it0 = all_decls.begin(); it0 != all_decls.end(); it0++)
            if (p_seen_nodes.find(it0->first) == p_seen_nodes.end()) {
                TraverseDecl(it0->second);
                std::map<std::string, DotNode>::iterator it = p_ast_nodes_map.find(it0->first);
                if (it == p_ast_nodes_map.end()) {
                    std::cerr << "\t" << "Runtime error...(Decl)" << std::endl;
                    exit(-1);
                }
                else {
                    it->second.setCompilerGenerated(true); // unneccessary
                    cont = true;
                }
            }

        std::map<std::string, clang::Stmt *>::const_iterator it1;
        for (it1 = p_all_stmts.begin(); it1 != p_all_stmts.end(); it1++)
            if (p_seen_nodes.find(it1->first) == p_seen_nodes.end()) {
                TraverseStmt(it1->second);
                std::map<std::string, DotNode>::iterator it = p_ast_nodes_map.find(it1->first);
                if (it == p_ast_nodes_map.end()) {
                    std::cerr << "\t" << "Runtime error...(Stmt)" << std::endl;
                    exit(-1);
                }
                else {
                    it->second.setCompilerGenerated(true);
                    cont = true;
                }
            }
    
        std::map<std::string, const clang::Type *>::const_iterator it2;
        for (it2 = p_all_types.begin(); it2 != p_all_types.end(); it2++)
            if (p_seen_nodes.find(it2->first) == p_seen_nodes.end()) {
                TraverseType(clang::QualType(it2->second, 0));
                std::map<std::string, DotNode>::iterator it = p_ast_nodes_map.find(it2->first);
                if (it == p_ast_nodes_map.end()) {
                    std::cerr << "\t" << "Runtime error...(Type)" << std::endl;
                    exit(-1);
                }
                else {
                    it->second.setCompilerGenerated(true);
                    cont = true;
                }
            } 
    }
}

// Helper methods for nodes' name generation:

std::string DotVisitor::genNameForEmptyNode() {
    std::ostringstream name;
    
    name << "empty_node_" << p_empty_cnt;

    p_empty_cnt++;

    return name.str();
}

std::string DotVisitor::findNameForDeclContext(clang::DeclContext * decl_context) {
    return DotVisitor::genNameForNode((clang::Decl *)decl_context);
}

std::string DotVisitor::genNameForNode(clang::Stmt * stmt) {
    if (stmt == NULL) return genNameForEmptyNode();
    std::map<const clang::Stmt *, std::string>::iterator name_it = p_stmts_name.find(stmt);
    if (name_it == p_stmts_name.end()) {
        std::ostringstream name;
        name << stmt->getStmtClassName() << "_" << stmt;
        name_it = p_stmts_name.insert(std::pair<const clang::Stmt *, std::string>(stmt, name.str())).first;
    }

    p_all_stmts.insert(std::pair<std::string, clang::Stmt *>(name_it->second, stmt));

    return name_it->second;
}

std::string DotVisitor::genNameForNode(clang::Decl * decl) {
    if (decl == NULL) return genNameForEmptyNode();
   std::map<const clang::Decl *, std::string>::iterator name_it = p_decls_name.find(decl);
   if (name_it == p_decls_name.end()) {
       std::ostringstream name;
       name << decl->getDeclKindName() << "_" << decl;
       name_it = p_decls_name.insert(std::pair<const clang::Decl *, std::string>(decl, name.str())).first;
   }

   p_all_decls.insert(std::pair<std::string, clang::Decl *>(name_it->second, decl));

   return name_it->second;
}

std::string DotVisitor::genNameForNode(const clang::Type * type) {
    if (type == NULL) return genNameForEmptyNode();
   std::map<const clang::Type *, std::string>::iterator name_it = p_types_name.find(type);
   if (name_it == p_types_name.end()) {
       std::ostringstream name;
       name << type->getTypeClassName() << "_" << type;
       name_it = p_types_name.insert(std::pair<const clang::Type *, std::string>(type, name.str())).first;
   }

   p_all_types.insert(std::pair<std::string, const clang::Type *>(name_it->second, type));

   return name_it->second;
}

void DotVisitor::addGlobalScopeChild(clang::Decl * decl) {
    p_nodes_in_global_scope.insert(genNameForNode(decl));
} 

// Declaration Traversal

#define DISPATCH(NAME, CLASS, VAR) \
  return getDerived().Traverse##NAME(static_cast<CLASS*>(VAR))

bool DotVisitor::TraverseDecl(clang::Decl * decl) {
  if (!decl)
    return true;

  switch (decl->getKind()) {
#define ABSTRACT_DECL(DECL)
#define DECL(CLASS, BASE) \
  case Decl::CLASS: DISPATCH(CLASS##Decl, CLASS##Decl, decl);
#include "clang/AST/DeclNodes.inc"
 }

  return true;
}

#undef DISPATCH

// Declaration visitors:

bool DotVisitor::VisitDecl(clang::Decl * decl) {
    if (decl == NULL) {
        std::cerr << "Try to visit a NULL pointer." << std::endl;
        exit(-1);
    }

    std::string name = genNameForNode(decl);

//  std::cerr << "DotVisitor::VisitDecl(clang::Decl * decl = " << decl << " - name = " << name << std::endl;

    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end())
        dot_node_it = p_ast_nodes_map.insert(std::pair<std::string, DotNode>(name, DotNode(DotNode::Decl, name))).first;

    p_seen_nodes.insert(name);

    if (p_final_stage)
        dot_node_it->second.setCompilerGenerated(true);

    return true;
}

bool DotVisitor::VisitAccessSpecDecl(clang::AccessSpecDecl * access_spec_decl) {
    std::string name = genNameForNode(access_spec_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Access Specifier Declaration");

    return true;
}

bool DotVisitor::VisitBlockDecl(clang::BlockDecl * block_decl) {
    std::string name = genNameForNode(block_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Block Declaration");

    // TODO

    dot_node_it->second.useAsDeclContext(block_decl, *this);

    return true;
}

bool DotVisitor::VisitNamedDecl(clang::NamedDecl * named_decl) {
    std::string name = genNameForNode(named_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }
/*
    std::string decl_name;
    if (named_decl->)
        decl_name = named_decl->getName().str();
    else
        decl_name = "";

    dot_node_it->second.addAttribute("name", decl_name);
*/

    dot_node_it->second.addAttribute("name", named_decl->getNameAsString());

    return true;
}

bool DotVisitor::VisitValueDecl(clang::ValueDecl * value_decl) {
    std::string name = genNameForNode(value_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // 'value_decl->getType()' is a qualified type (actually a pair type + qualifiers) but the child need to be a type.
    // TODO find a way to integer qualifiers in the DOT representation of the AST.
    // XXX  May add an other child for the Qualifiers (hopefully they will not be shared with other type references...) (will need a new map for qualifier transcription).
//    dot_node_it->second.label += "Type= \\\"" + clang::QualType::getAsString(value_decl->getType().split()) + "\\\"\\n";

    clang::QualType qual_type = value_decl->getType();

    dot_node_it->second.setChild(
        "type",
        genNameForNode(qual_type.getTypePtr())
    );

    return true;
}

bool DotVisitor::VisitIndirectFieldDecl(clang::IndirectFieldDecl * indirect_field_decl) {
    // TODO
    return true;
}

bool DotVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl) {
    // TODO
    return true;
}

bool DotVisitor::VisitDeclaratorDecl(clang::DeclaratorDecl * declarator_decl) {
    std::string name = genNameForNode(declarator_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.applySourceRange(declarator_decl->getSourceRange(), p_source_manager);

    return true;
}

bool DotVisitor::VisitFieldDecl(clang::FieldDecl * field_decl) {
    std::string name = genNameForNode(field_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Field Declaration");

    dot_node_it->second.setChild("parent", genNameForNode(field_decl->getParent()));

    dot_node_it->second.applySourceRange(field_decl->getSourceRange(), p_source_manager);

    return true;
}

bool DotVisitor::VisitFunctionDecl(clang::FunctionDecl * function_decl) {
    std::string name = genNameForNode(function_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Function Declaration");

    clang::FunctionDecl::param_iterator param_it;
    unsigned cnt = 0;
    for (param_it = function_decl->param_begin(); param_it != function_decl->param_end(); param_it++) {
        std::ostringstream oss;
        oss << "parameter[" << cnt << "]";

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(*param_it)
        );

        cnt++;
    }

    if (function_decl->isThisDeclarationADefinition())
    dot_node_it->second.setChild(
        "body",
        genNameForNode(function_decl->getBody())
    );

    dot_node_it->second.setChild(
        "first_decl",
        genNameForNode(function_decl->getFirstDeclaration())
    );

    dot_node_it->second.setChild(
        "most_recent_decl",
        genNameForNode(function_decl->getMostRecentDeclaration())
    );

    dot_node_it->second.setChild(
        "previous_decl",
        genNameForNode(function_decl->getPreviousDeclaration())
    );

    dot_node_it->second.setChild(
        "canonical_decl",
        genNameForNode(function_decl->getCanonicalDecl())
    );

    dot_node_it->second.useAsDeclContext(function_decl, *this);

    return true;
}

bool DotVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl * cxx_method_decl) {
    std::string name = genNameForNode(cxx_method_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("CXX Method Declaration");

    dot_node_it->second.setChild(
        "canonical_decl",
        genNameForNode(cxx_method_decl->getCanonicalDecl())
    );

    dot_node_it->second.setChild(
        "parent",
        genNameForNode(cxx_method_decl->getParent())
    );

    return true;
}

bool DotVisitor::VisitCXXConstructorDecl(clang::CXXConstructorDecl * cxx_constructor_decl) {
    std::string name = genNameForNode(cxx_constructor_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("CXX Constructor Declaration");

    // TODO

    return true;
}

bool DotVisitor::VisitCXXConversionDecl(clang::CXXConversionDecl * cxx_conversion_decl) {
    std::string name = genNameForNode(cxx_conversion_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("CXX Conversion Declaration");

    // TODO

    return true;
}

bool DotVisitor::VisitCXXDestructorDecl(clang::CXXDestructorDecl * cxx_destructor_decl) {
    std::string name = genNameForNode(cxx_destructor_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("CXX Destructor Declaration");

    // TODO

    return true;
}

bool DotVisitor::VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl * non_type_template_param_decl) {
    std::string name = genNameForNode(non_type_template_param_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitVarDecl(clang::VarDecl * var_decl) {
    std::string name = genNameForNode(var_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Variable Declaration");

    dot_node_it->second.setChild(
        "initializer",
        genNameForNode(var_decl->getInit())
    );

    return true;
}

bool DotVisitor::VisitParmVarDecl(clang::ParmVarDecl * param_var_decl) {
    std::string name = genNameForNode(param_var_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Parameter Variable Declaration");

    return true;
}

bool DotVisitor::VisitImplicitParamDecl(clang::ImplicitParamDecl * implicit_param_decl) {
    std::string name = genNameForNode(implicit_param_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Implicit Parameter");

    return true;
}

bool DotVisitor::VisitTypeDecl(clang::TypeDecl * type_decl) {
    std::string name = genNameForNode(type_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl * template_type_parm_decl) {
    std::string name = genNameForNode(template_type_parm_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Template Type Parameter Type");

    std::string default_argument;
    if (template_type_parm_decl->hasDefaultArgument()) {
        clang::QualType qual_type = template_type_parm_decl->getDefaultArgument();

        // FIXME type qualifiers

        default_argument = genNameForNode(qual_type.getTypePtr());
    }
    else
        default_argument = genNameForEmptyNode();

    dot_node_it->second.setChild("default_argument", default_argument);

    dot_node_it->second.applySourceRange(template_type_parm_decl->getSourceRange(), p_source_manager);

    return true;
}

bool DotVisitor::VisitTypedefNameDecl(clang::TypedefNameDecl * typedef_name_decl) {
    std::string name = genNameForNode(typedef_name_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    clang::QualType underlying_type = typedef_name_decl->getUnderlyingType();

    // FIXME type qualifier

    dot_node_it->second.setChild(
        "underlying_type",
        genNameForNode(underlying_type.getTypePtr())
    );

    dot_node_it->second.setChild(
        "canonical_decl",
        genNameForNode(typedef_name_decl->getCanonicalDecl())
    );

    return true;
}

bool DotVisitor::VisitTypedefDecl(clang::TypedefDecl * typedef_decl) {
    std::string name = genNameForNode(typedef_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Typedef Declaration");

    return true;
}

bool DotVisitor::VisitTypeAliasDecl(clang::TypeAliasDecl * type_alias_decl) {
    std::string name = genNameForNode(type_alias_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTagDecl(clang::TagDecl * tag_decl) {
    std::string name = genNameForNode(tag_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Tag Declaration");

    // TODO

    dot_node_it->second.useAsDeclContext(tag_decl, *this);

    return true;
}

bool DotVisitor::VisitRecordDecl(clang::RecordDecl * record_decl) {
    std::string name = genNameForNode(record_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Class/Struct/Union Declaration");

    dot_node_it->second.setChild(
        "previous_declaration",
        genNameForNode(record_decl->getPreviousDeclaration())
    );

    dot_node_it->second.setChild(
        "defining_declaration",
        genNameForNode(record_decl->getDefinition())
    );

    clang::RecordDecl::field_iterator it;
    unsigned cnt = 0;
    for (it = record_decl->field_begin(); it != record_decl->field_end(); it++) {
        std::ostringstream oss;
        oss << "field[" << cnt << "]";

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(*it)
        );

        cnt++;
    }

    return true;
}

bool DotVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl) {
    std::string name = genNameForNode(cxx_record_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("CXX Class/Struct Declaration");

    dot_node_it->second.setChild(
        "previous_declaration",
        genNameForNode(cxx_record_decl->getPreviousDeclaration())
    );

    dot_node_it->second.setChild(
        "defining_declaration",
        genNameForNode(cxx_record_decl->getDefinition())
    );

    dot_node_it->second.setChild(
        "canonical_declaration",
        genNameForNode(cxx_record_decl->getCanonicalDecl())
    );

    if (!cxx_record_decl->hasDefinition()) return true;

    clang::CXXRecordDecl::base_class_iterator it0;
    unsigned cnt = 0;
    for (it0 = cxx_record_decl->bases_begin(); it0 != cxx_record_decl->bases_end(); it0++) {
        std::ostringstream oss;
        oss << "base[" << cnt << "]";

        // FIXME AccessSpecifier

        clang::QualType qual_type = it0->getType();

        // FIXME type qualifiers

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(qual_type.getTypePtr())
        );

        cnt++;
    }

    cnt = 0;
    for (it0 = cxx_record_decl->vbases_begin(); it0 != cxx_record_decl->vbases_end(); it0++) {
        std::ostringstream oss;
        oss << "virtual_base[" << cnt << "]";

        // FIXME AccessSpecifier

        clang::QualType qual_type = it0->getType();

        // FIXME type qualifiers

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(qual_type.getTypePtr())
        );

        cnt++;
    }

    clang::CXXRecordDecl::method_iterator it1;
    cnt = 0;
    for (it1 = cxx_record_decl->method_begin(); it1 != cxx_record_decl->method_end(); it1++) {
        std::ostringstream oss;
        oss << "method[" << cnt << "]";

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(*it1)
        );

        cnt++;
    }

    clang::CXXRecordDecl::ctor_iterator it2;
    cnt = 0;
    for (it2 = cxx_record_decl->ctor_begin(); it2 != cxx_record_decl->ctor_end(); it2++) {
        std::ostringstream oss;
        oss << "ctor[" << cnt << "]";

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(*it2)
        );

        cnt++;
    }

    clang::CXXRecordDecl::friend_iterator it3;
    cnt = 0;
    for (it3 = cxx_record_decl->friend_begin(); it3 != cxx_record_decl->friend_end(); it3++) {
        std::ostringstream oss;
        oss << "friend[" << cnt << "]";

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(*it3)
        );

        cnt++;
    }

    return true;
}

bool DotVisitor::VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl * class_tpl_spec_decl) {
    std::string name = genNameForNode(class_tpl_spec_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Template Specialization Declaration");

    dot_node_it->second.setChild(
        "original_template",
        genNameForNode(class_tpl_spec_decl->getSpecializedTemplate())
    );

    const TemplateArgumentList & tpl_arg_list = class_tpl_spec_decl->getTemplateArgs();
    for (unsigned cnt = 0; cnt < tpl_arg_list.size(); cnt++) {
        std::ostringstream oss;
        oss << "template_argument[" << cnt << "]";

        const clang::TemplateArgument & tpl_arg = tpl_arg_list.get(cnt);

        std::string node_id;

        switch (tpl_arg.getKind()) {
            case clang::TemplateArgument::Type:
            {
                clang::QualType tpl_arg_qual_type = tpl_arg.getAsType();

                // FIXME type qualifiers

                node_id = genNameForNode(tpl_arg_qual_type.getTypePtr());

                break;
            }
            case clang::TemplateArgument::Declaration: node_id = genNameForNode(tpl_arg.getAsDecl()); break;
            case clang::TemplateArgument::Expression: node_id = genNameForNode(tpl_arg.getAsExpr()); break;
//            case : node_id = ; break; // TODO 5 other cases...
            default:
                std::cerr << "Unsupported template argument type..." << std::endl;
                exit(-1);
        }

        dot_node_it->second.setChild(
            oss.str(),
            node_id
        );
    }

    return true;
}

bool DotVisitor::VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl * class_tpl_part_spec_decl) {
    std::string name = genNameForNode(class_tpl_part_spec_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Template Partial Specialization Declaration");

    // TODO

    return true;
}

bool DotVisitor::VisitEnumDecl(clang::EnumDecl * enum_decl) {
    std::string name = genNameForNode(enum_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTemplateDecl(clang::TemplateDecl * template_decl) {
    std::string name = genNameForNode(template_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Template Declaration");

    clang::TemplateParameterList * tpl_param_list = template_decl->getTemplateParameters();
    clang::TemplateParameterList::iterator it;
    unsigned cnt = 0;
    for (it = tpl_param_list->begin(); it != tpl_param_list->end(); it++) {
        std::ostringstream oss;
        oss << "template_parameter[" << cnt << "]";

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(*it)
        );

        cnt++;
    }

    dot_node_it->second.setChild(
        "templated_decl",
        genNameForNode(template_decl->getTemplatedDecl())
    );

    dot_node_it->second.applySourceRange(template_decl->getSourceRange(), p_source_manager);

    return true;
}

bool DotVisitor::VisitRedeclarableTemplateDecl(clang::RedeclarableTemplateDecl * redeclarable_template_decl) {
    std::string name = genNameForNode(redeclarable_template_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitClassTemplateDecl(clang::ClassTemplateDecl * class_template_decl) {
    std::string name = genNameForNode(class_template_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitFunctionTemplateDecl(clang::FunctionTemplateDecl * function_template_decl) {
    std::string name = genNameForNode(function_template_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl * type_alias_template_decl) {
    std::string name = genNameForNode(type_alias_template_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl * template_template_parm_decl) {
    std::string name = genNameForNode(template_template_parm_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitLabelDecl(clang::LabelDecl * label_decl) {
    std::string name = genNameForNode(label_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitNamespaceDecl(clang::NamespaceDecl * namespace_decl) {
    std::string name = genNameForNode(namespace_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Namespace Declaration");

    dot_node_it->second.setChild(
        "next",
        genNameForNode(namespace_decl->getNextNamespace())
    );

    dot_node_it->second.setChild(
        "original",
        genNameForNode(namespace_decl->getOriginalNamespace())
    );

    dot_node_it->second.setChild(
        "anonymous",
        genNameForNode(namespace_decl->getAnonymousNamespace())
    );

    dot_node_it->second.setChild(
        "canonical",
        genNameForNode(namespace_decl->getCanonicalDecl())
    );

    dot_node_it->second.useAsDeclContext(namespace_decl, *this);

    return true;
}

bool DotVisitor::VisitFriendDecl(clang::FriendDecl * friend_decl) {
    std::string name = genNameForNode(friend_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitFriendTemplateDecl(clang::FriendTemplateDecl * friend_template_decl) {
    std::string name = genNameForNode(friend_template_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitFileScopeAsmDecl(clang::FileScopeAsmDecl * file_scope_asm_decl) {
    std::string name = genNameForNode(file_scope_asm_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl) {
    std::string name = genNameForNode(translation_unit_decl);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Translation Unit Declaration");

    // TODO

    dot_node_it->second.useAsDeclContext(translation_unit_decl, *this);

    return true;
}

// Statement visitors:

bool DotVisitor::VisitStmt(clang::Stmt * stmt) {
    if (stmt == NULL) {
        std::cerr << "Try to visit a NULL pointer." << std::endl;
        exit(1);
    }

    std::string name = genNameForNode(stmt);

    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end())
        dot_node_it = p_ast_nodes_map.insert(std::pair<std::string, DotNode>(name, DotNode(DotNode::Stmt, name))).first;

    p_seen_nodes.insert(name);

    if (p_final_stage)
        dot_node_it->second.setCompilerGenerated(true);

    return true;
}

bool DotVisitor::VisitAsmStmt(clang::AsmStmt * asm_stmt) {
    std::string name = genNameForNode(asm_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitBreakStmt(clang::BreakStmt * break_stmt) {
    std::string name = genNameForNode(break_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCompoundStmt(clang::CompoundStmt * compound_stmt) {
    std::string name = genNameForNode(compound_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Compound Statement");

    unsigned cnt = 0;
    clang::CompoundStmt::body_iterator body_it;
    for (body_it = compound_stmt->body_begin(); body_it != compound_stmt->body_end(); body_it++) {
        std::ostringstream oss;
        oss << "child[" << cnt << "]";

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(*body_it)
        );

        cnt++;
    }

    return true;
}

bool DotVisitor::VisitContinueStmt(clang::ContinueStmt * continue_stmt) {
    std::string name = genNameForNode(continue_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXCatchStmt(clang::CXXCatchStmt * cxx_catch_stmt) {
    std::string name = genNameForNode(cxx_catch_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXForRangeStmt(clang::CXXForRangeStmt * cxx_for_range_stmt) {
    std::string name = genNameForNode(cxx_for_range_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXTryStmt(clang::CXXTryStmt * cxx_try_stmt) {
    std::string name = genNameForNode(cxx_try_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDeclStmt(clang::DeclStmt * decl_stmt) {
    std::string name = genNameForNode(decl_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Declaration Statement");

    unsigned cnt = 0;
    clang::DeclStmt::decl_iterator decl_it;
    for (decl_it = decl_stmt->decl_begin(); decl_it != decl_stmt->decl_end(); decl_it++) {
        std::ostringstream oss;
        oss << "declaration[" << cnt << "]";

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(*decl_it)
        );

        cnt++;
    }

    return true;
}

bool DotVisitor::VisitDoStmt(clang::DoStmt * do_stmt) {
    std::string name = genNameForNode(do_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitExpr(clang::Expr * expr) {
    std::string name = genNameForNode(expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitAbstractConditionalOperator(clang::AbstractConditionalOperator * abstract_conditional_operator) {
    std::string name = genNameForNode(abstract_conditional_operator);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitBinaryConditionalOperator(clang::BinaryConditionalOperator * binary_conditionnal_operator) {
    std::string name = genNameForNode(binary_conditionnal_operator);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitConditionalOperator(clang::ConditionalOperator * conditional_operator) {
    std::string name = genNameForNode(conditional_operator);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitAddrLabelExpr(clang::AddrLabelExpr * addr_label_expr) {
    std::string name = genNameForNode(addr_label_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr) {
    std::string name = genNameForNode(array_subscript_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitArrayTypeTraitExpr(clang::ArrayTypeTraitExpr * array_type_trait_expr) {
    std::string name = genNameForNode(array_type_trait_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitAsTypeExpr(clang::AsTypeExpr * as_type_expr) {
    std::string name = genNameForNode(as_type_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitBinaryOperator(clang::BinaryOperator * binary_operator) {
    std::string name = genNameForNode(binary_operator);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Binary Operator");

    std::string opcode;
    switch (binary_operator->getOpcode()) {
        case clang::BO_PtrMemD:   opcode = "PtrMemD"; break;
        case clang::BO_PtrMemI:   opcode = "PtrMemI"; break;
        case clang::BO_Mul:       opcode = "Mul"; break;
        case clang::BO_Div:       opcode = "Div"; break;
        case clang::BO_Rem:       opcode = "Rem"; break;
        case clang::BO_Add:       opcode = "Add"; break;
        case clang::BO_Sub:       opcode = "Sub"; break;
        case clang::BO_Shl:       opcode = "Shl"; break;
        case clang::BO_Shr:       opcode = "Shr"; break;
        case clang::BO_LT:        opcode = "LT"; break;
        case clang::BO_GT:        opcode = "GT"; break;
        case clang::BO_LE:        opcode = "LE"; break;
        case clang::BO_GE:        opcode = "GE"; break;
        case clang::BO_EQ:        opcode = "EQ"; break;
        case clang::BO_NE:        opcode = "NE"; break;
        case clang::BO_And:       opcode = "And"; break;
        case clang::BO_Xor:       opcode = "Xor"; break;
        case clang::BO_Or:        opcode = "Or"; break;
        case clang::BO_LAnd:      opcode = "LAnd"; break;
        case clang::BO_LOr:       opcode = "LOr"; break;
        case clang::BO_Assign:    opcode = "Assign"; break;
        case clang::BO_MulAssign: opcode = "MulAssign"; break;
        case clang::BO_DivAssign: opcode = "DivAssign"; break;
        case clang::BO_RemAssign: opcode = "RemAssign"; break;
        case clang::BO_AddAssign: opcode = "AddAssign"; break;
        case clang::BO_SubAssign: opcode = "SubAssign"; break;
        case clang::BO_ShlAssign: opcode = "ShlAssign"; break;
        case clang::BO_ShrAssign: opcode = "ShrAssign"; break;
        case clang::BO_AndAssign: opcode = "AndAssign"; break;
        case clang::BO_XorAssign: opcode = "XorAssign"; break;
        case clang::BO_OrAssign:  opcode = "OrAssign"; break;
        case clang::BO_Comma:     opcode = "Comma"; break;
        default:
            std::cerr << "Unknown opcode for binary operator..." << std::endl;
            exit(-1);
    }

    dot_node_it->second.addAttribute("operator", opcode);

    dot_node_it->second.setChild(
        "lhs",
        genNameForNode(binary_operator->getLHS())
    );

    dot_node_it->second.setChild(
        "rhs",
        genNameForNode(binary_operator->getRHS())
    );

    return true;
}

bool DotVisitor::VisitCompoundAssignOperator(clang::CompoundAssignOperator * compound_assign_operator) {
    std::string name = genNameForNode(compound_assign_operator);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitBinaryTypeTraitExpr(clang::BinaryTypeTraitExpr * binary_type_trait_expr) {
    std::string name = genNameForNode(binary_type_trait_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitBlockDeclRefExpr(clang::BlockDeclRefExpr * block_decl_ref_expr) {
    std::string name = genNameForNode(block_decl_ref_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitBlockExpr(clang::BlockExpr * block_expr) {
    std::string name = genNameForNode(block_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCallExpr(clang::CallExpr * call_expr) {
    std::string name = genNameForNode(call_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Call Expression");

    dot_node_it->second.setChild(
        "callee",
        genNameForNode(call_expr->getCallee())
    );

    dot_node_it->second.setChild(
        "callee_decl",
        genNameForNode(call_expr->getCalleeDecl())
    );

    dot_node_it->second.setChild(
        "direct_callee",
        genNameForNode(call_expr->getDirectCallee())
    ); 

    for (unsigned cnt = 0; cnt < call_expr->getNumArgs(); cnt++) {
        std::ostringstream oss;
        oss << "argument[" << cnt << "]";
        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(call_expr->getArg(cnt))
        );
    }

    dot_node_it->second.applySourceRange(call_expr->getSourceRange(), p_source_manager);

    return true;
}

bool DotVisitor::VisitCUDAKernelCallExpr(clang::CUDAKernelCallExpr * cuda_kernel_call_expr) {
    std::string name = genNameForNode(cuda_kernel_call_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXMemberCallExpr(clang::CXXMemberCallExpr * cxx_member_call_expr) {
    std::string name = genNameForNode(cxx_member_call_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("CXX Member Call Expression");

    dot_node_it->second.setChild(
        "implicit_object_argument",
        genNameForNode(cxx_member_call_expr->getImplicitObjectArgument())
    );

    dot_node_it->second.setChild(
        "method_decl",
        genNameForNode(cxx_member_call_expr->getMethodDecl())
    );

    dot_node_it->second.setChild(
        "class_decl",
        genNameForNode(cxx_member_call_expr->getRecordDecl())
    );

    return true;
}

bool DotVisitor::VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr * cxx_operator_call_expr) {
    std::string name = genNameForNode(cxx_operator_call_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCastExpr(clang::CastExpr * cast_expr) {
    std::string name = genNameForNode(cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.addAttribute("cast type", std::string(cast_expr->getCastKindName()));

    dot_node_it->second.setChild(
        "sub_expression",
        genNameForNode(cast_expr->getSubExpr())
    );

    return true;
}

bool DotVisitor::VisitExplicitCastExpr(clang::ExplicitCastExpr * explicit_cast_expr) {
    std::string name = genNameForNode(explicit_cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast) {
    std::string name = genNameForNode(c_style_cast);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr * cxx_functional_cast_expr) {
    std::string name = genNameForNode(cxx_functional_cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXNamedCastExpr(clang::CXXNamedCastExpr * cxx_named_cast_expr) {
    std::string name = genNameForNode(cxx_named_cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXConstCastExpr(clang::CXXConstCastExpr * cxx_const_cast_expr) {
    std::string name = genNameForNode(cxx_const_cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXDynamicCastExpr(clang::CXXDynamicCastExpr * cxx_dynamic_cast_expr) {
    std::string name = genNameForNode(cxx_dynamic_cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr * cxx_reinterpret_cast_expr) {
    std::string name = genNameForNode(cxx_reinterpret_cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXStaticCastExpr(clang::CXXStaticCastExpr * cxx_static_cast_expr) {
    std::string name = genNameForNode(cxx_static_cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr) {
    std::string name = genNameForNode(implicit_cast_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Implicit Cast");

    // FIXME have something else to do here ?

    return true;
}

bool DotVisitor::VisitCharacterLiteral(clang::CharacterLiteral * character_literal) {
    std::string name = genNameForNode(character_literal);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitChooseExpr(clang::ChooseExpr * choose_expr) {
    std::string name = genNameForNode(choose_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal) {
    std::string name = genNameForNode(compound_literal);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXBindTemporaryExpr(clang::CXXBindTemporaryExpr * cxx_bind_temporary_expr) {
    std::string name = genNameForNode(cxx_bind_temporary_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr * cxx_bool_literal_expr) {
    std::string name = genNameForNode(cxx_bool_literal_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXConstructExpr(clang::CXXConstructExpr * cxx_constructor_expr) {
    std::string name = genNameForNode(cxx_constructor_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("CXX Construct Expression");

    dot_node_it->second.setChild(
        "constructor_decl",
        genNameForNode(cxx_constructor_expr->getConstructor())
    );

    for (unsigned cnt = 0; cnt < cxx_constructor_expr->getNumArgs(); cnt++) {
        std::ostringstream oss;
        oss << "argument[" << cnt << "]";
        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(cxx_constructor_expr->getArg(cnt))
        );
    }

    return true;
}

bool DotVisitor::VisitCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr * cxx_temporary_object_expr) {
    std::string name = genNameForNode(cxx_temporary_object_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXDefaultArgExpr(clang::CXXDefaultArgExpr * cxx_default_arg_expr) {
    std::string name = genNameForNode(cxx_default_arg_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXDeleteExpr(clang::CXXDeleteExpr * cxx_delete_expr) {
    std::string name = genNameForNode(cxx_delete_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr * cxx_dependent_scope_member_expr) {
    std::string name = genNameForNode(cxx_dependent_scope_member_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("CXX Dependent Scope Member Expression");

    dot_node_it->second.setChild(
        "base",
        genNameForNode(cxx_dependent_scope_member_expr->getBase())
    );

    clang::QualType base_qual_type = cxx_dependent_scope_member_expr->getBaseType();

    // FIXME type qualifiers

    dot_node_it->second.setChild(
        "base_type",
        genNameForNode(base_qual_type.getTypePtr())
    );

    dot_node_it->second.addAttribute(
        "type",
        cxx_dependent_scope_member_expr->isArrow() ? "arrow" : "dot"
    );

    dot_node_it->second.addAttribute(
        "member name",
        cxx_dependent_scope_member_expr->getMember().getAsString()
    );

    dot_node_it->second.setChild(
        "first_qualifier_found_in_scope",
        genNameForNode(cxx_dependent_scope_member_expr->getFirstQualifierFoundInScope())
    );

    return true;
}

bool DotVisitor::VisitCXXNewExpr(clang::CXXNewExpr * cxx_new_expr) {
    std::string name = genNameForNode(cxx_new_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXNoexceptExpr(clang::CXXNoexceptExpr * cxx_noexecept_expr) {
    std::string name = genNameForNode(cxx_noexecept_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr * cxx_null_ptr_literal_expr) {
    std::string name = genNameForNode(cxx_null_ptr_literal_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXPseudoDestructorExpr(clang::CXXPseudoDestructorExpr * cxx_pseudo_destructor_expr) {
    std::string name = genNameForNode(cxx_pseudo_destructor_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXScalarValueInitExpr(clang::CXXScalarValueInitExpr * cxx_scalar_value_init_expr) {
    std::string name = genNameForNode(cxx_scalar_value_init_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXThisExpr(clang::CXXThisExpr * cxx_this_expr) {
    std::string name = genNameForNode(cxx_this_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXThrowExpr(clang::CXXThrowExpr * cxx_throw_expr) {
    std::string name = genNameForNode(cxx_throw_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXTypeidExpr(clang::CXXTypeidExpr * cxx_typeid_expr) {
    std::string name = genNameForNode(cxx_typeid_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr * cxx_unresolved_construct_expr) {
    std::string name = genNameForNode(cxx_unresolved_construct_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCXXUuidofExpr(clang::CXXUuidofExpr * cxx_uuidof_expr) {
    std::string name = genNameForNode(cxx_uuidof_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr) {
    std::string name = genNameForNode(decl_ref_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Declaration Reference Expression");

    dot_node_it->second.setChild(
        "declaration",
        genNameForNode(decl_ref_expr->getDecl())
    );

    dot_node_it->second.setChild(
        "found_declaration",
        genNameForNode(decl_ref_expr->getFoundDecl())
    );

    // TODO template/position related attribute

    return true;
}

bool DotVisitor::VisitDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr * dependent_scope_decl_ref_expr) {
    std::string name = genNameForNode(dependent_scope_decl_ref_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr) {
    std::string name = genNameForNode(designated_init_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitExpressionTraitExpr(clang::ExpressionTraitExpr * expression_trait_expr) {
    std::string name = genNameForNode(expression_trait_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitExprWithCleanups(clang::ExprWithCleanups * expr_with_cleanups) {
    std::string name = genNameForNode(expr_with_cleanups);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr) {
    std::string name = genNameForNode(ext_vector_element_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitFloatingLiteral(clang::FloatingLiteral * floating_literal) {
    std::string name = genNameForNode(floating_literal);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitGenericSelectionExpr(clang::GenericSelectionExpr * generic_selection_expr) {
    std::string name = genNameForNode(generic_selection_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitGNUNullExpr(clang::GNUNullExpr * gnu_null_expr) {
    std::string name = genNameForNode(gnu_null_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal) {
    std::string name = genNameForNode(imaginary_literal);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitImplicitValueInitExpr(clang::ImplicitValueInitExpr * implicit_value_init_expr) {
    std::string name = genNameForNode(implicit_value_init_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitInitListExpr(clang::InitListExpr * init_list_expr) {
    std::string name = genNameForNode(init_list_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitIntegerLiteral(clang::IntegerLiteral * integer_literal) {
    std::string name = genNameForNode(integer_literal);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Integer Literal");

    std::ostringstream oss;

    oss << integer_literal->getValue().getSExtValue();

    dot_node_it->second.addAttribute("value", oss.str());

    // TODO position related attributes

    return true;
}

bool DotVisitor::VisitMemberExpr(clang::MemberExpr * member_expr) {
    std::string name = genNameForNode(member_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Member Expression");

    dot_node_it->second.setChild(
        "base",
        genNameForNode(member_expr->getBase())
    );

    dot_node_it->second.setChild(
        "declaration",
        genNameForNode(member_expr->getMemberDecl())
    );

    // TODO templates & qualifiers

    dot_node_it->second.applySourceRange(member_expr->getSourceRange(), p_source_manager);

    return true;
}

bool DotVisitor::VisitOffsetOfExpr(clang::OffsetOfExpr * offset_of_expr) {
    std::string name = genNameForNode(offset_of_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitOpaqueValueExpr(clang::OpaqueValueExpr * opaque_value_expr) {
    std::string name = genNameForNode(opaque_value_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitOverloadExpr(clang::OverloadExpr * overload_expr) {
    std::string name = genNameForNode(overload_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr * unresolved_lookup_expr) {
    std::string name = genNameForNode(unresolved_lookup_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr * unresolved_member_expr) {
    std::string name = genNameForNode(unresolved_member_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitPackExpansionExpr(clang::PackExpansionExpr * pack_expansion_expr) {
    std::string name = genNameForNode(pack_expansion_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitParenExpr(clang::ParenExpr * paren_expr) {
    std::string name = genNameForNode(paren_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitParenListExpr(clang::ParenListExpr * paren_list_expr) {
    std::string name = genNameForNode(paren_list_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr) {
    std::string name = genNameForNode(predefined_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitShuffleVectorExpr(clang::ShuffleVectorExpr * shuffle_vector_expr) {
    std::string name = genNameForNode(shuffle_vector_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitSizeOfPackExpr(clang::SizeOfPackExpr * size_of_pack_expr) {
    std::string name = genNameForNode(size_of_pack_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitStmtExpr(clang::StmtExpr * stmt_expr) {
    std::string name = genNameForNode(stmt_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitStringLiteral(clang::StringLiteral * string_literal) {
    std::string name = genNameForNode(string_literal);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitSubstNonTypeTemplateParmPackExpr(clang::SubstNonTypeTemplateParmPackExpr * subst_non_type_template_parm_pack_expr) {
    std::string name = genNameForNode(subst_non_type_template_parm_pack_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr) {
    std::string name = genNameForNode(unary_expr_or_type_trait_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitUnaryOperator(clang::UnaryOperator * unary_operator) {
    std::string name = genNameForNode(unary_operator);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Unary Operator");

    std::ostringstream oss;

    switch (unary_operator->getOpcode()) {
        case clang::UO_PostInc:
            oss << "Post-incrementation";
            break;
        case clang::UO_PostDec:
            oss << "Post-decrementation";
            break;
        case clang::UO_PreInc:
            oss << "Pre-incrementation";
            break;
        case clang::UO_PreDec:
            oss << "Pre-decrementation";
            break;
        case clang::UO_AddrOf:
            oss << "AddressOf";
            break;
        case clang::UO_Deref:
            oss << "Dereference";
            break;
        case clang::UO_Plus:
            oss << "Plus";
            break;
        case clang::UO_Minus:
            oss << "Minus";
            break;
        case clang::UO_Not:
            oss << "Not";
            break;
        case clang::UO_LNot:
            oss << "LNot";
            break;
        case clang::UO_Real:
            oss << "Real";
            break;
        case clang::UO_Imag:
            oss << "Imag";
            break;
        case clang::UO_Extension:
            oss << "Extension";
            break;
        default:
            std::cerr << "Unknown unary operator" << std::endl;
            exit(1);
    }

    dot_node_it->second.addAttribute("operator", oss.str());

    dot_node_it->second.setChild(
        "operand",
        genNameForNode(unary_operator->getSubExpr())
    );

    return true;
}

bool DotVisitor::VisitUnaryTypeTraitExpr(clang::UnaryTypeTraitExpr * unary_type_trait_expr) {
    std::string name = genNameForNode(unary_type_trait_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitVAArgExpr(clang::VAArgExpr * va_arg_expr) {
    std::string name = genNameForNode(va_arg_expr);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitForStmt(clang::ForStmt * for_stmt) {
    std::string name = genNameForNode(for_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitGotoStmt(clang::GotoStmt * goto_stmt) {
    std::string name = genNameForNode(goto_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitIfStmt(clang::IfStmt * if_stmt) {
    std::string name = genNameForNode(if_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitIndirectGotoStmt(clang::IndirectGotoStmt * indirect_goto_stmt) {
    std::string name = genNameForNode(indirect_goto_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitLabelStmt(clang::LabelStmt * label_stmt) {
    std::string name = genNameForNode(label_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitNullStmt(clang::NullStmt * null_stmt) {
    std::string name = genNameForNode(null_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

// XXX Objective-C statements

bool DotVisitor::VisitReturnStmt(clang::ReturnStmt * return_stmt) {
    std::string name = genNameForNode(return_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Return Statement");

    dot_node_it->second.setChild(
        "value",
        genNameForNode(return_stmt->getRetValue())
    );

    return true;
}

// XXX SEH statements (What is SEH ?)

bool DotVisitor::VisitSwitchCase(clang::SwitchCase * switch_case) {
    std::string name = genNameForNode(switch_case);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitCaseStmt(clang::CaseStmt * case_stmt) {
    std::string name = genNameForNode(case_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDefaultStmt(clang::DefaultStmt * default_stmt) {
    std::string name = genNameForNode(default_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitSwitchStmt(clang::SwitchStmt * switch_stmt) {
    std::string name = genNameForNode(switch_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitWhileStmt(clang::WhileStmt * while_stmt) {
    std::string name = genNameForNode(while_stmt);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

// Type visitors:

bool DotVisitor::VisitType(clang::Type * type) {
    if (type == NULL) {
        std::cerr << "Try to visit a NULL pointer." << std::endl;
        exit(1);
    }

    std::string name = genNameForNode(type);

    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end())
        dot_node_it = p_ast_nodes_map.insert(std::pair<std::string, DotNode>(name, DotNode(DotNode::Type, name))).first;

    p_seen_nodes.insert(name);

    if (p_final_stage)
        dot_node_it->second.setCompilerGenerated(true);

    return true;
}

bool DotVisitor::VisitArrayType(clang::ArrayType * array_type) {
    std::string name = genNameForNode(array_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitConstantArrayType(clang::ConstantArrayType * constant_array_type) {
    std::string name = genNameForNode(constant_array_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDependentSizedArrayType(clang::DependentSizedArrayType * dependent_sized_array_type) {
    std::string name = genNameForNode(dependent_sized_array_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type) {
    std::string name = genNameForNode(incomplete_array_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitVariableArrayType(clang::VariableArrayType * variable_array_type) {
    std::string name = genNameForNode(variable_array_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitAttributedType(clang::AttributedType * attributed_type) {
    std::string name = genNameForNode(attributed_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitAutoType(clang::AutoType * auto_type) {
    std::string name = genNameForNode(auto_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitBlockPointerType(clang::BlockPointerType * block_pointer_type) {
    std::string name = genNameForNode(block_pointer_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitBuiltinType(clang::BuiltinType * builtin_type) {
    std::string name = genNameForNode(builtin_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Built-in Type");

    dot_node_it->second.addAttribute(
        "built-in type",
        std::string(builtin_type->getName(p_lang_opts))
    );

    clang::QualType desugar_type = builtin_type->desugar();

    // FIXME type qualifiers

    dot_node_it->second.setChild(
        "desugar_type",
        genNameForNode(desugar_type.getTypePtr())
    );

    return true;
}

bool DotVisitor::VisitComplexType(clang::ComplexType * complex_type) {
    std::string name = genNameForNode(complex_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDecltypeType(clang::DecltypeType * decltype_type) {
    std::string name = genNameForNode(decltype_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDependentDecltypeType(clang::DependentDecltypeType * dependent_decltype_type) {
    std::string name = genNameForNode(dependent_decltype_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDependentSizedExtVectorType(clang::DependentSizedExtVectorType * dependent_sized_ext_vector_type) {
    std::string name = genNameForNode(dependent_sized_ext_vector_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitFunctionType(clang::FunctionType * function_type) {
    std::string name = genNameForNode(function_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    clang::QualType qual_return_type = function_type->getResultType();

    // FIXME type qualification

    dot_node_it->second.setChild(
        "return_type",
        genNameForNode(qual_return_type.getTypePtr())
    );

    return true;
}

bool DotVisitor::VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type) {
    std::string name = genNameForNode(function_no_proto_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Function Type (without prototype)");

    return true;
}

bool DotVisitor::VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type) {
    std::string name = genNameForNode(function_proto_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Function Type (with prototype)");

    for (unsigned cnt = 0; cnt != function_proto_type->getNumArgs(); cnt++) {
        std::ostringstream oss;
        oss << "param_type[" << cnt << "]";

        clang::QualType qual_param_type = function_proto_type->getArgType(cnt);

        // FIXME l_param_type qualifiers

        dot_node_it->second.setChild(
            oss.str(),
            genNameForNode(qual_param_type.getTypePtr())
        );
    }

    if (function_proto_type->hasExceptionSpec()) {
        // TODO exception (throw/noexcept)
    }

    return true;
}

bool DotVisitor::VisitInjectedClassNameType(clang::InjectedClassNameType * injected_class_name_type) {
    std::string name = genNameForNode(injected_class_name_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitLocInfoType(clang::LocInfoType * loc_info_type) {
    std::string name = genNameForNode(loc_info_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitMemberPointerType(clang::MemberPointerType * member_pointer_type) {
    std::string name = genNameForNode(member_pointer_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

// Objective-C visitors

bool DotVisitor::VisitPackExpansionType(clang::PackExpansionType * pack_expansion_type) {
    std::string name = genNameForNode(pack_expansion_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitParenType(clang::ParenType * paren_type) {
    std::string name = genNameForNode(paren_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitPointerType(clang::PointerType * pointer_type) {
    std::string name = genNameForNode(pointer_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Pointer Type");

    clang::QualType qual_base_type = pointer_type->getPointeeType();

    // FIXME type qualification

    dot_node_it->second.setChild(
        "base_type",
        genNameForNode(qual_base_type.getTypePtr())
    );

    return true;
}

bool DotVisitor::VisitReferenceType(clang::ReferenceType * reference_type) {
    std::string name = genNameForNode(reference_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitLValueReferenceType(clang::LValueReferenceType * lvalue_reference_type) {
    std::string name = genNameForNode(lvalue_reference_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("LValue Reference Type");

    clang::QualType qual_base_type = lvalue_reference_type->desugar();

    // FIXME type qualifiers

    dot_node_it->second.setChild(
        "desugar",
        genNameForNode(qual_base_type.getTypePtr())
    );

    return true;
}

bool DotVisitor::VisitRValueReferenceType(clang::RValueReferenceType * rvalue_reference_type) {
    std::string name = genNameForNode(rvalue_reference_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitSubstTemplateTypeParmPackType(clang::SubstTemplateTypeParmPackType * subst_template_type_parm_pack_type) {
    std::string name = genNameForNode(subst_template_type_parm_pack_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType * subst_template_type_parm_type) {
    std::string name = genNameForNode(subst_template_type_parm_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Template Parameter Substituted Type");

    dot_node_it->second.setChild(
        "replaced_template_param",
        genNameForNode(subst_template_type_parm_type->getReplacedParameter())
    );

    clang::QualType replacement_qual_type = subst_template_type_parm_type->getReplacementType();

    // FIXME type qualifiers

    dot_node_it->second.setChild(
        "replacement_type",
        genNameForNode(replacement_qual_type.getTypePtr())
    );

    clang::QualType desugar_qual_type = subst_template_type_parm_type->desugar();

    // FIXME type qualifiers

    dot_node_it->second.setChild(
        "desugar",
        genNameForNode(desugar_qual_type.getTypePtr())
    );

    return true;
}

bool DotVisitor::VisitTagType(clang::TagType * tag_type) {
    std::string name = genNameForNode(tag_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitEnumType(clang::EnumType * enum_type) {
    std::string name = genNameForNode(enum_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitRecordType(clang::RecordType * record_type) {
    std::string name = genNameForNode(record_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Class/Struct Type");

    dot_node_it->second.setChild(
        "declaration",
        genNameForNode(record_type->getDecl())
    );

    return true;
}

bool DotVisitor::VisitTemplateSpecializationType(clang::TemplateSpecializationType * template_specialization_type) {
    std::string name = genNameForNode(template_specialization_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Template Specialization Type");


    std::string aliased_type;
    if (template_specialization_type->isTypeAlias()) {
        clang::QualType aliased_qual_type = template_specialization_type->getAliasedType();

        // FIXME type qualifiers

        aliased_type = genNameForNode(aliased_qual_type.getTypePtr());
    }
    else
        aliased_type = genNameForEmptyNode();

    dot_node_it->second.setChild(
        "aliased_type",
        aliased_type
    );

    clang::QualType desugar_qual_type = template_specialization_type->desugar();

    // FIXME type qualifiers

    dot_node_it->second.setChild(
        "desugar",
        genNameForNode(desugar_qual_type.getTypePtr())
    );

    for (unsigned cnt = 0; cnt < template_specialization_type->getNumArgs(); cnt++) {
        std::ostringstream oss;
        oss << "template_argument[" << cnt << "]";

        const clang::TemplateArgument & tpl_arg = template_specialization_type->getArg(cnt);

        std::string node_id;

        switch (tpl_arg.getKind()) {
            case clang::TemplateArgument::Type:
            {
                clang::QualType tpl_arg_qual_type = tpl_arg.getAsType();

                // FIXME type qualifiers

                node_id = genNameForNode(tpl_arg_qual_type.getTypePtr());

                break;
            }
            case clang::TemplateArgument::Declaration: node_id = genNameForNode(tpl_arg.getAsDecl()); break;
            case clang::TemplateArgument::Expression: node_id = genNameForNode(tpl_arg.getAsExpr()); break;
//            case : node_id = ; break; // TODO 5 other cases...
            default:
                std::cerr << "Unsupported template argument type..." << std::endl;
                exit(-1);
        }

        dot_node_it->second.setChild(
            oss.str(),
            node_id
        );
    }

    return true;
}

bool DotVisitor::VisitTemplateTypeParmType(clang::TemplateTypeParmType * template_type_parm_type) {
    std::string name = genNameForNode(template_type_parm_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Template Type Param Type");

    dot_node_it->second.setChild(
        "declaration",
        genNameForNode(template_type_parm_type->getDecl())
    );

    clang::QualType qual_type = template_type_parm_type->desugar();

    // FIXME type qualifiers

    dot_node_it->second.setChild(
        "desugar",
        genNameForNode(qual_type.getTypePtr())
    );

    return true;
}

bool DotVisitor::VisitTypedefType(clang::TypedefType * typedef_type) {
    std::string name = genNameForNode(typedef_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTypeOfExprType(clang::TypeOfExprType * type_of_expr_type) {
    std::string name = genNameForNode(type_of_expr_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDependentTypeOfExprType(clang::DependentTypeOfExprType * dependent_type_of_expr_type) {
    std::string name = genNameForNode(dependent_type_of_expr_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTypeOfType(clang::TypeOfType * type_of_type) {
    std::string name = genNameForNode(type_of_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitTypeWithKeyword(clang::TypeWithKeyword * type_with_keyword) {
    std::string name = genNameForNode(type_with_keyword);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDependentNameType(clang::DependentNameType * dependent_name_type) {
    std::string name = genNameForNode(dependent_name_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitDependentTemplateSpecializationType(clang::DependentTemplateSpecializationType * dependent_template_specialization_type) {
    std::string name = genNameForNode(dependent_template_specialization_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitElaboratedType(clang::ElaboratedType * elaborated_type) {
    std::string name = genNameForNode(elaborated_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    dot_node_it->second.setTitle("Elaborated Type");

    clang::QualType named_type = elaborated_type->getNamedType();

    dot_node_it->second.setChild(
        "base_type",
        genNameForNode(named_type.getTypePtr())
    );

    return true;
}

bool DotVisitor::VisitUnaryTransformType(clang::UnaryTransformType * unary_transform_type) {
    std::string name = genNameForNode(unary_transform_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitUnresolvedUsingType(clang::UnresolvedUsingType * unresolved_using_type) {
    std::string name = genNameForNode(unresolved_using_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitVectorType(clang::VectorType * vector_type) {
    std::string name = genNameForNode(vector_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

bool DotVisitor::VisitExtVectorType(clang::ExtVectorType * ext_vector_type) {
    std::string name = genNameForNode(ext_vector_type);
    std::map<std::string, DotNode>::iterator dot_node_it = p_ast_nodes_map.find(name);
    if (dot_node_it == p_ast_nodes_map.end()) {
        std::cerr << "Runtime Error: non-top class instance have not been register." << std::endl;
        exit(1);
    }

    // TODO

    return true;
}

// struct DotNode

DotNode::DotNode(NodeClass class_node, std::string identifier) :
    p_children(),
    p_attributes(),
    p_title(),
    p_class_node(class_node),
    p_identifier(identifier),
    p_compiler_generated(false)
{}

DotNode::~DotNode() {}

void DotNode::setCompilerGenerated(bool val) { p_compiler_generated = val; }

bool DotNode::isCompilerGenerated() { return p_compiler_generated; }

std::string DotNode::getLabel() {
    std::string res = p_title + "\\n\\n";

    std::map<std::string, std::string>::iterator it;
    for (it = p_attributes.begin(); it != p_attributes.end(); it++) {
        res += it->first + " : " + it->second + "\\n";
    }

    return res;
}

void DotNode::addAttribute(std::string label, std::string val) {
    std::map<std::string, std::string>::iterator it = p_attributes.find(label);

    if (it == p_attributes.end()) p_attributes.insert(std::pair<std::string, std::string>(label, val));
//  else it->second += ", " + val;
}

void DotNode::setChild(std::string label, std::string node, Variant v) {
    p_children[label] = std::pair<std::string, Variant>(node, v);
}

void DotNode::setTitle(std::string title) {
    p_title = title;
}

void DotNode::applySourceRange(const clang::SourceRange & src_range, const clang::SourceManager & source_manager) {
#define COLLECT_SOURCE_RANGE 0
#if COLLECT_SOURCE_RANGE
    clang::SourceLocation begin  = src_range.getBegin();
    clang::SourceLocation end    = src_range.getEnd();

    clang::FileID file_begin = source_manager.getFileID(begin);
    clang::FileID file_end   = source_manager.getFileID(end);

    bool inv_begin_line;
    bool inv_begin_col;
    bool inv_end_line;
    bool inv_end_col;

    unsigned line_begin = source_manager.getSpellingLineNumber(begin, &inv_begin_line);
    unsigned col_begin  = source_manager.getSpellingColumnNumber(begin, &inv_begin_col);
    unsigned line_end   = source_manager.getSpellingLineNumber(end, &inv_end_line);
    unsigned col_end    = source_manager.getSpellingColumnNumber(end, &inv_end_col);

    if (inv_begin_line || inv_begin_col || inv_end_line || inv_end_col || file_begin != file_end) {
        std::cerr << "Impossible to retrieve a valid position for this code fragment." << std::endl;
        exit(1);
    }

    addAttribute("file_name", source_manager.getFileEntryForID(file_begin)->getName());

    std::ostringstream oss;

    oss << line_begin;
    addAttribute("line_begin", oss.str());
    oss.clear();

    oss << col_begin;
    addAttribute("col_begin", oss.str());
    oss.clear();

    oss << line_end;
    addAttribute("line_end", oss.str());
    oss.clear();

    oss << col_end;
    addAttribute("col_end", oss.str());
    oss.clear();
#endif /* COLLECT_SOURCE_RANGE */
}

void DotNode::useAsDeclContext(clang::DeclContext * decl_context, DotVisitor & dot_visitor) {
    addAttribute("declaration_kind", decl_context->getDeclKindName());

    setChild("parent", dot_visitor.findNameForDeclContext(decl_context->getParent()), DotNode::decl_context);
    setChild("lexical_parent", dot_visitor.findNameForDeclContext(decl_context->getLexicalParent()), DotNode::decl_context);
    setChild("lookup_parent", dot_visitor.findNameForDeclContext(decl_context->getLookupParent()), DotNode::decl_context);
    setChild("primary_context", dot_visitor.findNameForDeclContext(decl_context->getPrimaryContext()), DotNode::decl_context);
    setChild("redecl_context", dot_visitor.findNameForDeclContext(decl_context->getRedeclContext()), DotNode::decl_context);
    setChild("enclosing_context", dot_visitor.findNameForDeclContext(decl_context->getEnclosingNamespaceContext()), DotNode::decl_context);
    setChild("next_context", dot_visitor.findNameForDeclContext(decl_context->getNextContext()), DotNode::decl_context);

    clang::DeclContext::decl_iterator it;
    unsigned cnt = 0;
    for (it = decl_context->decls_begin(); it != decl_context->decls_end(); it++) {
        std::ostringstream oss;

        oss << "decl[" << cnt << "]";

        setChild(oss.str(), dot_visitor.genNameForNode(*it), DotNode::decl_context);

        cnt++;
    }
}

void DotNode::toDot(std::ostream & out, std::string indent) {
    out << indent << p_identifier << " [";
    out << "label=\"" << getLabel() << "\"";
    out << ", style=filled, color=\"";
    switch (p_class_node) {
        case Decl: out << ".2"; break;
        case Stmt: out << ".5"; break;
        case Type: out << ".8"; break;
    }
    if (!isCompilerGenerated())
        out << " .5 .8\"];" << std::endl;
    else
        out << " .3 .8\"];" << std::endl;

    std::map<std::string, std::pair<std::string, Variant> >::iterator it;
    for (it = p_children.begin(); it != p_children.end(); it++) {
        out << indent << "\t" << p_identifier << " -> " << it->second.first << " [";
        out << "label=\"" << it->first << "\"";

        switch (it->second.second) {
            case none: out << ""; break;
            case decl_context: out << ", color=\"red\""; break;
        }

        out << "]" << std::endl;
    }

    out << std::endl; 
}

