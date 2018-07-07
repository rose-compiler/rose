#ifndef ARITHCHECK_C
#define ARITHCHECK_C
#include "ArithCheck.h"

#include <string>


//#define GEFD(node) GEFD(node, true)

//using namespace SageInterface;
//using namespace SageBuilder;


SgType* ArithCheck::created_struct_type;
//unsigned int Util::VarCounter = 0;
//unsigned int Util::VarCounter = 0;
TypeMap_t ArithCheck::UnderlyingType;
ScopeTypeSet ArithCheck::TypesInScope;
#ifdef CPP_SCOPE_MGMT
Scope2TypeMap_t ArithCheck::RelevantStructType;
#else
TypeMap_t ArithCheck::RelevantStructType;
#endif
//NodeContainer ArithCheck::NodesToInstrument;
//NodeContainer ArithCheck::DeclsToClone;
NodeContainer ArithCheck::Clones;
SymbolMap_t ArithCheck::CloneSymbolMap;
VariableSymbolMap_t ArithCheck::varRemap;
VariableSymbolMap_t ArithCheck::ReverseMap;
VariableSymbolMap_t ArithCheck::ClassStructVarRemap;
SgFunctionDeclaration* ArithCheck::MallocWrapFnDecl;
FuncInfoList ArithCheck::ForwardDecls;
uint64_t ArithCheck::TravCtr = 0;
StringClassMap_t ArithCheck::TransClassDecls;
TypeMap_t ArithCheck::RelevantArgStructType;
NameVarMap_t ArithCheck::VarDeclForName;
ExprTypeMap_t ArithCheck::OriginalVarType;
ExprMap_t ArithCheck::OriginalVarRef;
ExprMap_t ArithCheck::OriginalExp;
SgProject* ArithCheck::ACProject;
ScopeLockKeyMap ArithCheck::SLKM;
Rose_STL_Container<SgClassDeclaration*> ArithCheck::str_decls;


SgInitializedName* Util::getNameForDecl(SgVariableDeclaration* decl);
void Util::printNodeExt(SgNode* node);

std::string Util::generateNameForType(SgType* ptr_type);



#ifdef CHECK_HDR_FILES
void checkIfGeneratedAndInsert(SgClassDeclaration* str_decl) {

    SgClassDefinition* str_def = str_decl->get_definition();

    ROSE_ASSERT(str_def != NULL);

    SgDeclarationStatementPtrList& members = str_def->get_members();
    
    if(members.size() != GENERATED_STRUCT_SIZE) {
        // Structs that we generate have GENERATED_STRUCT_SIZE number
        // of elems
        return;
    }


    SgDeclarationStatementPtrList::iterator iter = members.begin();

    SgDeclarationStatement* decl_stmt = *iter;
    SgVariableDeclaration* ptr_decl = isSgVariableDeclaration(decl_stmt);
    if(!ptr_decl) {
        // First element is not a pointer. Quit
        return;
    }

    // Not checking for second variable being an address type variable. Skipping it. 
    // Could implement it if necessary

    // Check if the name of the struct can be generated from the ptr decl type
    // using Util::generateNameForType
    SgInitializedName* var_name = Util::getNameForDecl(ptr_decl);
    SgType* ptr_type = var_name->get_type();
    SgName gen_name(Util::generateNameForType(ptr_type));

    if(strcmp(str_decl->get_name().getString().c_str(), 
                gen_name.getString().c_str()) == 0) {
        // Now, we insert this directly into the relevant struct type and underlying
        // type structures
        ArithCheck::UnderlyingType.insert(TypeMap_t::value_type(str_decl->get_type(), ptr_type));
        ArithCheck::RelevantStructType.insert(TypeMap_t::value_type(ptr_type, str_decl->get_type()));
    }
}

class ClassDeclsVisitor : public ROSE_VisitorPattern {
    public:
    void visit(SgClassDeclaration* str_decl) {
        if(str_decl->isForward()) {
            //printf("Forward str decl. nothing to do\n");
            return;
        }
        // Figure out if this str_decl is one that we constructed... for that,
        // we check if the name of the struct decl can be constructed from its
        // its member (which has to be a pointer.
        checkIfGeneratedAndInsert(str_decl);

        //printf("Found str_decl:%s\n", str_decl->unparseToString().c_str());
        ArithCheck::str_decls.push_back(str_decl);
    }
};
#endif

void printInfo(SgBinaryOp* Parent, SgExpression* LHS, SgExpression* RHS) {
    Sg_File_Info* ParentFile = isSgNode(Parent)->get_file_info();
    printf("----------------------------------------\n");
    printf("BinaryOp: (%s, %d, %d) %s = %s\n", ParentFile->get_filenameString().c_str(),
                                            ParentFile->get_line(),
                                            ParentFile->get_col(),
                                            isSgNode(Parent)->sage_class_name(),
                                            isSgNode(Parent)->unparseToString().c_str());


    printf("Operand: LHS: %s = %s\n", isSgNode(LHS)->sage_class_name(), isSgNode(LHS)->unparseToString().c_str());
    printf("Operand: RHS: %s = %s\n", isSgNode(RHS)->sage_class_name(), isSgNode(RHS)->unparseToString().c_str());
    printf("----------------------------------------\n");
}


#if 0
bool Util::isFree(SgFunctionCallExp* fncall) {
    if(fncall->getAssociatedFunctionDeclaration() == NULL) {
        return false;
    }

    return (strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "free") == 0);
}
#endif

char* ArithCheck::getStringFor(enum Util::OVL_TYPE op) {
    char* output = (char*)malloc(50*sizeof(char));
    switch(op) {
    case Util::DEREF: sprintf(output, "Deref"); break;
    case Util::CAST: sprintf(output, "Cast"); break;
    case Util::SPECIAL_CAST: sprintf(output, "SpecialCast"); break;
    case Util::STRUCT: sprintf(output, "Struct"); break;
    case Util::NORMAL: sprintf(output, "Normal"); break;
    default: sprintf(output, "DontKnow"); break;
    }

    return output;
}

        
char* getString(VariantT Res, VariantT Op, VariantT LHS, VariantT RHS) {

    char* ResString = Util::getStringForVariantT(Res);
    char* OpString = Util::getStringForVariantT(Op);
    char* LHSString = Util::getStringForVariantT(LHS);
    char* RHSString = Util::getStringForVariantT(RHS);

    char* FunctionBuf = (char*)malloc(50*sizeof(char));
    sprintf(FunctionBuf, "%s_%s_%s_%s", ResString, OpString, LHSString, RHSString);
    //printf("FunctionBuf: %s\n", FunctionBuf);
    return FunctionBuf;
}


// We need to treat variables of ArrayType the same way as we
// treat the PointerType -- i.e. everywhere the ArrayType is in an
// op, we need to overload it. In many cases, we wouldn't catch the
// array type, but the output of V_SgPntrArrRefExp --- which
// is similar to Deref

#if 0
bool Trav::NeedsToBeOverloaded(SgNode* node) {
    
    switch(node->variantT()) {
    case V_SgAddOp:
    case V_SgSubtractOp:
    case V_SgPlusPlusOp:
    case V_SgMinusMinusOp:
    case V_SgPlusAssignOp:
    case V_SgMinusAssignOp:
    case V_SgAssignOp: return true; 
    default: return false;
    }
}
#endif

#if 0
void CheckExprSanity(SgExpression* expr) {
    
    SgType* expr_type = expr->get_type();
    #ifdef STRIP_TYPEDEFS
    expr_type = Util::getType(expr_type);
    #endif

    if(Util::isQualifyingType(expr_type)) {
        if(SgBinaryOp* bop = isSgBinaryOp(expr)) {
            SgType* lhs_type = bop->get_lhs_operand()->get_type();
            #ifdef STRIP_TYPEDEFS
            lhs_type = Util::getType(lhs_type);
            #endif

            SgType* rhs_type = bop->get_rhs_operand()->get_type(); 
            #ifdef STRIP_TYPEDEFS
            rhs_type = Util::getType(rhs_type);
            #endif

            ROSE_ASSERT(Util::isQualifyingType(lhs_type) ||
                        Util::isQualifyingType(rhs_type));
        }
        else if(SgUnaryOp* uop = isSgUnaryOp(expr)) {
            SgType* oper_type = uop->get_operand()->get_type();
            #ifdef STRIP_TYPEDEFS
            oper_type = Util::getType(oper_type);
            #endif

            ROSE_ASSERT(Util::isQualifyingType(oper_type));
        }
        else {
            printf("Can't check sanity for expr:\n");
            printf("expr: %s = %s\n", isSgNode(expr)->sage_class_name(), isSgNode(expr)->unparseToString().c_str());
            ROSE_ASSERT(0);
        }
    }
}
#endif

#if 0
bool Trav::isQualifyingLibCall(SgFunctionCallExp* fncall) {

    if(fncall->getAssociatedFunctionDeclaration() == NULL) {
        return false;
    }

    // get the namespace for the function.. and if the namespace is
    // std... then label the function as a lib call
    #ifdef EXCLUDE_STD_NAMESP_FUNC
    SgFunctionDeclaration* fn_decl = fncall->getAssociatedFunctionDeclaration();
    if(Util::isInNamespace(fn_decl, "std")) {
        return true;
    }
    #endif


    std::string LibFns[] = {"free",
                            "printf",
                            "atoi",
                            "atol",
                            "sscanf",    
                            "fprintf",
                            "open",
                            "read",
                            "__builtin_object_size",
                            "__builtin___memset_chk",
                            "__inline_memset_chk",
                            "__builtin___memcpy_chk",
                            "__inline_memcpy_chk",
                            "__builtin___memmove_chk",
                            "__inline_memmove_chk",
                             "__builtin___strcpy_chk",
                            "__inline_strcpy_chk",
                            "__builtin___strncpy_chk",
                            "__inline_strncpy_chk",
                            "__builtin___strcat_chk",
                            "__inline_strcat_chk",
                            "__builtin___strncat_chk",
                            "__inline_strncat_chk",
                            "memset",
                            "memcpy",
                            "strlen",
                            "strchr",
                            "strpbrk",
                            "strspn",
                            "strstr",
                            "strtok",
                            "strcmp",
                            "feof",
                            "fopen",
                            "rewind",
                            "fscanf",
                            "fclose",
                            "fgetc",
                            "gets",
                            "fgets",
                            "fputc",
                            "__assert_rtn",
                            "__assert_fail",
                            "__builtin___sprintf_chk",
                            "fflush",
                            "__error",
                            "strerror",
                            "perror",
                            "remove",
                            "pthread_create",
                            "pthread_join",
                            "pthread_exit",
                            "pthread_mutex_init",
                            "pthread_mutex_lock",
                            "pthread_mutex_unlock",
                            "__errno_location()"
                            };

    for(unsigned int index = 0; index < sizeof(LibFns)/sizeof(std::string); index++) {
        if(strcmp(LibFns[index].c_str(), 
            fncall->getAssociatedFunctionDeclaration()->get_name().str()) == 0) {
            return true;
        }
    }
    
    return false;
}
#endif

bool checkIfStatic(SgDeclarationStatement* decl_stmt) {

    SgDeclarationModifier& decl_mod = decl_stmt->get_declarationModifier();    
    SgStorageModifier& stor_mod = decl_mod.get_storageModifier();
    #ifdef STATIC_DEBUG
    printf("checkIfStatic:\n");
    printf("decl_stmt: ");
    printf("%s = %s\n", decl_stmt->sage_class_name(), decl_stmt->unparseToString().c_str());
    printf("static: ");
    #endif
    if(stor_mod.isStatic()) {
        #ifdef STATIC_DEBUG
        printf("true\n");
        #endif
        return true;
    }
    else {
        #ifdef STATIC_DEBUG
        printf("false\n");
        #endif
        return false;
    }
}

#if 0
bool candidateFnCall(SgNode* node) {
    SgFunctionCallExp* fncall = isSgFunctionCallExp(node);
    ROSE_ASSERT(fncall);
    #ifdef CAND_FN_DEBUG
    printf("candidateFnCall\n");
    #endif

    // Check if the return type is qual
    // if CLASS_STRUCT_COPY, return type is class/struct
    SgExprListExp* param_list = fncall->get_args();
    SgExprListExp* new_param_list = SB::buildExprListExp();

    SgExpressionPtrList& exprs = param_list->get_expressions();
    SgExpressionPtrList::iterator it = exprs.begin();

    for(; it != exprs.end(); ++it) {

        SgType* ce_type = (*it)->get_type();
        #ifdef CAND_FN_DEBUG
        Util::printNode(ce_type);
        #endif

        #ifdef STRIP_TYPEDEFS
        ce_type = Util::getType(ce_type);
        #endif

        if(Util::isQualifyingType(ce_type)) {
            return true;
        }

        #ifdef CLASS_STRUCT_COPY
        if(strDecl::isOriginalClassType(ce_type)) {
            return true;
        }
        #endif
    }        
    
    SgType* fncall_retType = fncall->get_type();
    #ifdef STRIP_TYPEDEFS
    fncall_retType = Util::getType(fncall_retType);
    #endif

    if(Util::isQualifyingType(fncall_retType)) {
        return true;
    }
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(fncall_retType)) {
        return true;
    }
    #endif
    else {
        return false;
    }
}
#endif


#if 0
bool candidateFnDecl(SgFunctionDeclaration* fn_decl) {
    // Find return type
    SgFunctionType* fn_type = fn_decl->get_type();
    SgType* ret_type = fn_type->get_return_type();

    #ifdef STRIP_TYPEDEFS
    ret_type = Util::getType(ret_type);
    #endif

    if(Util::isQualifyingType(ret_type)) {
        return true;
    }
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(ret_type)) {
        return true;
    }
    #endif
    return false;
}
#endif

#if 0
Util::nodeType TopBotTrack3::evaluateSynthesizedAttribute(SgNode* node,
                Util::nodeType inh, SynthesizedAttributesList synList) {
    #ifdef SYNTH3_DEBUG
    printf("TopBotTrack3: Ev_Synth_Attr\n");
    printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());    
    Util::printNodeExt(node);
    #endif
    
    if(inh.inhattr == DOT_EXP) {
        #ifdef SYNTH3_DEBUG
        printf("synth:DOT_EXP: Node added\n");
        #endif
        // This is a dot or arrow exp
        ROSE_ASSERT(Util::isDotOrArrow(node));
        ArithCheck::NodesToInstrument.push_back(node);
    }
    else if(inh.inhattr == DOT_EXP_CHILD) {
        ROSE_ASSERT(Util::isDotOrArrow(node->get_parent()));
        // If the node is a var ref or this exp, no need to transform
        // If the node is the rhs of a dot or arrow, no need to transform
        if(!isSgVarRefExp(node) && !isSgThisExp(node) && !Util::isRHSOfDotOrArrow(node)) {
            // This is an expression besides a var ref exp within an enclosing dot expression... needs to be transformed.
            #ifdef SYNTH3_DEBUG
            printf("synth:WITHIN_DOT_EXP: Node added\n");
            #endif
            ArithCheck::NodesToInstrument.push_back(node);
        }
        else {
            #ifdef SYNTH3_DEBUG
            printf("synth:WITHIN_DOT_EXP: Node **not** added\n");
            #endif    
        }
    }

    // The attribute we are looking for is inherited, not synthesized
    // so, always propagate NOT_DOT_EXP and Util::INH_UNKNOWN from each node in the
    // synthesized traversal
    Util::nodeType synattr(Util::UNKNOWN,Util::INH_UNKNOWN);
    return synattr;
}

Util::nodeType TopBotTrack3::evaluateInheritedAttribute(SgNode* node,
                                                Util::nodeType inh) {
    #ifdef INH3_DEBUG
    printf("TopBotTrack3: Ev_Inh_Attr\n");
    printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());    
    #endif
    
    if(Util::isDotOrArrow(node)) {
        #ifdef INH3_DEBUG
        printf("inh:DotOrArrow: WITHIN_DOT_EXP\n");
        #endif
        Util::nodeType new_inh(Util::UNKNOWN, DOT_EXP);    
        return new_inh;
    }
    else if(inh.inhattr == DOT_EXP) {
        // This is a node that is inheriting DOT_EXP
        // from its parent... which is a dot or arrow
        ROSE_ASSERT(Util::isDotOrArrow(node->get_parent()));
        if(Util::isRHSOfDotOrArrow(node)) {
            #ifdef INH3_DEBUG
            printf("inh:RHS: DOT_EXP: Util::INH_UNKNOWN\n");
            #endif
            // RHS of dot or arrow, nothing to do on this side
            Util::nodeType new_inh(Util::UNKNOWN, Util::INH_UNKNOWN);
            return new_inh;
        }
        else {
            #ifdef INH3_DEBUG
            printf("inh:LHS: DOT_EXP: WITHIN_DOT_EXP\n");
            #endif
            // But since this node by itself isn't 
            // a dot or arrow, nothing to do here...
            Util::nodeType new_inh(Util::UNKNOWN, DOT_EXP_CHILD);
            return new_inh;
        }
    }
    else if(inh.inhattr == DOT_EXP_CHILD) {
        // This is now the second generation... the child of
        // the child. Nothing to do...
        #ifdef INH3_DEBUG
        printf("inh:WITHIN_DOT_EXP: Util::INH_UNKNOWN\n");
        #endif
        Util::nodeType new_inh(Util::UNKNOWN, Util::INH_UNKNOWN);
        return new_inh;
    }
    else {
        Util::nodeType new_inh(Util::UNKNOWN, Util::INH_UNKNOWN);
        return new_inh;
    }
}    
#endif
    
#if 0
// Not using inherited attribute since we aren't looking at classes and data
// structures for now
Util::nodeType TopBotTrack2::evaluateSynthesizedAttribute(SgNode* node,
                Util::nodeType inh, SynthesizedAttributesList synList) {
    #ifdef SYNTH_DEBUG
    printf("TopBotTrack2: Ev_Synth_Attr\n");
    printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());    
    Util::printNodeExt(node);
    #endif

    #ifdef CHECK_STATIC
    if(isSgDeclarationStatement(node)) {
        Util::checkIfStatic(isSgDeclarationStatement(node));
    }
    #endif

    #ifdef DISCARD_GENERATED_NODES
    Sg_File_Info* File = isSgNode(node)->get_file_info();
    if(strcmp(File->get_filenameString().c_str(), "compilerGenerated") == 0) {
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;
    }
    #endif


    
    #ifdef SYNTH_DEBUG
    printf("Is variable declaration? ");
    #endif
    if(isSgVariableDeclaration(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif
            
        SgVariableDeclaration* var_decl = isSgVariableDeclaration(node);

        SgInitializedName* var_name = Util::getNameForDecl(var_decl);
        
    
        SgType* var_type = Util::getType(var_name->get_type());
        #ifdef STRIP_TYPEDEFS
        var_type = Util::getType(var_type);
        #endif
        
        #ifdef ALLOW_UNTRACKED_VARIABLES
        // Even if this is a var decl that we don't intend to track,
        // we should maintain its metadata, since it might be part of an expression
        // for which we require metadata. In effect, allowing untracked variables only
        // removes the var refs... and other related expressions from contention, not
        // the metadata gathering itself.
        
        #else
        if(Util::isQualifyingType(var_type) && (var_name->get_initializer() != NULL)) {
            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        #ifdef CLASS_STRUCT_COPY
        else if(strDecl::isOriginalClassType(var_type) && (var_name->get_initializer() != NULL)) {
            // Only handling assign initializers
            if(isSgAssignInitializer(var_name->get_initializer())) {
                // Now, this var decl only matters if its being initialized here
                // Hence, check if it has an initializer...
                ArithCheck::NodesToInstrument.push_back(node);
                Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
                return synattr;
            }
        }
        #endif
        else {
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        #endif

    }
    else {
        #ifdef SYNTH_DEBUG
        // Not var decl at all. don't do anything
        printf("No\n");
        #endif
    }


    #ifdef SYNTH_DEBUG
    printf("is variable reference? ");
    #endif
    if(isSgVarRefExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        printf("Is it an lvalue? %s", isSgVarRefExp(node)->get_lvalue() ? "Yes\n" : "No\n");
        printf("Is it an LVal? %s", isSgVarRefExp(node)->isLValue() ? "Yes\n" : "No\n");
        #endif

        SgVarRefExp* var_ref = isSgVarRefExp(node);
        SgType* var_type = var_ref->get_type();
        #ifdef STRIP_TYPEDEFS
        var_type = Util::getType(var_type);
        #endif

        // Require this to replace var refs to str from varRemap -- for
        // params to a function
        if(Util::isQualifyingType(var_type)) {
            if(strcmp(var_ref->get_symbol()->get_name().getString().c_str(),"Fibonacci") == 0) {
                printf("Found Fibonacci\n");
            }
            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        // Require this to replace var refs with reference versions
        // from ClassStructVarRemap -- for params to a function
        #ifdef CLASS_STRUCT_COPY
        else if(strDecl::isOriginalClassType(var_type)) {
            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        #endif
        else {
            #ifdef INIT_CHECKS
            // For performing initialization checks, we need to track all
            // the uses of variables
            ArithCheck::NodesToInstrument.push_back(node);
            #endif
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }

    }
    else {
        // Not var decl at all. don't do anything
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    #ifdef INSTR_THIS_EXP
    #ifdef SYNTH_DEBUG
    printf("is this exp? ");
    #endif
    if(isSgThisExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif
        // "this" is a pointer, so we'll be converting it to
        // struct type
        ArithCheck::NodesToInstrument.push_back(node);
        Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
        return synattr;
    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }
    #endif
        


    #ifdef SYNTH_DEBUG
    printf("is function ref? ");
    #endif
    if(isSgFunctionRefExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        SgFunctionRefExp* fnref = isSgFunctionRefExp(node);
        // Just that we know that we are dealing with a function pointer I guess...? 
    }
    else {
        // Not a function ref. 
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    #ifdef SYNTH_DEBUG
    printf("Is it a function call?");
    #endif
    if(isSgFunctionCallExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif
        SgFunctionCallExp* fncall = isSgFunctionCallExp(node);

        #ifdef PANIC_DEBUG
        SgFunctionDeclaration* fn_decl = fncall->getAssociatedFunctionDeclaration();
        ROSE_ASSERT(fn_decl);
        if(strcmp(fn_decl->get_name().getString().c_str(), "panic") == 0) {
            printf("FnCall:Found panic\n");
            SgExprListExp* args = fncall->get_args();
            SgExpressionPtrList& exprs = args->get_expressions();
            SgExpressionPtrList::iterator iter = exprs.begin();
            for(; iter != exprs.end(); iter++) {
                SgExpression* exp = *iter;
                printf("exp: \n");
                Util::printNode(exp);
            }
            printf("printing args\n");
            // Associated Function Declaration.. param list types
            SgFunctionDeclaration* fn_decl = fncall->getAssociatedFunctionDeclaration();
            SgInitializedNamePtrList& decl_args = fn_decl->get_args();
            SgInitializedNamePtrList::iterator iter2 = decl_args.begin();
            for(; iter2 != decl_args.end(); iter2++) {
                SgInitializedName* arg = *iter2;
                printf("arg: \n");
                Util::printNode(arg);
                printf("arg_type: \n");
                Util::printNode(arg->get_type());
            }
            printf("Done printing panic\n");
        }
        #endif

        // Don't need to check for implicit casts... So, lets filter out
        // the useless ones here.
        // Always add... to check for implicit casts in the arguments
        
        // FIXME: If the fncall returns a class/struct or ptr/qual.
        // Or if one or more arguments is ptr/qual or class/struct. 

        if(candidateFnCall(node)) {
            #ifdef SYNTH_DEBUG
            printf("Added to NodesToInstrument\n");
            #endif
            ArithCheck::NodesToInstrument.push_back(node);    
        }

        SgType* fncall_type = fncall->get_type();
        #ifdef STRIP_TYPEDEFS
        fncall_type = Util::getType(fncall_type);
        #endif

        if(Util::isQualifyingType(fncall_type)) {
//            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else if(Util::isFree(fncall)) {
//            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else if(Trav::isQualifyingLibCall(fncall)) {
//            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else {
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }

        
    }
    else {
        // not a func call exp. don't do anything
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    // Shifted to Inherited Attribute...
    #if 0
    #ifdef LOCK_KEY_INSERT
    printf("LOCK_KEY_INSERT: Is it a basic block?");
    if(isSgBasicBlock(node)) {
        printf("Yes\n");
        ArithCheck::NodesToInstrument.push_back(node);
    }
    else {
        printf("No\n");
    }
    #endif
    #endif


    // FIXME: In the case of deref exp, we can't blind move from
    // Util::STR_TYPE to Util::NOT_STR_TYPE since it could be a double pointer
    // in which case, the return type would also be Util::STR_TYPE
    // We might need to investigate how our technique would handle
    // such a case since we shouldn't always return VoidStruct or
    // the POD type for a deref. The return type changes based on 
    // the input type... which is not known in VoidStruct
    // But we would know this statically, so, we could use a different
    // Deref_Overload_DoublePtr in that case which would output
    // a VoidStruct
    
    // In case of a deref_exp we would need to check here if the
    // operand is a single or double pointer, since that defines
    // where we return a Util::STR_TYPE or a Util::NOT_STR_TYPE

    // FIXME: I don't think the code below detects double pointers correctly
    // And, I am not sure, how we would actually handle double pointer cases
    // This piece of code is causing the runs to crash for some reason.
    // Commenting it out
    #if 1
    #ifdef SYNTH_DEBUG
    printf("is it a deref exp?");
    #endif
    if(isSgPointerDerefExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        SgPointerDerefExp* ptr_deref = isSgPointerDerefExp(node);

        
        // Check that all derefs do in fact, operate on pointers or
        // arrays!
        SgExpression* oper = ptr_deref->get_operand();
        SgType* oper_type = oper->get_type();
        #ifdef STRIP_TYPEDEFS
        oper_type = Util::getType(oper_type);
        #endif
        #ifdef SUPPORT_REFERENCES
        ROSE_ASSERT(Util::isQualifyingType(oper_type) || isSgReferenceType(oper_type));
        #else
        ROSE_ASSERT(Util::isQualifyingType(oper_type));
        #endif
        

        // There is always work to do on a deref!
        ArithCheck::NodesToInstrument.push_back(node);

        SgType* ptr_deref_type = ptr_deref->get_type();
        #ifdef STRIP_TYPEDEFS
        ptr_deref_type = Util::getType(ptr_deref_type);
        #endif

        if(Util::isQualifyingType(ptr_deref_type)) {
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else {
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
    
        
    }
    else {
        // Not a pointer deref exp. do nothing
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }
    #endif
        
    #ifdef SYNTH_DEBUG
    printf("is it a pntr arr ref exp? ");
    #endif
    if(isSgPntrArrRefExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        // It is possible that the pntr arr ref is not
        // used by a ptr/array. Case in question:
        // #define PART_SEP "-"
        // PART[0] -- This is a macro based use of pntr arr ref.
        SgPntrArrRefExp* array_ref = isSgPntrArrRefExp(node);
        SgType* lhs_type = array_ref->get_lhs_operand()->get_type();
        #ifdef STRIP_TYPEDEFS
        lhs_type = Util::getType(lhs_type);
        #endif

        if(!Util::isQualifyingType(lhs_type)) {
            #ifdef SYNTH_DEBUG
            printf("PntrArrRef. Not qualifing type... \n");
            Util::printNode(node);
            #endif
            // Nothing to do in this case.
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }

        

        // Check that all pnts arr refs operate on pointers or
        // arrays
        //SgExpression* oper = array_ref->get_lhs_operand();
        //ROSE_ASSERT(Util::isQualifyingType(oper->get_type()));
        ROSE_ASSERT(Util::isQualifyingType(lhs_type));

        // INIT_CHECKS: PntrArrRefs are always added to
        // the NodesToInstrument. So, nothing more to do.
        // There is always work to do on an pntr arr ref!
        #ifdef SYNTH_DEBUG
        printf("Added PntrArrRef to NodesToInstrument\n");
        #endif
        ArithCheck::NodesToInstrument.push_back(node);

        SgType* array_type = array_ref->get_type();
        #ifdef STRIP_TYPEDEFS
        array_type = Util::getType(array_type);
        #endif
        if(Util::isQualifyingType(array_type)) {
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else {
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
            
    }
    else {
        // Not a pointer deref exp. do nothing
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    #ifdef SYNTH_DEBUG
    printf("is it a dot exp? ");
    #endif
    if(isSgDotExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        SgDotExp* dot_exp = isSgDotExp(node);

        // Technically, only those dot expressions dealing with
        // pointers and arrays matter to us... since the others
        // are dealing with obj.var refs.
        SgType* dot_exp_type = dot_exp->get_type();
        #ifdef STRIP_TYPEDEFS
        dot_exp_type = Util::getType(dot_exp_type);
        #endif

        if(Util::isQualifyingType(dot_exp_type)) {
            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else {
            #ifdef INIT_CHECKS
            // For initialization, we need to know about all the
            // var refs-- which includes dot and arrow exps
            ArithCheck::NodesToInstrument.push_back(node);
            #endif
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    #ifdef SYNTH_DEBUG
    printf("is it an arrow exp? ");
    #endif
    if(isSgArrowExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        SgArrowExp* arrow_exp = isSgArrowExp(node);

        // Arrow expressions always work on pointers... and maybe
        // array types -- not sure how though. In any case,
        // has to qualifying type. And there's work to do!
        //SgExpression* oper = arrow_exp->get_lhs_operand();
        //ROSE_ASSERT(Util::isQualifyingType(oper->get_type()));
        SgType* lhs_type = arrow_exp->get_lhs_operand()->get_type();
        #ifdef STRIP_TYPEDEFS
        lhs_type = Util::getType(lhs_type);
        #endif
        ROSE_ASSERT(Util::isQualifyingType(lhs_type));

        // INIT_CHECKS: All arrow exps are already added
        // to NodesToInstrument. Nothing more to do.
        // There is always work to do on an arrow exp!
        ArithCheck::NodesToInstrument.push_back(node);
        
        SgType* arrow_type = arrow_exp->get_type();
        #ifdef STRIP_TYPEDEFS
        arrow_type = Util::getType(arrow_type);
        #endif
        
        if(Util::isQualifyingType(arrow_type)) {
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else {
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    // Handle cast exp here itself since it can create a pointer out
    // of nowhere, through arbitrary casts.
    #ifdef SYNTH_DEBUG
    printf("is it a cast exp? ");
    #endif
    if(isSgCastExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        SgCastExp* cast_exp = isSgCastExp(node);

        // If the operand or
        // the return type are of qualifying type, then
        // there is work to do.
        SgType* cast_type = cast_exp->get_type();
        #ifdef STRIP_TYPEDEFS
        cast_type = Util::getType(cast_type);
        #endif

        SgType* oper_type = cast_exp->get_operand()->get_type();

        #ifdef STRIP_TYPEDEFS
        oper_type = Util::getType(oper_type);
        #endif

        if(Util::isQualifyingType(cast_type) || 
            Util::isQualifyingType(oper_type)) {
            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else {
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    // Address Of Op can also create a qualifying type out of any variable....
    // So, we are definitely looking at a qualifying type here.
    #ifdef SYNTH_DEBUG
    printf("is address of op? ");
    #endif
    if(isSgAddressOfOp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        SgAddressOfOp* addr_of = isSgAddressOfOp(node);
        SgType* addr_of_type = addr_of->get_type();
        #ifdef STRIP_TYPEDEFS
        addr_of_type = Util::getType(addr_of_type);
        #endif

        // Assert that we create a qualifying type
        ROSE_ASSERT(Util::isQualifyingType(addr_of_type));
        ArithCheck::NodesToInstrument.push_back(node);
        Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
        return synattr;
    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

            
    // Lets handle the comma op case, where the type is determined
    // by the last expression type
    // No need to add to NodesToInstrument since we don't have to
    // do anything for a comma op
    #ifdef SYNTH_DEBUG
    printf("is comma op?");
    #endif
    if(isSgCommaOpExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        // Find the type we are looking for in the 
        // last synthesized attribute
        Util::nodeType nt = synList[synList.size() - 1];
        if(nt.attr == Util::NOT_STR_TYPE) {
            #ifdef SYNTH_DEBUG
            printf("Not returning pointer from comma op\n");
            #endif
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else if(nt.attr == Util::STR_TYPE) {
            #ifdef SYNTH_DEBUG
            printf("Returning pointer from comma op\n");
            #endif
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else {
            // FIXME: Should we return UNKNOWN?
            printf("FIXME: Unknown type from comma op. Need to look at node type\n");
            ROSE_ASSERT(0);
        }
    }
    else {
        //Not comma op. don't do anything
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }


    // Conditional ops. They might take in structs, but always
    // return bool (Util::NOT_STR_TYPE)
    #ifdef SYNTH_DEBUG
    printf("is it a conditional op?");
    #endif
    if(Util::isConditionalOp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif
    
        SgBinaryOp* bop = isSgBinaryOp(node);

        SgType* lhs_type = bop->get_lhs_operand()->get_type();

        #ifdef STRIP_TYPEDEFS
        lhs_type = Util::getType(lhs_type);
        #endif

        SgType* rhs_type = bop->get_rhs_operand()->get_type();

        #ifdef STRIP_TYPEDEFS
        rhs_type = Util::getType(rhs_type);
        #endif
        

        if(Util::isQualifyingType(lhs_type) ||
            Util::isQualifyingType(rhs_type)) {
            ArithCheck::NodesToInstrument.push_back(node);
        }
        #ifdef SYNTH_DEBUG
        printf("Return Util::NOT_STR_TYPE\n");
        #endif
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;

    
    }
    else {
        // Not a conditional op. Nothing to do
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    #ifdef SYNTH_DEBUG
    printf("is it a param list?");
    #endif
    if(isSgFunctionParameterList(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        // Skip this function param list if its the main function
        SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(node->get_parent());
        ROSE_ASSERT(fn_decl != NULL);

        #ifdef PANIC_DEBUG
        if(strcmp(fn_decl->get_name().getString().c_str(), "panic") == 0) {
            printf("ParamList:Found panic\n");
        }
        #endif

        #ifdef SKIP_MAIN_PARAMS
        if(SI::isMain(fn_decl)) {
            #ifdef SYNTH_DEBUG
            printf("Main function param list found. Skipping\n");
            printf("Always returning Util::NOT_STR_TYPE\n");
            #endif
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        #endif


        SgInitializedNamePtrList& name_list = isSgFunctionParameterList(node)->get_args();

        for(SgInitializedNamePtrList::iterator iter = name_list.begin(); iter != name_list.end(); ++iter) {
            
            SgInitializedName* name = *iter;
            SgType* name_type = name->get_type();
            #ifdef STRIP_TYPEDEFS
            name_type = Util::getType(name_type);
            #endif

            if(Util::isQualifyingType(name_type)) {
                #ifdef SYNTH_DEBUG
                printf("Arg qualifies\n");
                #endif

                #ifdef CLONE_FUNCTIONS
                // If this function won't be inserted below... then insert it here... 
                // This makes sure that there is no duplication.
                if(!candidateFnDecl(fn_decl)) {    
                    if(SI::isMain(fn_decl)) {
                        printf("adding main at func param 1\n");
                    }
                    // add to decls to clone
                    Trav::DeclsToClone.push_back(fn_decl);
                }
                #endif

                ArithCheck::NodesToInstrument.push_back(node);
                break;
            }
            #ifdef CLASS_STRUCT_COPY
            else if(strDecl::isOriginalClassType(name_type)) {
                #ifdef SYNTH_DEBUG
                printf("Arg qualifies -- class type\n");
                #endif

                #ifdef CLONE_FUNCTIONS
                // If this function won't be inserted below... then insert it here... 
                // This makes sure that there is no duplication.
                if(!candidateFnDecl(fn_decl)) {    
                    if(SI::isMain(fn_decl)) {
                        printf("adding main at func param 2\n");
                    }
                    // add to decls to clone
                    Trav::DeclsToClone.push_back(fn_decl);
                }
                #endif


                ArithCheck::NodesToInstrument.push_back(node);
                break;
            }
            #endif
        }
        #ifdef SYNTH_DEBUG
        printf("Always returning Util::NOT_STR_TYPE\n");
        #endif
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;

    }
    else {
        // Not a param list. nothing to do
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

    #ifdef SYNTH_DEBUG
    printf("is it a func decl?");
    #endif
    if(isSgFunctionDeclaration(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif
        

        SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(node);

        #if 1
        if(candidateFnDecl(fn_decl)) {
            if(SI::isMain(fn_decl)) {
                printf("adding main at isSgFunctionDecl\n");
            }
            // add to decls to clone
            Trav::DeclsToClone.push_back(fn_decl);

            ArithCheck::NodesToInstrument.push_back(node);
        }
        #else
        // Find return type
        SgFunctionType* fn_type = fn_decl->get_type();
        SgType* ret_type = fn_type->get_return_type();

        #ifdef STRIP_TYPEDEFS
        ret_type = Util::getType(ret_type);
        #endif

        if(Util::isQualifyingType(ret_type)) {
            ArithCheck::NodesToInstrument.push_back(node);
        }
        #ifdef CLASS_STRUCT_COPY
        else if(strDecl::isOriginalClassType(ret_type)) {
            ArithCheck::NodesToInstrument.push_back(node);
        }
        #endif
        #endif
        
        #ifdef SYNTH_DEBUG
        printf("Always returning Util::NOT_STR_TYPE\n");
        #endif
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;


        
    }
    else {
        // Not a func decl, nothing to do
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }

//    #ifdef CLASS_STRUCT_COPY
    #ifdef SYNTH_DEBUG
    printf("is it a return stmt?");
    #endif
    if(isSgReturnStmt(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        
        #ifdef DELAYED_INSTR
        // Should add all returns stmts which return
        // pointers -- some of these pointers might need to
        // be converted to struct at the return stmt...
        // If the return exp is of array type, then
        // create_entry needs to be performed as well
        

        SgExpression* ret_exp = isSgReturnStmt(node)->get_expression();
        
        #ifdef LOCK_KEY_INSERT
        ArithCheck::NodesToInstrument.push_back(node);
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;
        #endif


        if(ret_exp != NULL) {
            SgType* ret_type = ret_exp->get_type();
            #ifdef STRIP_TYPEDEFS
            ret_type = Util::getType(ret_type);
            #endif

            #ifdef CLASS_STRUCT_COPY
            if(strDecl::isOriginalClassType(ret_type)) {
                ArithCheck::NodesToInstrument.push_back(node);
            }
            // If the ret_exp returning qual type stays as qual
            // type by the time we hit the return stmt (in InstrumentNodes4)
            // then we'll use create_struct and create_entry (if its 
            // an array
            else if(Util::isQualifyingType(ret_type)) {
                ArithCheck::NodesToInstrument.push_back(node);
            }
            #else
            if(Util::isQualifyingType(ret_type)) {
                ArithCheck::NodesToInstrument.push_back(node);
            }
            #endif
        }
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;

        #else
        #if 0
        #ifdef LOCK_KEY_INSERT
        // all return stmts need to be instrumented with exit scope
        #ifdef SYNTH_DEBUG
        printf("LOCK_KEY_INSERT: Adding to NodesToInstrument\n");
        #endif
        ArithCheck::NodesToInstrument.push_back(node);
        #ifdef SYNTH_DEBUG
        printf("Always returning Util::NOT_STR_TYPE\n");
        #endif
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;
        #else


        
        // Check if its of the OriginalClassType...
        #ifdef SYNTH_DEBUG
        printf("is it original class type?");
        #endif
        SgExpression* ret_exp = isSgReturnStmt(node)->get_expression();
        if(ret_exp == NULL) {
            // Nothing to do. This return statement has no expression associated with it.
            #ifdef SYNTH_DEBUG
            printf("No expression in the ret stmt\n");
            #endif
        }
        else {

            if(strDecl::isOriginalClassType(ret_exp->get_type())) {
                #ifdef SYNTH_DEBUG
                printf("Yes\n");
                #endif
                ArithCheck::NodesToInstrument.push_back(node);
            }
            else {
                #ifdef SYNTH_DEBUG
                printf("No\n");
                #endif
            }
        }
        #ifdef SYNTH_DEBUG
        printf("Always returning Util::NOT_STR_TYPE\n");
        #endif
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;


        #endif
        #endif
        #endif

    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }
//    #endif

    #ifdef CLASS_STRUCT_COPY
//    #if defined(CLASS_STRUCT_COPY) || defined(LOCK_KEY_INSERT)
    #ifdef SYNTH_DEBUG
    printf("is it an assign stmt?");
    #endif
    if(isSgAssignOp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif
        SgType* lhs_type = isSgAssignOp(node)->get_lhs_operand()->get_type();

        #ifdef STRIP_TYPEDEFS
        lhs_type = Util::getType(lhs_type);
        #endif

        if(strDecl::isOriginalClassType(lhs_type)) {
            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }
    #endif

    #ifdef NEW_OVL_ENABLED
    if(isSgNewExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        #ifdef NEW_EXP_DEBUG
        SgNewExp* ne = isSgNewExp(node);
        printf("new exp\n");
        Util::printNodeExt(ne);

        SgType* nty = ne->get_type();
        printf("new_exp->get_type()\n");
        Util::printNode(nty);
    
        SgType* sty = ne->get_specified_type();
        printf("new_exp->get_specified_type()\n");
        Util::printNode(sty);
        #endif


        ArithCheck::NodesToInstrument.push_back(node);
        Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
        return synattr;
    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }
    #endif

    #ifdef DELETE_OVL_ENABLED
    if(isSgDeleteExp(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif

        #ifdef DELETE_EXP_DEBUG
        #endif

        ArithCheck::NodesToInstrument.push_back(node);
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;
    }
    else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }
    #endif
        

    #ifdef SYNTH_DEBUG    
    printf("is it an overloadable op?");
    #endif
    // Arith ops...
    if(Trav::NeedsToBeOverloaded(node)) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif
        
        // These are all expressions
        ROSE_ASSERT(isSgBinaryOp(node) || isSgUnaryOp(node));
        SgExpression* expr = isSgExpression(node); 
        ROSE_ASSERT(expr != NULL);
        // The result of should be of qualifying type
        // to consider this node.

        SgType* expr_type = expr->get_type();
        #ifdef STRIP_TYPEDEFS
        expr_type = Util::getType(expr_type);
        #endif

        if(Util::isQualifyingType(expr_type)) {
            CheckExprSanity(expr);
            ArithCheck::NodesToInstrument.push_back(node);
            Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
        else {
            Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
            return synattr;
        }
    }
    else {
        // need not be overloaded. Nothing to do.
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
    }


    // One golden rule which we can apply to figure out the relevant nodes -- i.e. the
    // ones that we need to manipulate -- is to see if a node actually has a get_type()
    // or rather, if a node has an associated type. If it doesn't have a type
    // associated with it, it doesn't return any value, right? this means,
    // it need not be overloaded, or changed in any way.


    //printf("Not propagating attributes upwards\n");
    //Util::nodeType synattr(UNKNOWN, Util::INH_UNKNOWN);

    #ifdef SYNTH_DEBUG
    printf("Propagating Util::NOT_STR_TYPE\n");
    #endif
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;


}

Util::nodeType TopBotTrack2::evaluateInheritedAttribute(SgNode* node,
                                                Util::nodeType inh) {
    #if 1
    #ifdef INH_DEBUG
    printf("TopBotTrack2: Ev_Inh_Attr\n");
    printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());    
    Sg_File_Info* File = isSgNode(node)->get_file_info();
    printf("file: %s\n", File->get_filename());    
    printf("Doing nothing right now.\n");
    #endif
    #endif

    #ifdef CPP_SCOPE_MGMT
    static bool first_decl_stmt = false;



    if((isSgFunctionDeclaration(node) || isSgClassDeclaration(node)
        || isSgVariableDeclaration(node)) && !first_decl_stmt) {
        SgName fn_name;
        if(SgFunctionDeclaration* fndecl = isSgFunctionDeclaration(node)) {
            fn_name = fndecl->get_name();
        }
        else if(SgClassDeclaration* classdecl = isSgClassDeclaration(node)) {
            fn_name = classdecl->get_name();
        }
        else if(SgVariableDeclaration* vardecl = isSgVariableDeclaration(node)) {
            fn_name = Util::getNameForDecl(vardecl)->get_name();
        }
        else {
            ROSE_ASSERT(0);
        }

        if(!Util::isNamespaceScope(isSgDeclarationStatement(node)->get_scope(), "std")) {
            // check if the decl stmt node is in the current file... 
            Sg_File_Info* File = isSgNode(node)->get_file_info();
            //printf("file: %s\n", File->get_filename());    
            SgFilePtrList file_list = ArithCheck::ACProject->get_fileList();
            ROSE_ASSERT(file_list.size() == 1);
                        
            SgSourceFile* cur_file = isSgSourceFile(*file_list.begin());
            if(strcmp(cur_file->getFileName().c_str(), File->get_filename()) == 0) {
                Util::FirstDeclStmtInFile = isSgDeclarationStatement(node);
                first_decl_stmt = true;
                #ifdef INH_DEBUG
                Util::printNodeExt(node);
                printf("Found decl stmt in input file\n");
                #endif
            }
        }
    }
    #endif



    #ifdef LOCK_KEY_INSERT
    #ifdef INH_DEBUG
    printf("INHERITED_ATTRIBUTE: LOCK_KEY_INSERT: Is it a basic block?");
    #endif

    if(isSgDeclaration(node)) {
        // check if the decl stmt node is in the current file... 
        Sg_File_Info* File = isSgNode(node)->get_file_info();
        printf("file: %s\n", File->get_filename());    
        SgFilePtrList file_list = ArithCheck::ACProject->get_fileList();
        ROSE_ASSERT(file_list.size() == 1);
                
        SgSourceFile* cur_file = isSgSourceFile(*file_list.begin());
        if(strcmp(cur_file->getFileName().c_str(), File->get_filename()) == 0) {
            printf("Found decl stmt in input file\n");
        }

    }

    if(isSgBasicBlock(node)) {
        #ifdef INH_DEBUG
        printf("Yes\n");
        #endif
        ArithCheck::NodesToInstrument.push_back(node);
    }
    else if(isSgGlobal(node)) {
        // This handles global variables by inserting a lock
        // at the global scope.
        #ifdef INH_DEBUG
        printf("Yes\n");
        #endif
        ArithCheck::NodesToInstrument.push_back(node);
    }
    else {
        #ifdef INH_DEBUG
        printf("No\n");
        #endif
    }
    #endif

    return inh;

}
#endif
        
void ArithCheck::process(BopContainer& BopCtr) {
    printf("ArithCheck:process BopCtr\n");

    // Can't do a mass copy. Need to figure out which binary ops we need.
    //SelectedBops.insert(SelectedBops.end(), BopCtr.begin(), BopCtr.end());
    for(BopContainer::iterator bpit = BopCtr.begin(); bpit != BopCtr.end();
                                                                bpit++) {
        SgBinaryOp* Bop = *bpit;
        process(Bop);
    }
}

bool ArithCheck::isUseful(SgBinaryOp* Bop) {
    
    switch(Bop->variantT()) {
    case V_SgAddOp: return true;
    case V_SgSubtractOp: return true;
    case V_SgMultiplyOp: return true;
    case V_SgDivideOp: return true;
    default: return false;
    }

    assert(0);
}



void ArithCheck::process(SgBinaryOp* Bop) {
    printf("ArithCheck:process Bop\n");

    if(isUseful(Bop)) {
        printInfo(Bop, Bop->get_lhs_operand(), Bop->get_rhs_operand());
        SelectedBops.push_back(Bop);
    }

    #if 0
    switch(Bop->variantT()) {
    case V_SgAddOp: process(isSgAddOp(Bop))
    #endif
}

#if 0
void ArithCheck::insertLibHeader() {

    //SgProject* Util::project = ArithCheck::ACProject;

       SgFilePtrList file_list = Util::project->get_fileList();
    SgFilePtrList::iterator iter;
    for(iter = file_list.begin(); iter!=file_list.end(); iter++) {
        SgSourceFile* cur_file = isSgSourceFile(*iter);
        SgGlobal* global_scope = cur_file->get_globalScope();
//        insertHeader("metadata.h", PreprocessingInfo::after,false,global_scope);
        // Removed Rajesh Sept 11 5PM
        //insertHeader("foo2_c.h", PreprocessingInfo::after,false,global_scope);
        //insertHeader("ptr_hdr.h", PreprocessingInfo::after,false,global_scope);
    }

    

} 
#endif

SgStatement* ArithCheck::getSuitablePrevStmt(SgStatement* stmt) {

    SgNode* parent = isSgNode(stmt)->get_parent();
    #ifdef PREV_STMT_DEBUG
    printf("Parent: %s = %s\n", isSgNode(parent)->sage_class_name(), isSgNode(parent)->unparseToString().c_str());
    #endif
    
    if(isSgForInitStatement(parent)) {
        #ifdef PREV_STMT_DEBUG
        printf("Found forloop case.\n");
        #endif
        // This is a for loop initialization statement.
        // We can't put just the variable declaration here, without
        // any initialization. So, lets put it prior to the
        // this statement
        SgNode* forloopstmt = parent->get_parent();
        #ifdef PREV_STMT_DEBUG
        printf("ForInit: %s = %s\n", isSgNode(forloopstmt)->sage_class_name(), isSgNode(forloopstmt)->unparseToString().c_str());    
        #endif
        ROSE_ASSERT(isSgForStatement(isSgStatement(forloopstmt)));
        return isSgStatement(forloopstmt);
    }
    else if(isSgSwitchStatement(parent)) {
        #ifdef PREV_STMT_DEBUG
        printf("Switch stmt: parent: %s\n", isSgNode(parent)->unparseToString().c_str());
        #endif
        return isSgStatement(parent);
    }
    return stmt;
        
}



SgType* ArithCheck::findInRelevantStructType(SgType* type) {
    #ifdef CPP_SCOPE_MGMT
    return NULL;    
    #else

    TypeMap_t::iterator tm;

    tm = ArithCheck::RelevantStructType.begin();

    for(; tm != ArithCheck::RelevantStructType.end(); ++tm) {
        SgType* curr = (tm->first);
        
        // Working one
//        if(Util::getNameForType(type) == Util::getNameForType(curr)) {
        if(strcmp(Util::getNameForType(type).getString().c_str(), 
                Util::getNameForType(curr).getString().c_str()) == 0) {
            SgType* ret = (tm->second);
            return ret;
        }
    }

    return NULL;
    #endif
}

SgType* ArithCheck::checkStructDeclsInHdr(SgType* type) {
    
    SgName fn_name(Util::generateNameForType(type));

    for(unsigned int index = 0; index < ArithCheck::str_decls.size(); index++) {
        SgClassDeclaration* decl = ArithCheck::str_decls[index];
        SgName decl_name = decl->get_name();
        //printf("decl_name: %s\n", decl_name.getString().c_str());
        //printf("generated_name: %s\n", fn_name.getString().c_str()); 
        if(strcmp(decl_name.getString().c_str(), fn_name.getString().c_str()) == 0) {
            //printf("Found!\n");
            return decl->get_type();
        }
    }
    return NULL;
}

char* ArithCheck::getStringFor(SgBinaryOp* bop, enum Util::OVL_TYPE op) {

    char* BString = Util::getStringForVariantT(bop->variantT());
    char* OpString = getStringFor(op);
    
    char* output = (char*)malloc(50*sizeof(char));
    sprintf(output, "%s_%s", OpString, BString);

    return output;

}

SgStatement* ArithCheck::findInsertLocationFromParams(SgExprListExp* param_list) {

    SgExpressionPtrList& exprs = param_list->get_expressions();

    ROSE_ASSERT(exprs.size() >= 1);
    SgExpressionPtrList::iterator iter = exprs.begin();
    SgExpression* arg1 = *iter;
    SgStatement* loc = GEFD(arg1);
    
    return loc;
}

SgFunctionDeclaration* ArithCheck::createDefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project, SgNode* pos, bool derived, SgName s_name) {
    #ifdef DEFDECL_DEBUG
    printf("Creating Defining decl\n");    
    printf("transf_name: %s\n", transf_name.str());
    #endif

    #if 0
    SgGlobal* globalScope = SI::getFirstGlobalScope(project);
    pushScopeStack(globalScope);
    #endif

    SgFunctionParameterList* new_param_list = SB::buildFunctionParameterList();

    SgExpressionPtrList& exprs = param_list->get_expressions();
    
    #ifdef DEFDECL_DEBUG
    printf("creating param_list\n");
    #endif

    unsigned int arg_counter = 0;
    for(SgExpressionPtrList::iterator iter = exprs.begin(); iter != exprs.end();
                                                            iter++) {
        arg_counter++;        
        SgExpression* ce = *iter;
        #ifdef DEFDECL_DEBUG
        printf("ce: %s = %s\n", isSgNode(ce)->sage_class_name(), isSgNode(ce)->unparseToString().c_str());
        #endif
        std::string arg_name = "input" + boost::lexical_cast<std::string>(arg_counter);
        SgInitializedName* new_name = SB::buildInitializedName(SgName(arg_name),ce->get_type(), NULL);
        #ifdef STRIP_STATIC_PARAMS
        SgStorageModifier& sm = new_name->get_storageModifier();
        printf("storage_modifier: %d\n", sm.get_modifier());
        #endif


        #ifdef DEFDECL_DEBUG
        printf("created new_name\n");
        #endif
        // FIXME: Scope should be fndecl, not globalScope.
        //new_name->set_scope(globalScope);
        //new_name->set_scope(new_param_list->get_scope());
        new_param_list->append_arg(new_name);
        new_name->set_parent(new_param_list);
    }

    #ifdef DEFDECL_DEBUG
    printf("creating defdecl\n");
    #endif

    // Working one
    #if 0
    SgFunctionDeclaration* defdecl = SB::buildDefiningFunctionDeclaration(transf_name,
                                                                        retType,
                                                                        new_param_list);
    #else

    #ifdef CPP_SCOPE_MGMT
    if(strDecl::functionInteractsWithStdOrPrimitives(param_list, retType, pos) && !Util::isExternal(s_name) && !derived) {
        pos = Util::FirstDeclStmtInFile;
        #ifdef DEFDECL_DEBUG
        printf("changed pos to first decl stmt\n");
        #endif
    }
    #ifdef DEFDECL_DEBUG
    printf("pos\n");
    Util::printNode(pos);
    #endif
    SgFunctionDeclaration* defdecl;
    if(isSgMemberFunctionDeclaration(pos)) {
        SgScopeStatement* scope = isSgStatement(pos)->get_scope();
        ROSE_ASSERT(isSgClassDefinition(scope));
        defdecl = SB::buildDefiningMemberFunctionDeclaration(transf_name, retType, new_param_list, scope);
        scope->insertStatementInScope(defdecl, false);
        
    }
    else {
        SgScopeStatement* scope = isSgStatement(pos)->get_scope();
        // For C++, we should consider namespace definition as another
        // possible place for insertion of the struct.
        ROSE_ASSERT(isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope));
        defdecl = SB::buildDefiningFunctionDeclaration(transf_name, retType, new_param_list, scope);    
        SI::insertStatementBefore(isSgStatement(pos), defdecl);
    }
    
    #else
    SgFunctionDeclaration* defdecl = SB::buildDefiningFunctionDeclaration(transf_name,
                                                                        retType,
                                                                        new_param_list,
                                                                        isSgStatement(pos)->get_scope());
    
    // Instead of simply placing the defdecl at the top of the file... we'll
    // place it as close to the use as possible. This should ensure that all the
    // necessary data structures used, are declared.
//    prependStatement(isSgStatement(defdecl));
//    SgStatement* loc = findInsertLocationFromParams(param_list);
//    SI::insertStatementBefore(loc, defdecl);
//    SI::insertStatementBefore(scope, defdecl);
    
    #if 0
    insertStmtAfterLast(defdecl, globalScope);
    #else

    #ifdef USE_INSERT_AFTER
    SI::insertStatementAfter(isSgStatement(pos), defdecl);
    #else
    SI::insertStatementBefore(isSgStatement(pos), defdecl);
    #endif

    #endif
    #endif
    #endif

    #ifdef MAKE_DECLS_STATIC
    Util::makeDeclStatic(isSgDeclarationStatement(defdecl));
    #endif

    #if 0
    popScopeStack();
    #endif

    return defdecl;
}

void ArithCheck::appendReturnStmt(SgExpression* ret_exp, SgBasicBlock* fnbody) {
    
    SgReturnStmt* ret_stmt = SB::buildReturnStmt(ret_exp);

    fnbody->append_statement(ret_stmt);
}

void ArithCheck::handleCreateStructDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                        SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);

    // There are two types of functions which end up here.
    // 1. create_struct(orig_ptr, &orig_ptr)
    // translates to create_struct(input1, input2)
    // str output;
    //output.ptr = input1;
    //output.addr = input2;
    //return output;
    
    // 2. create_struct(orig_ptr, &orig_ptr, size)
    // translates to create_struct(input1, input2, input3)
    // str output;
    //output.ptr = input1;
    //output.addr = input2;
    //create_entry(input2, reinterpret_cast<unsigned long long>(input1), input3);
    //return output;

    // In the second case, we actually create an entry using the size argument.
    // The create_entry library function will handle this. We don't need to emulate that
    // part.

    // Assert that we are returning a struct type... 
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(retType, GEFD(fndecl)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(retType));
    #endif

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // create local variable of return type
    SgVariableDeclaration* output_var = Util::createLocalVariable("output", retType, NULL, fnbody);
    
    // Both are equivalent...
    //SI::getScope(fnbody)->insertStatementInScope(output_var, true);
    fnbody->append_statement(output_var);

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();
    ROSE_ASSERT(args.size() >= 2);

    // Lets pluck the first argument out of the fndecl    
    SgInitializedName* arg1 = *iter;
    
    // Now, pluck the second argument out
    iter++;
    SgInitializedName* arg2 = *iter;


    SgExprStatement* ptr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "ptr", SB::buildVarRefExp(arg1));
    fnbody->append_statement(ptr_assign);

    SgExprStatement* addr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "addr", SB::buildVarRefExp(arg2));
    fnbody->append_statement(addr_assign);
        
    // Now, use buildOverloadFn to form a create_entry, if a third argument exists.
    if(args.size() == 3) {
        // Get the third arg.
        ++iter;
        SgInitializedName* arg3 = *iter;

        // the function call: create_entry(input2, reinterpret_cast<unsigned long long>(input1), input3);
        SgExpression* ce_arg1 = SB::buildVarRefExp(arg2);
        SgExpression* ce_arg2 = Util::castToAddr(SB::buildVarRefExp(arg1));
        // cast it to size type... 
        //SgExpression* ce_arg3 = SB::buildCastExp(SB::buildVarRefExp(arg3), Util::getSizeOfType(), SgCastExp::e_reinterpret_cast);
        SgExpression* ce_arg3 = SB::buildCastExp(SB::buildVarRefExp(arg3), Util::getSizeOfType(), SgCastExp::e_C_style_cast);

        // Working one
        //SgExpression* ce_call = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), fnbody, GEFD(fnbody));
        //SgExpression* ce_call = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), fnbody, fndecl);
        SgExpression* ce_call = buildMultArgOverloadFn("create_entry", SB::buildExprListExp(ce_arg1, ce_arg2, ce_arg3), SgTypeVoid::createType(), fnbody, fndecl, true);

        SgExprStatement* ce_stmt = SB::buildExprStatement(ce_call);

        fnbody->append_statement(ce_stmt);
    }

    // Now, for the return value
    // the return stmt: return output;
    appendReturnStmt(SB::buildVarRefExp(output_var), fnbody);
}

SgFunctionDefinition* ArithCheck::getFuncDef(SgFunctionDeclaration* fndecl) {
    SgFunctionDefinition* fndef = fndecl->get_definition();
    ROSE_ASSERT(fndef != NULL);
    return fndef;
}

SgBasicBlock* ArithCheck::getFuncBody(SgFunctionDefinition* fndef) {
    SgBasicBlock* bb = fndef->get_body();
    ROSE_ASSERT(bb != NULL);
    return bb;
}


void ArithCheck::handleCreateStructFromAddrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                    SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // This fn takes in an address, and create a struct out of it.
    // fn call: create_struct_from_addr(input1)
    // str output;
    // output.ptr = *(reinterpret_cast<class node**>(input1));
    // output.addr = input1;
    // return output;
    // 
    // Convert the input1 to the addr of the underlying type of the return type. :) 

    // Assert that we are returning a struct type... 
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(retType, isSgFunctionDeclaration(pos)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(retType));
    #endif

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // create local variable of return type
    SgVariableDeclaration* output_var = Util::createLocalVariable("output", retType, NULL, fnbody);
    
    // Both are equivalent...
    //SI::getScope(fnbody)->insertStatementInScope(output_var, true);
    fnbody->append_statement(output_var);

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // create_struct_using_addr has only one addr as arg
    ROSE_ASSERT(args.size() == 1);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Lets pluck the first argument out of the fndecl    
    SgInitializedName* arg1 = *iter;

    // Create the RHS for the first assignment above: output.ptr = *(reinterpret_cast<class node**>(input1))
    SgExpression* input_ref = SB::buildVarRefExp(arg1);
    // Pointer to the underlying ptr, in the return type
    SgType* ret_type_ptr = strDecl::findInUnderlyingType(retType);
    ROSE_ASSERT(ret_type_ptr != NULL);
    SgType* ptr_of = SgPointerType::createType(ret_type_ptr);
//    SgExpression* input_casted = SB::buildCastExp(input_ref, ptr_of, SgCastExp::e_reinterpret_cast);
    SgExpression* input_casted = SB::buildCastExp(input_ref, ptr_of, CAST_TYPE);
    // Now, deref it.
    SgExpression* input_cast_deref = SB::buildPointerDerefExp(input_casted);

    // this is the ptr assignment: output.ptr = *(reinterpret_cast<class node**>(input1))
    SgExprStatement* ptr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "ptr", input_cast_deref);
    fnbody->append_statement(ptr_assign);

    // this is the addr assignment: output.addr = input
    SgExprStatement* addr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "addr", SB::buildVarRefExp(arg1));
    fnbody->append_statement(addr_assign);

    // the return stmt: return output
    appendReturnStmt(SB::buildVarRefExp(output_var), fnbody);
        
}

void ArithCheck::handleDerefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // fn call: deref(input1_str)
    // check_entry((unsigned long long)input1_str.ptr, input1_str.addr)
    // return input_str.ptr;
    
    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // deref has only one str arg
    ROSE_ASSERT(args.size() == 1);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;
    // the arg is of struct type.
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), isSgFunctionDeclaration(pos)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()));
    #endif

    // get input1_str.ptr and cast it to unsigned long long 
    SgExpression* input1_ptr = Util::castToAddr(Util::createDotExpFor(arg1, "ptr"));
    // get input1_str.addr
    SgExpression* input1_addr = Util::createDotExpFor(arg1, "addr");

    // The alternative to "pos" would be GEFD(fnbody) -- this may not work with the Namespace Definition stuff
    // in C++, since the scope of this declaration won't be the same of us insertion...  
    // Working one
    #if 0
    SgExpression* ce = buildOverloadFn("check_entry", input1_ptr, input1_addr, SgTypeVoid::createType(), scope, pos);
    #else
    //SgExpression* ce = buildOverloadFn("check_entry", input1_ptr, input1_addr, SgTypeVoid::createType(), scope, fndecl);
    SgExpression* ce = buildMultArgOverloadFn("check_entry", SB::buildExprListExp(input1_ptr, input1_addr), SgTypeVoid::createType(), scope, fndecl, true);
    #endif

    SgExprStatement* ce_stmt = SB::buildExprStatement(ce);
    fnbody->append_statement(ce_stmt);

    appendReturnStmt(Util::createDotExpFor(arg1, "ptr"), fnbody);

}

void ArithCheck::handlePntrArrRefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // fn call: pntrarrref(input1_str, index)
    // return input1_str.ptr + index;
    
    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // pntrarrref has two args. first is of struct type
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;
    // the first arg is of struct type
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), fndecl));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()));
    #endif

    // create pointer dot exp
    SgExpression* ptr_exp = Util::createDotExpFor(arg1, "ptr");

    // get the second arg
    iter++;
    SgInitializedName* arg2 = *iter;

    // Now, create the add op, in the return stmt... 
    appendReturnStmt(SB::buildAddOp(ptr_exp, SB::buildVarRefExp(arg2)), fnbody);

}

void ArithCheck::handlePtrCheckDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, 
                                SgNode* pos, SgFunctionDeclaration* fndecl) {
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // fn call: ptr_check(input1_str, index)
    // body: array_bound_check_using_lookup(....)
    // return input1_str.ptr;
    
    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // pntrarrref has two args. first is of struct type
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;
    // the first arg is of struct type
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()));
    #endif

    // create pointer dot exp
    SgExpression* ptr_exp = Util::createDotExpFor(arg1, "ptr");

    // get the second arg
    iter++;
    SgInitializedName* arg2 = *iter;

    // create addr dot exp
    SgExpression* addr_exp = Util::castToAddr(Util::createDotExpFor(arg1, "addr"));

    // arg1 is designed to be the metadata lookup using addr... this is not required
    // for arrays...
    // arg2 is the pointer/array var
    // arg3 is the index
    // arg4 is the base type for array/pointer
    // scope gives the scope in which the function needs to be inserted
    // insert specifies where to insert the forward decl/def of the function
    SgExpression* overload = createArrayBoundCheckFn(addr_exp, ptr_exp, SB::buildVarRefExp(arg2),
                                                isSgPointerType(ptr_exp->get_type())->get_base_type(), 
                                                scope, 
                                                fndecl);
    
    SgExprStatement* ovl_stmt = SB::buildExprStatement(overload);

    fnbody->append_statement(ovl_stmt);

    appendReturnStmt(SI::copyExpression(ptr_exp), fnbody);
}

void ArithCheck::handleDerefCheckDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, 
                                SgNode* pos, SgFunctionDeclaration* fndecl) {

    //SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);        
    // fn call: deref_check(ptr, addr)
    // body: check_entry(ptr, addr)
    // ptr_not_null_check(ptr)
    // return ptr

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // pntrarrref has two args. first is of struct type
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;
    // Cast it to addr... 
    SgExpression* ce_arg1 = Util::castToAddr(SB::buildVarRefExp(arg1));

    // Get the second arg
    iter++;
    SgInitializedName* arg2 = *iter;
    // Take a var ref for this
    SgExpression* ce_arg2 = SB::buildVarRefExp(arg2);

    // insert check_entry call
    SgExprListExp* p_list = SB::buildExprListExp(ce_arg1, ce_arg2);
    
    SgExpression* overload = buildMultArgOverloadFn("check_entry", p_list,
                                                    SgTypeVoid::createType(),
                                                    scope,
                                                    fndecl, true);

    SgExprStatement* ovl_stmt = SB::buildExprStatement(overload);

    fnbody->append_statement(ovl_stmt);

    // insert null_check call
    SgExprListExp* p_list2 = SB::buildExprListExp(SI::copyExpression(ce_arg1));

    SgExpression* overload2 = buildMultArgOverloadFn("null_check", p_list2,
                                                    SgTypeVoid::createType(),
                                                    scope,
                                                    fndecl, true); 
    SgExprStatement* ovl_stmt2 = SB::buildExprStatement(overload2);
    fnbody->append_statement(ovl_stmt2);

    // Now, return the pointer itself,for the deref operation
    appendReturnStmt(SB::buildVarRefExp(arg1), fnbody);

}


void ArithCheck::handleDerefCheckWithStrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);        
    // fn call: deref_check_with_str(str)
    // body: return deref_check(str.ptr, str.addr)

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // deref_check_with_str has 1 struct arg
    ROSE_ASSERT(args.size() == 1);
    SgInitializedNamePtrList::iterator iter = args.begin();


    // Get the first arg
    SgInitializedName* arg1 = *iter;
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()));
    #endif

    // str.ptr
    SgExpression* ptr_exp = Util::createDotExpFor(arg1, "ptr");

    // str.addr
    SgExpression* addr_exp = Util::castToAddr(Util::createDotExpFor(arg1, "addr"));

    // create the param list for the deref_check call
    SgExprListExp* p_list = SB::buildExprListExp(ptr_exp, addr_exp);

    SgExpression* overload = buildMultArgOverloadFn("deref_check", p_list,
                                                    ptr_exp->get_type(), // returning this value out
                                                    scope,
                                                    fndecl, true);
    
    // Now, put this fncall in the return stmt
    appendReturnStmt(overload, fnbody);
}

void ArithCheck::handleDerefAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {    


//        SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);        
        // fncall: deref_assign_and_copy(ptr1, ptr2)
        // body: *ptr1 = *ptr2
        // body: create_entry(ptr1, ptr2)
        // return create_struct(*ptr1, ptr1) 

        // Get the function definition...
        SgFunctionDefinition* fndef = getFuncDef(fndecl);
        SgBasicBlock* fnbody = getFuncBody(fndef);
        SgStatementPtrList& stmts = fnbody->get_statements();

        // the args
        SgInitializedNamePtrList& args = fndecl->get_args();
        SgInitializedNamePtrList::iterator iter = args.begin();

        // ptr1
        SgInitializedName* ptr1 = *iter;

        // ptr2
        iter++;
        SgInitializedName* ptr2 = *iter;

        // *ptr1 = *ptr2
        SgExpression* assign_op = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1)), 
                                                SB::buildPointerDerefExp(SB::buildVarRefExp(ptr2)));
        fnbody->append_statement(SB::buildExprStatement(assign_op));

        // create_entry(ptr1, ptr2)
        SgExprListExp* ce_list = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(ptr1)), Util::castToAddr(SB::buildVarRefExp(ptr2)));

        SgExpression* overload = buildMultArgOverloadFn("create_entry", ce_list, SgTypeVoid::createType(), scope, fndecl, true);
        fnbody->append_statement(SB::buildExprStatement(overload));

        // create_struct(*ptr1, ptr1)
        SgExpression* ptr1_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1));
        SgExprListExp* cs_list = SB::buildExprListExp(ptr1_deref, Util::castToAddr(SB::buildVarRefExp(ptr1)));
        SgExpression* ovl2 = buildMultArgOverloadFn("create_struct", cs_list, retType, scope, fndecl, true); 

        appendReturnStmt(ovl2, fnbody);

}

void ArithCheck::handleLHSDerefAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {    


//        SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);        
        // fncall: lhs_deref_assign_and_copy(ptr1, str)/lhs_deref_assign_and_copy(ptr1, ptr2, &ptr2)
        // body: *ptr1 = str.ptr / *ptr1 = ptr2
        // body: create_entry(ptr1, str.addr)/create_entry(ptr1, &ptr2)
        // return create_struct(*ptr1, ptr1)

        // Get the function definition...
        SgFunctionDefinition* fndef = getFuncDef(fndecl);
        SgBasicBlock* fnbody = getFuncBody(fndef);
        SgStatementPtrList& stmts = fnbody->get_statements();

        // the args
        SgInitializedNamePtrList& args = fndecl->get_args();
        SgInitializedNamePtrList::iterator iter = args.begin();

        // ptr1
        SgInitializedName* ptr1 = *iter;
        SgExprListExp* ce_list;

        if(args.size() == 2) {
            // str
            iter++;
            SgInitializedName* str = *iter;

            // *ptr1 = str.ptr
            SgExpression* assign_op = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1)), Util::createDotExpFor(str, "ptr"));
            fnbody->append_statement(SB::buildExprStatement(assign_op));

            // create_entry(ptr1, str.addr)
            ce_list = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(ptr1)), 
                                        Util::castToAddr(Util::createDotExpFor(str, "addr")));
        }
        else {
            ROSE_ASSERT(args.size() == 3);
            // ptr2
            iter++;
            SgInitializedName* ptr2 = *iter;

            // &ptr2
            iter++;
            SgInitializedName* ptr2_addr = *iter;

            // *ptr1 = ptr2
            SgExpression* assign_op = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1)), 
                                                    SB::buildVarRefExp(ptr2));
            fnbody->append_statement(SB::buildExprStatement(assign_op));

            // create_entry(ptr1, &ptr2)
            ce_list = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(ptr1)),
                                            Util::castToAddr(SB::buildVarRefExp(ptr2_addr)));
        }

        SgExpression* overload = buildMultArgOverloadFn("create_entry", ce_list, SgTypeVoid::createType(),
                                                            scope, fndecl, true);
        fnbody->append_statement(SB::buildExprStatement(overload));

        // return create_struct(*ptr1, ptr1)
        SgExprListExp* cs_list = SB::buildExprListExp(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1)), Util::castToAddr(SB::buildVarRefExp(ptr1)));
        SgExpression* ovl2 = buildMultArgOverloadFn("create_struct", cs_list, retType, scope, fndecl, true);

        appendReturnStmt(ovl2, fnbody);

}

void ArithCheck::handleRHSDerefAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {    


//        SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);        
        // fncall: rhs_deref_assign_and_copy(&ptr1, *ptr2), rhs_deref_assign_and_copy(str, *ptr2)
        // body: *ptr1 = *ptr2 / str.ptr = *ptr2
        // body: create_entry(ptr1, ptr2) / create_entry(str.addr, ptr2)
        // return create_struct(*ptr1, ptr1) / return str


        // Get the function definition...
        SgFunctionDefinition* fndef = getFuncDef(fndecl);
        SgBasicBlock* fnbody = getFuncBody(fndef);
        SgStatementPtrList& stmts = fnbody->get_statements();

        // the args
        SgInitializedNamePtrList& args = fndecl->get_args();
        SgInitializedNamePtrList::iterator iter = args.begin();

        // ptr1 / str
        SgInitializedName* arg1 = *iter;

        // ptr2
        iter++;
        SgInitializedName* ptr2 = *iter;

        SgExprListExp* ce_list;

        #ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
        #else
        if(strDecl::isValidStructType(arg1->get_type()))
        #endif
        {
            
            SgExpression* assign_op = SB::buildAssignOp(Util::createDotExpFor(arg1, "ptr"), 
                                        SB::buildPointerDerefExp(SB::buildVarRefExp(ptr2)));
            fnbody->append_statement(SB::buildExprStatement(assign_op));

            ce_list = SB::buildExprListExp(Util::createDotExpFor(arg1, "addr"), SB::buildVarRefExp(ptr2));
        }
        else {
            ROSE_ASSERT(Util::isQualifyingType(arg1->get_type()));

            SgExpression* assign_op = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)), 
                                                    SB::buildPointerDerefExp(SB::buildVarRefExp(ptr2)));
            fnbody->append_statement(SB::buildExprStatement(assign_op));

            ce_list = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)), Util::castToAddr(SB::buildVarRefExp(ptr2)));
        }

        SgExpression* overload = buildMultArgOverloadFn("create_entry", ce_list, SgTypeVoid::createType(),
                                                        scope, fndecl, true);
        fnbody->append_statement(SB::buildExprStatement(overload));

        // return create_struct(*ptr1, ptr1) / return str
        #ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
        #else
        if(strDecl::isValidStructType(arg1->get_type()))
        #endif
        {
            appendReturnStmt(SB::buildVarRefExp(arg1), fnbody);
        }
        else {
            ROSE_ASSERT(Util::isQualifyingType(arg1->get_type()));

            SgExprListExp* cs_list = SB::buildExprListExp(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)), 
                                                        Util::castToAddr(SB::buildVarRefExp(arg1)));
            SgExpression* ovl2 = buildMultArgOverloadFn("create_struct", cs_list, retType, scope, fndecl, true);
            appendReturnStmt(ovl2, fnbody);
        }

}


void ArithCheck::handleCSAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {    


//        SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);        

        // Get the function definition...
        SgFunctionDefinition* fndef = getFuncDef(fndecl);
        SgBasicBlock* fnbody = getFuncBody(fndef);
        SgStatementPtrList& stmts = fnbody->get_statements();

        // the args
        SgInitializedNamePtrList& args = fndecl->get_args();
        SgInitializedNamePtrList::iterator iter = args.begin();

        // lhs
        SgInitializedName* lhs = *iter;
        iter++;
        // rhs
        SgInitializedName* rhs = *iter;

        // *lhs = *rhs
        SgExpression* lhs_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(lhs));
        SgExpression* rhs_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(rhs));

        SgExpression* assign_op = SB::buildAssignOp(lhs_deref, rhs_deref);
        fnbody->append_statement(SB::buildExprStatement(assign_op));

        // rtc_copy(lhs, rhs)
        SgExprListExp* p_list = SB::buildExprListExp(SB::buildVarRefExp(lhs), SB::buildVarRefExp(rhs));
        SgExpression* overload = buildMultArgOverloadFn("rtc_copy", p_list, SgTypeVoid::createType(), scope, fndecl, true);
        fnbody->append_statement(SB::buildExprStatement(overload));

        // return *lhs
        appendReturnStmt(SI::copyExpression(lhs_deref), fnbody);

}


void ArithCheck::handleAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {    


//        SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);        

        // Get the function definition...
        SgFunctionDefinition* fndef = getFuncDef(fndecl);
        SgBasicBlock* fnbody = getFuncBody(fndef);
        SgStatementPtrList& stmts = fnbody->get_statements();

        // the args
        SgInitializedNamePtrList& args = fndecl->get_args();
        SgInitializedNamePtrList::iterator iter = args.begin();


        SgExprListExp* entry_param_list;
        SgExprListExp* struct_param_list;

        SgInitializedName* arg1 = *iter;
        iter++;
        SgInitializedName* arg2 = *iter;

        #ifdef CPP_SCOPE_MGMT
        if(args.size() == 3 && !strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
        #else
        if(args.size() == 3 && !strDecl::isValidStructType(arg1->get_type()))
        #endif
        {
            // input: var_ref_L = var_ref_R
            // output: assign_copy(&var_ref_L, var_ref_R, &var_ref_R)
            // body: var_ref_L = var_ref_R -- this is required since the original variable needs to be updated
            // body: create_entry(&var_ref_L, &var_ref_R),
            // return create_struct(var_ref_R, &var_ref_L)    

            // Cast the lhs of the assign op to the rhs type
            SgExpression* lhs = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));

            SgExpression* assign_op = SB::buildAssignOp(lhs,
                                                    SB::buildVarRefExp(arg2));
            fnbody->append_statement(SB::buildExprStatement(assign_op));

            iter++;
            SgInitializedName* arg3 = *iter;

            entry_param_list = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)),
                                                SB::buildVarRefExp(arg3));

            struct_param_list = SB::buildExprListExp(SB::buildVarRefExp(arg2),
                                                Util::castToAddr(SB::buildVarRefExp(arg1)));

        }
        #ifdef CPP_SCOPE_MGMT
        else if(args.size() == 2 && !strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
        #else
        else if(args.size() == 2 && !strDecl::isValidStructType(arg1->get_type()))     
        #endif
        {
            // input: var_ref_L = struct_R
            // output: assign_copy(&var_ref_L, struct_R)
            // body: var_ref_L = struct_R.ptr -- this is required since the original variable needs to be updated
            // body: create_entry(&var_ref_L, struct_R.addr);
            // return create_struct(struct_R.ptr, &var_ref_L)
            
            // Cast the lhs to the type of the rhs
            SgExpression* rhs = Util::createDotExpFor(arg2, "ptr");
            SgExpression* lhs = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));

            SgExpression* assign_op = SB::buildAssignOp(lhs,
                                                    rhs);
            fnbody->append_statement(SB::buildExprStatement(assign_op));

            entry_param_list = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)),
                                        Util::castToAddr(Util::createDotExpFor(arg2, "addr")));

            struct_param_list = SB::buildExprListExp(Util::createDotExpFor(arg2, "ptr"),
                                                Util::castToAddr(SB::buildVarRefExp(arg1)));
        }
        #ifdef CPP_SCOPE_MGMT
        else if(args.size() == 3 && strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))     
        #else
        else if(args.size() == 3 && strDecl::isValidStructType(arg1->get_type()))    
        #endif
        {
            // input: struct_L = var_ref_R
            // output: assign_copy(struct_L, var_ref_R, &var_ref_R)
            // body: struct_L.ptr = var_ref_R; -- This is required since *ptr_index = NULL still
            // requires that *ptr_index is updated!
            // body: create_entry(struct_L.addr, &var_ref_R)
            // return create_struct(var_ref_R, struct_L.addr)

            SgExpression* ptr_assign = SB::buildAssignOp(Util::createDotExpFor(arg1, "ptr"),
                                                    SB::buildVarRefExp(arg2));
            fnbody->append_statement(SB::buildExprStatement(ptr_assign));
    
            iter++;
            SgInitializedName* arg3 = *iter;

            entry_param_list = SB::buildExprListExp(Util::castToAddr(Util::createDotExpFor(arg1, "addr")),
                                                SB::buildVarRefExp(arg3));

            struct_param_list = SB::buildExprListExp(SB::buildVarRefExp(arg2),
                                                Util::castToAddr(Util::createDotExpFor(arg1, "addr")));
        }
        else {
            // input: struct_L = struct_R
            // output: assign_copy(struct_L, struct_R)
            // body: struct_L.ptr = struct_R.ptr; -- This is required since *ptr_index = NULL still
            // requires that *ptr_index is updated!
            // body: create_entry(struct_L.addr, struct_R.addr)
            // return create_struct(struct_R.ptr, struct_L.addr)
            
            SgExpression* ptr_assign = SB::buildAssignOp(Util::createDotExpFor(arg1, "ptr"), 
                                                    Util::createDotExpFor(arg2, "ptr"));
            fnbody->append_statement(SB::buildExprStatement(ptr_assign));


            entry_param_list = SB::buildExprListExp(Util::castToAddr(Util::createDotExpFor(arg1, "addr")),
                                                Util::castToAddr(Util::createDotExpFor(arg2, "addr")));

            struct_param_list = SB::buildExprListExp(Util::createDotExpFor(arg2, "ptr"),
                                                Util::castToAddr(Util::createDotExpFor(arg1, "addr")));
        }

        // create_entry_if_src_exists is used instead of create_entry since the rhs can be NULL.. 
        // and its metadata may not be recorded.
        SgExpression* entry_ovl = buildMultArgOverloadFn("create_entry_if_src_exists", entry_param_list,
                                                        SgTypeVoid::createType(),
                                                        scope,
                                                        fndecl, true);
        
        fnbody->append_statement(SB::buildExprStatement(entry_ovl));

        SgExpression* struct_ovl = buildMultArgOverloadFn("create_struct", struct_param_list,
                                                        retType,
                                                        scope,
                                                        fndecl, true);

        appendReturnStmt(struct_ovl, fnbody);
}



void ArithCheck::handleAssignDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // Two cases: 
    // Case 1:
    // this fn takes in two structs. first by ref, second by val
    // and performs as "assignment". 
    // fncall: assign(*input1_str, input2_str)
    // input1_str->ptr = input2_str.ptr;
    // create_entry(input1_str->addr, input2_str.addr);
    // return *input1_str;
    //
    // Case 2:
    // this fn takes in one struct and a char* (string val)
    // and performs an "assignment".
    // fncall: assign(*input_str, input2)
    // input1_str->ptr = input2
    // create_entry(input1_str->addr, input1_str->ptr, sizeof(input2)) 
    // return *input1_str;

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // assign has two args. first is of pointer type
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;
    // first arg is pointer type -- pointer to struct
    ROSE_ASSERT(isSgPointerType(arg1->get_type()));

    SgExprStatement* ce_stmt;
    SgExpression* ce_call;
    
    // Get the second arg
    iter++;
    SgInitializedName* arg2 = *iter;

    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(arg2->get_type(), GEFD(fndecl))) 
    #else
    if(strDecl::isValidStructType(arg2->get_type()))
    #endif
    {
    // second arg is struct type

        // input2_str.ptr
        SgExpression* ptr_exp = Util::createDotExpFor(arg2, "ptr");
    
        // input1_str->ptr = input2_str.ptr
        SgExprStatement* ptr_assign = Util::createArrowAssign(arg1, "ptr", ptr_exp);
        fnbody->append_statement(ptr_assign);
    
        // create_entry fn call: create_entry(input1_str->addr, input2_str.addr)
        SgExpression* ce_arg1 = Util::createArrowExpFor(arg1, "addr");
        SgExpression* ce_arg2 = Util::createDotExpFor(arg2, "addr");
    
        // Working one
        // FIXME: This might need to be changed to comply with the Namespace Definition stuff in C++
        //ce_call = buildOverloadFn("create_entry", ce_arg1, ce_arg2, SgTypeVoid::createType(), scope, GEFD(fnbody));
        //ce_call = buildOverloadFn("create_entry", ce_arg1, ce_arg2, SgTypeVoid::createType(), scope, fndecl);
        ce_call = buildMultArgOverloadFn("create_entry", SB::buildExprListExp(ce_arg1, ce_arg2), SgTypeVoid::createType(), scope, fndecl, true);

    
    }
    else {
        // this is a value -- like a string value..., or a char pointer
        
        // input1_str.ptr = input2
        SgExprStatement* ptr_assign = Util::createArrowAssign(arg1, "ptr", SB::buildVarRefExp(arg2));
        fnbody->append_statement(ptr_assign);

        // create entry fn call: create_entry(input1_str->addr, input1_str.ptr, sizeof(input2))
        SgExpression* ce_arg1 = Util::createArrowExpFor(arg1, "addr");
        SgExpression* ce_arg2 = Util::castToAddr(Util::createArrowExpFor(arg1, "ptr"));
        SgExpression* size_of_arg2 = SB::buildSizeOfOp(SB::buildVarRefExp(arg2));
        SgExpression* ce_arg3 = SB::buildCastExp(size_of_arg2, Util::getSizeOfType(), SgCastExp::e_C_style_cast);

        // Working one
        // FIXME: This might need to be changed to comply with the Namespace Definition stuff in C++
        //ce_call = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, GEFD(fnbody));

        //ce_call = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, fndecl);
        ce_call = buildMultArgOverloadFn("create_entry", SB::buildExprListExp(ce_arg1, ce_arg2, ce_arg3), SgTypeVoid::createType(), scope, fndecl, true);

    }

    ce_stmt = SB::buildExprStatement(ce_call);
    
    fnbody->append_statement(ce_stmt);

    // return *input1_str
    appendReturnStmt(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)), fnbody);

}


void ArithCheck::handleRTCCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // this fn takes two pointer arguments.
    // fncall: rtc_copy(*dest, *src)
    // body: foreach pointer arg in struct
    // create_entry(dest.ptr, src.ptr)

    #ifdef RTC_COPY_DEBUG
    printf("handleRTCCopyDef\n");
    #endif

    // Here, we need to get hold of the params in def_fn_decl.
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator i = args.begin();
    // get the first arg
    SgInitializedName* dest_arg = *i;
    ROSE_ASSERT(isSgPointerType(dest_arg->get_type()));
    SgType* dest_base_type = isSgPointerType(dest_arg->get_type())->get_base_type();
    ROSE_ASSERT(strDecl::isOriginalClassType(dest_base_type));


    // get the second arg
    ++i;
    SgInitializedName* src_arg = *i;
    ROSE_ASSERT(isSgPointerType(src_arg->get_type()));
    SgType* src_base_type = isSgPointerType(src_arg->get_type())->get_base_type();
    ROSE_ASSERT(strDecl::isOriginalClassType(src_base_type));

    // Now, find all the pointers in the data structure, and do a source to dest copy... 
    SgClassType* str_type = isSgClassType(dest_base_type);

    // This part below gets us the pointers... 
    SgClassDeclaration * str_decl = isSgClassDeclaration (str_type->get_declaration());

    ROSE_ASSERT (str_decl);

    #ifdef RTC_COPY_DEBUG
    printf("str_decl\n");
    Util::printNodeExt(str_decl);
    #endif

    ROSE_ASSERT(str_decl->get_firstNondefiningDeclaration()!= NULL);

    #ifdef RTC_COPY_DEBUG
    printf("str_first_nondefining_decl\n");
    Util::printNode(str_decl->get_firstNondefiningDeclaration());
    Util::printNodeExt(str_decl->get_firstNondefiningDeclaration());
    #endif

    ROSE_ASSERT(isSgClassDeclaration(str_decl->get_firstNondefiningDeclaration()) == str_decl);
    
    SgClassDeclaration* str_decl_def = isSgClassDeclaration(str_decl->get_definingDeclaration());

    SgClassDefinition* str_def = str_decl_def->get_definition();

    ROSE_ASSERT(str_def != NULL);

    SgDeclarationStatementPtrList& members = str_def->get_members();

    SgDeclarationStatementPtrList::iterator iter = members.begin();

    // Now, as we iterate over the members... we check if they are pointer type,
    // and if they are, perform a create_entry(dest, src)... We will accumulate all
    // these create_entry fn calls into a single expr list exp.. which will be returned
    SgExprListExp* exprs = SB::buildExprListExp();

    // Now, we get the basic block for def_fn_decl and insert this expression there.
    SgBasicBlock* bb = Util::getBBForFn(fndecl);    

    for(; iter != members.end(); ++iter) {
        
        SgDeclarationStatement* decl_stmt = *iter;    
        // Function calls within structs/classes in C++...

        if(!isSgVariableDeclaration(decl_stmt)) {
            continue;
        }

        SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl_stmt);
        SgInitializedName* var_name = Util::getNameForDecl(var_decl);


        // Util::createAddressOfOpFor strips off the typedefs...
        SgExpression* dest_arrow = SB::buildArrowExp(SB::buildVarRefExp(dest_arg), SB::buildVarRefExp(var_name));
        SgExpression* ce1 = Util::createAddressOfOpFor(dest_arrow); 

        SgExpression* src_arrow = SB::buildArrowExp(SB::buildVarRefExp(src_arg), SB::buildVarRefExp(var_name));
        SgExpression* ce2 = Util::createAddressOfOpFor(src_arrow);

        SgType* retType = SgTypeVoid::createType();
                
        SgScopeStatement* scope = SI::getScope(bb);

        SgType* var_type = var_name->get_type();
        #ifdef STRIP_TYPEDEFS
        var_type = Util::getType(var_type);
        #endif

        if(isSgPointerType(var_type)) {
            // Now we have a pointer member... we need to create:
             
            // create_entry(&(dest->var_name), &(src->var_name))
            // casting to addr
            // Both args casted, so no need to strip typedefs
            SgExprListExp* p_list = SB::buildExprListExp(Util::castToAddr(ce1), Util::castToAddr(ce2)); 
            
            SgExpression* overload = buildMultArgOverloadFn("create_entry_if_src_exists", p_list, retType,
                                                            scope, fndecl, true);

            exprs->append_expression(overload);
        }
        else if(strDecl::isOriginalClassType(var_type)) {
            // This is a class/struct member. Generate an rtc copy for this one.

            SgExprListExp* p_list = SB::buildExprListExp(ce1, ce2);
            SgExpression* overload = buildMultArgOverloadFn("rtc_copy", p_list, retType, scope, fndecl, true);
            exprs->append_expression(overload);

        }
    }
    
    Util::insertAtTopOfBB(bb, SB::buildExprStatement(exprs));
}

void ArithCheck::handleIncrDecrDerefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, 
                                SgFunctionDeclaration* fndecl, bool isDecr) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // fncall: increment_deref/decrement_deref(str)
    // for increment: *str.ptr++
    // for decrement: *str.ptr--
    // return create_struct(*str.ptr, str.ptr)
    //
    // fncall: increment/decrement(ptr)
    // for increment: (*ptr)++;
    // for decrement: (*ptr)--;
    // return create_struct(*ptr, (addr_type)ptr)

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    ROSE_ASSERT(args.size() == 1);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // ptr/str
    SgInitializedName* arg1 = *iter;

    SgExpression* ptr;

    // str
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
    #else
    if(strDecl::isValidStructType(arg1->get_type()))
    #endif
    {
        ptr = Util::createDotExpFor(arg1, "ptr");
    }
    // ptr
    else if(Util::isQualifyingType(arg1->get_type())) {
        ptr = SB::buildVarRefExp(arg1);
    }
    else {
        printf("handleIncrDecrDerefDef - unsupported case for arg\n");
        printf("arg\n");
        Util::printNode(arg1);
        ROSE_ASSERT(0);
    }

    SgExpression* ptr_deref = SB::buildPointerDerefExp(ptr);

    SgExpression* arith_op;
    if(isDecr) {
        arith_op = SB::buildMinusMinusOp(ptr_deref);
    }
    else {
        arith_op = SB::buildPlusPlusOp(ptr_deref);
    }

    fnbody->append_statement(SB::buildExprStatement(arith_op));

    // return create_struct(...)
    SgExprListExp* p_list = SB::buildExprListExp(SI::copyExpression(ptr_deref), Util::castToAddr(SI::copyExpression(ptr)));
    SgExpression* overload = buildMultArgOverloadFn("create_struct", p_list, retType, scope, fndecl, true);

    appendReturnStmt(overload, fnbody);
}



void ArithCheck::handleIncrDecrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, 
                                SgFunctionDeclaration* fndecl, bool isDecr) {

    #ifdef DELAYED_INSTR
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // fncall: increment/decrement(str)
    // for increment: str.ptr++
    // for decrement: str.ptr--
    // return str
    //
    // fncall: increment/decrement(**ptr)
    // for increment: (*ptr)++;
    // for decrement: (*ptr)--;
    // struct output;
    // output.ptr = *ptr;
    // output.addr = (addr type)ptr;
    // return output;

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    ROSE_ASSERT(args.size() == 1);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;
    
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
    #else
    if(strDecl::isValidStructType(arg1->get_type()))
    #endif
    {
        // fncall: incr/decr(str)
        SgExpression* incr_decr;

        SgExpression* ptr_dot = Util::createDotExpFor(arg1, "ptr");

        if(isDecr) {
            incr_decr = SB::buildMinusMinusOp(ptr_dot);
        }
        else {
            incr_decr = SB::buildPlusPlusOp(ptr_dot);
        }

        fnbody->append_statement(SB::buildExprStatement(incr_decr));

        appendReturnStmt(SB::buildVarRefExp(arg1), fnbody);
    }
    else {
        // fncall: incr/decr(ptr)

        SgExpression* incr_decr;

        // (*ptr)++ / (*ptr)--
        if(isDecr) {
                incr_decr = SB::buildMinusMinusOp(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)));
        }
        else {
                incr_decr = SB::buildPlusPlusOp(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)));
        }

        fnbody->append_statement(SB::buildExprStatement(incr_decr));


        // struct output
        // arg1 base type
        SgType* arg1_base_type = isSgPointerType(arg1->get_type())->get_base_type();

        // output type
        SgType* output_type = strDecl::getStructType(arg1_base_type, fndecl, true);    

        SgVariableDeclaration* output_var = SB::buildVariableDeclaration("output", output_type, NULL, SI::getScope(fnbody));
        fnbody->append_statement(output_var);

        // output.ptr = *ptr;
        SgExpression* ptr_assign = SB::buildAssignOp(Util::createDotExpFor(Util::getNameForDecl(output_var), "ptr"), 
                        SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)));
        fnbody->append_statement(SB::buildExprStatement(ptr_assign));

        // output.addr = (addr type)ptr;
        SgExpression* addr_assign = SB::buildAssignOp(Util::createDotExpFor(Util::getNameForDecl(output_var), "addr"),
                        Util::castToAddr(SB::buildVarRefExp(arg1)));
        fnbody->append_statement(SB::buildExprStatement(addr_assign));

        // return output
        appendReturnStmt(SB::buildVarRefExp(output_var), fnbody);
    }

    #else
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // this fn takes in a single struct, by ref
    // fncall: increment/decrement(*input1_str)
    // for increment: input1_str->ptr++;
    // for decrement: input1_str->ptr--;
    // return *input1_str;

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    ROSE_ASSERT(args.size() == 1);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;
    // first arg is pointer type -- pointer to struct
    ROSE_ASSERT(isSgPointerType(arg1->get_type()));

    // create an arrow exp, and perform incr/decr on it.
    SgExpression* ptr_exp = Util::createArrowExpFor(arg1, "ptr");

    SgExpression* incr_decr_exp;
    if(isDecr) {
        incr_decr_exp = SB::buildMinusMinusOp(ptr_exp);
    }
    else {
        incr_decr_exp = SB::buildPlusPlusOp(ptr_exp);
    }

    SgExprStatement* incr_decr_stmt = SB::buildExprStatement(incr_decr_exp);
    fnbody->append_statement(incr_decr_stmt);

    // return *input_str
    appendReturnStmt(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)), fnbody);
    #endif
}

SgExpression* ArithCheck::getCondExpOper(SgInitializedName* var_name) {

    // create the appropriate oper --
    // if arg1 is a struct, create str.ptr
    // else just create a reference to the argument...
    SgExpression* oper;

    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(var_name->get_type(), GEFD(var_name))) 
    #else
    if(strDecl::isValidStructType(var_name->get_type()))    
    #endif
    {
        oper = Util::createDotExpFor(var_name, "ptr");
    }
    else {
        // anything else, will be seen within the function as
        // a variable...
        oper = SB::buildVarRefExp(var_name);
    }

    return oper;
}

SgExpression* ArithCheck::createConditionalExp(SgName s_name, SgExpression* lhs, SgExpression* rhs) {
    // For reference
    #if 0
    case V_SgLessThanOp: sprintf(output, "LessThan"); break;
    case V_SgGreaterThanOp: sprintf(output, "GreaterThan"); break;
    case V_SgNotEqualOp: sprintf(output, "NotEqual"); break;
    case V_SgLessOrEqualOp: sprintf(output, "LessOrEqual"); break;
    case V_SgGreaterOrEqualOp: sprintf(output, "GreaterOrEqual"); break;
    case V_SgEqualityOp: sprintf(output, "Equality"); break;
    #endif

    SgExpression* cond_exp;

    if(Util::compareNames(s_name, "LessThan")) {
        cond_exp = SB::buildLessThanOp(lhs, rhs);
    }
    else if(Util::compareNames(s_name, "GreaterThan")) {
        cond_exp = SB::buildGreaterThanOp(lhs, rhs);
    }
    else if(Util::compareNames(s_name, "NotEqual")) {
        cond_exp = SB::buildNotEqualOp(lhs, rhs);
    }
    else if(Util::compareNames(s_name, "LessOrEqual")) {
        cond_exp = SB::buildLessOrEqualOp(lhs, rhs);
    }
    else if(Util::compareNames(s_name, "GreaterOrEqual")) {
        cond_exp = SB::buildGreaterOrEqualOp(lhs, rhs);
    }
    else if(Util::compareNames(s_name, "Equality")) {
        cond_exp = SB::buildEqualityOp(lhs, rhs);
    }
    else {
        printf("Unsupported conditional op: %s\n", s_name.getString().c_str());
        ROSE_ASSERT(0);
    }

    return cond_exp;

}

#if 0
bool isCondString(SgName s_name) {

    if(Util::compareNames(s_name, "LessThan")) {
        return true;
    }
    else if(Util::compareNames(s_name, "GreaterThan")) {
        return true;
    }
    else if(Util::compareNames(s_name, "NotEqual")) {
        return true;
    }
    else if(Util::compareNames(s_name, "LessOrEqual")) {
        return true;
    }
    else if(Util::compareNames(s_name, "GreaterOrEqual")) {
        return true;
    }
    else if(Util::compareNames(s_name, "Equality")) {
        return true;
    }
    else {
        return false;
    }


}
#endif

void ArithCheck::handleConditionalDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgName s_name, SgNode* node,
                                SgFunctionDeclaration* fndecl) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, node);
    // this fn takes in a single struct, by ref
    // fncall: cond(*input1_str)
    // return lhs.ptr (cond) rhs.ptr; 

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;

    // create the appropriate lhs expr --
    // if arg1 is a struct, create str.ptr
    // else just create a reference to the argument...
    SgExpression* lhs_exp;
    
    #ifdef CPP_SCOPE_MGMT 
    if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
    #else
    if(strDecl::isValidStructType(arg1->get_type()))    
    #endif
    {
        lhs_exp = Util::createDotExpFor(arg1, "ptr");
    }
    else {
        // anything else, will be seen within the function as
        // a variable...
        lhs_exp = SB::buildVarRefExp(arg1);
    }

    // Get the second arg
    iter++;
    SgInitializedName* arg2 = *iter;

    SgExpression* rhs_exp;
    // treat as first arg
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(arg2->get_type(), GEFD(fndecl))) 
    #else
    if(strDecl::isValidStructType(arg2->get_type()))
    #endif
    {
        rhs_exp = Util::createDotExpFor(arg2, "ptr");
    }
    else {
        rhs_exp = SB::buildVarRefExp(arg2);
    }

    SgExpression* cond_exp = createConditionalExp(s_name, lhs_exp, rhs_exp); 

    appendReturnStmt(cond_exp, fnbody);
}

void ArithCheck::handleAddSubDerefDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, 
                                SgFunctionDeclaration* fndecl, bool isSub) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // fncall: add/sub(ptr, exp)
    // return create_struct(*ptr +/- exp, ptr);

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // arg1
    SgInitializedName* arg1 = *iter;

    // arg2 
    iter++;
    SgInitializedName* arg2 = *iter;

    SgExprListExp* p_list;

    // one of the args should be a double pointer... since we are taking
    // it before a deref, and after the deref, it still has pointer type
    if(Util::isQualifyingType(arg1->get_type())) {
        SgPointerType* arg1_type = isSgPointerType(arg1->get_type());
        ROSE_ASSERT(Util::isQualifyingType(arg1_type->get_base_type()));

        SgExpression* ptr_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));
        SgExpression* exp = SB::buildVarRefExp(arg2);
        
        SgExpression* arith_op;
        if(isSub) {
            arith_op = SB::buildSubtractOp(ptr_deref, exp);
        }
        else {
            arith_op = SB::buildAddOp(ptr_deref, exp);
        }

        p_list = SB::buildExprListExp(arith_op, Util::castToAddr(SB::buildVarRefExp(arg1)));
    }
    else {
        ROSE_ASSERT(Util::isQualifyingType(arg2->get_type()));
        SgPointerType* arg2_type = isSgPointerType(arg2->get_type());
        ROSE_ASSERT(Util::isQualifyingType(arg2_type->get_base_type()));

        SgExpression* exp = SB::buildVarRefExp(arg1);
        SgExpression* ptr_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(arg2));

        SgExpression* arith_op;
        if(isSub) {
            arith_op = SB::buildSubtractOp(exp, ptr_deref);
        }
        else {
            arith_op = SB::buildAddOp(exp, ptr_deref);
        }

        p_list = SB::buildExprListExp(arith_op, Util::castToAddr(SB::buildVarRefExp(arg2)));

    }

    SgExpression* overload = buildMultArgOverloadFn("create_struct", p_list, retType,
                                                    scope, fndecl, true);
    appendReturnStmt(overload, fnbody);

}

void ArithCheck::handleAddSubDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, 
                                SgFunctionDeclaration *fndecl, bool isSub) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // this fn takes in a single struct, by ref
    // fncall: add/sub(input1_str, input2)
    // for addition: str.ptr += val;
    // for subtraction: str.ptr -= val;
    // return str;

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // get the first arg
    SgInitializedName* arg1 = *iter;
    

    // get the second arg
    iter++;
    SgInitializedName* arg2 = *iter;

    // If one of the two arguments is of struct type, then its
    // good enough.

    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)) ||
                strDecl::isValidStructType(arg2->get_type(), GEFD(fndecl)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()) ||
                strDecl::isValidStructType(arg2->get_type()));
    #endif

    SgExpression* arith_assign;
    if(isSub) {
        #ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
        #else
        if(strDecl::isValidStructType(arg1->get_type()))
        #endif
        {
            arith_assign = SB::buildMinusAssignOp(Util::createDotExpFor(arg1, "ptr"), SB::buildVarRefExp(arg2));
        }
        else {
            arith_assign = SB::buildMinusAssignOp(Util::createDotExpFor(arg2, "ptr"), SB::buildVarRefExp(arg1));
        }
    }
    else {
        #ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
        #else
        if(strDecl::isValidStructType(arg1->get_type())) 
        #endif
        {
            arith_assign = SB::buildPlusAssignOp(Util::createDotExpFor(arg1, "ptr"), SB::buildVarRefExp(arg2));
        }
        else {
            arith_assign = SB::buildPlusAssignOp(Util::createDotExpFor(arg2, "ptr"), SB::buildVarRefExp(arg1));
        }
    }
    SgExprStatement* arith_stmt = SB::buildExprStatement(arith_assign);
    fnbody->append_statement(arith_stmt);

    // pass the struct back.. in return
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
    #else
    if(strDecl::isValidStructType(arg1->get_type()))
    #endif
    {
        appendReturnStmt(SB::buildVarRefExp(arg1), fnbody);
    }
    else {
        appendReturnStmt(SB::buildVarRefExp(arg2), fnbody);
    }
}

void ArithCheck::handleValueToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project) {


    // This is a value to struct cast.
    // fncall: cast(ptr, addr, size, lock)
    // body: 
    // create_entry(addr, base, size, lock)
    // return create_struct(ptr, addr);


    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // the first arg... ptr
    SgInitializedName* ptr = *iter;
    SgExpression* ptr_casted = Util::castToAddr(SB::buildVarRefExp(ptr));
    
    // the second arg... addr
    ++iter;
    SgInitializedName* addr = *iter;

    // the third arg... size
    ++iter;
    SgInitializedName* size = *iter;

    // the fourth arg... lock
    ++iter;
    SgInitializedName* lock = *iter;

    // create_entry
    #if 0
    SgExprListExp* p_list = SB::buildExprListExp(SB::buildVarRefExp(addr), SB::buildVarRefExp(ptr),
                                            SB::buildVarRefExp(size), SB::buildVarRefExp(lock));
    #endif
    SgExprListExp* p_list = SB::buildExprListExp(SB::buildVarRefExp(addr), ptr_casted,
                                            SB::buildVarRefExp(size), SB::buildVarRefExp(lock));

    
    SgExpression* ce_ovl = buildMultArgOverloadFn("create_entry", p_list,
                                                SgTypeVoid::createType(),
                                                scope, fndecl, true);    

    fnbody->append_statement(SB::buildExprStatement(ce_ovl));

    // create_struct
    SgExprListExp* p_list2 = SB::buildExprListExp(SB::buildVarRefExp(ptr), SB::buildVarRefExp(addr));
    
    SgExpression* cs_ovl = buildMultArgOverloadFn("create_struct", p_list2, 
                                                retType, scope, fndecl, true);
    
    appendReturnStmt(cs_ovl, fnbody);
    

    
    #if 0
    // this is a value to struct cast. So, no metadata to be recorded.
    // the address would be the address of the output.ptr --- FIXME: NOT SURE if this right...  
    // the fncall: Cast(input1)
    // str output;
    // output.ptr = reinterpret_cast<(str.ptr type)>(input1);
    // output.addr = &output.ptr
    // return output;

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // the first arg...
    SgInitializedName* arg1 = *iter;

    // create local variable of return type: str output;
    SgVariableDeclaration* output_var = Util::createLocalVariable("output", retType, NULL, SI::getScope(fnbody));
    fnbody->append_statement(output_var);

    // now: output.ptr = reinterpret_cast<(str.ptr type)>(input1);
    SgType* ptr_type = strDecl::findInUnderlyingType(retType);
//    SgExpression* rhs = SB::buildCastExp(SB::buildVarRefExp(arg1), ptr_type, SgCastExp::e_reinterpret_cast);
    SgExpression* rhs = SB::buildCastExp(SB::buildVarRefExp(arg1), ptr_type, CAST_TYPE);

    SgExprStatement* ptr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "ptr", rhs); 
    fnbody->append_statement(ptr_assign);

    // now: output.addr = &output.ptr
    SgType* addr_type = Util::getAddrType();
    SgExpression* ptr_exp = Util::createDotExpFor(Util::getNameForDecl(output_var), "ptr");
    SgExpression* addr_ptr_exp = Util::createAddressOfOpFor(ptr_exp);
//    SgExpression* addr_rhs = SB::buildCastExp(addr_ptr_exp, addr_type, SgCastExp::e_reinterpret_cast);
    // Changing it from output.addr = &output.ptr 
    // to output.addr = NULL (represented as an integer 0)
    #if 0
    SgExpression* addr_rhs = SB::buildCastExp(addr_ptr_exp, addr_type, CAST_TYPE);
    SgExprStatement* addr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "addr", addr_rhs);
    #else
    SgExprStatement* addr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "addr", SB::buildIntVal(0));
    #endif

    fnbody->append_statement(addr_assign);

    // now: return output;
    appendReturnStmt(SB::buildVarRefExp(output_var), fnbody);
    #endif

}

void ArithCheck::handleStructToValueCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project) {

    // FIXME: Looks broken. Why are we casting to addr type when we
    // should casting to return type????
    // this is a struct to value cast. So, we simply cast the pointer
    // within the struct to the ret Type.
    // the fncall: Cast(input_str)
    // return reinterpret_cast<(addr type)>(input_str.ptr);

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // the first arg...
    SgInitializedName* arg1 = *iter;

    SgExpression* dot_exp = Util::createDotExpFor(arg1, "ptr");
//    SgExpression* casted_exp = SB::buildCastExp(dot_exp, Util::getAddrType(), SgCastExp::e_reinterpret_cast);
    SgExpression* casted_exp = SB::buildCastExp(dot_exp, Util::getAddrType(), CAST_TYPE);

    // send this through the return stmt
    appendReturnStmt(casted_exp, fnbody);

}

void ArithCheck::handleStructToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project) {

    // this is a struct to struct cast. probably the most common type.
    // here, we cast the pointer from the input type, to the output type (return type)
    // copy the address over as is. this simple copy should be okay since
    // this operation by itself doesn't change the metadata anyways.
    // the fncall: Cast(input_str)
    // struct __Pb__Ui__Pe__ output;
    // output.ptr = (output_str.ptr type*)input_str.ptr;
    // output.addr = input_str.addr;
    // return output;

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // the first arg...
    SgInitializedName* arg1 = *iter;

    // create local variable of return type: str output;
    SgVariableDeclaration* output_var = Util::createLocalVariable("output", retType, NULL, SI::getScope(fnbody));
    fnbody->append_statement(output_var);

    // now: output.ptr = (output.ptr type*)input_str.ptr;
    SgExpression* input_ptr_exp = Util::createDotExpFor(arg1, "ptr");
    SgType* ret_type_ptr = strDecl::findInUnderlyingType(retType);
//    SgExpression* casted_input_ptr_exp = SB::buildCastExp(input_ptr_exp, ret_type_ptr, SgCastExp::e_reinterpret_cast);
    SgExpression* casted_input_ptr_exp = SB::buildCastExp(input_ptr_exp, ret_type_ptr, CAST_TYPE);
    SgExprStatement* ptr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "ptr", casted_input_ptr_exp);
    fnbody->append_statement(ptr_assign);

    // now: output.addr = input_str.addr;
    SgExpression* input_addr_exp = Util::createDotExpFor(arg1, "addr");
    SgExprStatement* addr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "addr", input_addr_exp);
    fnbody->append_statement(addr_assign);

    // return output;
    appendReturnStmt(SB::buildVarRefExp(output_var), fnbody);

}

void ArithCheck::handleVarRefToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project) {

    // the fncall: Cast(var_ref, &var_ref)
    // struct_retType output;
    // output.ptr = (retType)var_ref;
    // output.addr = &var_ref;
    // return output

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // the first arg...
    SgInitializedName* arg1 = *iter;

    // the second arg...
    ++iter;
    SgInitializedName* arg2 = *iter;

    // create local variable of return type: str output;
    SgVariableDeclaration* output_var = Util::createLocalVariable("output", retType, NULL, SI::getScope(fnbody));
    fnbody->append_statement(output_var);

    // output.ptr
    SgExpression* output_ptr = Util::createDotExpFor(Util::getNameForDecl(output_var), "ptr");
    SgExpression* casted_var_ref = SB::buildCastExp(SB::buildVarRefExp(arg1), output_ptr->get_type(), SgCastExp::e_C_style_cast);
    SgExprStatement* ptr_assign = SB::buildExprStatement(SB::buildAssignOp(output_ptr, casted_var_ref));
    fnbody->append_statement(ptr_assign);

    // output.addr
    SgExpression* output_addr = Util::createDotExpFor(Util::getNameForDecl(output_var), "addr");
    SgExprStatement* addr_assign = SB::buildExprStatement(SB::buildAssignOp(output_addr, SB::buildVarRefExp(arg2)));
    fnbody->append_statement(addr_assign);

    // return output
    appendReturnStmt(SB::buildVarRefExp(output_var), fnbody);
}



void ArithCheck::handleCastDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project, 
                            SgNode* pos, SgFunctionDeclaration* fndecl) {
    
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // Four cases
    // 1. value_exp to struct
    // 2. qual_type_exp to struct
    // 2. struct to struct
    // 3. struct to value


    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
//    ROSE_ASSERT(args.size() == 1);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // get the first arg
    SgInitializedName* arg1 = *iter;

    if(Util::isQualifyingType(arg1->get_type()) && args.size() == 2) {
        handleVarRefToStructCastDef(fndecl, retType, param_list, scope, project);
    }
    #ifdef CPP_SCOPE_MGMT
    else if(!strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) 
    #else
    else if(!strDecl::isValidStructType(arg1->get_type()))    
    #endif
    {
        // fncall: cast(ptr, addr, sizeof(ptr), lock)
        // The output should be a struct in this case.
    
        #ifdef CPP_SCOPE_MGMT
        ROSE_ASSERT(strDecl::isValidStructType(retType, GEFD(fndecl)));
        #else
        ROSE_ASSERT(strDecl::isValidStructType(retType));
        #endif
        // This is a value to struct... 
        handleValueToStructCastDef(fndecl, retType, param_list, scope, project);
    }
    else {
        // in the other two cases, the argument is a struct
        #ifdef CPP_SCOPE_MGMT
        ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));
        #else
        ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()));
        #endif

        #ifdef CPP_SCOPE_MGMT
        if(!strDecl::isValidStructType(retType, GEFD(fndecl))) 
        #else
        if(!strDecl::isValidStructType(retType))
        #endif
        {
            // returning a value from a struct
            handleStructToValueCastDef(fndecl, retType, param_list, scope, project);
        }
        else {
            // struct to struct cast
            handleStructToStructCastDef(fndecl, retType, param_list, scope, project);
        }
    }
}

void ArithCheck::handleStructAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {

//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // This is the function that handles something like "&ptr"... This turns into... "&str"...
    // and the fncall here looks like
    // fncall: AddressOf(input1_ptr, input2)
    // body: input1_ptr->ptr = input2.addr (since we are taking the address of the pointer)
    //       input1_ptr->addr = &input1_ptr->ptr (this is the variable that comes in from the caller)
    //     create_entry(input1_ptr->addr, (unsigned long long)input1_ptr->ptr, sizeof(input2.addr))
    //       return (void)

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // get the first arg
    SgInitializedName* arg1 = *iter;
    
    // get the second arg
    iter++;
    SgInitializedName* arg2 = *iter;
        
    // input2.addr
    SgExpression* input2_addr = Util::createDotExpFor(arg2, "addr");
    SgExpression* input2_addr_casted = SB::buildCastExp(input2_addr, strDecl::findInUnderlyingType(isSgPointerType(arg1->get_type())->get_base_type()), SgCastExp::e_C_style_cast);
    
    // input1_ptr->ptr = input2.addr
    SgExprStatement* ptr_assign = Util::createArrowAssign(arg1, "ptr", input2_addr_casted);
    fnbody->append_statement(ptr_assign);

    // input1_ptr->addr = &input1_ptr->ptr;
    SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(Util::createArrowExpFor(arg1, "ptr")));
    SgExprStatement* addr_assign = Util::createArrowAssign(arg1, "addr", ptr_addr);
    fnbody->append_statement(addr_assign);
    
    // create_entry(input1_ptr->addr, (unsigned long long)input1_ptr->ptr, sizeof(input2.addr))
    SgExpression* ce_arg1 = Util::createArrowExpFor(arg1, "addr");
    SgExpression* ce_arg2 = Util::castToAddr(Util::createArrowExpFor(arg1, "ptr"));
    SgExpression* ce_arg3 = SB::buildCastExp(SB::buildSizeOfOp(Util::createDotExpFor(arg2, "addr")), Util::getSizeOfType(), SgCastExp::e_C_style_cast); 
    
    // Working one
    #if 0
    SgExpression* overload = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, GEFD(fnbody));
    #else
//    SgExpression* overload = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, pos);
    //SgExpression* overload = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, fndecl);

    SgExpression* overload = buildMultArgOverloadFn("create_entry", SB::buildExprListExp(ce_arg1, ce_arg2, ce_arg3), SgTypeVoid::createType(), scope, fndecl, true);

    #endif
    SgExprStatement* ovl_stmt = SB::buildExprStatement(overload);
    fnbody->append_statement(ovl_stmt);

    // returning void... nothing to do.
}

void ArithCheck::handleNormalAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {
    
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    #ifdef LOCK_KEY_INSERT
    // The fn call: AddressOf(input1_ptr, input2, lock)
    // The third input is the lock and we get the key using the getKey function implemented in metadata.C 
    #else
    // Difference between AddressOf and create_struct -- AddressOf provides the pointer and the size,
    // but never the address to be stored in the struct. create_struct provides the ptr and addr. 
    // AddressOf provides the ptr and size fields, not the addr field.
    // AddressOf(&stack_var, sizeof(stackvar))
    // the fncall: AddressOf(input1_ptr, input2)
    // str output;
    // output.ptr = reinterpret_cast<output.ptr type>(input1_ptr);
    // output.addr = &output.ptr;
    // create_entry(output.addr, output.ptr, input2);
    // return output;
    #endif



    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    #ifdef LOCK_KEY_INSERT
    // One more arg for the lock...
    ROSE_ASSERT(args.size() == 3);
    #else
    ROSE_ASSERT(args.size() == 2);
    #endif
    SgInitializedNamePtrList::iterator iter = args.begin();

    // get the first arg
    SgInitializedName* arg1 = *iter;

    // create local variable of return type: str output;
    SgVariableDeclaration* output_var = Util::createLocalVariable("output", retType, NULL, fnbody);
    fnbody->append_statement(output_var);

    // output.ptr = reinterpret_cast<output.ptr type>(input1_ptr);
    SgType* ret_ptr_type = strDecl::findInUnderlyingType(retType);
//    SgExpression* casted_input = SB::buildCastExp(SB::buildVarRefExp(arg1), ret_ptr_type, SgCastExp::e_reinterpret_cast);
    SgExpression* casted_input = SB::buildCastExp(SB::buildVarRefExp(arg1), ret_ptr_type, CAST_TYPE);
    SgExprStatement* ptr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "ptr", casted_input);
    fnbody->append_statement(ptr_assign);

    // output.addr = &output.ptr
    SgExpression* ptr_addr = Util::createAddressOfOpFor(Util::createDotExpFor(Util::getNameForDecl(output_var), "ptr"));
//    SgExpression* casted_ptr_addr = SB::buildCastExp(ptr_addr, Util::getAddrType(), SgCastExp::e_reinterpret_cast);
    SgExpression* casted_ptr_addr = SB::buildCastExp(ptr_addr, Util::getAddrType(), CAST_TYPE);
    SgExprStatement* addr_assign = Util::createDotAssign(Util::getNameForDecl(output_var), "addr", casted_ptr_addr);
    fnbody->append_statement(addr_assign);

    // create_entry(output.addr, output.ptr, input2)
    SgExpression* ce_arg1 = Util::createDotExpFor(Util::getNameForDecl(output_var), "addr");
    // need to cast the ptr to addr type...
//    SgExpression* ce_arg2 = SB::buildCastExp(Util::createDotExpFor(Util::getNameForDecl(output_var), "ptr"), Util::getAddrType(), SgCastExp::e_reinterpret_cast);
    SgExpression* ce_arg2 = SB::buildCastExp(Util::createDotExpFor(Util::getNameForDecl(output_var), "ptr"), Util::getAddrType(), CAST_TYPE);
    // Get second arg
    iter++;
    SgInitializedName* arg2 = *iter;
    // cast second arg to size of type
    //SgExpression* ce_arg3 = SB::buildCastExp(SB::buildVarRefExp(arg2), Util::getSizeOfType(), SgCastExp::e_reinterpret_cast);
    SgExpression* ce_arg3 = SB::buildCastExp(SB::buildVarRefExp(arg2), Util::getSizeOfType(), SgCastExp::e_C_style_cast);

    #ifdef LOCK_KEY_INSERT
    // Use the third arg to create a lock argument
    iter++;
    SgInitializedName* arg3 = *iter;
    SgExpression* ce_arg4 = SB::buildCastExp(SB::buildVarRefExp(arg3), getLockType(), SgCastExp::e_C_style_cast);
    SgExprListExp* parameter_list = SB::buildExprListExp(ce_arg1, ce_arg2, ce_arg3, ce_arg4);
    SgExpression* ce_fn = buildMultArgOverloadFn("create_entry", parameter_list, SgTypeVoid::createType(), scope, fndecl, true);
    #else
    // the create_entry function
    // Working one
    #if 0
    SgExpression* ce_fn = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, GEFD(fnbody));
    #else
    //SgExpression* ce_fn = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, pos);

    //SgExpression* ce_fn = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, fndecl);
    SgExpression* ce_fn = buildMultArgOverloadFn("create_entry", SB::buildExprListExp(ce_arg1, ce_arg2, ce_arg3), SgTypeVoid::createType(), scope, fndecl, true);

    #endif
    #endif
    SgExprStatement* ce_stmt = SB::buildExprStatement(ce_fn);
    fnbody->append_statement(ce_stmt);

    // return output;
    appendReturnStmt(SB::buildVarRefExp(Util::getNameForDecl(output_var)), fnbody);
    
}


void ArithCheck::handleDoubleDerefCheckDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);    
    // input: DoubleDerefCheck(ptr)
    // body: check_entry(*ptr, (addr_type)ptr)
    // return *ptr

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // ptr
    SgInitializedName* ptr = *iter;

    SgExprListExp* p_list = SB::buildExprListExp(Util::castToAddr(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr))),
                                            Util::castToAddr(SB::buildVarRefExp(ptr)));
    SgExpression* overload = buildMultArgOverloadFn("check_entry", p_list, SgTypeVoid::createType(), scope, fndecl, true);
    fnbody->append_statement(SB::buildExprStatement(overload));

    // return *ptr
    appendReturnStmt(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr)), fnbody);
}


void ArithCheck::handleReturnPointerDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);    
    // input: return_pointer(str)
    // return str.ptr

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // str
    SgInitializedName* arg1 = *iter;

    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()));
    #endif

    appendReturnStmt(Util::createDotExpFor(arg1, "ptr"), fnbody);
}


void ArithCheck::handleDerefCreateStructDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos,
                                SgFunctionDeclaration* fndecl) {
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);    
    // input: DerefCreateStructDef(str) / DerefCreateStruct(ptr)
    // return create_struct(*str.ptr, (addr_type)str.ptr) / create_struct(*ptr, (addr_type)ptr)

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // str/ptr
    SgInitializedName* arg1 = *iter;

    SgExpression* cs_arg1;
    SgExpression* cs_arg2;

    if(Util::isQualifyingType(arg1->get_type())) {    
        cs_arg1 = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));
        cs_arg2 = Util::castToAddr(SB::buildVarRefExp(arg1));
    }
    else {
        #ifdef CPP_SCOPE_MGMT
        ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));
        #else
        ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()));
        #endif
        cs_arg1 = SB::buildPointerDerefExp(Util::createDotExpFor(arg1, "ptr"));
        cs_arg2 = Util::castToAddr(Util::createDotExpFor(arg1, "ptr"));
    }

    SgExpression* overload = buildMultArgOverloadFn("create_struct", 
                                                SB::buildExprListExp(cs_arg1, cs_arg2), retType,
                                                scope, fndecl, true);
    
    appendReturnStmt(overload, fnbody);
    
}



void ArithCheck::handleAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {

    #ifdef DELAYED_INSTR
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);    
    // input: AddressOf(addr, ptr, size, lock)
    // body: create_entry(addr, (addr_type)ptr, size, lock);
    // return create_struct(ptr, addr)

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    SgInitializedNamePtrList::iterator iter = args.begin();

    // addr
    SgInitializedName* addr = *iter;

    // ptr
    iter++;
    SgInitializedName* ptr = *iter;

    // size
    iter++;
    SgInitializedName* size = *iter;

    // lock
    iter++;
    SgInitializedName* lock = *iter;

    SgExprListExp* p_list = SB::buildExprListExp(SB::buildVarRefExp(addr), Util::castToAddr(SB::buildVarRefExp(ptr)),
                                            SB::buildVarRefExp(size), SB::buildVarRefExp(lock));
    
    SgExpression* overload = buildMultArgOverloadFn("create_entry", p_list, SgTypeVoid::createType(),
                                                    scope, fndecl, true);
    fnbody->append_statement(SB::buildExprStatement(overload));

    SgExprListExp* p_list2 = SB::buildExprListExp(SB::buildVarRefExp(ptr), SB::buildVarRefExp(addr));

    SgExpression* ovl2 = buildMultArgOverloadFn("create_struct", p_list2, retType, scope, fndecl, true);

    appendReturnStmt(ovl2, fnbody);
    
    #else
    // Here, we need to figure out if this function call is catching the address of a struct
    // of a normal var... for that, the second arg will be of struct type, in the former....

    SgExpressionPtrList& exprs = param_list->get_expressions();
    SgExpressionPtrList::iterator iter = exprs.begin();
    ++iter;

    SgExpression* second_arg = *iter;
    
    if(strDecl::isValidStructType(second_arg->get_type())) {
        handleStructAddressOfDef(transf_name, retType, param_list, scope, project, pos, fndecl);
    }
    else {
        handleNormalAddressOfDef(transf_name, retType, param_list, scope, project, pos, fndecl);
    }
    #endif
}

// NOT USING THIS... 
void ArithCheck::handleBoundCheckDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos) {
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);

    
}
        

void ArithCheck::handlePlusMinusAssignDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                    SgScopeStatement* scope, SgProject* project, SgNode* pos, 
                                    SgFunctionDeclaration* fndecl, bool minus) {
    
//    SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
    // This function does a plus/minus assign on a struct.
    // fncall: PlusAssign(*input1_str, input2)
    // plusassign body: input1_str->ptr += input2;
    // minusassign body: input1_str->ptr -= input2;
    // return *input1_str

    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // assign has two args. first is of pointer type
    ROSE_ASSERT(args.size() == 2);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // Get the first arg
    SgInitializedName* arg1 = *iter;
    // first arg is pointer type -- pointer to struct
    ROSE_ASSERT(isSgPointerType(arg1->get_type()));
    
    // input1_str->ptr
    SgExpression* ptr_exp = Util::createArrowExpFor(arg1, "ptr");

    // Get the second arg
    iter++;
    SgInitializedName* arg2 = *iter;

    SgExpression* assign;
    if(minus) {
        assign = SB::buildMinusAssignOp(ptr_exp, SB::buildVarRefExp(arg2));
    }
    else {
        assign = SB::buildPlusAssignOp(ptr_exp, SB::buildVarRefExp(arg2));
    }

    SgExprStatement* assign_stmt = SB::buildExprStatement(assign);
    fnbody->append_statement(assign_stmt);

    // return *input1_str
    appendReturnStmt(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)), fnbody);

}

void ArithCheck::handleCreateEntryWithNewLock(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl) {
    
    // Get the function definition...
    SgFunctionDefinition* fndef = getFuncDef(fndecl);
    SgBasicBlock* fnbody = getFuncBody(fndef);
    SgStatementPtrList& stmts = fnbody->get_statements();
    
    // fncall: create_entry_with_new_lock_gen(addr, ptr, size)
    // body: create_entry_with_new_lock(addr, (unsigned long long)ptr, size)
    //          return {ptr, addr}

    // the args
    SgInitializedNamePtrList& args = fndecl->get_args();
    // assign has two args. first is of pointer type
    ROSE_ASSERT(args.size() == 3);
    SgInitializedNamePtrList::iterator iter = args.begin();

    // addr
    SgExpression* ce1 = SB::buildVarRefExp(*iter); 

    // ptr
    iter++;
    SgExpression* ce2 = SB::buildVarRefExp(*iter);
    SgExpression* ce2_casted = Util::castToAddr(ce2);

    // size
    iter++;
    SgExpression* ce3 = SB::buildVarRefExp(*iter);

    // params
    SgExprListExp* plist = SB::buildExprListExp(ce1, ce2_casted, ce3);

    // create_entry_with_new_lock
    SgExpression* ovl = buildMultArgOverloadFn("create_entry_with_new_lock", plist, 
                                                SgTypeVoid::createType(),
                                                scope, fndecl, true);
    
    fnbody->append_statement(SB::buildExprStatement(ovl));

    // {ptr, addr} 
    SgAggregateInitializer* constinit = SB::buildAggregateInitializer(SB::buildExprListExp(SI::copyExpression(ce2), SI::copyExpression(ce1)),
                                                        retType);
    
    // var = {ptr, addr}    
    SgName var_name("retvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    SgVariableDeclaration* newvar = SB::buildVariableDeclaration(var_name, retType, constinit, scope);
    fnbody->append_statement(newvar);

    // return {ptr, addr}
    appendReturnStmt(SB::buildVarRefExp(newvar), fnbody);

}

void ArithCheck::createFunctionFrom(SgName s_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgName transf_name, SgNode* pos, bool derived) {
    #if 0    
    if(strcmp(transf_name.getString().c_str(), "v_Ret_create_entry_UL_Arg_UL_Arg") == 0) {
        printf("Found\n");
    }
    #endif



        



    // create nondefining decl for all these functions before the first statement...
    #ifndef CPP_SCOPE_MGMT
    createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
    #endif



    if(Util::isExternal(s_name)) {
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else {
        // Create a defining decl... and then call the corresponding handler
        SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos, derived, s_name);

        // Now, select the right handler... pass the fndecl in the arg list...
        if(Util::compareNames(s_name, "create_struct")) {
            handleCreateStructDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "create_struct_from_addr")) {
            handleCreateStructFromAddrDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "rtc_copy")) {
            handleRTCCopyDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "AddressOf")) {
            handleAddressOfDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "Deref")) {
            handleDerefDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "PntrArrRef")) {
            handlePntrArrRefDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "Assign")) {
            handleAssignDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "Increment")) {
            handleIncrDecrDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "Increment_deref")) {
            handleIncrDecrDerefDef(transf_name, retType, param_list, scope, project, pos, fndecl);    
        }
        else if(Util::compareNames(s_name, "Decrement")) {
            handleIncrDecrDef(transf_name, retType, param_list, scope, project, pos, fndecl, true);
        }
        else if(Util::compareNames(s_name, "Decrement_deref")) {
            handleIncrDecrDerefDef(transf_name, retType, param_list, scope, project, pos, fndecl, true);
        }
        else if(Util::isCondString(s_name)) {
            handleConditionalDefs(transf_name, retType, param_list, scope, project, s_name, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "Add")) {
            handleAddSubDefs(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "Add_deref")) {
            handleAddSubDerefDefs(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "Sub")) {
            handleAddSubDefs(transf_name, retType, param_list, scope, project, pos, fndecl, true);
        }
        else if(Util::compareNames(s_name, "Sub_deref")) {
            handleAddSubDerefDefs(transf_name, retType, param_list, scope, project, pos, fndecl, true);
        }
        else if(Util::compareNames(s_name, "Cast")) {
            handleCastDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "PlusAssign")) {
            handlePlusMinusAssignDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "MinusAssign")) {
            handlePlusMinusAssignDef(transf_name, retType, param_list, scope, project, pos, fndecl, true);
        }
        else if(Util::compareNames(s_name, "ptr_check")) {
            handlePtrCheckDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "deref_check")) {
            handleDerefCheckDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "deref_check_with_str")) {
            handleDerefCheckWithStrDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "assign_and_copy")) {
            handleAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "deref_assign_and_copy")) {
            handleDerefAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, fndecl);    
        }    
        else if(Util::compareNames(s_name, "lhs_deref_assign_and_copy")) {
            handleLHSDerefAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, fndecl);    
        }
        else if(Util::compareNames(s_name, "rhs_deref_assign_and_copy")) {
            handleRHSDerefAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, fndecl);    
        }
        else if(Util::compareNames(s_name, "double_deref_check")) {
            handleDoubleDerefCheckDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "deref_create_struct")) {
            handleDerefCreateStructDef(transf_name, retType, param_list, scope, project, pos, fndecl); 
        }
        else if(Util::compareNames(s_name, "return_pointer")) {
            handleReturnPointerDef(transf_name, retType, param_list, scope, project, pos, fndecl);    
        }
        else if(Util::compareNames(s_name, "cs_assign_and_copy")) {
            handleCSAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else if(Util::compareNames(s_name, "create_entry_with_new_lock_gen")) {
            handleCreateEntryWithNewLock(transf_name, retType, param_list, scope, project, pos, fndecl);
        }
        else {
            // This should become a ROSE_ASSERT(0) once all fns are defined properly
            printf("Can't create def. transf_name: %s\n", transf_name.getString().c_str());
            // already created nondefining decl above..
            //createNondefiningDecl(transf_name, retType, param_list, scope, project);
            ROSE_ASSERT(0);
        }

    }
    #if 0
    
    if(Util::compareNames(s_name, "execAtFirst")) {
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
        // we use nondefining decl for this
    }
    else if(Util::compareNames(s_name, "execAtLast")) {
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
        // we use nondefining decl for this
    }
    else if(Util::compareNames(s_name, "create_struct")) {
        handleCreateStructDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "create_struct_from_addr")) {
        handleCreateStructFromAddrDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "create_entry")) {
        // create_entry is handled in the library. This is due to 2 reasons:
        // 1. The implementation of the metadata structure is not linked to the implementation
        // here. This will allow us to plug in different implementations without changing
        // the instrumentation generated from here.
        // 2. create_entry can get complicated, in terms of the statements used within,
        // and creating a definition here would be difficult, and possibly buggy.
        
        // In light of these reasons, we create a nondefiningdecl...
        #ifdef CREATE_FUNCTION_DEBUG
        printf("create_entry -- creating nondefining decl\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif

        //createNondefiningDecl(transf_name, retType, param_list, scope, project, node);
        //handleCreateEntryDef(transf_name, retType, param_list, scope, project);
    }
    else if(Util::compareNames(s_name, "create_entry_if_src_exists")) {
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
        // defined externally. nondefining decl added
    }
    else if(Util::compareNames(s_name, "create_dummy_entry")) {
        #ifdef CREATE_FUNCTION_DEBUG
        printf("create_dummy_entry - nondefining decl\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "getTopLock")) {
        #ifdef CREATE_FUNCTION_DEBUG
        printf("getTopLock - forward declared\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "getTopKey")) {
        #ifdef CREATE_FUNCTION_DEBUG
        printf("getTopKey - forward declared\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "EnterScope")) {
        #ifdef CREATE_FUNCTION_DEBUG
        printf("EnterScope - forward declared\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "ExitScope")) {
        #ifdef CREATE_FUNCTION_DEBUG
        printf("ExitScope - forward declared\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    
    }
    else if(Util::compareNames(s_name, "check_entry")) {
        #ifdef CREATE_FUNCTION_DEBUG
        printf("check_entry -- creating nondefining decl\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif

    }
    else if(Util::compareNames(s_name, "null_check")) {
        #ifdef CREATE_FUNCTION_DEBUG
        printf("null_check -- create nondefining decl\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "rtc_copy")) {
        handleRTCCopyDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "update_initinfo")) {
        // Defined in metadata
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "check_initinfo")) {
        // Defined in metadata
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    #ifdef RETAIN_FUNC_PROTOS
    else if(Util::compareNames(s_name, "push_to_stack")) {
        // Defined in metadata
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "pop_from_stack")) {
        // Defined in metadata
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "get_from_stack")) {
        // Defined in metadata
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    #endif
    else if(Util::compareNames(s_name, "malloc_overload")) {
        // Since these functions are 
        // 1. tied to the metadata structure (possibly), and
        // 2. only one version needs to be implemented,
        // we create a nondefining decl..
        //handleMallocOverloadDef(transf_name, retType, param_list, scope, project);
        #ifdef CREATE_FUNCTION_DEBUG
        printf("malloc_overload -- creating nondefining decl -- already done above\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
        //createNondefiningDecl(transf_name, retType, param_list, scope, project);

    }
    else if(Util::compareNames(s_name, "realloc_overload")) {
        // see explanation for malloc_overload above
        #ifdef CREATE_FUNCTION_DEBUG
        printf("realloc_overload -- creating nondefining decl -- already done above\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "free_overload")) {
        // see explanation for malloc_overload above
        //handleFreeOverloadDef(transf_name, retType, param_list, scope, project);
        #ifdef CREATE_FUNCTION_DEBUG
        printf("free_overload -- creating nondefining decl -- already done above\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
        //createNondefiningDecl(transf_name, retType, param_list, scope, project);

    }
    else if(Util::compareNames(s_name, "AddressOf")) {
        handleAddressOfDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "Deref")) {
        handleDerefDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "PntrArrRef")) {
        handlePntrArrRefDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "Assign")) {
        handleAssignDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "Increment")) {
        handleIncrDecrDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "Increment_deref")) {
        handleIncrDecrDerefDef(transf_name, retType, param_list, scope, project, pos, derived);    
    }
    else if(Util::compareNames(s_name, "Decrement")) {
        handleIncrDecrDef(transf_name, retType, param_list, scope, project, pos, derived, true);
    }
    else if(Util::compareNames(s_name, "Decrement_deref")) {
        handleIncrDecrDerefDef(transf_name, retType, param_list, scope, project, pos, derived, true);
    }
    else if(Util::isCondString(s_name)) {
        handleConditionalDefs(transf_name, retType, param_list, scope, project, s_name, pos, derived);
    }
    else if(Util::compareNames(s_name, "Add")) {
        handleAddSubDefs(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "Add_deref")) {
        handleAddSubDerefDefs(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "Sub")) {
        handleAddSubDefs(transf_name, retType, param_list, scope, project, pos, derived, true);
    }
    else if(Util::compareNames(s_name, "Sub_deref")) {
        handleAddSubDerefDefs(transf_name, retType, param_list, scope, project, pos, derived, true);
    }
    else if(Util::compareNames(s_name, "Cast")) {
        handleCastDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "PlusAssign")) {
        handlePlusMinusAssignDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "MinusAssign")) {
        handlePlusMinusAssignDef(transf_name, retType, param_list, scope, project, pos, derived, true);
    }
    else if(Util::compareNames(s_name, "array_bound_check")) {
        //handleBoundCheckDef(transf_name, retType, param_list, scope, project, pos);
        #ifdef CREATE_FUNCTION_DEBUG
        printf("array_bound_check: nondefining decl. externally implemented\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "array_bound_check_using_lookup")) {
        #ifdef CREATE_FUNCTION_DEBUG
        printf("array_bound_check_using lookup: nondefining decl. externally implemented\n");
        #endif
        #ifdef CPP_SCOPE_MGMT
        createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name, derived);
        #endif
    }
    else if(Util::compareNames(s_name, "ptr_check")) {
        handlePtrCheckDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "deref_check")) {
        handleDerefCheckDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "deref_check_with_str")) {
        handleDerefCheckWithStrDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "assign_and_copy")) {
        handleAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "deref_assign_and_copy")) {
        handleDerefAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, derived);    
    }    
    else if(Util::compareNames(s_name, "lhs_deref_assign_and_copy")) {
        handleLHSDerefAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, derived);    
    }
    else if(Util::compareNames(s_name, "rhs_deref_assign_and_copy")) {
        handleRHSDerefAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, derived);    
    }
    else if(Util::compareNames(s_name, "double_deref_check")) {
        handleDoubleDerefCheckDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else if(Util::compareNames(s_name, "deref_create_struct")) {
        handleDerefCreateStructDef(transf_name, retType, param_list, scope, project, pos, derived); 
    }
    else if(Util::compareNames(s_name, "return_pointer")) {
        handleReturnPointerDef(transf_name, retType, param_list, scope, project, pos, derived);    
    }
    else if(Util::compareNames(s_name, "cs_assign_and_copy")) {
        handleCSAssignAndCopyDef(transf_name, retType, param_list, scope, project, pos, derived);
    }
    else {
        // This should become a ROSE_ASSERT(0) once all fns are defined properly
        printf("Can't create def. transf_name: %s\n", transf_name.getString().c_str());
        // already created nondefining decl above..
        //createNondefiningDecl(transf_name, retType, param_list, scope, project);
        ROSE_ASSERT(0);
    }
    #endif
    
}

void ArithCheck::printAndCheckScopeContents(SgScopeStatement* scope, SgFunctionDeclaration* decl) {

    SgStatementPtrList stmtList;

    if(scope->containsOnlyDeclarations()) {
        SgDeclarationStatementPtrList declList = scope->getDeclarationList();
        SgDeclarationStatementPtrList::iterator i;
        for(i = declList.begin(); i != declList.end(); i++) {
            stmtList.push_back(*i);
        }
    }
    else {
        stmtList = scope->getStatementList();
    }

    printf("Printing Scope Contents - Begin\n");
    SgStatementPtrList::iterator j;
    for(j = stmtList.begin(); j != stmtList.end(); j++) {
        
        SgStatement* stmt = isSgStatement(*j);
        Util::printNode(stmt);
        
        SgFunctionDeclaration* fndecl = isSgFunctionDeclaration(stmt);
        if(fndecl) {
            if(SI::isSameFunction(fndecl, decl)) {
                printf("----------------------------<<<<<< Current Func Decl\n");
            }
        }
    }
    printf("Printing Scope Contents - End\n");

}


void ArithCheck::createNondefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                    SgScopeStatement* scope, SgProject* project, SgNode* pos, SgName s_name, bool derived) {

    #ifdef NONDEFDECL_DEBUG
    printf("Creating Nondefining forward decl\n");    
    #endif
    
    #if 0
    SgGlobal* globalScope = SI::getFirstGlobalScope(project);
    pushScopeStack(globalScope);
    #endif

    SgFunctionParameterList* new_param_list = SB::buildFunctionParameterList();

    SgExpressionPtrList& exprs = param_list->get_expressions();
    
    #ifdef NONDEFDECL_DEBUG
    printf("creating param_list\n");
    #endif
    
    for(SgExpressionPtrList::iterator iter = exprs.begin(); iter != exprs.end();
                                                            iter++) {
        
        SgExpression* ce = *iter;
        #ifdef NONDEFDECL_DEBUG
        printf("ce: %s = %s\n", isSgNode(ce)->sage_class_name(), isSgNode(ce)->unparseToString().c_str());
        #endif
        SgInitializedName* new_name = SB::buildInitializedName(SgName(""),ce->get_type(), NULL);
        #ifdef NONDEFDECL_DEBUG
        printf("created new_name\n");
        #endif
        // FIXME: Scope should be fndecl, not globalScope.
        // Removed since we don't want the new_name to be in the globalScope
        //new_name->set_scope(globalScope);
        new_param_list->append_arg(new_name);
        new_name->set_parent(new_param_list);
    }
    
    #ifdef NONDEFDECL_DEBUG
    printf("creating fwddecl\n");
    #endif
    #if 0
    SgFunctionDeclaration* fwddecl = SB::buildNondefiningFunctionDeclaration(s_name,
                                    retType,
                                    new_param_list);
    #endif
    // Working one
    #if 0
    SgFunctionDeclaration* fwddecl = SB::buildNondefiningFunctionDeclaration(transf_name,
                                    retType,
                                    new_param_list);
    printAndCheckScopeContents(globalScope, fwddecl);
    #else

    #ifdef CPP_SCOPE_MGMT

    // If the function being inserted works on std namespace and/or primitive types
    // exclusively, then we place it before the Util::FirstDeclStmtInFile
    if(strDecl::functionInteractsWithStdOrPrimitives(param_list, retType, pos) && !Util::isExternal(s_name) && !derived) {
        pos = Util::FirstDeclStmtInFile;
    }
    else {
        // For CPP_SCOPE_MGMT, the fwddecl is always in global scope... 
        // since the forward decl is only for externally implemented fns
        ROSE_ASSERT(isSgFunctionDeclaration(pos));
    }

    #ifdef NONDEFDECL_DEBUG
    printf("s_name: %s\n", s_name.str());
    printf("pos\n");
    Util::printNode(pos);
    #endif


    // This is function is only called for externally implemented functions...    
    // Insert the fwddecl prior to the first statement in global scope...    
    // FIXME: these func decls should be inserted at the beginning of the instrumentation
    // itself... no need to have this function called at all 
    SgFunctionDeclaration* fwddecl = SB::buildNondefiningFunctionDeclaration(transf_name,
                                    retType,
                                    new_param_list,
                                    isSgStatement(pos)->get_scope());
                                    //SI::getGlobalScope(pos));
    
    #else
    SgFunctionDeclaration* fwddecl = SB::buildNondefiningFunctionDeclaration(transf_name,
                                    retType,
                                    new_param_list,
                                    isSgStatement(pos)->get_scope());
    #endif
    // This can screw code in deref cases, since there is a deref operation on a struct
    // and ROSE doesn't like that.
//    printAndCheckScopeContents(isSgStatement(pos)->get_scope(), fwddecl);
    #endif


    // Instead of simply placing the defdecl at the top of the file... we'll
    // place it as close to the use as possible. This should ensure that all the
    // necessary data structures used, are declared.
    //prependStatement(isSgStatement(fwddecl));
    //SgStatement* loc = findInsertLocationFromParams(param_list);
    //SI::insertStatementBefore(loc, fwddecl);
    //SI::insertStatementBefore(scope, fwddecl);

    // Working one
    //insertStmtBeforeFirst(fwddecl, globalScope);
//    SI::insertStatementBefore(GEFD(node), fwddecl);

//    SI::insertStatementBefore(getInsertLocation(node, scope), fwddecl);


    #ifdef USE_INSERT_AFTER
    SI::insertStatementAfter(isSgStatement(pos), fwddecl);
    #else

    #ifdef CPP_SCOPE_MGMT
    // If its a normal func decl... using insertstmtbefore, 
    // else get the class declaration... then, insertstmtbefore
    if(isSgMemberFunctionDeclaration(pos)) {
        SgScopeStatement* scope = isSgMemberFunctionDeclaration(pos)->get_scope();
        SgClassDefinition* def = isSgClassDefinition(scope);
        pos = def->get_parent(); 
        ROSE_ASSERT(isSgClassDeclaration(pos));
    }
    #endif

    SI::insertStatementBefore(isSgStatement(pos), fwddecl);

    #endif

    #if 0
    if(isSgVarRefExp(node)) {
        // For var ref exps, the scope is based on their declaration
        // However, we don't want that here. We would like to know
        // where the statement using the var ref lies, and find
        // the insert location from that.
        SI::insertStatementBefore(GEFD(node), fwddecl);
    }
    else {
        SI::insertStatementBefore(GEFD(scope), fwddecl);
    }
    #endif

    

    #ifdef MAKE_DECLS_STATIC 
    // Util::isExternal checks if the definition of the function is in
    // the library -- 
    if(!Util::isExternal(s_name)) {
        Util::makeDeclStatic(isSgDeclarationStatement(fwddecl));
    }
    #endif

    #if 0
    popScopeStack();
    #endif
}

#if 0
bool Util::isFreeOrFlush(SgFunctionCallExp* fncall) {

    if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "free") == 0) {
        return true;
    }
    else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "fflush") == 0) {
        return true;
    }
    return false;
}
#endif

void ArithCheck::checkType(SgType* type) {
    
    #ifdef CHECK_TYPE_DEBUG
    printf("checkType\n");
    printf("type: \n");
    Util::printNode(type);
    #endif

    SgClassType *cls_type = isSgClassType (type); 
    ROSE_ASSERT(cls_type != NULL);

    #ifdef CHECK_TYPE_DEBUG
    printf("cls_type: \n");
    Util::printNode(cls_type);
    #endif
    
    SgClassDeclaration *cls_decl = isSgClassDeclaration (cls_type->get_declaration());
    ROSE_ASSERT(cls_decl);

    #ifdef CHECK_TYPE_DEBUG
    printf("cls_decl:\n");
    Util::printNode(cls_decl);
    #endif
    ROSE_ASSERT(cls_decl->get_firstNondefiningDeclaration()!= NULL);

    #ifdef CHECK_TYPE_DEBUG
    printf("First Defining decl: \n");
    Util::printNode(isSgClassDeclaration(cls_decl->get_firstNondefiningDeclaration()));
    #endif

    ROSE_ASSERT(isSgClassDeclaration(cls_decl->get_firstNondefiningDeclaration()) == cls_decl);
}

SgFunctionDeclaration* ArithCheck::getFuncDeclForName(SgInitializedName* name) {
    SgNode* grand_parent = name->get_parent()->get_parent();
    ROSE_ASSERT(isSgFunctionDeclaration(grand_parent));
    return isSgFunctionDeclaration(grand_parent);
}

bool ArithCheck::ScopeIsGlobal(SgNode* node) {

    SgStatement* stmt = isSgStatement(node);
    SgScopeStatement* scope = stmt->get_scope();
    return isSgGlobal(scope);
}

SgStatement* ArithCheck::findInsertLocation(SgNode* node) {
    
    #ifdef FIND_INS_LOC_DEBUG
    printf("Find Ins Loc\n");
    printf("node\n");
    Util::printNode(node);
    #endif

    #ifdef CPP_SCOPE_MGMT
    return GEFD(node);

    #else
    #if 0
    if(isSgStatement(node)) {
        if(isSgFunctionDeclaration(node)) {
            return isSgStatement(node);
        }
        else if(isSgMemberFunctionDeclaration(node)) {
            return isSgStatement(node);
        }
        else {
            return GEFD(node->get_parent());
        }
    }
    else {
        return GEFD(node->get_parent());
    }
    #endif

    #if 0
    if(isSgStatement(node)) {
        if(isSgMemberFunctionDeclaration(node)) {
            SgClassDefinition* class_def = isSgClassDefinition(isSgMemberFunctionDeclaration(node)->get_scope());
            ROSE_ASSERT(class_def);
            SgClassDeclaration* class_decl = isSgClassDeclaration(class_def->get_parent());
            ROSE_ASSERT(class_decl);
            return class_decl;
        }
        else if(isSgGlobal(node->get_parent())) {
            return isSgStatement(node);
        }
        else {
            return GEFD(node->get_parent());
        }
    }
    else {
        return GEFD(node->get_parent());
    }
    #endif

    #if 0
    if(!isSgStatement(node)) {
        return GEFD(node->get_parent());
    }
    else if(isSgGlobal(isSgStatement(node)->get_parent())) {
        return isSgStatement(node);
    }
    else {
        return GEFD(node->get_parent());
    }
    #endif


    #if 0
    if(isSgInitializedName(node) && isSgFunctionParameterList(node->get_parent())) {
        SgFunctionDeclaration* funcdecl = getFuncDeclForName(isSgInitializedName(node));
        #ifdef FIND_INS_LOC_DEBUG
        printf("Function decl\n");
        #endif
        return GEFD(funcdecl);
    }

    // Insert before global variables
    if(isSgVariableDeclaration(node) && ScopeIsGlobal(node)) {
        #ifdef FIND_INS_LOC_DEBUG
        printf("global var decl\n");
        #endif
        return isSgStatement(node);
    }
    
    // Motivation for this: If the current node's parent is SgGlobal,
    // the decl/def can go before this -- since this is likely to be
    // a class decl/fn decl/temp inst decl
    #ifdef HACKY_QUICK_INSERT_LOC
    if(isSgMemberFunctionDeclaration(node)) {
        SgStatement* class_def = isSgMemberFunctionDeclaration(node)->get_scope();
        ROSE_ASSERT(isSgClassDefinition(class_def));
        #ifdef FIND_INS_LOC_DEBUG
        printf("class decl found\n");
        #endif
        return isSgClassDeclaration(isSgClassDefinition(class_def)->get_declaration());
    }
    #if 0
    if(isSgGlobal(node->get_parent())) {
        ROSE_ASSERT(isSgStatement(node));
        return isSgStatement(node);
    }
    #endif
    #endif

    if(isSgClassDeclaration(node) && ScopeIsGlobal(node)) {
        #ifdef FIND_INS_LOC_DEBUG
        printf("ClassDecl/Global scope\n");
        #endif
        return isSgStatement(node);
    }
    else if(isSgFunctionDeclaration(node)) {
        if(isSgMemberFunctionDeclaration(node)) {
            if(node->get_parent() == NULL) {
                printf("Can't find suitable parent for... \n");
                Util::printNode(node);
                ROSE_ASSERT(0);
            }
            else {
                #ifdef FIND_INS_LOC_DEBUG
                printf("Check parent\n");
                #endif
                return GEFD(node->get_parent());
            }
        }
        else if(ScopeIsGlobal(node) || isSgNamespaceDefinitionStatement(isSgFunctionDeclaration(node)->get_scope())) {
            #ifdef FIND_INS_LOC_DEBUG
            printf("NamespaceDef/Global scope\n");
            #endif
            return isSgStatement(node);
        }
        else {
            #ifdef FIND_INS_LOC_DEBUG
            printf("Check parent\n");
            #endif
            return GEFD(node->get_parent());
        }
    }
    else {
        if(node->get_parent() == NULL) {
            #ifdef FIND_INS_LOC_DEBUG
            printf("node->get_parent() == NULL\n");
            #endif
            printf("Can't find suitable parent for... \n");
            Util::printNode(node);
            ROSE_ASSERT(0);
        }
        else {
            #ifdef FIND_INS_LOC_DEBUG
            printf("Check parent\n");
            #endif
            return GEFD(node->get_parent());
        }
    }
    #endif
    #endif
}


void ArithCheck::insertStmtBeforeFirst(SgStatement* stmt, SgScopeStatement* scope) {

    SgStatement* first_stmt = getFirstStatementInScope(scope);
    SI::insertStatementBefore(first_stmt, stmt);
}

void ArithCheck::insertStmtAfterLast(SgStatement* stmt, SgScopeStatement* scope) {

    SgStatement* last_stmt = getLastStatementInScope(scope);
    SI::insertStatementAfter(last_stmt, stmt);
}

SgStatement* ArithCheck::getLastStatementInScope(SgScopeStatement* scope) {

    SgDeclarationStatementPtrList& decls = scope->getDeclarationList();
    SgDeclarationStatementPtrList::iterator iter = decls.begin();
    iter += (decls.size() - 1);
    return *(iter);

}

SgStatement* ArithCheck::getFirstStatementInScope(SgScopeStatement* scope) {

    static SgStatement* stmt = *(scope->getDeclarationList().begin());
    
    #if 0
    printf("First stmt:\n");
    Util::printNode(stmt);
    #endif
    
    return stmt;

    #if 0
    #if 0
    return scope->firstStatement();
    #endif
    SgDeclarationStatementPtrList& decls = scope->getDeclarationList();
    return *(decls.begin());
    #endif
    
}

void ArithCheck::insertForwardDecl(SgType* cls_type, SgScopeStatement* scope) {

    ROSE_ASSERT(isSgClassType(cls_type));

    // now, insert a struct declaration.
    //SgClassDeclaration* str_decl = buildStructDeclaration(isSgClassType(cls_type)->get_name(), scope);
    SgClassDeclaration* str_decl = SB::buildNondefiningClassDeclaration(isSgClassType(cls_type)->get_name(), scope);
    // Builds a class definition, not a forward decl.
    //SgClassDeclaration* str_decl = buildClassDeclaration(isSgClassType(cls_type)->get_name(), scope);
    
    insertStmtBeforeFirst(str_decl, scope);

}

SgStatement* ArithCheck::getInsertLocation(SgNode* node, SgScopeStatement* scope) {

    
    if(isSgVarRefExp(node)) {
        // For var ref exps, the scope is based on their declaration
        // However, we don't want that here. We would like to know
        // where the statement using the var ref lies, and find
        // the insert location from that.
        return GEFD(node);
    }
    else {
        return GEFD(scope);
    }
}

bool symbolExists(SgScopeStatement* scope, SgName name) {
    return scope->symbol_exists(name);
}


//SgVariableDeclaration* ArithCheck::createStructVariableFor(SgVariableDeclaration* var_decl) {
SgVariableDeclaration* ArithCheck::createStructVariableFor(SgVariableDeclaration* var_decl, bool array_var) {


    SgInitializedName* var_name = Util::getNameForDecl(var_decl);

    SgScopeStatement* var_scope = var_name->get_scope();

    std::string str_var_name_String;

    if(strcmp(var_name->get_name().getString().c_str(), "") == 0) {
        str_var_name_String = "";
    }
    else {
        str_var_name_String = var_name->get_name().getString() + "_str";
    }

    #ifdef CREATE_STRUCT_VAR_DEBUG
    printf("str_var_name_String: %s\n", str_var_name_String.c_str());
    #endif

    #if 0
    SgType* str_ptr_type = SgPointerType::createType(SgTypeVoid::createType());

    // Use the VoidStruct. To find that, use a void pointer.
    TypeMap_t::iterator iter = ArithCheck::RelevantStructType.find(str_ptr_type);

    ROSE_ASSERT(iter != ArithCheck::RelevantStructType.end());

    SgType* str_var_type = iter->second;
    #endif

    #if 0
    SgType* var_type;

    if(array_var) {
        // For arrays.. get the base type, and create a pointer out of it
        // since the array variable is a pointer to the type contained in the
        // array itself. So, for example, a variable which represents an array
        // of pointers, is, in itself, a double pointer.
        SgType* temp_type = var_name->get_type();

        SgArrayType* arr_type = isSgArrayType(temp_type);

        SgType* base_type = arr_type->get_base_type();

        // create the struct type necessary to hold the base pointer
        // for this array
        // Before that, we need to change the base_type to its corresponding
        // pointer type
        var_type = SgPointerType::createType(base_type);
    }
    else {
        // For pointers, simply get the type of init name    
        var_type = var_name->get_type();
    }
    #endif

        

//    SgType* str_var_type = getStructType(Util::getTypeForPntrArr(var_name->get_type()), SI::getScope(var_decl), true);

    #ifdef REMOVE_TYPEDEFS_FROM_VARS
    SgType* var_name_type = Util::skip_Typedefs(var_name->get_type());
    SgType* str_var_type = strDecl::getStructType(Util::getTypeForPntrArr(var_name_type), GEFD(var_decl), true);
    #else
    SgType* str_var_type = strDecl::getStructType(Util::getTypeForPntrArr(var_name->get_type()), GEFD(var_decl), true);
    #endif


    // Lets zero out the initializer for now
    SgVariableDeclaration* str_var_decl = SB::buildVariableDeclaration(str_var_name_String, str_var_type, NULL, var_scope); 

    return str_var_decl;
}

void ArithCheck::handleSymbolTable(SgInitializedName* orig_name, SgInitializedName* str_name, bool addToMap) {

    SgScopeStatement* orig_scope = orig_name->get_scope();

    SgSymbol* orig_symbol = orig_name->get_symbol_from_symbol_table();
    SgVariableSymbol *orig_var_symbol = isSgVariableSymbol(orig_symbol);
    #ifdef HANDLE_SYMBOL_TABLE_DEBUG
    printf("Got scope. Now, symbolTable\n");
    #endif

    SgSymbol* str_symbol = str_name->get_symbol_from_symbol_table();

    if(str_symbol == NULL) {

        SgVariableSymbol* str_var_symbol = new SgVariableSymbol(str_name);
        isSgNode(str_var_symbol)->set_parent(isSgNode(orig_var_symbol)->get_parent());
        SgSymbolTable* currSymbolTable = isSgSymbolTable(isSgNode(orig_var_symbol)->get_parent());
        currSymbolTable->insert(str_name->get_name(), str_var_symbol);

        // Check that this worked
        str_symbol = str_name->get_symbol_from_symbol_table();
        ROSE_ASSERT(str_symbol);
    }

    if(addToMap) {
        varRemap.insert(VariableSymbolMap_t::value_type(orig_var_symbol, isSgVariableSymbol(str_symbol)));
    
        ReverseMap.insert(VariableSymbolMap_t::value_type(isSgVariableSymbol(str_symbol), orig_var_symbol));
    }
}





void ArithCheck::handleSymbolTable(SgVariableDeclaration* orig_decl, SgVariableDeclaration* str_decl, bool addToMap) {
    
    SgInitializedName* orig_name = Util::getNameForDecl(orig_decl);
    SgInitializedName* str_name = Util::getNameForDecl(str_decl);

    handleSymbolTable(orig_name, str_name, addToMap);
}

//SgCommaOpExp* ArithCheck::initializeStructWith(SgVariableDeclaration* var_decl, SgVariableDeclaration* str_var_decl) {
//SgCommaOpExp* ArithCheck::initializeStructWith(SgInitializedName* var_name, SgInitializedName* str_var_name) {
SgCommaOpExp* ArithCheck::initializeStructWith(SgInitializedName* var_name, SgInitializedName* str_var_name, bool flip_ptr_init) {
    #ifdef INIT_STRUCT_DEBUG
    printf("initializeStructWith -- \n");
    printf("original var -\n");
    Util::printNode(var_name);
    #endif

    // Lets create a variable reference expression for the pointer var
    SgVarRefExp* var_ref_exp = SB::buildVarRefExp(var_name, var_name->get_scope());

    // This cast is not required since the type of the ptr in the struct is already the same type
    #if 0
    // Now, for the ptr assignment, cast it to a void pointer (ptr type in the struct)
    SgCastExp* cast_exp = SB::buildCastExp(var_ref_exp, Util::getVoidPointerType(), SgCastExp::e_reinterpret_cast);
    #endif

    SgDotExp* dot_exp = Util::createDotExpFor(str_var_name, "ptr");
    
    #if 0
    SgAssignOp* assign_op = SB::buildAssignOp(dot_exp, cast_exp);
    #endif

    // This assign input_recr = input_str.ptr instead of the default case (else below)
    // where input_str.ptr = input_recr
    SgAssignOp* assign_op;
    if(flip_ptr_init) {
        assign_op = SB::buildAssignOp(var_ref_exp, dot_exp);
    }
    else {
        assign_op = SB::buildAssignOp(dot_exp, var_ref_exp);
    }

    #ifdef INIT_STRUCT_DEBUG
    printf(" ptr init: %s = %s\n", isSgNode(assign_op)->sage_class_name(), 
                    isSgNode(assign_op)->unparseToString().c_str());
    #endif

    

    // Get variable address
    // Now, address of op on this var ref
    SgAddressOfOp* addr_of_op = Util::createAddressOfOpFor(var_name);


    // This casting is required since the addr in the struct is of unsigned long long type
    // Now, cast it to long long -- this is the type of addr in the VoidStruct
    //SgType* str_addr_type = SgTypeUnsignedLongLong::createType();  
    SgType* str_addr_type = Util::getAddrType();  

//    SgCastExp* cast_exp2 = SB::buildCastExp(addr_of_op, str_addr_type, SgCastExp::e_reinterpret_cast);
    SgCastExp* cast_exp2 = SB::buildCastExp(addr_of_op, str_addr_type, CAST_TYPE);

    SgDotExp* dot_exp2 = Util::createDotExpFor(str_var_name, "addr");

    SgAssignOp* assign_op2 = SB::buildAssignOp(dot_exp2, cast_exp2);

    #ifdef INIT_STRUCT_DEBUG
    printf(" addr init: %s = %s\n", isSgNode(assign_op2)->sage_class_name(), 
                    isSgNode(assign_op2)->unparseToString().c_str());
    #endif

    // Create comma exp with both these exprs
    SgCommaOpExp* comma_op = SB::buildCommaOpExp(assign_op, assign_op2);

    return comma_op;

}

SgExpression* ArithCheck::buildMultArgOverloadFn(SgName fn_name, SgExprListExp* parameter_list, SgType* retType, SgScopeStatement* scope, SgNode* pos, bool derived) {

    #ifdef OVLFN_DEBUG
    printf("buildMultArgOverloadFn\n");
    #endif
    
    // The parameter list or the retType cannot be Typedef types.
    ROSE_ASSERT(!isSgTypedefType(retType));

    SgExpressionPtrList& exprs = parameter_list->get_expressions();
    SgExpressionPtrList::iterator it = exprs.begin();

    for(; it != exprs.end(); ++it) {
        SgExpression* ce = *it;
        ROSE_ASSERT(!isSgTypedefType(ce->get_type()));
    }

    return funFact::buildFuncCall(fn_name, parameter_list, retType, scope, pos);

    #if 0
    SgFunctionDeclaration* fnDecl = funFact::buildFuncCall(fn_name, parameter_list, retType, scope, pos);
    //FIXME: The scope used below might be wrong since we might change the scope in the buildFuncCall function.
    SgExpression* NewFunction = SB::buildFunctionCallExp(fnDecl->get_name(),
                                                                    retType,
                                                                    parameter_list,
                                                                    scope);
    return NewFunction;
    #endif

    #if 0    
    


    #ifdef CPP_SCOPE_MGMT
    // We'll use the function sym to build the function call exp... 
    SgFunctionSymbol* fn_sym = createForwardDeclIfNotAvailable(fn_name, retType, parameter_list, scope, ArithCheck::ACProject, pos, derived);
    #if 0
    SgExpression* NewFunction = SB::buildFunctionCallExp(transf_name,
                                                                retType,
                                                                parameter_list,
                                                                isSgStatement(pos)->get_scope());
    #endif
    SgExpression* NewFunction = SB::buildFunctionCallExp(fn_sym, parameter_list);
    #else
    SgName transf_name = createForwardDeclIfNotAvailable(fn_name, retType, parameter_list, scope, ArithCheck::ACProject, pos, derived);
    SgExpression* NewFunction = SB::buildFunctionCallExp(transf_name,
                                                                retType,
                                                                parameter_list,
                                                                scope);
    #endif
    
    #ifdef OVLFN_DEBUG
    printf("NewFunction\n");
    Util::printNode(NewFunction);
    printf("NewFunction: %p\n", NewFunction);
    printf("NewFunction - type\n");
    Util::printNode(NewFunction->get_type());
    printf("retType - type\n");
    Util::printNode(retType);
    #endif

    if(NewFunction->get_type() != retType) {
        printf("NewFunction->get_type() != retType\n");
        printf("NewFunction->get_type()\n");
        Util::printNode(NewFunction->get_type());
        printf("retType\n");
        Util::printNode(retType);
        ROSE_ASSERT(0);
    }

    ROSE_ASSERT(NewFunction->get_type() == retType); 
    
    #ifdef OVLFN_DEBUG
    printf("buildMultArgOverloadFn - Done\n");
    #endif
    return NewFunction;
    #endif
}


SgExpression* ArithCheck::buildAssignOverload(SgExpression* lhs, SgExpression* rhs, SgScopeStatement* scope, SgNode* pos) {
    
    SgType* retType = lhs->get_type();

    // Need to use the address of lhs - since we are updating it during the assign op.
    ROSE_ASSERT(isSgVarRefExp(lhs));
    SgExpression* lhs_addr = Util::createAddressOfOpFor(isSgVarRefExp(lhs));

//    SgExpression* overload = buildOverloadFn("Assign", lhs_addr, rhs, retType, scope);
//    SgExpression* overload = buildOverloadFn("Assign", lhs_addr, rhs, retType, scope, pos);
    SgExpression* overload = buildMultArgOverloadFn("Assign", SB::buildExprListExp(lhs_addr, rhs), retType, scope, pos);    

    return overload;
}

//SgExpression* ArithCheck::updatePointer(SgVariableDeclaration* var_decl, SgVariableDeclaration* str_var_decl) {
SgExpression* ArithCheck::updatePointer(SgInitializedName* var_name, SgInitializedName* str_var_name) {

    // LHS - original pointer ref
    SgVarRefExp* var_ref = SB::buildVarRefExp(var_name, var_name->get_scope());

    // RHS - str.ptr ref
    SgDotExp* dot_exp = Util::createDotExpFor(str_var_name, "ptr");


    #if 0
    // We don't need to cast here since the type of the variable from the dot_exp
    // is correct
    // Cast it to the right type
    SgType* var_type = var_name->get_type();
    SgExpression* cast_exp = SB::buildCastExp(dot_exp, var_type, SgCastExp::e_C_style_cast);
    #endif

    // Do the assignment
    //SgAssignOp* update_op = SB::buildAssignOp(var_ref, cast_exp);
    SgAssignOp* update_op = SB::buildAssignOp(var_ref, dot_exp);
    
    return update_op;

}

SgCommaOpExp* ArithCheck::copyVarInitAndUpdate(SgVariableDeclaration* var_decl, 
                            SgVariableDeclaration* str_var_decl) {

    #ifdef COPY_VAR_INIT_DEBUG
    printf("copy var init and update\n");
    #endif
    SgInitializedName* var_name = Util::getNameForDecl(var_decl);
    #ifdef COPY_VAR_INIT_DEBUG
    printf("Original var decl\n");
    Util::printNode(var_decl);
    printf("Struct var decl\n");
    Util::printNode(str_var_decl);
    #endif
    
    SgInitializer* var_initializer = var_name->get_initializer();

    if(var_initializer == NULL) {
        #ifdef COPY_VAR_INIT_DEBUG
        printf("variable not initialized at decl. Nothing to do\n");
        #endif
        return NULL;
    }

    #ifdef COPY_VAR_INIT_DEBUG
    printf("initializer: %s = %s\n", isSgNode(var_initializer)->sage_class_name(), 
                                                isSgNode(var_initializer)->unparseToString().c_str());
    #endif

    // Handle only assign initializers for now. 
    if(!isSgAssignInitializer(var_initializer)) {
        printf("Only handling assign initializers for now.\n");
        printf("This is not assign initializer\n");
        ROSE_ASSERT(0);
    }

    SgAssignInitializer* assign_init = isSgAssignInitializer(var_initializer);

    // All the variable references would be replaced by their struct counterparts. 
    // At initialization, the lhs will still be struct since its not a variable reference
    // exp, but instead, its an intialized name.
    // So, we copy over the operand of the initializer, which should be of struct type
    // and use assign_overload to do the struct to struct copy
    // Afterwards, we update the original pointer, using the ptr field in the struct var

    SgExpression* init_exp = assign_init->get_operand();
    SgExpression* init_exp_copy = SI::copyExpression(init_exp);

    SgExpression* var_ref_exp = SB::buildVarRefExp(str_var_decl);
    
    SgExpression* init_op = buildAssignOverload(var_ref_exp, init_exp_copy, var_name->get_scope(), GEFD(var_decl)); 

    #ifdef COPY_VAR_INIT_DEBUG
    printf("init_op\n");
    Util::printNode(init_op);
    #endif
    
    SI::deepDelete(isSgNode(var_initializer));
    var_name->set_initializer(NULL);

    // Now, to update the original pointer
    SgExpression* update_op = updatePointer(Util::getNameForDecl(var_decl), Util::getNameForDecl(str_var_decl));
    #ifdef COPY_VAR_INIT_DEBUG
    printf("update_op\n");
    Util::printNode(update_op);
    #endif

    SgCommaOpExp* comma_op = SB::buildCommaOpExp(init_op, update_op);
    #ifdef COPY_VAR_INIT_DEBUG
    printf("comma_op\n");
    Util::printNode(comma_op);
    #endif

    return comma_op;
    
}

// Using multiple types of structs seems to be the best way to handle the 
// the type and arithmetic problem.

void ArithCheck::handlePointerVars(SgVariableDeclaration* var_decl) {

    // input: int* ptr = assign_init
    // output: int* ptr;
    // assign_and_copy(ptr....);


    // Nothing to do if pointer is not initialized
    if(Util::NoInitializer(var_decl)) {
        return;
    }

    // Only assign initializers supported    
    SgInitializedName* var_name = Util::getNameForDecl(var_decl);
    SgAssignInitializer* assign_init = isSgAssignInitializer(var_name->get_initializer());
    ROSE_ASSERT(assign_init != NULL);
    
    // Copy the initializer
    SgExpression* init_copy = SI::copyExpression(assign_init->get_operand());

    // Set the initializer in the var decl to NULL.
    var_name->set_initializer(NULL);
    
    // Create an assign op.. which will be replaced by the appropriate
    // operation by the handleAssignOverloadOps function below.
    SgAssignOp* var_assign = SB::buildAssignOp(SB::buildVarRefExp(var_name), init_copy);
    SgExprStatement* assign_stmt = SB::buildExprStatement(var_assign);


    // If its a global variable, then insert the initialization in the main function 
    SgScopeStatement* var_scope = var_decl->get_scope();
    if(isSgGlobal(var_scope)) {
        // Find the main function, and insert the var_assign there.
        SgFunctionDeclaration* MainFn = SI::findMain(isSgGlobal(var_scope));
        if(!MainFn) {
            printf("handlePointerVars: can't find main");
            printf("var_scope\n");
            Util::printNode(var_scope);
            ROSE_ASSERT(0);
        }
        SgBasicBlock *bb = Util::getBBForFn(MainFn);
        Util::insertAtTopOfBB(bb, assign_stmt);
    }
    else {
        SI::insertStatementAfter(var_decl, assign_stmt);
    }

    // assign_and_copy(ptr...)
    SgExpression* overload = handleAssignOverloadOps(var_assign);

}


SgVariableDeclaration* ArithCheck::pushIndexToLocalVar(SgName arr_name, SgExpression* index_exp, SgScopeStatement* scope) {
    // Move the index_exp into a single statement by itself, and assign it to a local
    // variable.

//    SgType* arr_type = isSgArrayType(array_name->get_type());

    SgType* index_var_type = getArrayIndexType(); 
    SgExpression* index_var_init = SI::copyExpression(index_exp);
//    SgName index_var_sgname(array_name->get_name().getString() + "_index" + boost::lexical_cast<std::string>(Util::VarCounter++));
    SgName index_var_sgname(arr_name.getString() + "_index" + boost::lexical_cast<std::string>(Util::VarCounter++));
    
    SgVariableDeclaration* index_var_decl = Util::createLocalVariable(index_var_sgname, index_var_type, index_var_init, scope);

    return index_var_decl;
}

SgType* ArithCheck::getArrayIndexType() {
    return SgTypeUnsignedLong::createType();
}

SgExpression* ArithCheck::getArrayIndex(SgInitializedName* name) {

    SgType* type = name->get_type();
    SgArrayType* array_type = isSgArrayType(type);
    return array_type->get_index();
    
}

SgExpression* ArithCheck::getDummyLock() {
    return SB::buildIntVal(DUMMY_LOCK);
}

SgExpression* ArithCheck::getLookupTicket() {
    static uint64_t TicketCounter = 1; 
    return SB::buildIntVal(TicketCounter++);
}

bool ArithCheck::isClassStructMemberVar(SgVariableDeclaration* var_decl) {
    
    if(isSgClassDefinition(var_decl->get_parent())) {
        return true;
    }
    return false;
}

void ArithCheck::handleArrayVars(SgVariableDeclaration* var_decl) {

    SgProject* project = ArithCheck::ACProject;

    // This is an array declaration ... which means that 
    // the bounds are set by the declaration itself. To find the
    // the bounds, we will use the fields in the ArrayType

    SgInitializedName* var_name = Util::getNameForDecl(var_decl);
    // Check that we only ever get array variables here.
    SgType* var_type = var_name->get_type();

    #ifdef ARRAY_VAR_DEBUG
    printf("var_name\n");
    Util::printNode(var_name);
    if(var_name->get_initializer()) {
        printf("Initializer:\n");
        Util::printNode(var_name->get_initializer());
    }
    #endif

    #ifdef STRIP_TYPEDEFS
    var_type = Util::getType(var_type);
    #endif

    //ROSE_ASSERT(isSgArrayType(var_name->get_type()));
    ROSE_ASSERT(isSgArrayType(var_type));

//    SgType* var_type = var_name->get_type();
    #ifdef ARRAY_VAR_DEBUG
    printf("var_type\n");
    Util::printNode(var_type);
    #endif

    SgArrayType* arr_type = isSgArrayType(var_type);
    #ifdef ARRAY_VAR_DEBUG
    printf("arr_type\n");
    Util::printNode(arr_type);

    printf("rank\n");
    printf("%u\n", arr_type->get_rank());
    
    printf("base_type\n");
    Util::printNode(arr_type->get_base_type());

    if(arr_type->get_dim_info()) {
        printf("dim_info\n");
        Util::printNode(arr_type->get_dim_info());
    }

    if(arr_type->get_index()) {
        printf("index\n");
        Util::printNode(arr_type->get_index());
    }
    #endif

    SgType* base_type = arr_type->get_base_type();
    //ROSE_ASSERT(!isSgPointerType(base_type));
    #ifdef ARRAY_VAR_DEBUG
    printf("base_type\n");
    Util::printNode(base_type);
    #endif
}

SgVariableSymbol* ArithCheck::checkInVarRemap(SgVariableSymbol* var_sym) {
    ROSE_ASSERT(var_sym != NULL);
    VariableSymbolMap_t::iterator iter = ArithCheck::varRemap.find(var_sym);
    if(iter == ArithCheck::varRemap.end()) {
        return NULL;
    }
    
    return iter->second;
}


void ArithCheck::handleVarRefs(SgVarRefExp* var_ref) {
    // FIXME: Instead of looking up the map to use a previously declared struct,
    // create a struct everytime a variable is referenced. 
    // Currently, this handles both pointer and array var refs
    #ifdef VAR_REF_DEBUG
    printf("handleVarRefs\n");
    #endif

    SgVariableSymbol* var_symbol = var_ref->get_symbol();
    //SgSymbol* var_symbol = var_ref->get_symbol();
    ROSE_ASSERT(var_symbol != NULL);
    VariableSymbolMap_t::iterator iter = ArithCheck::varRemap.find(var_ref->get_symbol());
    ROSE_ASSERT(iter != ArithCheck::varRemap.end());
    SgVariableSymbol* str_symbol = iter->second;
        
    ROSE_ASSERT(str_symbol != NULL);
    SgVarRefExp* str_var_ref = SB::buildVarRefExp(isSgVariableSymbol(str_symbol));
    ROSE_ASSERT(str_var_ref != NULL);
    SgType* var_ref_type = var_ref->get_type();
    #ifdef STRIP_TYPEDEFS
    var_ref_type = Util::getType(var_ref_type);
    #endif

    mapExprToOriginalType(str_var_ref, var_ref_type);
    #ifdef VAR_REF_DEBUG
    printf("handleVarRefs\n");
    #endif
    #ifdef ONLY_ADD_ARRAYS_TO_OVR
    if(isSgArrayType(var_ref_type)) {
        mapExprToOriginalVarRef(str_var_ref, var_ref);
        #ifdef VAR_REF_DEBUG
        printf("mapping\n");
        printf("str_var_ref\n");
        Util::printNode(str_var_ref);
        printf("key: %llu\n", (unsigned long long)str_var_ref);
        printf("var_ref\n");
        Util::printNode(var_ref);
        #endif

    }
    SI::replaceExpression(var_ref, str_var_ref);
    #else
    mapExprToOriginalVarRef(str_var_ref, var_ref);
    #ifdef VAR_REF_DEBUG
    printf("mapping\n");
    printf("str_var_ref\n");
    Util::printNode(str_var_ref);
    printf("key: %llu\n", (unsigned long long)str_var_ref);
    printf("var_ref\n");
    Util::printNode(var_ref);
    #endif

    SI::replaceExpression(var_ref, str_var_ref);
    #endif

    #ifdef VAR_REF_DEBUG
    printf("Done\n");
    #endif
}

SgName ArithCheck::getFunctionName(SgFunctionCallExp* fncall) {

    SgFunctionSymbol* fnsymbol = fncall->getAssociatedFunctionSymbol();
    return fnsymbol->get_name();
}

bool ArithCheck::argumentsAreStructs(SgFunctionCallExp* fncall) {

    SgExprListExp* args = fncall->get_args();
    SgExpressionPtrList& exprs = args->get_expressions();
    SgExpressionPtrList::iterator it = exprs.begin();

    for(; it != exprs.end(); ++it) {
        
        SgExpression* ce = *it;
        // This arg is of class type, but not of the structs that we created...
        // If its a struct that we created, no need to transform since it is a pointer
        // begin passed. No implicit copy involved.
        if(strDecl::isOriginalClassType(ce->get_type())) {
            return true;
        }
    }

    return false;

}


SgExprListExp* ArithCheck::convertStructArgsToRefs(SgFunctionCallExp* fncall) {

    SgExprListExp* args = fncall->get_args();
    SgExpressionPtrList& exprs = args->get_expressions();
    
    SgExprListExp* new_params = SB::buildExprListExp();

    SgExpressionPtrList::iterator it = exprs.begin();

    for(; it != exprs.end(); ++it) {
        SgExpression* ce = *it;

        if(strDecl::isOriginalClassType(ce->get_type())) {
            //FIXME: Would this work for cases where address of can't be taken... ?
            SgExpression* ce_addr = Util::createAddressOfOpFor(SI::copyExpression(ce));
            new_params->append_expression(ce_addr);
        }
        else {
            new_params->append_expression(SI::copyExpression(ce));
        }
    }

    SgExpressionPtrList& new_exprs = new_params->get_expressions();

    ROSE_ASSERT(exprs.size() == new_exprs.size()); 

    return new_params;
}

SgFunctionRefExp* ArithCheck::getOrCreateWrapperFunction(SgFunctionRefExp* fn_ref, SgExpression* arg, SgFunctionCallExp* fncall) {

    SgFunctionSymbol* fn_sym = fn_ref->get_symbol();
    SgName new_fn_name(fn_sym->get_name().getString() + "_wrap");
        
    SgType* retType = SgPointerType::createType(SgTypeVoid::createType());

    SgFunctionRefExp* new_fn_ref;

    // First check, if we have already created the wrapper before...
    // Check if the wrapper function exists. If it does exist, it'll exist
    // in the same scope (hence the same symbol table) as the orig func
    SgScopeStatement* scope = fn_sym->get_declaration()->get_scope();
    if(SgFunctionSymbol* new_fn_sym = scope->lookup_function_symbol(new_fn_name)) {
        // Wrapper function already created...
        new_fn_ref = SB::buildFunctionRefExp(new_fn_sym);    
    }
    else {
        // Wrapper functions needs to be created
        
        SgExprListExp* param_list = SB::buildExprListExp(arg); 
        SgFunctionDeclaration* wrapper_fn = createDefiningDecl(new_fn_name, retType, param_list, scope, 
                                                    ArithCheck::ACProject, 
                                                    GEFD(SI::getEnclosingStatement(fncall)), false, fn_sym->get_name());    

        // Get the function definition...
        SgFunctionDefinition* fndef = getFuncDef(wrapper_fn);
        SgBasicBlock* fnbody = getFuncBody(fndef);
        SgStatementPtrList& stmts = fnbody->get_statements();
    
        SgType* var_type = strDecl::getStructType(retType, wrapper_fn, true);

        
        // the args
        SgInitializedNamePtrList& args = wrapper_fn->get_args();
        // Get the first arg
        SgInitializedName* arg1 = *(args.begin());
        // first arg is pointer type -- pointer to struct
        ROSE_ASSERT(isSgPointerType(arg1->get_type()));

        // the arg is pointer to void struct type... but its been cast to void pointer type... so, cast
        // it to pointer to void struct type
        // This is the void* argument casted to pointer to void struct type...
        SgExpression* arg_casted = SB::buildCastExp(SB::buildVarRefExp(arg1), SgPointerType::createType(var_type), CAST_TYPE);
        
        // Dereference the void* to reveal a void struct
        //SgExpression* deref_arg = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));
        SgExpression* deref_arg = SB::buildPointerDerefExp(arg_casted);

        // now create a function call using the original fn ref... 
        SgFunctionCallExp* transf_fn_call = SB::buildFunctionCallExp(SI::copyExpression(fn_ref), SB::buildExprListExp(deref_arg)); 

        // create local variable of return type... which is initialized by the function call above
        SgVariableDeclaration* output_var = Util::createLocalVariable("output", var_type, transf_fn_call, fnbody);
        fnbody->append_statement(output_var);
        
        // return the output_var.ptr
        appendReturnStmt(Util::createDotExpFor(Util::getNameForDecl(output_var), "ptr"), fnbody);


        // get the new_fn_ref from the wrapper_fn
        new_fn_ref = SB::buildFunctionRefExp(wrapper_fn); 
    }

    return new_fn_ref;
}

SgExpression* ArithCheck::insertPushCall(SgExpression* ins, SgExpression* top) {
    
    if(!ins) {
        // Nothing to do... 
        return NULL;
    }
    if(top) {
        return SB::buildCommaOpExp(top, ins);
    }
    else {
        return ins;
    }
}

void ArithCheck::handleFuncCalls(SgFunctionCallExp* fncall) {
    
    #ifdef FUNCCALL_DEBUG
    printf("handleFuncCalls\n");
    #endif

    if(fncall->getAssociatedFunctionDeclaration() == NULL) {
        #ifdef FUNCCALL_DEBUG
        printf("Can't find associated fn decl for... ");
        Util::printNode(fncall);
        printf("Must be virtual function call. Returning... \n");
        #endif
        return;
    }

    #ifdef FUNCCALL_DEBUG
    printf("Function: %s\n", fncall->getAssociatedFunctionDeclaration()->get_name().str());
    Util::printNode(fncall);
    #endif

    SgType* fncall_type = fncall->get_type();

    #ifdef STRIP_TYPEDEFS
    fncall_type = Util::getType(fncall_type);
    #endif

    // Replace malloc calls with malloc_overload
    if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "malloc") == 0) {
        #ifdef FUNCCALL_DEBUG
        printf("Handling malloc\n");    
        #endif
        SgStatement* stmt = Util::getSurroundingStatement(*isSgExpression(fncall));
           SgScopeStatement* scope = stmt->get_scope();

        SgExprListExp* args = fncall->get_args();
        SgExpressionPtrList& exprs = args->get_expressions();
        ROSE_ASSERT(exprs.size() == 1);
        SgExpression* arg1 = *(exprs.begin());

        SgExpression* arg1_copy = SI::copyExpression(arg1);
        // Do a c_Style_cast to unsigned int
        SgExpression* arg1_copy_casted = SB::buildCastExp(arg1_copy, Util::getSizeOfType(), SgCastExp::e_C_style_cast);
        SgExprListExp* params = SB::buildExprListExp(arg1_copy_casted);

        SgType* retType = strDecl::getStructType(fncall_type, GEFD(SI::getEnclosingStatement(fncall)), true);

        SgExpression* overload = buildMultArgOverloadFn("malloc_overload", params, retType, scope,
                                                GEFD(SI::getEnclosingStatement(fncall)));

        SI::replaceExpression(fncall, overload);
        
    }
    else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "realloc") == 0) {
        #ifdef FUNCCALL_DEBUG
        printf("Handling realloc\n");    
        #endif
        SgStatement* stmt = Util::getSurroundingStatement(*isSgExpression(fncall));
           SgScopeStatement* scope = stmt->get_scope();

        SgExprListExp* args = fncall->get_args();
        SgExpressionPtrList& exprs = args->get_expressions();
        ROSE_ASSERT(exprs.size() == 2);
        SgExpressionPtrList::iterator iter = exprs.begin();
        SgExpression* arg1 = *iter;
        SgType* arg1_type = arg1->get_type();
        arg1_type = Util::getType(arg1_type);
        #ifdef CPP_SCOPE_MGMT
        ROSE_ASSERT(strDecl::isValidStructType(arg1_type, GEFD(fncall)));
        #else
        ROSE_ASSERT(strDecl::isValidStructType(arg1_type));
        #endif
        
        SgExpression* arg1_copy = SI::copyExpression(arg1);
        
        #ifdef STRIP_TYPEDEFS
        // No need to do any typedef checks here since arg1 is ValidStructType
        // and arg2 is casted to SizeOfType
        #endif

        ++iter;
        SgExpression* arg2 = *iter;

        SgExpression* arg2_copy = SI::copyExpression(arg2);
        // Do a c_Style_cast to unsigned int
        SgExpression* arg2_copy_casted = SB::buildCastExp(arg2_copy, Util::getSizeOfType(), SgCastExp::e_C_style_cast);
        SgExprListExp* params = SB::buildExprListExp(arg1_copy, arg2_copy_casted);

        SgType* retType = strDecl::getStructType(fncall_type, GEFD(SI::getEnclosingStatement(fncall)), true);

        SgExpression* overload = buildMultArgOverloadFn("realloc_overload", params, retType, scope,
                                            GEFD(SI::getEnclosingStatement(fncall)));

        SI::replaceExpression(fncall, overload);
        
    }
    else if(Util::isFree(fncall)) {
        #ifdef FUNCCALL_DEBUG
        printf("Handling free\n");
        #endif
        SgStatement* stmt = Util::getSurroundingStatement(*isSgExpression(fncall));
        SgScopeStatement* scope = stmt->get_scope();

        SgExprListExp* args = fncall->get_args();
        SgExpressionPtrList& exprs = args->get_expressions();
        ROSE_ASSERT(exprs.size() == 1);
        SgExpression* arg1 = *(exprs.begin());
        SgExpression* arg1_copy = SI::copyExpression(arg1);

        // assert that the input type is valid struct type
        SgType* arg1_type = arg1->get_type();
        #ifdef STRIP_TYPEDEFS
        arg1_type = Util::getType(arg1_type);
        #endif
        
        #ifdef CPP_SCOPE_MGMT
        ROSE_ASSERT(strDecl::isValidStructType(arg1_type, GEFD(fncall)));
        #else
        ROSE_ASSERT(strDecl::isValidStructType(arg1_type));
        #endif

        SgExprListExp* params = SB::buildExprListExp(arg1_copy);

        ROSE_ASSERT(fncall->get_type()->get_mangled() == SgTypeVoid::createType()->get_mangled());
        SgType* retType = SgTypeVoid::createType();
        SgExpression* overload = buildMultArgOverloadFn("free_overload", params, retType, scope,
                                        GEFD(SI::getEnclosingStatement(fncall)));

        SI::replaceExpression(fncall, overload);

    }
    #ifdef WRAP_PTHREAD_ROUTINE_CALLS
    else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "pthread_create") == 0) {
        #ifdef FUNCCALL_DEBUG
        printf("Handling pthread_create by wrapping routine calls\n");
        #endif
        
        // Get the args
        SgExprListExp* args = fncall->get_args();
        SgExpressionPtrList& exprs = args->get_expressions();
        ROSE_ASSERT(exprs.size() == 4);
        SgExpressionPtrList::iterator i = exprs.begin();
        SgExpression* arg1 = *i++;
        SgExpression* arg2 = *i++;
        SgExpression* arg3 = *i++;
        SgExpression* arg4 = *i;
        
        // Check that the third arg is a function ref
        ROSE_ASSERT(isSgFunctionRefExp(arg3));

        // Check that the fourth arg is a qual type if its a var ref (not converted to str yet)
        // otherwise, its a valid struct type
        #ifdef CPP_SCOPE_MGMT
        ROSE_ASSERT(Util::isQualifyingType(arg4->get_type()) || strDecl::isValidStructType(arg4->get_type(), GEFD(arg4)));
        #else
        ROSE_ASSERT(Util::isQualifyingType(arg4->get_type()) || strDecl::isValidStructType(arg4->get_type())); 
        #endif
        
        // Now, we need to create a def func decl prior for the wrapper func... this wrapper
        // will be called from pthread_create, and the wrapper calls the original function in turn
        // this ensures that the penalty of conforming to pthread_create arg specs is only paid once
        // when the pthread_create call is made, and not always
        // In addition, if multiple functions call the same routine, we can use the same wrapper function
        // We can use our overload function system to include this function
        
                
        // Now, comes the tricky part... converting the argument to the pthread_routine to a void pointer
        // while passing metadata through... 

        // In the caller... (func that is calling pthread_create)
        // The strategy here is to push the arg before the function call, and assign it a local variable.
        // This local variable will be of valid struct type
        // We pass the address of the local variable, casted to void pointer
        
        // In the wrapper... (func called by pthread_create)
        // the void pointer is derefed to yield a void struct... We don't know what the 
        // underlying type is, while in the wrapper. So, we'll assume its a void struct... 
        // This works because when the transf function is called, it is written in such a way that it can 
        // cast void struct to the appropriate struct type... and proceed.

        // In the trans func... (func originally meant to be called by pthread_create... this has been
        // transformed to receive void struct and return void struct.
        // This function won't notice any difference, and hence can be transformed as usual.
        SgVariableDeclaration* pthread_arg_var;
        SgExpression* init;
        SgType* var_ty;

        if(Util::isQualifyingType(arg4->get_type())) {
            // Not considering double dim arrays here yet... 
            ROSE_ASSERT(Util::isVarRefOrDotArrow(arg4));

            SgType* arg4_type = arg4->get_type();

            #ifdef STRIP_TYPEDEFS
            arg4_type = Util::getType(arg4_type);
            #endif

            // We might also need to do a create_entry if the arg is
            // of array type
            #ifdef ARRAY_CREATE_ENTRY
            // Also, we need to insert create_entry for the arg, if its
            // array type.
            if(isSgArrayType(arg4_type)) {
                insertCreateEntry(SI::copyExpression(arg4), Util::getScopeForExp(fncall),
                                SI::getEnclosingStatement(fncall));
            }
            #endif    
            
            SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(arg4)));
            SgExpression* ptr = SI::copyExpression(arg4);

            #ifdef STRIP_TYPEDEFS
            ptr = Util::castAwayTypedef(ptr);
            // ptr_addr is already casted
            #endif
    
            var_ty = strDecl::getStructType(Util::getTypeForPntrArr(arg4_type),
                                        GEFD(SI::getEnclosingStatement(fncall)),
                                        true);
                            
            SgExprListExp* cs_p = SB::buildExprListExp(ptr, ptr_addr);
        
    
            init = buildMultArgOverloadFn("create_struct", cs_p,
                                        var_ty, Util::getScopeForExp(fncall),
                                        GEFD(SI::getEnclosingStatement(fncall)));
            
        }
        else {
            #ifdef CPP_SCOPE_MGMT
            ROSE_ASSERT(strDecl::isValidStructType(arg4->get_type(), GEFD(arg4)));
            #else
            ROSE_ASSERT(strDecl::isValidStructType(arg4->get_type()));
            #endif
            // Expressions using pointers are converted to struct temporaries by our scheme.
            // Since these are temporaries, we cannot take their address. 
            // TODO: We might be able
            // to place them before the fncall exp and store the result in a local variable
            // and pass the address of the local variable through the function call.. haven't 
            // done that yet.
            init = SI::copyExpression(arg4);        
            var_ty = arg4->get_type();
        }

        
        // name, type, init, scope    
        SgName s_name(var_ty->get_mangled() + "_pthread_arg_" + boost::lexical_cast<std::string>(Util::VarCounter++)); 
        pthread_arg_var = Util::createLocalVariable(s_name, var_ty, SB::buildAssignInitializer(init, init->get_type()), SI::getScope(fncall));    
        SI::insertStatementBefore(Util::getSuitablePrevStmt(SI::getEnclosingStatement(fncall)), pthread_arg_var);

        // Now that we have the local variable, take the address of that variable, and cast it to void*
        SgExpression* addr_of_arg = Util::createAddressOfOpFor(SB::buildVarRefExp(pthread_arg_var));
        #if 0
        SgType* arg_orig_type = addr_of_arg->get_type();
        ROSE_ASSERT(isSgPointerType(arg_orig_type) && strDecl::isValidStructType(isSgPointerType(arg_orig_type)->get_base_type()));
        // the outer type is pointer type, and the base type is valid struct type
        SgType* arg_orig_base_type = isSgPointerType(arg_orig_type)->get_base_type();
        #endif
        
        SgExpression* void_arg = SB::buildCastExp(addr_of_arg, SgPointerType::createType(SgTypeVoid::createType()), CAST_TYPE); 

        // We have the arg in the right format now. What we need is the wrap function which can take arg now.
        // the second arg here is reqd to use createDefiningDecl to create the function def... not for actual use in the
        // wrapper function.. since it will be called by pthread_create
        // fncall is required to determine where to insert the wrapper function
        SgFunctionRefExp* new_fn_ref = getOrCreateWrapperFunction(isSgFunctionRefExp(arg3), SI::copyExpression(void_arg), fncall);

        // Now, change the old pthread_create call with a new one which has new_fn_ref and void_arg as the arguments.
        // The first arg is a struct... call return_pointer to get the original pointer back... the pointer is basically
        // &pthread_t... which has been converted to a struct
        SgType* arg1_type = arg1->get_type();
        #ifdef STRIP_TYPEDEFS
        arg1_type = Util::getType(arg1_type);
        #endif

        SgExpression* arg1_ptr = buildMultArgOverloadFn("return_pointer", SB::buildExprListExp(SI::copyExpression(arg1)), 
                                                        strDecl::findInUnderlyingType(arg1_type),
                                                        Util::getScopeForExp(fncall),
                                                        GEFD(SI::getEnclosingStatement(fncall)));

        //SgExprListExp* p_list = SB::buildExprListExp(SI::copyExpression(arg1), SI::copyExpression(arg2), new_fn_ref, void_arg);
        SgExprListExp* p_list = SB::buildExprListExp(arg1_ptr, SI::copyExpression(arg2), new_fn_ref, void_arg);
        SgExpression* new_fncall = SB::buildFunctionCallExp("pthread_create", fncall->get_type(), p_list, SI::getScope(fncall));
        
        SI::replaceExpression(fncall, new_fncall);

    }
    #endif
    // This should work in general for all "qualifying lib calls"
    else if(Trav::isQualifyingLibCall(fncall)) {
        #ifdef FUNCCALL_DEBUG
        printf("Handling qual lib call\n");    
        #endif
        // Basically, deref all the struct arguments

        SgExprListExp* args = fncall->get_args();
        SgExpressionPtrList& exprs = args->get_expressions();
            
        SgExpressionPtrList::iterator iter = exprs.begin();
        SgExprListExp* new_args = SB::buildExprListExp();

        for(; iter != exprs.end(); ++iter) {
            SgExpression* ce = *iter;
            SgExpression* ce_copy = SI::copyExpression(ce);
            
            SgType* ce_type = ce->get_type();

            #ifdef STRIP_TYPEDEFS
            ce_type = Util::getType(ce_type);
            #endif

            #ifdef CPP_SCOPE_MGMT
            if(strDecl::isValidStructType(ce_type, GEFD(fncall))) 
            #else
            if(strDecl::isValidStructType(ce_type))    
            #endif
            {    
                
                // Shouldn't do deref here since it will involve a check... and thats not 
                // necessary since we are not derefencing anything here. Instead, 
                // we should call a function which returns the pointer from the struct
                SgExprListExp* p_list = SB::buildExprListExp(SI::copyExpression(ce));
                SgType* retType = strDecl::findInUnderlyingType(ce_type);

                SgExpression* new_exp = buildMultArgOverloadFn("return_pointer", p_list, retType,
                                                        Util::getScopeForExp(fncall),
                                                        GEFD(SI::getEnclosingStatement(fncall)));

                new_args->append_expression(new_exp);
            }
            else {
                new_args->append_expression(ce_copy);
            }
        }

        SgName fn_name(fncall->getAssociatedFunctionDeclaration()->get_name());
        // if this get_type doesn't work, use fn->getAssociatedFunctionDeclaration()->get_orig_return_type()
        SgType* retType = fncall->get_type();

        // This can change the return type of
        // a library function call...
        #ifdef STRIP_TYPEDEFS
        retType = Util::getType(retType);
        #endif

        #ifdef FUNCCALL_DEBUG
        printf("fncall\n");
        Util::printNode(fncall);
        printf("retType\n");
        Util::printNode(retType);
        #endif

        // if return type is of struct type, return the underlying type... this is a library call
        // we don't change the return types for these things
        #ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(retType, GEFD(fncall))) 
        #else
        if(strDecl::isValidStructType(retType))
        #endif
        {
            // We will never return a struct type --- correct?
            ROSE_ASSERT(0);
        }


        SgExpression* new_fncall = SB::buildFunctionCallExp(fn_name, retType, new_args, SI::getScope(fncall));
        SI::replaceExpression(fncall, new_fncall);
        
        if(Util::isQualifyingType(retType)) {
            // Need to create a struct out of it.    
            // First create a local variable... which will take the return value of the function
            // then, create_entry from that local variable.
            // retType libcall_ret;
            // libcall_ret = qual_lib_call(), create_entry(&ret, ret, sizeof(ret)), create_struct_from_addr(&ret)
            
            #ifdef STRIP_TYPEDEFS
            ROSE_ASSERT(!isSgTypedefType(retType));
            #endif

            // name, type, init, scope    
            SgName s_name(retType->get_mangled() + "_libcall_ret_" + boost::lexical_cast<std::string>(Util::VarCounter++)); 
            SgVariableDeclaration* var_decl = Util::createLocalVariable(s_name, retType, NULL, SI::getScope(new_fncall));    
            SI::insertStatementBefore(Util::getSuitablePrevStmt(SI::getEnclosingStatement(new_fncall)), var_decl);

            // libcall_ret = qual_lib_call()
            SgExpression* assign_op = SB::buildAssignOp(SB::buildVarRefExp(var_decl), SI::copyExpression(new_fncall));

            // create_entry
            SgExpression* ce1 = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(var_decl)));
            SgExpression* ce2 = SB::buildCastExp(SB::buildVarRefExp(var_decl), Util::getAddrType(), SgCastExp::e_C_style_cast);
            SgExpression* ce3 = SB::buildCastExp(SB::buildSizeOfOp(SB::buildVarRefExp(var_decl)), Util::getSizeOfType(), SgCastExp::e_C_style_cast);
            
            SgExprListExp* params = SB::buildExprListExp(ce1, ce2, ce3);
            SgExpression* ce_fncall = buildMultArgOverloadFn("create_entry", params, SgTypeVoid::createType(), SI::getScope(new_fncall),
                                                GEFD(SI::getEnclosingStatement(new_fncall)));

            // create_struct_from_addr(&ret)
            SgExpression* cs_from_addr = createStructUsingAddressOf(SB::buildVarRefExp(var_decl), GEFD(var_decl));

            SgExpression* init_ce = SB::buildCommaOpExp(assign_op, ce_fncall);
            SgExpression* init_ce_cs = SB::buildCommaOpExp(init_ce, cs_from_addr);

            SI::replaceExpression(new_fncall, init_ce_cs);
        }

    }
    else {

        // Four things to do here.
        // 1. If one or more args is of qualifying type, convert to appropriate struct
        // 2. If return type is qualifying type, convert it to relevant struct type
        // 3. CLASS_STRUCT_COPY: If one or more arguments is an original class/struct,
        // convert it to its ref version
        // 4. CLASS_STRUCT_COPY: If return type is original class/struct, add additional
        // arg to the param_list (&retvar) and change return type to void.

    
        #ifdef FUNCCALL_DEBUG
        printf("Normal function call: Check the args and return type\n");
        #endif

        SgExprListExp* param_list = fncall->get_args();
        SgExprListExp* new_param_list = SB::buildExprListExp();

        SgExpressionPtrList& exprs = param_list->get_expressions();
        SgExpressionPtrList::iterator it = exprs.begin();

        #ifdef RETAIN_FUNC_PROTOS
        SgExpression* pcalls = NULL;
        unsigned int arg_num = 0;
        #endif

        

        for(; it != exprs.end(); ++it) {

                SgExpression* ce = *it;
                SgType* ce_type = ce->get_type();

                #ifdef STRIP_TYPEDEFS
                ce_type = Util::getType(ce_type);
                #endif


                #ifdef FUNCCALL_DEBUG
                printf("params: ce: \n");
                Util::printNode(ce);
                printf("param_type: ce_type:\n");
                Util::printNode(ce_type);
                #endif

                #ifdef CLASS_STRUCT_COPY
                // 3. CLASS_STRUCT_COPY: If one or more arguments is an original class/struct,
                // convert it to its ref version
                if(strDecl::isOriginalClassType(ce_type)) {
                    #ifdef FUNCCALL_DEBUG
                    printf("param is orig class/struct type\n");
                    #endif
                    // This needs to be converted to its ref version.
                    SgExpression* ce_addr = Util::createAddressOfOpFor(SI::copyExpression(ce));
                    new_param_list->append_expression(ce_addr);
                    continue;
                }
                #endif

                // 1. If one or more args is of qualifying type, convert to appropriate struct
                if(Util::isQualifyingType(ce_type)) {

                    #ifdef FUNCCALL_DEBUG
                    printf("FuncCalls-qual_type: ce\n");
                    Util::printNode(ce);
                    #endif
                    
                    
                    // We might also need to do a create_entry if the arg is
                    // of array type
                    #ifdef ARRAY_CREATE_ENTRY
                    // Also, we need to insert create_entry for the arg, if its
                    // array type.
                    if(isSgArrayType(ce_type)) {
                        insertCreateEntry(SI::copyExpression(ce), Util::getScopeForExp(fncall),
                                SI::getEnclosingStatement(fncall));
                    }
                    #endif
                    

                    // If its var ref or dot arrow, then create_struct(ptr, &ptr) -- 
                    // if RETAIN_FUNC_PROTOS is defined, then insert push(&ptr), fn(ptr) 
                    if(Util::isVarRefOrDotArrow(ce)) {
                        SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(ce)));
                        SgExpression* ptr = SI::copyExpression(ce);

                        #ifdef STRIP_TYPEDEFS
                        ptr = Util::castAwayTypedef(ptr);
                        // ptr_addr is already casted
                        #endif
    
                        #ifdef RETAIN_FUNC_PROTOS

                        SgExprListExp* cs_p = SB::buildExprListExp(ptr_addr);
                
                        SgExpression* pushcall = buildMultArgOverloadFn("push_to_stack", cs_p,
                                                            SgTypeVoid::createType(), Util::getScopeForExp(fncall),
                                                            GEFD(SI::getEnclosingStatement(fncall)));
                        pcalls = insertPushCall(pushcall, pcalls);

                        new_param_list->append_expression(ptr);
                        
                        arg_num++;
                        #else
                        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(ce_type),
                                                GEFD(SI::getEnclosingStatement(fncall)),
                                                true);
                            
                        SgExprListExp* cs_p = SB::buildExprListExp(ptr, ptr_addr);
        
    
                        SgExpression* overload = buildMultArgOverloadFn("create_struct", cs_p,
                                                            retType, Util::getScopeForExp(fncall),
                                                                GEFD(SI::getEnclosingStatement(fncall)));
                        new_param_list->append_expression(overload);
                        #endif
                        
                    }
                    
                    #ifdef DOUBLE_DIM_ENABLED
                    else if(isSgPointerDerefExp(ce)) {
                        // Something like *ptr / *str
                        // 1. Strip the deref. 
                        // 2. Call deref_create_struct(ptr), deref_create_struct(str)

                        #ifdef RETAIN_FUNC_PROTOS
                        printf("Not retaining function prototypes for double dim pointer derefs... quitting\n");
                        exit(1);
                        #endif

                        SgExpression* ptr = SI::copyExpression(Util::stripDeref(ce));

                        #ifdef STRIP_TYPEDEFS
                        ptr = Util::castAwayTypedef(ptr);
                        #endif
                        
                        SgExpression* overload = buildMultArgOverloadFn("deref_create_struct", SB::buildExprListExp(ptr),
                                                            strDecl::getStructType(Util::getTypeForPntrArr(ce_type),
                                                            GEFD(SI::getEnclosingStatement(fncall)), true),
                                                            Util::getScopeForExp(fncall),
                                                            GEFD(SI::getEnclosingStatement(fncall)));
                        new_param_list->append_expression(overload);
                        
                    }
                    else {
                        printf("Unsupported case in handleFuncCalls - normal function calls\n");
                        printf("ce:\n");
                        Util::printNode(ce);
                        ROSE_ASSERT(0);
                    }
                    #endif

                    continue;
                }
                #ifdef RETAIN_FUNC_PROTOS
                #ifdef CPP_SCOPE_MGMT
                else if(strDecl::isValidStructType(ce_type, GEFD(ce))) 
                #else
                else if(strDecl::isValidStructType(ce_type))
                #endif
                {
                        #ifdef FUNCCALL_DEBUG
                        printf("valid struct type found...\n");
                        #endif
                        // the transf will be from: fn(str)
                        // to: str argvar; argvar = str, push(argvar.addr), fn(argvar.ptr)
                        
                        // Create a variable declaration
                        // 1. create a local varible... argvar
                        SgName var_name("argvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
                        SgVariableDeclaration* argvar = SB::buildVariableDeclaration(var_name, ce->get_type(), NULL, SI::getScope(fncall));
                        SI::insertStatementBefore(SI::getEnclosingStatement(fncall), argvar);    

                        // 2. argvar = str;
                        SgExpression* argvar_assign = SB::buildAssignOp(SB::buildVarRefExp(argvar), SI::copyExpression(ce));

                        // 3. push(argvar.addr)
                        SgExprListExp* cs_p = SB::buildExprListExp(Util::createDotExpFor(Util::getNameForDecl(argvar), "addr"));
                        SgExpression* pushcall = buildMultArgOverloadFn("push_to_stack", cs_p,
                                                                    SgTypeVoid::createType(), Util::getScopeForExp(fncall),
                                                                    GEFD(SI::getEnclosingStatement(fncall)));

                        // 4. create comma op exp with the argvar_assign and pushcall
                        pushcall = SB::buildCommaOpExp(argvar_assign, pushcall);

                        // 5. add it to the list of exps to be inserted before the fncall
                        pcalls = insertPushCall(pushcall, pcalls);
                        
                        // 6. add argvar.ptr to the new param list
                        new_param_list->append_expression(Util::createDotExpFor(Util::getNameForDecl(argvar), "ptr"));

                        arg_num++;
                        
                        continue;
                }
                #endif

                #ifdef FUNCCALL_DEBUG
                printf("Nothing to do with ce\n");
                #endif

                // If nothing is done to the struct, simply add it to the new_param_list
                new_param_list->append_expression(SI::copyExpression(ce));
        }

        SgType* fn_retType = fncall->get_type();

        #ifdef STRIP_TYPEDEFS
        fn_retType = Util::getType(fn_retType);
        #endif

        #ifdef FUNCCALL_DEBUG
        printf("fncall -- mid way through...\n");
        Util::printNode(fncall);
        #endif
        
        // 2. If return type is qualifying type, convert it to relevant struct type
        if(Util::isQualifyingType(fn_retType)) {
            // convert return type to corresponding struct type
            SgType * new_retType = strDecl::getStructType(Util::getTypeForPntrArr(fn_retType), 
                                    GEFD(SI::getEnclosingStatement(fncall)), true);

            #ifdef RETAIN_FUNC_PROTOS
            // pop_from_stack returns the top most, while popping a total of arg_num + 1 out... 
            // This will return the metadata we are looking for, while removing the args from the stack...
            // transform from: fn(...) to int t; ret* ptr; (pcalls, ptr = new_fn(...), t = pop_from_stack(arg_num), create_struct(ptr, t))
            // new_fn(...)
            #if 1
            SgExpression* new_fncall = SB::buildFunctionCallExp(SI::copyExpression(fncall->get_function()),
                                                        new_param_list);

            #else
            SgExpression* new_fncall = SB::buildFunctionCallExp(getFunctionName(fncall),
                                                        fn_retType,
                                                        new_param_list,
                                                        SI::getScope(fncall));
            #endif

            #ifdef FUNCCALL_DEBUG
            printf("new_fncall\n");
            Util::printNode(new_fncall);
            #endif

            // ret* ptr
            SgName var_name("retvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
            SgVariableDeclaration* retvar = SB::buildVariableDeclaration(var_name, fn_retType, NULL, SI::getScope(fncall));
            SI::insertStatementBefore(SI::getEnclosingStatement(fncall), retvar);    

            // ptr = new_fn(...)
            SgExpression* fn_ret = SB::buildAssignOp(SB::buildVarRefExp(retvar), new_fncall);


            
            // pop()
            SgExprListExp* cs_p = SB::buildExprListExp(SB::buildIntVal(arg_num + 1));
            SgExpression* popcall = buildMultArgOverloadFn("pop_from_stack", cs_p, 
                                                    Util::getAddrType(), Util::getScopeForExp(fncall),
                                                    GEFD(SI::getEnclosingStatement(fncall)));

            // int t
            SgName popvar_name("popvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
            SgVariableDeclaration* popvar = SB::buildVariableDeclaration(popvar_name, Util::getAddrType(), NULL, SI::getScope(fncall));
            SI::insertStatementBefore(SI::getEnclosingStatement(fncall), popvar);    

            // t = pop()
            SgExpression* pop_assign = SB::buildAssignOp(SB::buildVarRefExp(popvar), popcall);

            // (pcalls, ptr = new_fn())
            if(pcalls) {
                fn_ret = SB::buildCommaOpExp(pcalls, fn_ret);
            }

            // (pcalls, ptr = new_fn(...), t = pop_from_stack(arg_num))
            fn_ret = SB::buildCommaOpExp(fn_ret, pop_assign);
        
            // Instead of using the aggregate initializer, we can call create_struct... 
            //new_fncall = SB::buildAggregateInitializer(SB::buildExprListExp(new_fncall, popcall), new_retType);    
            #if 0
            new_fncall = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(new_fncall, popcall),
                                            new_retType, Util::getScopeForExp(fncall),
                                            GEFD(SI::getEnclosingStatement(fncall)));
            #endif
            // create_struct(ptr, t)
            SgExpression* cs = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(SB::buildVarRefExp(retvar), SB::buildVarRefExp(popvar)),
                                            new_retType, Util::getScopeForExp(fncall),
                                            GEFD(SI::getEnclosingStatement(fncall)));
            
            // t here contains the address of the stack variable returned, but the 
            // op above this one should handle any copying necessary, properly
            // (pcalls, ptr = new_fn(...), t = pop_from_stack(arg_num), create_struct(ptr, t)
            new_fncall = SB::buildCommaOpExp(fn_ret, cs);

            // replace fncall with new_fncall
            SI::replaceExpression(fncall, new_fncall);

            #else
            
            SgExpression* new_fncall = SB::buildFunctionCallExp(getFunctionName(fncall),
                                                        new_retType,
                                                        new_param_list,
                                                        SI::getScope(fncall));

            SI::replaceExpression(fncall, new_fncall);
            #endif
        }
        #ifdef CLASS_STRUCT_COPY
        // 4. CLASS_STRUCT_COPY: If return type is original class/struct, add additional
        // arg to the param_list (&retvar) and change return type to void.
        else if(strDecl::isOriginalClassType(fn_retType)) {
            // Here we are returning a variable/object of struct/class type... other than the ones we created... so, 
            // we should create a local variable to hold the ret value, and pass it by reference.
            // original code: struct A var = fn(...)
            // transformed code: struct A retvar; struct A var = (fn(..., &retvar), retvar)

            // 1. create a local varible... of the same type as the return type.
            SgName var_name("retvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
            SgVariableDeclaration* retvar = SB::buildVariableDeclaration(var_name, fn_retType, NULL, SI::getScope(fncall));
            SI::insertStatementBefore(SI::getEnclosingStatement(fncall), retvar);

            // 2. add &retvar to the end of a parameters list
            new_param_list->append_expression(Util::createAddressOfOpFor(SB::buildVarRefExp(retvar)));


            SgScopeStatement* scope = Util::getScopeForExp(fncall);
            SgName fn_name(getFunctionName(fncall));

            // 3. this part requires the final fncall expr... fncall(..., &retvar), retvar
            // 4. Also convert return type to void
            SgExpression* new_fncall = SB::buildFunctionCallExp(getFunctionName(fncall), 
                                                        SgTypeVoid::createType(), 
                                                        new_param_list, 
                                                        SI::getScope(fncall)); 

            SgExpression* comma_op = SB::buildCommaOpExp(new_fncall, SB::buildVarRefExp(retvar));
            SI::replaceExpression(fncall, comma_op);    
        }
        #endif
        else {

            #ifdef RETAIN_FUNC_PROTOS
            // transform from: fn(...) to pcalls, new_fn(...), pop_from_stack(arg_num)
            #if 0
            SgExpression* new_fncall = SB::buildFunctionCallExp(getFunctionName(fncall),
                                                            fn_retType,
                                                            new_param_list,
                                                            SI::getScope(fncall));
            #else
            SgExpression* new_fncall = SB::buildFunctionCallExp(SI::copyExpression(fncall->get_function()),
                                                            new_param_list);
            #endif

            #ifdef FUNCCALL_DEBUG
            printf("new_fncall\n");
            Util::printNode(new_fncall);
            #endif

            #ifdef FUNCCALL_DEBUG
            printf("fn_sym\n");
            Util::printNode(fncall->getAssociatedFunctionSymbol());
            #endif

            // (pcalls, new_fn(..))
            if(pcalls) {
                new_fncall = SB::buildCommaOpExp(pcalls, new_fncall);
            }

            // In this case, pop_from_stack will return the top most, but it will be discarded anyway...
            // pop_from_stack
            SgExpression* popcall = buildMultArgOverloadFn("pop_from_stack", SB::buildExprListExp(SB::buildIntVal(arg_num)),
                                                        Util::getAddrType(), Util::getScopeForExp(fncall),
                                                    GEFD(SI::getEnclosingStatement(fncall)));

            // ((pcalls, new_fn(..)), pop_from_stack(arg_num))
            new_fncall = SB::buildCommaOpExp(new_fncall, popcall);

            // replace fn(...) with (pcalls, new_fn())
            SI::replaceExpression(fncall, new_fncall);

            #else
            // retain the original return type, and create a new func call with the
            // new param list
            #if 0
            SgExpression* new_fncall = SB::buildFunctionCallExp(getFunctionName(fncall),
                                                        fn_retType,
                                                        new_param_list,
                                                        Util::getScopeForExp(fncall));
            #endif
            // get scope above only gives the scope of the fncall expression, not of the 
            // function call... which is what is required.
            SgExpression* new_fncall = SB::buildFunctionCallExp(fncall->getAssociatedFunctionSymbol(),
                                                            new_param_list);
            SI::replaceExpression(fncall, new_fncall);
            #endif
        }

    }
    #ifdef FUNCCALL_DEBUG
    printf("handleFuncCalls - Done\n");
    #endif
}

SgType* ArithCheck::getUnderlyingTypeForStruct(SgType* str_type) {

    TypeMap_t::iterator iter = ArithCheck::UnderlyingType.find(str_type);

    ROSE_ASSERT(iter != ArithCheck::UnderlyingType.end());

    return iter->second;
}


SgExpression* ArithCheck::createStructUsingAddressOf(SgExpression* exp, SgNode* pos) {
    #ifdef CS_ADDROF_DEBUG
    printf("createStructUsingAddressOf\n");
    Util::printNode(exp);
    #endif

    SgScopeStatement* scope;
    if(isSgVarRefExp(exp)) {
        scope = Util::getScopeForVarRef(isSgVarRefExp(exp));
    }
    else {    
        //scope = SI::getScope(exp);
        scope = Util::getScopeForExp(exp);
    }

    // Lvalue should be set as false since we are using the address
    // here to create a struct. Not writing to this expression at all
    SgExpression* copy_exp = SI::copyExpression(exp);
    copy_exp->set_lvalue(false);

    SgExpression* addr_of = Util::castToAddr(Util::createAddressOfOpFor(copy_exp));

//    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp->get_type()), Util::getScopeForExp(exp), true);
//    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp->get_type()), GEFD(SI::getEnclosingStatement(exp)), true);
    #ifdef REMOVE_TYPEDEFS_FROM_VARS
    SgType* exp_type = Util::skip_Typedefs(exp->get_type());
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp_type), pos, true);
    #else
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp->get_type()), pos, true);
    #endif


    #if 0
    if(isSgArrayType(exp->get_type())) {
        // If its an array type, then we need to use the pointer to the base type 
        // struct
        SgType* base_type = (isSgArrayType(exp->get_type()))->get_base_type();
        retType = strDecl::getStructType(SgPointerType::createType(base_type), true);
    }
    else {
        retType = strDecl::getStructType(exp->get_type(), true);
    }
    #endif

    //SgScopeStatement* scope = Util::getScopeForExp(exp);

    #if 0
    printf("exp->get_parent()\n");
    Util::printNode(exp->get_parent());
    #endif

    #ifdef OLD_NAME_GEN
    SgName fn_name(Util::getStringForFn("create_struct_from_addr", addr_of->get_type(), NULL, NULL, retType));
//    SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope); 
//    SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope, GEFD(SI::getEnclosingStatement(exp))); 
//    SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope, pos); 
    SgExpression* createfn = buildMultArgOverloadFn(fn_name, SB::buildExprListExp(addr_of), retType, scope, pos);


    #else
//    SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope);
//    SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope, GEFD(SI::getEnclosingStatement(exp)));
//    SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope, pos);
    SgExpression* createfn = buildMultArgOverloadFn("create_struct_from_addr", SB::buildExprListExp(addr_of), retType, scope, pos);
    #endif

    return createfn;

}

SgExpression* ArithCheck::createDerefOutputStruct(SgPointerDerefExp* ptr_deref) {
    printf("createDerefOutputStruct\n");
    return createStructUsingAddressOf(ptr_deref, GEFD(SI::getEnclosingStatement(ptr_deref)));
    
}

SgExpression* ArithCheck::createDerefCheckWithStr(SgExpression* str, SgScopeStatement* scope, 
    SgStatement* pos) {

    #ifdef STRIP_TYPEDEFS
    // No need to do any casting here since the type is 
    // valid struct type
    #endif

    // Expect a struct if its not a var ref
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(str->get_type(), GEFD(pos)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(str->get_type()));
    #endif
        
    SgExprListExp* param_list = SB::buildExprListExp(str);

    // The underlying type will give the actual pointer type that will be returned
    SgType* retType = strDecl::findInUnderlyingType(str->get_type());

    SgExpression* overload = buildMultArgOverloadFn("deref_check_with_str", param_list,
                          retType, scope,
                          pos);
    
    return overload;
}

SgExpression* ArithCheck::createDerefCheck(SgExpression* ptr, SgExpression* addr, SgScopeStatement* scope,
                    SgStatement* pos) {

    #ifdef STRIP_TYPEDEFS
    ptr = Util::castAwayTypedef(ptr);
    // This casting should make sure that the retType
    // and ptr_casted are not typedefs. We are basically
    // trying to create a boundary... by removing all typedefs
    // before going into the create function part. So all the
    // functions we have generated wouldn't have a typedef anywhere in them
    #endif

    SgType* retType = Util::getTypeForPntrArr(ptr->get_type());

    SgType* ptr_type = ptr->get_type();

    SgExpression* ptr_casted;

    #ifdef SUPPORT_REFERENCES
    if(isSgReferenceType(ptr_type)) {
        ptr_casted = ptr;
    }
    else if(isSgPointerType(ptr_type)) 
    #else
    if(isSgPointerType(ptr_type))
    #endif
    {
        ptr_casted = ptr;
    }
    else if(isSgArrayType(ptr_type)) {
        ptr_casted = SB::buildCastExp(ptr, retType, SgCastExp::e_C_style_cast); 
    }
    else {
        printf("createDerefCheck: ptr is not qual type\n");
        printf("ptr\n");
        Util::printNode(ptr);
        ROSE_ASSERT(0);
    }
    
    SgExprListExp* param_list = SB::buildExprListExp(ptr_casted, addr);
    
    SgExpression* overload = buildMultArgOverloadFn("deref_check", param_list,
                        retType, scope, pos);

    return overload;
}

void ArithCheck::handleDerefExp(SgPointerDerefExp* ptr_deref) {
    
    #ifdef DEREF_DEBUG
    printf("handleDerefExp\n");
    #endif


    // Here, we expect a var ref as the operand... or a struct.
    // First, the var ref case. 
    // Single function call does deref_check and returns the 
    // appropriate pointer. This handles only single dimensional
    // pointers/arrays for now. 

    SgExpression* overload;
    
    SgExpression* oper = ptr_deref->get_operand();
    SgType* oper_type = oper->get_type();

    #ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
    #endif

    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(oper_type, GEFD(oper))) 
    #else
    if(strDecl::isValidStructType(oper_type)) 
    #endif
    {
        // We need to insert deref_check_with_str
        // input: *str
        // output: *deref_check_with_str(str);
        overload = createDerefCheckWithStr(SI::copyExpression(ptr_deref->get_operand()),
                        Util::getScopeForExp(ptr_deref),
                        GEFD(SI::getEnclosingStatement(ptr_deref)));
    }
    else {
        // If this is a var ref 
        if(isSgVarRefExp(oper)) {
            SgVarRefExp* var_ref = isSgVarRefExp(oper);
            
            #ifndef RETAIN_FUNC_PROTOS
            SgVariableSymbol* param = checkInVarRemap(var_ref->get_symbol());
            if(param != NULL) {
                // This was a param, which has been converted to struct
                // This is a pointer. Arrays don't go through fncalls
                // directly. They need to be inserted within structs

                SgType* var_ref_type = var_ref->get_type();

                #ifdef STRIP_TYPEDEFS
                var_ref_type = Util::getType(var_ref_type);
                #endif
                ROSE_ASSERT(isSgPointerType(var_ref_type));
    
                // input: *ptr
                // output: *deref_check_with_str(ptr_str)

                overload = createDerefCheckWithStr(SB::buildVarRefExp(param),
                                SI::getScope(ptr_deref),
                                GEFD(SI::getEnclosingStatement(ptr_deref)));
            }
            else {
            #endif
                // This is var ref which is not part of params
                // input: *var_ref
                // output: *deref_check(var_ref, &var_ref)

                // We might also need to do a create_entry if the operand is
                // of array type
                #ifdef ARRAY_CREATE_ENTRY
                // Also, we need to insert create_entry, if its
                // array type.
                if(isSgArrayType(oper_type)) {
                    insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(ptr_deref),
                            SI::getEnclosingStatement(ptr_deref));
                }
                #endif


                overload = createDerefCheck(SI::copyExpression(oper),
                            Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper))),
                            SI::getScope(ptr_deref),
                            GEFD(SI::getEnclosingStatement(ptr_deref)));
            #ifndef RETAIN_FUNC_PROTOS
            }
            #endif
        }
        else if(isSgDotExp(oper) || isSgArrowExp(oper)) {
            // This is a dot/arrow beneath... with a var ref on the rhs. 
            // input: *var.ptr or *var->ptr
            // output: *deref_check(var.ptr, &var.ptr) or *deref_check(var->ptr, &var->ptr)

            // We might also need to do a create_entry if the operand is
            // of array type
            #ifdef ARRAY_CREATE_ENTRY
            // Also, we need to insert create_entry, if its
            // array type.
            if(isSgArrayType(oper_type)) {
                    insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(ptr_deref),
                                    SI::getEnclosingStatement(ptr_deref));
            }
            #endif

            overload = createDerefCheck(SI::copyExpression(oper),
                        Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper))),
                        SI::getScope(ptr_deref),
                        GEFD(SI::getEnclosingStatement(ptr_deref)));
        }
        #ifdef DOUBLE_DIM_ENABLED
        else if(isSgPointerDerefExp(oper)) {
            // This is a double pointer deref... **ptr    
            // input: **(deref_check(ptr, &ptr))
            // output: *(double_deref(deref_check(ptr, &ptr)))

            #ifdef ARRAY_CREATE_ENTRY
            // Also, we need to insert create_entry, if its
            // array type.
            if(isSgArrayType(oper_type)) {
                    insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(ptr_deref),
                                    SI::getEnclosingStatement(ptr_deref));
            }
            #endif

            SgExpression* strip_deref = SI::copyExpression(Util::stripDeref(ptr_deref->get_operand()));

            #ifdef STRIP_TYPEDEFS
            strip_deref = Util::castAwayTypedef(strip_deref);
            #endif

            overload = buildMultArgOverloadFn("double_deref_check", 
                                SB::buildExprListExp(strip_deref),
                                oper_type,
                                Util::getScopeForExp(ptr_deref),
                                GEFD(SI::getEnclosingStatement(ptr_deref)));

        }
        else if(isSgPntrArrRefExp(oper)) {
            // This is deref of a pntr arr ref... *array[index]
            // input: *array[index]
            // output: *deref_check(array[index], &array[index])

            // We have performed the array bound check for
            // the pntr arr ref. Here, we need to perform a deref check on that.

            #ifdef ARRAY_CREATE_ENTRY
            // Also, we need to insert create_entry, if its
            // array type.
            if(isSgArrayType(oper_type)) {
                    insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(ptr_deref),
                                    SI::getEnclosingStatement(ptr_deref));
            }
            #endif        

            overload = createDerefCheck(SI::copyExpression(oper),
                        Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper))),
                        SI::getScope(ptr_deref),
                        GEFD(SI::getEnclosingStatement(ptr_deref)));
        }
        #endif
        else if(isSgThisExp(oper)) {
            // "this" would be derefed before being used in a dot exp... 
            // nothing to do here..
            return;
        }
        else {
            printf("Unsupported case in DerefExp\n");
            printf("ptr_deref\n");
            Util::printNode(ptr_deref);
            ROSE_ASSERT(0);
        }
    }

    // Need to figure out here if the ptr_deref output type is also of qualifying type
    // If it is, then we call DoublePointerDeref on the overload created above
    // In the DoublePointerDeref... 
    // for structs, we'll say return create_struct(*str.ptr, str.ptr)
    // for variables, we'll say return create_struct(*ptr, ptr)

    SgExpression* new_exp;
    // Make sure that overload is of qualifying type
    //ROSE_ASSERT(isSgPointerType(overload->get_type()));
    ROSE_ASSERT(Util::isQualifyingType(overload->get_type()));

    new_exp = SB::buildPointerDerefExp(overload);

    SI::replaceExpression(ptr_deref, new_exp);
    
}

void ArithCheck::handleConditionals(SgBinaryOp* bop) {

    #ifdef COND_DEBUG
    printf("handleConditionals\n");    
    #endif

    //std::string op_name(Util::getStringForVariantT(bop->variantT()));

    // not defined..
    #ifdef COND_BOOL_RETVAL
    SgType* retType = SgTypeBool::createType();
    #else
    SgType* retType = SgTypeInt::createType();
    #endif

    // Conditionals always return bool
    std::string op_name = Util::getStringForOp(isSgNode(bop), retType);

    SgExpression* LHS = bop->get_lhs_operand();
    SgExpression* LHS_Copy = SI::copyExpression(LHS);

    #ifdef STRIP_TYPEDEFS
    LHS_Copy = Util::castAwayTypedef(LHS_Copy);
    #endif

    SgExpression* RHS = bop->get_rhs_operand();
    SgExpression* RHS_Copy = SI::copyExpression(RHS);

    #ifdef STRIP_TYPEDEFS
    RHS_Copy = Util::castAwayTypedef(RHS_Copy);
    #endif

    if(isSgValueExp(RHS) || isSgValueExp(LHS)) {
        // Both can't be values -- right?
        ROSE_ASSERT(isSgValueExp(LHS) && isSgValueExp(RHS));
        op_name = "_ValueCast";
    }
    
    #ifdef OLD_NAME_GEN
//    SgExpression* overload = buildOverloadFn(SgName(op_name), LHS_Copy, RHS_Copy, retType, Util::getScopeForExp(bop));
//    SgExpression* overload = buildOverloadFn(SgName(op_name), LHS_Copy, RHS_Copy, retType, Util::getScopeForExp(bop), GEFD(SI::getEnclosingStatement(bop)));
    SgExpression* overload = buildMultArgOverloadFn(SgName(op_name), SB::buildExprListExp(LHS_Copy, RHS_Copy), retType, Util::getScopeForExp(bop), GEFD(SI::getEnclosingStatement(bop)));
    #else
//    SgExpression* overload = buildOverloadFn(SgName(Util::getStringForVariantT(bop->variantT())), LHS_Copy, RHS_Copy, retType, Util::getScopeForExp(bop));
//    SgExpression* overload = buildOverloadFn(SgName(Util::getStringForVariantT(bop->variantT())), LHS_Copy, RHS_Copy, retType, Util::getScopeForExp(bop), GEFD(SI::getEnclosingStatement(bop)));
    SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(bop->variantT())), SB::buildExprListExp(LHS_Copy, RHS_Copy), 
                                                    retType, Util::getScopeForExp(bop), GEFD(SI::getEnclosingStatement(bop)));
    #endif

    SI::replaceExpression(bop, overload);

    #ifdef INLINING_CHECK
//    bool status = inlining(isSgFunctionCallExp(overload));
    bool status = doInline(isSgFunctionCallExp(overload));
    #ifdef COND_DEBUG
    printf("INLINING_CHECK: status: %s\n", status ? "true" : "false");
    #endif
    #endif
}

SgType* skipArrayTypes(SgArrayType* arr_type) {

    SgType* base_type = arr_type->get_base_type();
    if(isSgArrayType(base_type)) {
        return skipArrayTypes(isSgArrayType(base_type));
    }
    return base_type;
}

SgType* ArithCheck::getTypeForPntrArr(SgType* type) {

    #ifdef STRIP_TYPEDEFS
    ROSE_ASSERT(!isSgTypedefType(type));
    #endif

    #ifdef SKIP_TYPEDEFS_QUAL_TYPES
    type = Util::skip_Typedefs(type);
    #endif

    #ifdef SUPPORT_REFERENCES
    if(isSgReferenceType(type)) {
        return type;
    }
    #endif

    if(isSgPointerType(type)) {
        return type;
    }
    else if(isSgArrayType(type)) {


        #ifdef DELAYED_INSTR
        SgType* base_type = isSgArrayType(type)->get_base_type();
        return SgPointerType::createType(base_type);

        #else

        // Original working version... 
        #if 0
        SgType* base_type = isSgArrayType(type)->get_base_type();
        SgType* retType = SgPointerType::createType(base_type);
        return retType;
        #else
        SgType* base_type = skipArrayTypes(isSgArrayType(type));
        // Should we do this? -- I think we shouldn't remove the 
        // pointer since this could be a case of array of pointers
        #if 0
        if(isSgPointerType(base_type)) {
            base_type = skip_RefsPointersAndTypedefs(base_type);
        }
        #endif
        // Now that we have the correct base type (i.e. non array type)
        // create a pointer version of it...
        return SgPointerType::createType(base_type);
        #endif
        #endif
    }
    else {
        ROSE_ASSERT(Util::isQualifyingType(type));
        printf("Qualifying Type not supported in Util::getTypeForPntrArr\n");
        ROSE_ASSERT(0);
    }
}

SgInitializedName* ArithCheck::createStructVariableFor(SgInitializedName* name) {

    std::string new_name_string;

    if(strcmp(name->get_name().getString().c_str(), "") == 0) {
        new_name_string = "";
    }
    else {
        new_name_string = name->get_name().getString() + "_str";
    }

    SgName new_name_sg(new_name_string);
    #ifdef STRUCT_VAR_DEBUG
    printf("created new name: %s\n", new_name_string.c_str());
    #endif

    // FIXME: Assuming no initializer
    SgInitializer* name_init = name->get_initializer();
    ROSE_ASSERT(name_init == NULL);

    #if 0
    // Get the VoidStruct Type
    SgType* new_name_type = strDecl::getStructType(SgPointerType::createType(SgTypeVoid::createType()));
    #endif

    // create one if necessary

    // Working one
//    SgType* new_name_type = strDecl::getStructType(Util::getTypeForPntrArr(name->get_type()), name, true);
    // Remove the typedefs before creating a struct...

    SgType* name_type = name->get_type();

    #ifdef STRIP_TYPEDEFS
    name_type = Util::getType(name_type);
    #endif

    SgType* new_name_type = strDecl::getStructType(Util::getTypeForPntrArr(name_type), GEFD(name), true);

    #ifdef STRUCT_VAR_DEBUG
    printf("name_type: %s\n", name_type->unparseToString().c_str());
    printf("new_name_type: %s\n", new_name_type->unparseToString().c_str());
    #endif

    SgInitializedName* new_name = SB::buildInitializedName(new_name_sg, new_name_type, NULL); 

    ROSE_ASSERT(new_name != NULL);

    new_name->set_scope(name->get_scope());

    return new_name;
}
    

void ArithCheck::createLocalVariableAndUpdateMetadata(SgInitializedName* vs,
                                                      SgInitializedName* orig,
                                                      SgFunctionDeclaration* fn_decl) {
    
    
    SgVariableDeclaration* local_var_decl = Util::createLocalVariable(orig);

    // Insert this variable at the top of the body
    SgFunctionDefinition* fn_def = fn_decl->get_definition();
    ROSE_ASSERT(fn_def != NULL);
    SgBasicBlock* bb = fn_def->get_body();
    SgStatementPtrList& stmts = bb->get_statements();

    //isSgScopeStatement(fn_decl->get_scope())->insertStatementInScope(local_var_decl, true);
    SI::insertStatementBefore(*(stmts.begin()), local_var_decl);
    printf("Insert local_var_decl\n");


    // Now, insert a function call which creates an entry from this variable's address
    // and uses data from the vs addr
    SgExpression* addressOfOp = Util::createAddressOfOpFor(Util::getNameForDecl(local_var_decl));
    // Lets cast this addressOfOp to unsigned long long -- addr type
    //SgCastExp* cast_exp = SB::buildCastExp(addressOfOp, Util::getAddrType(), SgCastExp::e_reinterpret_cast);
    SgCastExp* cast_exp = SB::buildCastExp(addressOfOp, Util::getAddrType(), CAST_TYPE);

    SgExpression* dot_exp = Util::createDotExpFor(vs, "addr");
    
    // Could also use the fn def scope
    //SgExpression* overload = buildOverloadFn("create_entry", addressOfOp, dot_exp, SgTypeVoid::createType(), Util::getNameForDecl(local_var_decl)->get_scope()); 
    // Working one.. Commented out to add local_var_decl
//    SgExpression* overload = buildOverloadFn("create_entry", cast_exp, dot_exp, SgTypeVoid::createType(), Util::getNameForDecl(local_var_decl)->get_scope()); 

//    SgExpression* overload = buildOverloadFn("create_entry", cast_exp, dot_exp, SgTypeVoid::createType(), Util::getNameForDecl(local_var_decl)->get_scope(), local_var_decl); 
    SgExpression* overload = buildMultArgOverloadFn("create_entry", SB::buildExprListExp(cast_exp, dot_exp), SgTypeVoid::createType(), Util::getNameForDecl(local_var_decl)->get_scope(), local_var_decl); 

    // We should assign input_recr = input_str.ptr -- not the other way round
    // This tweak makes initializeStructWith act this way.
    bool flip_ptr_init = true;
    // Initialize this local_var with the ptr value in the vs
    SgCommaOpExp* init_op = initializeStructWith(Util::getNameForDecl(local_var_decl), vs, flip_ptr_init);

    // Combine both
    SgCommaOpExp* overload_init = SB::buildCommaOpExp(overload, init_op);


    // create a statement and insert it here
    SgExprStatement* ov_in = SB::buildExprStatement(isSgExpression(overload_init));

    SI::insertStatementAfter(local_var_decl, ov_in);

    
    
    // Now, update the symbol so that all references to the orig
    // variable are replaced with references to vs
    handleSymbolTable(orig, vs);

}

void ArithCheck::handleFuncDecls(SgFunctionDeclaration* fn_decl) {

    #ifdef RETAIN_FUNC_PROTOS
    // Nothing to do, since we retain the return type
    return;
    #endif

    // Changes to parameter lists alter the body of the function -- to insert
    // instructions that update metadata. This work is done in handleFuncParams
    // since doing it here would be too late --- we arrive at this function after
    // having transformed the body of the function.
    // This function only changes the return type to struct type if its of 
    // qualifying type.
    // Order of traversal: param_list, basic_block, fndecl
    // we transform params and update varRemap and other structures in param_list
    // so that we can use them while we traverse the basic_block

    #ifdef FUNCDECL_DEBUG
    printf("handleFuncDecls\n");
    printf("fn_decl\n");
    Util::printNode(fn_decl);
    #endif

    SgName fn_name = fn_decl->get_name();

    SgScopeStatement* scope = fn_decl->get_scope();
    #ifdef FUNCDECL_DEBUG
    printf("Got scope\n");
    #endif
    
    SgFunctionParameterList* param_list = fn_decl->get_parameterList();
    ROSE_ASSERT(param_list != NULL);
    
    #ifdef FUNCDECL_DEBUG
    printf("Got paramList\n");
    #endif

    SgFunctionDefinition* fndef = fn_decl->get_definition();
    bool defining_decl = (fndef != NULL);

    
    // Return type is given by strDecl::getStructType on the current poitner type
    SgFunctionType* fn_type = fn_decl->get_type();
    SgType* ret_type = fn_type->get_return_type();

    #ifdef STRIP_TYPEDEFS
    ret_type = Util::getType(ret_type);
    #endif

    SgType* new_ret_type;

    #ifdef FUNCDECL_DEBUG
    printf("Return type is qualifying type?\n");
    #endif
    if(Util::isQualifyingType(ret_type)) {
        #ifdef FUNCDECL_DEBUG
        printf("Yes\n");
        #endif
        // fn_decl is a scope in itself
        new_ret_type = strDecl::getStructType(Util::getTypeForPntrArr(ret_type), fn_decl, true);
    }
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(ret_type)) {
        #ifdef FUNCDECL_DEBUG
        printf("Original Class Type\n");
        #endif
        // return void type.. since we will be using the arguments to return this value...
        new_ret_type = SgTypeVoid::createType();
    }
    #endif
    else {
        #ifdef FUNCDECL_DEBUG
        printf("No. Retaining old ret type\n");
        #endif
        new_ret_type = ret_type;
        // FIXME: Can't we just leave here... ? Nothing has changed...
    }

    // Since the parameter work is done in handleFuncParams already, just copy over the parameter list
    SgFunctionParameterList* new_args = isSgFunctionParameterList(SI::deepCopyNode(isSgNode(param_list)));

    // Keeping decorator list the same
    SgExprListExp* dec_list = fn_decl->get_decoratorList();
    SgExprListExp* new_dec_list = isSgExprListExp(SI::deepCopyNode(isSgNode(dec_list)));

    SgFunctionDeclaration* new_fn_decl;

    
    // Already doing this in cloneFunctions
    #if 0
    //#ifdef CLONE_FUNCTIONS
    std::string new_fn_name_string = fn_name.getString() + "_mod";
    SgName new_fn_name(new_fn_name_string);
    #else
    SgName new_fn_name = fn_name;
    #endif

    if(fndef) {
        #ifdef FUNCDECL_DEBUG
        printf("Defining fn decl\n");
        #endif
        // This is a defining func decl.
        new_fn_decl = SB::buildDefiningFunctionDeclaration(new_fn_name,
                                new_ret_type,
                                new_args,
                                scope,
                                new_dec_list);

        SgFunctionDefinition* new_fndef = isSgFunctionDefinition(SI::deepCopyNode(isSgNode(fndef)));

        new_fn_decl->set_definition(new_fndef);
        isSgNode(new_fndef)->set_parent(new_fn_decl);

        #ifdef FUNCDECL_DEBUG
        printf("fn_decl\n");
        Util::printNode(fn_decl);

        printf("new_fn_decl\n");
        Util::printNode(new_fn_decl);
        #endif

        SI::replaceStatement(fn_decl, new_fn_decl);

    }
    else {
        #ifdef FUNCDECL_DEBUG
        printf("Nondefining fn decl\n");
        #endif
        // Nondefining fn decl
        new_fn_decl = SB::buildNondefiningFunctionDeclaration(new_fn_name,
                                new_ret_type,
                                new_args,
                                scope,
                                new_dec_list);
        #ifdef FUNCDECL_DEBUG
        printf("fn_decl\n");
        Util::printNode(fn_decl);

        printf("new_fn_decl\n");
        Util::printNode(new_fn_decl);
        #endif

        SI::replaceStatement(fn_decl, new_fn_decl);

    }


    // Checking that we did the job correctly
    if(Util::isQualifyingType(ret_type)) {
        #ifdef CPP_SCOPE_MGMT
        ROSE_ASSERT(strDecl::isValidStructType(new_fn_decl->get_type()->get_return_type(), new_fn_decl));
        #else
        ROSE_ASSERT(strDecl::isValidStructType(new_fn_decl->get_type()->get_return_type()));
        #endif
    }
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(ret_type)) {
        ROSE_ASSERT(Util::compareTypes(SgTypeVoid::createType(), new_fn_decl->get_type()->get_return_type()));
    }
    #endif
    else {
        ROSE_ASSERT(Util::compareTypes(ret_type, new_fn_decl->get_type()->get_return_type()));
    }

}


void ArithCheck::updateMetaDataAndReassignAddr(SgInitializedName* str, 
                    SgInitializedName* orig, 
                    SgFunctionDeclaration* fn_decl) {
    
    #ifdef UPDATE_MD_DEBUG
    printf("updateMetaDataAndReassignAddr\n");
    #endif
    // create_entry(dest, src)    
    // this will be: create_entry(&str.ptr, str.addr);
    // At this stage, str.addr contains the caller variable's address.
    // so we copy over the metadata
    SgExpression* ptr_expr = Util::createDotExpFor(str, "ptr");
    SgExpression* dest_addr = Util::castToAddr(Util::createAddressOfOpFor(ptr_expr));

    SgExpression* src_addr = Util::createDotExpFor(str, "addr");

    SgScopeStatement* scope = str->get_scope();
    
    // create_entry function call.
//    SgExpression* createfn = buildOverloadFn("create_entry", dest_addr, src_addr, SgTypeVoid::createType(), scope); 
//    SgExpression* createfn = buildOverloadFn("create_entry", dest_addr, src_addr, SgTypeVoid::createType(), scope, GEFD(fn_decl));


//    SgExpression* createfn = buildOverloadFn("create_entry", dest_addr, src_addr, SgTypeVoid::createType(), scope, fn_decl);
    SgExpression* createfn = buildMultArgOverloadFn("create_entry", SB::buildExprListExp(dest_addr, src_addr), SgTypeVoid::createType(), scope, fn_decl);

    #ifdef UPDATE_MD_DEBUG
    printf("createfn\n");
    Util::printNode(createfn);
    #endif

    // Now, we make the following assignment:
    // str.addr = &str.ptr;

    SgExpression* lhs = SI::copyExpression(src_addr);
    SgExpression* rhs = SI::copyExpression(dest_addr);

    SgExpression* assign_op = SB::buildAssignOp(lhs, rhs);
    #ifdef UPDATE_MD_DEBUG
    printf("assign_op\n");
    Util::printNode(assign_op);
    #endif

    // Now, create a comma op to hold both these exprs
    SgExpression* comma_op = SB::buildCommaOpExp(createfn, assign_op);

    // Create a statement from this
    SgStatement* expr_stmt = SB::buildExprStatement(comma_op);
        

    // Insert this statement at the top of the body
    #if 0
    SgFunctionDefinition* fn_def = fn_decl->get_definition();
    ROSE_ASSERT(fn_def != NULL);
    SgBasicBlock* bb = fn_def->get_body();
    #endif
    SgBasicBlock* bb = Util::getBBForFn(fn_decl);
    SgStatementPtrList& stmts = bb->get_statements();

    SI::insertStatementBefore(*(stmts.begin()), expr_stmt);

    #ifdef UPDATE_MD_DEBUG
    printf("expr_stmt\n");
    Util::printNode(expr_stmt);
    #endif


    //handleSymbolTable(orig, str); -- already completed in parent function ---
}

SgInitializedName* ArithCheck::createRefVersionFor(SgInitializedName* name) {

    std::string new_name_string;

    if(strcmp(name->get_name().getString().c_str(), "") == 0) {
        new_name_string = "";
    }
    else {
        new_name_string = name->get_name().getString() + "_ref" + boost::lexical_cast<std::string>(Util::VarCounter++);
     }

    SgName new_name_sg(new_name_string);
    #ifdef REF_VER_DEBUG
    printf("created new name: %s\n", new_name_string.c_str());
    #endif

    // FIXME: Assuming no initializer
    SgInitializer* name_init = name->get_initializer();
    ROSE_ASSERT(name_init == NULL);

    #if 0
    // Get the VoidStruct Type
    SgType* new_name_type = strDecl::getStructType(SgPointerType::createType(SgTypeVoid::createType()));
    #endif

    // create one if necessary

    // Working one

    SgType* name_type = name->get_type();

    #ifdef STRIP_TYPEDEFS
    name_type = Util::getType(name_type);
    #endif

    SgType* new_name_type = SgPointerType::createType(name_type);

    SgInitializedName* new_name = SB::buildInitializedName(new_name_sg, new_name_type, NULL); 

    ROSE_ASSERT(new_name != NULL);

    new_name->set_scope(name->get_scope());

    return new_name;
}

SgVariableDeclaration* ArithCheck::assignRefToLocalVar(SgInitializedName* name, SgFunctionDeclaration* fn_decl, SgScopeStatement* bb_scope) {

    SgExpression* init = SB::buildPointerDerefExp(SB::buildVarRefExp(name));
    SgAssignInitializer* init_assign = SB::buildAssignInitializer(init, init->get_type());
    ROSE_ASSERT(isSgPointerType(name->get_type()));
    SgType* type = isSgPointerType(name->get_type())->get_base_type();
    ROSE_ASSERT(!isSgPointerType(type));

    //SgScopeStatement* scope = name->get_scope();

    SgName s_name(name->get_name().getString() + "_local");

    SgVariableDeclaration* local_var = SB::buildVariableDeclaration(s_name, type, init_assign, bb_scope);

    return local_var;
}

void ArithCheck::addToSymbolTable(SgInitializedName* name, SgNode* parent) {

    SgSymbol* symbol = name->get_symbol_from_symbol_table();

    if(symbol == NULL) {

        SgVariableSymbol* var_symbol = new SgVariableSymbol(name);
        isSgNode(var_symbol)->set_parent(parent);
        SgSymbolTable* currSymbolTable = isSgSymbolTable(isSgNode(var_symbol)->get_parent());
        currSymbolTable->insert(name->get_name(), var_symbol);

        // Check that this worked
        symbol = name->get_symbol_from_symbol_table();
        ROSE_ASSERT(symbol);
    }
}

void ArithCheck::addToSymbolTableAndClassRemap(SgInitializedName* orig_name, SgInitializedName* local_name) {

    SgScopeStatement* orig_scope = orig_name->get_scope();

    SgSymbol* orig_symbol = orig_name->get_symbol_from_symbol_table();
    SgVariableSymbol *orig_var_symbol = isSgVariableSymbol(orig_symbol);
    #ifdef SYM_TAB_REMAP_DEBUG
    printf("Got scope. Now, symbolTable\n");
    #endif

    addToSymbolTable(local_name, isSgNode(orig_var_symbol)->get_parent()); 

    #if 0

    SgSymbol* local_symbol = local_name->get_symbol_from_symbol_table();

    if(local_symbol == NULL) {

        SgVariableSymbol* local_var_symbol = new SgVariableSymbol(local_name);
        isSgNode(local_var_symbol)->set_parent(isSgNode(orig_var_symbol)->get_parent());
        SgSymbolTable* currSymbolTable = isSgSymbolTable(isSgNode(local_var_symbol)->get_parent());
        currSymbolTable->insert(local_name->get_name(), local_var_symbol);

        // Check that this worked
        local_symbol = local_name->get_symbol_from_symbol_table();
        ROSE_ASSERT(local_symbol);
    }
    #endif

    SgSymbol* local_symbol = local_name->get_symbol_from_symbol_table();

    ClassStructVarRemap.insert(VariableSymbolMap_t::value_type(orig_var_symbol, isSgVariableSymbol(local_symbol)));

}

SgExpression* ArithCheck::copyElementMetadata(SgInitializedName* src, SgInitializedName* dest, SgFunctionDeclaration* fn_decl) {

    // This function performs a copy of the metadata, of all the pointer variables within
    // src to those within dest...
    // The copying is performed using a function. This function will be placed in the body
    // of a class/struct in case of C++, and outside the struct in case of C. The code would
    // look like follows:
    // C++:
    // class/struct A{
    //     int *ptr;
    //     rtc_copy(&src, lock, key) {
    //          // call to inheritance copy functions...
    //          create_entry(&ptr, &src.ptr, lock, key); // order: dest_addr, src_addr, lock, key
    //       }
    // };
    //
    // C:
    // struct A {
    //       int *ptr;
    // };
    // rtc_copy(&dest, &src, lock, key) { -- This function has to be defined in the current function...
    //     create_entry(&dest.ptr, &src.ptr, lock, key) // order: dest_addr, src_addr, lock, key
    // }
    // 
    // In an example function: 
    // fn1(class A As) {
    // }
    // 
    //
    // main() {
    //      class A Am;
    //    fn1(Am);
    // }
    //
    // Transformed to....
    // fn1(class A* As) {
    //    lock, key;
    //    class A Al = *As;
    //    Al.rtc_copy(As); -- This has to be done in this function...
    // }
    // 
    // main() {
    //    class A Am;
    //    fn1(&Am);
    // }
    // 
    //
    // In C++, the copy constructor can't be used since we need the updated lock and key for the new scope.
    // These are not available in the traditional copy constructor call. An option that can be explored here
    // is the object level lock and key, rather than identifier level lock and key, but that would run against
    // the rest of the setup... which is pointer id based.
    //
    // In this function: src: As, dest: Al
    
    // For the case of C functions, 
    // In the example function:
    // fn1(struct A As) {
    // }
    //
    // main() {
    //   struct A Am;
    //   fn1(Am);
    //
    // Transformed to...
    // fn1(struct A* As) {
    //   class A Al = *As;
    //   rtc_copy(&Al, As); // order: dest, src
    // }
    //
    // main() {
    //   struct A Am;
    //   fn1(&Am);
    //

    #ifdef COPY_ELEMENT_MD_DEBUG    
    printf("copyElementMetadata\n");
    #endif


    // Just to make sure that src and dest are in fact from the same data struct... 
    // src is a pointer... dest is an object...
    ROSE_ASSERT(isSgPointerType(src->get_type())->get_base_type() == dest->get_type());
    // Its a user defined struct, not one we inserted
    ROSE_ASSERT(strDecl::isOriginalClassType(dest->get_type()));
    
    #if 0
    SgExpression* src_ref = SB::buildVarRefExp(src);
    SgExpression* dest_ref = SB::buildVarRefExp(dest);

    // Now, find all the pointers in the data structure, and do a source to dest copy... 
    SgClassType* str_type = isSgClassType(dest->get_type());

    // Two parts now...
    // 1. create the rtc_copy function...
    // 2. call the rtc_copy function from this function... this fncall exp is the one that will
    // be returned

    // create rtc_copy function... and return the function declaration.
    // the declaration will be inserted prior to the current function for structs, and in
    // the class body for class objects.
    // FIXME: Only create if not available... use FuncInfo to track this.

        
    SgFunctionDeclaration* rtc_copy_fn = createRTCCopy(src, dest, fn_decl);


    // 2. creating the function call exp
    SgExprListExp* param_list = SB::buildExprListExp(Util::createAddressOfOpFor(SB::buildVarRefExp(dest)), SB::buildVarRefExp(src));
    SgFunctionSymbol* rtc_copy_fn_symbol = isSgFunctionSymbol(rtc_copy_fn->get_symbol_from_symbol_table());
    ROSE_ASSERT(rtc_copy_fn_symbol);

    SgExpression* overload = SB::buildFunctionCallExp(rtc_copy_fn_symbol, param_list);
    #else
    // The src is a pointer... *As
    // The dest needs to be changed to a pointer. And then we'll call rtc_copy from
    // Order of args for rtc_copy: dest, src
    
    SgExpression* ce1 = Util::createAddressOfOpFor(SB::buildVarRefExp(dest)); 
    SgExpression* ce2 = SB::buildVarRefExp(src);

    #if 0
    SgExpression* overload = buildOverloadFn("rtc_copy", ce1, ce2, SgTypeVoid::createType(), 
                                            SI::getScope(fn_decl), fn_decl);
    #endif
    SgExpression* overload = buildMultArgOverloadFn("rtc_copy", SB::buildExprListExp(ce1, ce2), SgTypeVoid::createType(), 
                                            SI::getScope(fn_decl), fn_decl);

    #endif
    return overload;
}

SgFunctionDeclaration* ArithCheck::getFuncDeclForNode(SgNode* node) {

    SgNode* parent = node->get_parent();
    if(isSgFunctionDeclaration(parent)) {
        return isSgFunctionDeclaration(parent);
    }
    else {
        return getFuncDeclForNode(parent);
    }
}

void ArithCheck::handleClassStructReturnStmts(SgReturnStmt* retstmt) {

    #ifdef CLASSSTRUCT_RETURN_DEBUG
    printf("handleClassStructReturnStmts\n");
    #endif

    #ifdef RETAIN_FUNC_PROTOS
    // Nothing to do for now... 
    // FIXME: Should push onto the stack, element
    // by element for C structs
    return;
    #endif

    SgType* retType = retstmt->get_expression()->get_type();

    #ifdef STRIP_TYPEDEFS
    retType = Util::getType(retType);
    #endif

    ROSE_ASSERT(strDecl::isOriginalClassType(retType));

    SgFunctionDeclaration* fn_decl = getFuncDeclForNode(retstmt);        

    SgInitializedNamePtrList& args = fn_decl->get_args();

    // Now, find the last arg in the list.
    SgInitializedNamePtrList::iterator i = args.begin();
    i += args.size() - 1;
    SgInitializedName* last_arg = *i;
    
    #ifdef CLASSSTRUCT_RETURN_DEBUG
    printf("arg check:\n");
    Util::printNode(last_arg);
    #endif

    // Check that its type is of the pointer type of the ret stmt type
    SgType* last_arg_type = last_arg->get_type();                      
    #ifdef STRIP_TYPEDEFS
    last_arg_type = Util::getType(last_arg_type);
    #endif
    ROSE_ASSERT(isSgPointerType(last_arg->get_type()));

    SgType* last_arg_base_type = isSgPointerType(last_arg_type)->get_base_type();
    #ifdef STRIP_TYPEDEFS
    last_arg_base_type = Util::getType(last_arg_base_type);
    #endif
    ROSE_ASSERT(last_arg_base_type == retType);

    // Now, do the assignment and void return substitution
    // original: return ret_exp
    // transformed: *retvar = ret_exp
    //              return
    
    // this copies the expression in the return statement and uses it as rhs of the assign op
    // *retvar = ret_exp;
    SgAssignOp* assign_op = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(last_arg)), SI::copyExpression(retstmt->get_expression()));
    SgExprStatement* assign_stmt = SB::buildExprStatement(assign_op);
                
    SgStatement* voidretstmt = SB::buildReturnStmt();

    SI::replaceStatement(retstmt, assign_stmt);

    #ifdef LOCK_KEY_INSERT
    #ifdef CLASSSTRUCT_RETURN_DEBUG
    printf("LOCK_KEY_INSERT: inserting ExitScope before last return\n");
    #endif
    //SgExpression* overload = buildOverloadFn("ExitScope", NULL, NULL, SgTypeVoid::createType(), SI::getScope(retstmt), GEFD(retstmt));
    SgExpression* overload = buildMultArgOverloadFn("ExitScope", SB::buildExprListExp(), SgTypeVoid::createType(), SI::getScope(retstmt), GEFD(retstmt));

    SgStatement* exit_scope = SB::buildExprStatement(overload);
    // assign_stmt
    // exit_scope
    // return
    SI::insertStatementAfter(assign_stmt, exit_scope);
    SI::insertStatementAfter(exit_scope, voidretstmt);
    #else
    SI::insertStatementAfter(assign_stmt, voidretstmt);
    #endif
    
    return;
}

// The problem with this function is as follows. We are trying to 
// create a new param list to replace the current one. However, what we are doing 
// is that we are actually creating a new param list, setting the parent for the
// new args as the old param list, and adding them to the new param list... which
// seems completely wrong... Through out the duration of this function, the new param
// list considers the fn_decl as its parent, and so does the old one... the fn_decl only
// thinks the old one is its child, until at the very end, we set the new param list
// as the child. I think there should be a much cleaner way to handle this transition
// and to handle the symbol tables. It looks like this is the only big function
// which actually handleSymbolTable. There should be a clean solution to how func
// params are handled, and how we can replace them without bringing the house down
void ArithCheck::handleFuncParams(SgFunctionParameterList* param_list) {

    // Change those parameters which are pointers, to VoidStruct. 
    // Add patch up code to the body
    #ifdef FUNCPARAM_DEBUG
    printf("handleFuncParams\n");
    #endif



    // 2. parameter list
    // Instead of using the type list, we are using the initialized name
    // list, since we would need to look up the symbol in case we 
    // chnage the arg in a defining declaration
    SgInitializedNamePtrList& args = param_list->get_args();

    SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(isSgNode(param_list)->get_parent());
    ROSE_ASSERT(fn_decl != NULL);

    SgFunctionDefinition* fndef = fn_decl->get_definition();
    bool defining_decl = (fndef != NULL);


    // If its a forward decl, nothing to do in the case of RETAIN_FUNC_PROTOS
    #ifdef RETAIN_FUNC_PROTOS
    if(!defining_decl) {
        return;
    }
    #endif

    SgFunctionParameterList* new_param_list = SB::buildFunctionParameterList();
    SgInitializedNamePtrList& new_args = new_param_list->get_args();

    #ifdef FUNCPARAM_DEBUG
    printf("created empty func params\n");
    #endif

    // Doing this to ensure that new_args has a parent... this is required when
    // printing out where the node is... 
//    new_args->set_parent(isSgNode(fn_decl));
    new_param_list->set_parent(isSgNode(fn_decl));

    #ifdef RETAIN_FUNC_PROTOS
    unsigned int arg_num = 0;
    #endif


    #ifdef FUNCPARAM_DEBUG
    printf("Traversing parameter list\n");
    #endif

    for(SgInitializedNamePtrList::iterator iter = args.begin(); iter != args.end(); iter++) {
        
        // Get the initialize name
        SgInitializedName* arg_name = *iter;
        
        #ifdef FUNCPARAM_DEBUG
        printf("arg_name: %s = %s\n", isSgNode(arg_name)->sage_class_name(), isSgNode(arg_name)->unparseToString().c_str());
        #endif
    
        // Check if this arg is a pointer
        SgType* arg_type = arg_name->get_type();

        #ifdef STRIP_TYPEDEFS
        arg_type = Util::getType(arg_type);
        #endif
        
//        printf("pointer type?");
        #ifdef FUNCPARAM_DEBUG
        printf("is qualifying type?");
        #endif
        if(Util::isQualifyingType(arg_type)) {
            #ifdef FUNCPARAM_DEBUG
            printf("Yes\n");    
            printf("arg_type: %s\n", arg_type->unparseToString().c_str());
            #endif
        

            #ifdef RETAIN_FUNC_PROTOS
            // Can't be a forward decl if we got till here...
            ROSE_ASSERT(defining_decl);
            // First up, we don't have to change the arg... it retains its original type
            // transform from: fn(ptr) to: fn(ptr) { create_entry_if_src_exists(&ptr, get_from_stack(arg_num) }
            SgInitializedName* new_arg_name = isSgInitializedName(SI::deepCopyNode(isSgNode(arg_name)));

            new_arg_name->set_parent(arg_name->get_parent());
            new_param_list->append_arg(new_arg_name);

            handleSymbolTable(arg_name, new_arg_name, false);
    
            // reading metadata sources off the stack, and insert at the address of the args
            // get src - get_from_stack(argnum)
            SgExpression* src = buildMultArgOverloadFn("get_from_stack", SB::buildExprListExp(SB::buildIntVal(arg_num)),
                                                        Util::getAddrType(), SI::getScope(param_list), fn_decl); 
            // increment argnum..
            arg_num++;

            // get the dest... 
            SgExpression* dest = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(new_arg_name)));
            
            // create_entry_if_src_exists(&ptr, get_from_stack(arg_num))
            SgExpression* overload = buildMultArgOverloadFn("create_entry_if_src_exists", SB::buildExprListExp(dest, src),
                                                        SgTypeVoid::createType(),SI::getScope(param_list) , fn_decl);
                
            // { create_entry(&ptr, get_from_stack(arg_num)) }
            SgBasicBlock* bb = Util::getBBForFn(fn_decl);
            SgStatementPtrList& stmts = bb->get_statements();

            SI::insertStatementBefore(*(stmts.begin()), SB::buildExprStatement(overload));

            #else
            // Now, we have to replace this arg with a VoidStruct version of it
            SgInitializedName* new_arg_name = createStructVariableFor(arg_name);

            // The underlying type shouldn't be typedef
            SgType* under_type = strDecl::findInUnderlyingType(new_arg_name->get_type());
            ROSE_ASSERT(!isSgTypedefType(under_type));

            #ifdef FUNCPARAM_DEBUG
            printf("new_arg_type: %s\n", new_arg_name->get_type()->unparseToString().c_str());
            #endif

//            new_args->append_arg(new_arg_name);
//            new_arg_name->set_parent(new_args);
//            new_args.push_back(new_arg_name);
            new_arg_name->set_parent(arg_name->get_parent());
            new_param_list->append_arg(new_arg_name);

            // FIXME: If its not a forward declaration, we would need to make
            // further changes -- like create a local version of the original argument
            // and then copy over the pointer data from VoidStruct and update 
            // VoidStruct metadata
            if(defining_decl) {
                #ifdef FUNCPARAM_DEBUG
                printf("Defining decl\n");
                #endif
//                createLocalVariableAndUpdateMetadata(new_arg_name, arg_name, fn_decl);
                // Replace all uses of the original arg_name with the new_arg_name
                // This creates a symbol for new_arg_name, and creates the necessary
                // mappings
                // By default, handleSymbolTable creates a mapping. But, just to be sure.. 
                // insert true at the end of the arg list -- addToMap = true
                handleSymbolTable(arg_name, new_arg_name, true);    

                ROSE_ASSERT(new_arg_name->get_symbol_from_symbol_table() != NULL);

                // Things to do: 1. create local variable
                // 2. create entry at address given by local variable.
                // 3. initialize this variable with local variable address  
                // FIXME: 4. copy over any initialization from orig variable, and updatePointer afterwards
                // also add new struct variable at the end... since return for the whole expression
                // has to be the new struct variable

                //createLocalVariableAndUpdateMetadata(new_arg_name, arg_name, fn_decl);

                // CHANGES FROM HERE ---- 

                // The changes are as follows:
                // 1. Instead of creating a local and then transfering metadata to its address in the TrackingDB,
                // we use the struct variable itself... and the address of str.ptr to keep a local copy of
                // the metadata
                // 2. We create a mapping from the original parameter (arg_name) and the str variable. Whenever
                // we encounter the use of the arg_name in handlePointerVarRefs/handleArrayVarRefs, we will
                // replace it with the str variable. No need to create a struct variable at that point. We already
                // have that.
                // 3. overloaded ops will have an update pointer at the end... in cases of ++/-- and assign. 
                // This can be left as is, since it will only cause an update of hte pointer to itself...
                // *str.addr = str.ptr is harmless, and in this case, its equal to str.ptr = str.ptr since
                // str.addr = &str.ptr !
                
                // arg_name is used to update the symbol table mapping in varRemap
                updateMetaDataAndReassignAddr(new_arg_name, arg_name, fn_decl);

            }
            #endif

        }
        #if defined(CLASS_STRUCT_COPY) && !defined(RETAIN_FUNC_PROTOS)
        else if(strDecl::isOriginalClassType(arg_type)) {
            #ifdef FUNCPARAM_DEBUG
            printf("Original Struct/Class object copy\n");    
            #endif

            // Here we have a struct var being passed to a function. We need to convert it to
            // a ref... and then create a local variable within the function, which takes the value of this
            // ref... That variable will be fed to ClassStructVarRemap so that it can replace this original 
            // variable. Also, the whole point of this operation is to copy over the internal pointers
            // within the ref to the local variable newly created... 

            // Now, replace this arg with a ref version of it...    
            SgInitializedName* new_arg_name = createRefVersionFor(arg_name);
            new_arg_name->set_parent(arg_name->get_parent());
            new_param_list->append_arg(new_arg_name);

            if(defining_decl) {
                
                #if 1
                // Symbol tables only exist for defining decls... if I am not wrong..
                // Add this new arg to the symbol table
                SgSymbol* arg_symbol = arg_name->get_symbol_from_symbol_table();
                SgVariableSymbol *arg_var_symbol = isSgVariableSymbol(arg_symbol);
                #ifdef FUNCPARAM_DEBUG
                printf("Got scope. Now, symbolTable\n");
                #endif
                #endif
    
                // This is probably required since we need to do an assignment below...
                // and the assignment depends on inserting the new_arg_name into the symbol
                // table. But why not add it to class Remap here itself?
                //addToSymbolTable(new_arg_name, isSgNode(arg_var_symbol)->get_parent()); 

                SgName name= new_arg_name->get_name();
                SgScopeStatement* scope = arg_name->get_scope();
                ROSE_ASSERT(scope == new_arg_name->get_scope());
                  // symbol table
                  ROSE_ASSERT(scope != NULL);
                  SgVariableSymbol* varSymbol = scope->lookup_variable_symbol(name);
                  if (varSymbol==NULL)
                  {
                    varSymbol = new SgVariableSymbol(new_arg_name);
                    ROSE_ASSERT(varSymbol);
                    scope->insert_symbol(name, varSymbol);
                }

                // Here, we do the assigning from ref to the local variable... and then the ClassStructVarRemap
                // and finally, the element to element copy

                SgBasicBlock* bb = Util::getBBForFn(fn_decl);
                // Assign ref to local variable - instead of handling it this way, we could
                // use handleCSAssignOverloadOps - which will call the necessary functions.
                SgName s_name(arg_name->get_name().getString() + "_local" + boost::lexical_cast<std::string>(Util::VarCounter++));
                SgVariableDeclaration* local_var = SB::buildVariableDeclaration(s_name,
                                                                            arg_name->get_type(), NULL, 
                                                                            isSgScopeStatement(bb));
                //Util::insertAtTopOfBB(bb, local_var);
                bb->prepend_statement(local_var);

                // local_var = *new_arg_name
                SgPointerDerefExp* new_arg_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(new_arg_name));
                SgAssignOp* assign_op = SB::buildAssignOp(SB::buildVarRefExp(local_var), new_arg_deref);
                SI::insertStatementAfter(local_var, SB::buildExprStatement(assign_op));

                ROSE_ASSERT(arg_var_symbol);
                SgVariableSymbol* local_var_symbol = bb->lookup_variable_symbol(Util::getNameForDecl(local_var)->get_name());
                ROSE_ASSERT(local_var_symbol);

                ClassStructVarRemap.insert(VariableSymbolMap_t::value_type(arg_var_symbol, local_var_symbol));
                handleCSAssignOverloadOps(assign_op);

                #if 0
                #if 0
                Util::insertAtTopOfBB(bb, local_var);

                // local_var = *new_arg_name
                SgExpression* new_arg_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(new_arg_name));
                SgExpression* assign_op = SB::buildAssignOp(SB::buildVarRefExp(local_var), new_arg_deref);
                SI::insertStatementAfter(local_var, SB::buildExprStatement(assign_op));
                #endif

                ROSE_ASSERT(arg_var_symbol);
                //SgSymbol* local_symbol = Util::getNameForDecl(local_var)->get_symbol_from_symbol_table();
                //SgSymbol* local_symbol = Util::getNameForDecl(local_var)->search_for_symbol_from_symbol_table();

                SgVariableSymbol* local_var_symbol = bb->lookup_variable_symbol(Util::getNameForDecl(local_var)->get_name());
                ROSE_ASSERT(local_var_symbol);

                ClassStructVarRemap.insert(VariableSymbolMap_t::value_type(arg_var_symbol, local_var_symbol));

                // FIXME: Needs to be uncommented
                // Now, call handleCSAssignOverloadOps to do the work
                //handleCSAssignOverloadOps(assign_op);
                #endif
            

            }
        }
        #endif
        else {
            #ifdef FUNCPARAM_DEBUG
            printf("No\n");
            #endif
            SgInitializedName* new_arg_name = isSgInitializedName(SI::deepCopyNode(isSgNode(arg_name)));

//            new_args->append_arg(new_arg_name);
//            new_arg_name->set_parent(new_args);
//            new_args.push_back(new_arg_name);
            new_arg_name->set_parent(arg_name->get_parent());
            new_param_list->append_arg(new_arg_name);
            if(defining_decl) {        
                handleSymbolTable(arg_name, new_arg_name, false);
            }

            // Nothing else to do in this case
        }
    }

    // We hit this parameter list before we change the function decl, so we can check if the
    // fn_decl has a OriginalClassType return type, and make the necessary changes...
    
    bool modifiedArgSize = false;
    #if defined(CLASS_STRUCT_COPY) && !defined(RETAIN_FUNC_PROTOS)
    SgType* fn_retType = fn_decl->get_type()->get_return_type();
    
    #ifdef STRIP_TYPEDEFS
    fn_retType = Util::getType(fn_retType);
    #endif
    
    if(strDecl::isOriginalClassType(fn_retType)) {
        modifiedArgSize = true;
        // We need to add a ref variable to the end of arg list...
        // If its a defining decl, we also need to replace all return statements
        // with assignments, following by return void stmts. The return type will
        // be changed to void in handleFuncDecls.
        // original code: class_type fn1(....)
        // transformed code: class_type fn1(...., class_type *retvar)


        std::string new_name_string;
        if(!defining_decl) {
            new_name_string = "";
        }
        else {
            new_name_string = "retvar" + boost::lexical_cast<std::string>(Util::VarCounter++);
        }
        SgName new_name(new_name_string);

        SgType* new_arg_type = SgPointerType::createType(fn_retType);

        // Using param_list as scope. Shoudl be okay I guess...
        SgInitializedName* new_arg_name = SB::buildInitializedName(new_name, new_arg_type, NULL);

        new_arg_name->set_scope(SI::getScope(param_list));
        
        new_arg_name->set_parent(param_list);
        new_param_list->append_arg(new_arg_name);

        if(defining_decl) {

            // Symbol tables only exist for defining decls... if I am not wrong..
            // Add this new arg to the symbol table
            SgScopeStatement* scope = SI::getScope(param_list);
            SgSymbolTable* symtab = scope->get_symbol_table();
    
            addToSymbolTable(new_arg_name, symtab); 


            // We change return stmts in handleReturnStmts. This ensures that all the modifications on
            // the expression in the ret stmt take place as usual.
            #if 0
            // Now change all the return statements to return void, and do an assignment instead.
            // Get hold of all the return stmts
            Rose_STL_Container <SgNode*> stmts = NodeQuery::SI::querySubTree(fn_decl, V_SgReturnStmt);

            Rose_STL_Container<SgNode*>::iterator i;
            for(i = stmts.begin(); i != stmts.end(); i++) {
                SgReturnStmt* cur_stmt = isSgReturnStmt(*i);
                ROSE_ASSERT(cur_stmt);
                // paranoid checks... 
                ROSE_ASSERT(isSgReturnStmt(cur_stmt));
                ROSE_ASSERT(isSgReturnStmt(cur_stmt)->get_expression()->get_type() == fn_decl->get_type()->get_return_type());

                // this copies the expression in the return statement and uses it as rhs of the assign op
                // *retvar = ret_exp;
                SgAssignOp* assign_op = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(new_arg_name)), SI::copyExpression(isSgReturnStmt(cur_stmt)->get_expression()));
                SgExprStatement* assign_stmt = SB::buildExprStatement(assign_op);
                
                SgStatement* retstmt = SB::buildReturnStmt();

                // original: return ret_exp
                // transformed: *retvar = ret_exp
                //              return
                SI::replaceStatement(cur_stmt, assign_stmt);
                SI::insertStatementAfter(assign_stmt, retstmt);
                // Need to do this in a separate function for return stmts... 
                // It can't be done here since the expression hasn't yet been modified, and whatever modifications need
                // to take place on the expression, haven't yet taken place. So, its best if we catch these return stmts
                // in TopBotTrack2 and handle the return stmts like we handle assign ops, and other such stmts.
                // Right now, the bug is: *retvar = copy_this, instead of *retvar = copy_this_local
            }
            #endif
        }
        

    }
    #endif
        


//    SI::deepDelete(isSgNode(param_list));
    fn_decl->set_parameterList(new_param_list);
    
    #ifdef FUNCPARAM_DEBUG
    printf("fn_decl\n");
    Util::printNode(fn_decl);

    printf("size: new_args: %d, args: %d\n", new_args.size(), args.size());
    #endif

    #ifdef CLASS_STRUCT_COPY
    // We shouldn't add more than 1 in any case
    if(modifiedArgSize) {
        ROSE_ASSERT(new_args.size() == args.size() + 1);
    }
    else {
        ROSE_ASSERT(new_args.size() == args.size());    
    }
    #else
    ROSE_ASSERT(new_args.size() == args.size());
    #endif
    
    #if 0
//    new_args->set_parent(isSgNode(fn_decl));
    
//    fn_decl->set_parameterList(new_args);
    fn_decl->set_parameterList(new_param_list);
    #endif

}

bool ArithCheck::ArithOrAssignOp(SgNode* node) {

    if(ArithOpWithSelfUpdate(node)) {
        return true;
    }

    // These are arith ops which don't change
    // the original variable
    switch(node->variantT()) {
    case V_SgAddOp:
    case V_SgSubtractOp: return true;
    default: return false;
    }
}

// These arith ops change the original variable as well
bool ArithCheck::ArithOpWithSelfUpdate(SgNode* node) {
    switch(node->variantT()) {
    case V_SgPlusPlusOp:
    case V_SgMinusMinusOp:
    case V_SgPlusAssignOp:
    case V_SgMinusAssignOp:
    case V_SgAssignOp: return true;
    default: return false;
    }
}

// FIXME: In order to generate the sizeof argument properly, it is
// is critical that we know the base type we are refering to -- 
// or atleast, the offset for the base size. In fact, in order to
// correctly cast any pointer to another, we would need to know the
// exact type, but since we always use VoidStructs, we don't have that
// problem at the moment.
// Since we do not have that base type available to us, at this point 
// of time, we need to find a way to atleast get the size of that
// base type, which would be used as a factor when changing the pointer
// size
// For now, we always increment by sizeof(char)
SgExpression* ArithCheck::createSizeOfArg(SgNode* node) {

    // Defaulting to sizeof(int) for now.
    SgExpression* sizeofArg = SB::buildSizeOfOp(SgTypeInt::createType());


    return sizeofArg;

}

SgVariableSymbol* ArithCheck::findSymbolForVarRef(SgVarRefExp* exp) {

    if(exp == NULL) {
        ROSE_ASSERT(0);
    }

    return exp->get_symbol();
}

SgVariableSymbol* ArithCheck::findInReverseMap(SgVariableSymbol* var_symbol) {


    VariableSymbolMap_t::iterator iter = ArithCheck::ReverseMap.find(var_symbol);
    ROSE_ASSERT(iter != ArithCheck::ReverseMap.end());
    SgVariableSymbol* str_symbol = iter->second;

    return str_symbol;
}

SgInitializedName* ArithCheck::getNameForSymbol(SgVariableSymbol* var_symbol) {

    return var_symbol->get_declaration();
}

std::string ArithCheck::getStringForFn(std::string fn_name, SgType* arg1, SgType* arg2, SgType* arg3,
                                        SgType* retType) {

    SgName arg_name;
    if(arg1 != NULL) {
        arg_name = Util::getNameForType(arg1);
        fn_name += "_" + arg_name + "_Arg_";
    }

    if(arg2 != NULL) {
        arg_name = Util::getNameForType(arg2);
        fn_name += "_" + arg_name + "_Arg_";
    }

    if(arg3 != NULL) {
        arg_name = Util::getNameForType(arg3);
        fn_name += "_" + arg_name + "_Arg_";
    }

    if(retType != NULL) {
        arg_name = Util::getNameForType(retType);
        fn_name += "_" + arg_name + "_Ret_";
    }

    return fn_name;
}

std::string ArithCheck::getStringForOp(SgNode* node, SgType* retType) {

    std::string fn_name_String = Util::getStringForVariantT(node->variantT());

    if(retType != NULL) {
        fn_name_String = retType->get_mangled() + "_Ret_" + fn_name_String;
    }
    else {
        fn_name_String = fn_name_String;
    }

    if(isSgUnaryOp(node)) {
        SgUnaryOp* uop = isSgUnaryOp(node);

        SgExpression* oper = uop->get_operand();

        fn_name_String = fn_name_String + oper->get_type()->get_mangled() + "_Arg_";
    }
    else if(isSgBinaryOp(node)) {
        SgBinaryOp* bop = isSgBinaryOp(node);

        SgExpression* lhs_oper = bop->get_lhs_operand();
        SgExpression* rhs_oper = bop->get_rhs_operand();

        fn_name_String = fn_name_String + lhs_oper->get_type()->get_mangled() 
                                        + "_Arg_" + rhs_oper->get_type()->get_mangled() + "_Arg_";
    }
    else {
        printf("Util::getStringForOp: Unknown op\n");
        ROSE_ASSERT(0);
    }

    return fn_name_String;
}

SgExpression* ArithCheck::handleNormalVarAddressOfOp(SgAddressOfOp* aop) {

    SgUnaryOp* uop =isSgUnaryOp(aop);

    SgExpression* oper = uop->get_operand();    

    ROSE_ASSERT(isSgVarRefExp(oper));    
    // The lvalue for an operand to the addressofop should be false.
    // since the expression's address is being taken, its not being changed 
    // in the process.
    ROSE_ASSERT(oper->get_lvalue() == false);

    // We need the address of this variable -- so just copy this op itself!
    //    SgExpression* aop_copy = SI::copyExpression(aop);    

    SgExpression* aop_copy = Util::createAddressOfOpFor(isSgVarRefExp(SI::copyExpression(oper)));

    // Need to cast it to address type -- and thats our first parameter
    SgExpression* lhs = Util::castToAddr(aop_copy);

    // Second parameter is the sizeof the variable type
    //SgExpression* rhs = SB::buildSizeOfOp(oper->get_type());
    // sizeof works on variable and type. sizeof(variable) should give correct
    // responses for arrays and normal stack variables.
    SgExpression* rhs = SB::buildSizeOfOp(SI::copyExpression(oper));
    #ifdef LOCK_KEY_INSERT 
    // The function call will look like... AddressOf(var, sizeof(var), lock)
    // where lock is the lock for the scope in which the variable is defined.
    // We are not inserting the "key" in the function call for two reasons:
    // 1. Its probably not necessary since this lock is still in use
    // and hence its key value cannot be changed.
    // 2. build3ArgOverloadFn is the largest number of args that we can manage now.
    // Need to write another function to manage larger nums of args
    // Get the variable declaration
    SgInitializedName* varname = isSgVarRefExp(oper)->get_symbol()->get_declaration();
    LockKeyPair lock_key = findInSLKM(varname->get_scope());
    SgExpression* third_arg = SB::buildVarRefExp(lock_key.first);
    // FIXME: The scope may not be correct if the initialized name is in parameter list
    // since the scope for parameter list is diff from the basic blcok scope (wehre the lock
    // and key are declared)
    #endif


    // Return type is the struct type of the address of return type
    // create one if necessary
    // We effectively create a struct variable of this type, which
    // will hold the address of this variable. This is similar
    // to a cast in that sense, since we create a struct variable in the
    // overloaded function --- but its unlike a cast function, in 
    // that we are providing the limits, explicitly as arguments
    // to the address of function
    // This operand is not of a pointer -- convert it to a pointer
    // before sending it in, since strDecl::getStructType expects a pointer type
    // Remove the typedefs before doing strDecl::getStructType
    #ifdef REMOVE_TYPEDEFS_FROM_VARS
    SgType* oper_base_type = Util::skip_Typedefs(oper->get_type());
    SgType* retType = strDecl::getStructType(SgPointerType::createType(oper_base_type),GEFD(SI::getEnclosingStatement(aop)) , true);
    #else
    SgType* retType = strDecl::getStructType(SgPointerType::createType(oper->get_type()),GEFD(SI::getEnclosingStatement(aop)) , true);
    #endif

//    SgExpression* overload = buildOverloadFn("AddressOf", lhs, rhs, retType, Util::getScopeForExp(aop));  
    #ifdef LOCK_KEY_INSERT
//    SgExpression* overload = build3ArgOverloadFn("AddressOf", lhs, rhs, third_arg, retType, Util::getScopeForExp(aop), GEFD(SI::getEnclosingStatement(aop)));  
    SgExpression* overload = buildMultArgOverloadFn("AddressOf", SB::buildExprListExp(lhs, rhs, third_arg), retType, Util::getScopeForExp(aop), GEFD(SI::getEnclosingStatement(aop))); 
    
    #else
//    SgExpression* overload = buildOverloadFn("AddressOf", lhs, rhs, retType, Util::getScopeForExp(aop), GEFD(SI::getEnclosingStatement(aop))); 
    SgExpression* overload = buildMultArgOverloadFn("AddressOf", SB::buildExprListExp(lhs, rhs), retType, Util::getScopeForExp(aop), GEFD(SI::getEnclosingStatement(aop)));  
    #endif

    return overload;
}

// RTED_Example:c_G_1_1_b.c
void ArithCheck::handleStructVarAddressOfOp(SgAddressOfOp* aop) {

    // FIXME: This is a little more tricky than NormalVarAddress... since here, we need to know
    // the original variable's scope. In other words, the struct represents a poitner variable,
    // and we need to know what the scope ofthat pointer variable is. The struct for the poitner
    // variable contains the lock and key for the location that the pointer points to, not the pointer
    // itself. When we are taking the address of a pointer, we need the scope in which the pointer is
    // declared, not the scope which the pointer points to.
    // Example: fn1() {
    //          int *ptr = malloc(); -- lock and key stored at &ptr would be those allocated to the malloc
    //             int **ptr2 = &ptr; -- lock and key stored at &ptr2 should be those allocated to ptr (not the malloc)

    SgExpression* oper = isSgUnaryOp(aop)->get_operand();
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(oper->get_type(), GEFD(aop)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(oper->get_type()));
    #endif

    // the address that we need is str.addr... this is the address that 
    // the pointer will point to...
    // the bounds will be size(str.ptr)
    // So, we create... from &(str)
    // new_str.ptr = str.addr
    // new_str.addr = &new_str.ptr -- we are taking the address of a local pointer here...
    // so, instead we pass a local temp variable in... 
    // addr(&temp, str), temp --- so taht we return the temp var

    SgName s_name(oper->get_type()->get_mangled() + "_Var_addr_" + boost::lexical_cast<std::string>(Util::VarCounter++)); 

    // The struct type for the return type of the address op.
    // Find the underlying type for the struct operand... and then, create a pointer of that type... and
    // a struct of that again...
    // No need to remove typedefs here, since the str type is derived from the UnderlyingType
    SgType* str_type = strDecl::findInUnderlyingType(oper->get_type());
    SgType* ptr_str_type = SgPointerType::createType(str_type);
    SgType* var_type = strDecl::getStructType(ptr_str_type, GEFD(SI::getEnclosingStatement(aop)), true);

    SgVariableDeclaration* local_var_decl = Util::createLocalVariable(s_name, var_type, NULL, Util::getScopeForExp(aop));

    SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(aop));
    SI::insertStatementBefore(stmt, local_var_decl);            

    // Now, for the addressof function...
    // addr(&temp, str)
    SgExpression* var_addr = Util::createAddressOfOpFor(SB::buildVarRefExp(local_var_decl));
    SgExpression* str = SI::copyExpression(oper);

//    SgExpression* overload = buildOverloadFn("AddressOf", var_addr, str, SgTypeVoid::createType(), Util::getScopeForExp(aop));

//    SgExpression* overload = buildOverloadFn("AddressOf", var_addr, str, SgTypeVoid::createType(), Util::getScopeForExp(aop), GEFD(SI::getEnclosingStatement(aop)));

    SgExpression* overload = buildMultArgOverloadFn("AddressOf", SB::buildExprListExp(var_addr, str), SgTypeVoid::createType(), Util::getScopeForExp(aop), GEFD(SI::getEnclosingStatement(aop)));

    // final ref to temp
    SgExpression* var_ref = SB::buildVarRefExp(local_var_decl);

    // addr(&temp, str), temp
    SgExpression* comma_op = SB::buildCommaOpExp(overload, var_ref);

    SI::replaceExpression(aop, comma_op);

    return;
}

void ArithCheck::handleAddressOfOp(SgAddressOfOp* aop) {

    SgExpression* oper = aop->get_operand();
    SgType* oper_type = oper->get_type();

    SgType* aop_type = aop->get_type();

    #ifdef STRIP_TYPEDEFS
    aop_type = Util::getType(aop_type);
    #endif

    #ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
    #endif
    
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(oper_type, GEFD(aop))) 
    #else
    if(strDecl::isValidStructType(oper_type)) 
    #endif
    {
        printf("handleAddressOfOp: valid struct type as operand???\n");
        printf("aop\n");
        Util::printNode(aop);
        ROSE_ASSERT(0);
    }
    else {
        // Address of op creates a value, so what we need to do 
        // is to get the metadata, if available, for the location
        // we are taking the address of. If that isn't available then, 
        // we create an entry
        // using a lookup ticket (we need a location in TrackingDB, and we'll
        // use a lookup ticket since there's really no address here to use)
        
        SgExprListExp* param_list;

        if(isSgArrayType(oper_type)) {
            // For arrays, it is straightforward. We'll create an entry for it
            // and use its address for the struct.addr
            // input: &array
            // output: address_of(&array, &array, sizeof(array), dummy_lock)
            // Interestingly... here, we'll simply do: output.ptr = &array
            // and output.addr = &array since the lookup for the metadata is
            // the addr of the array, and the pointer will point to the array...
            // same thing, basically!
            
            #if 0
            SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(oper->get_type()),
                                    GEFD(SI::getEnclosingStatement(aop)),
                                    true);
            #endif

                                    
            SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper)));
            SgExpression* ptr = Util::createAddressOfOpFor(SI::copyExpression(oper));
            SgExpression* size = SB::buildCastExp(SB::buildSizeOfOp(SI::copyExpression(oper)), Util::getSizeOfType(), SgCastExp::e_C_style_cast); 
            SgExpression* lock = Util::castToAddr(Util::getDummyLock());
            
            param_list = SB::buildExprListExp(addr, ptr, size, lock);            
            
        }
        else if(isSgPointerType(oper_type)) {

            // We are taking the address of a pointer variable here,
            // So, we need to generate metadata for the memory location given by
            // the pointer itself... 
            // Here, there isn't an address at which we can store this info... 
            
            SgExpression* addr = Util::castToAddr(Util::getLookupTicket());
            SgExpression* ptr = Util::createAddressOfOpFor(SI::copyExpression(oper));
            SgExpression* size = SB::buildCastExp(SB::buildSizeOfOp(SI::copyExpression(oper)), Util::getSizeOfType(),
                                                SgCastExp::e_C_style_cast);
            SgExpression* lock = Util::castToAddr(Util::getDummyLock());
            
            param_list = SB::buildExprListExp(addr, ptr, size, lock);    
        }
        else {
            // Same as pointer above... 
            // We are taking the address of a pointer variable here,
            // So, we need to generate metadata for the memory location given by
            // the pointer itself... 
            // Here, there isn't an address at which we can store this info... 
            
            SgExpression* addr = Util::castToAddr(Util::getLookupTicket());
            #ifndef COUT_FIX
            SgExpression* ptr = Util::createAddressOfOpFor(SI::copyExpression(oper));
            #else
            SgExpression* copy = SI::copyExpression(oper); 
            copy->set_lvalue(false);
            SgExpression* ptr = SB::buildAddressOfOp(copy);
            #endif

            SgExpression* size = SB::buildCastExp(SB::buildSizeOfOp(SI::copyExpression(oper)), Util::getSizeOfType(),
                                                SgCastExp::e_C_style_cast);
            SgExpression* lock = Util::castToAddr(Util::getDummyLock());
            
            param_list = SB::buildExprListExp(addr, ptr, size, lock);    
        }

        // FIXME: Util::getTypeForPntrArr may not be the right thing to use here
        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(aop_type),
                                    GEFD(SI::getEnclosingStatement(aop)),
                                    true);

        SgExpression* overload = buildMultArgOverloadFn("AddressOf", param_list,
                                                            retType, Util::getScopeForExp(aop),
                                                            GEFD(SI::getEnclosingStatement(aop)));
        
        SI::replaceExpression(aop, overload);    

        
    }

    return;
}

SgExpression* ArithCheck::createBoundsCheckFn(SgInitializedName* str_array_name, SgExpression* index_exp, SgScopeStatement* scope) {

    SgExpression* str_addr = Util::createDotExpFor(str_array_name, "addr");    
    
//    SgExpression* fncall = buildOverloadFn("bounds_check", str_addr, index_exp, SgTypeVoid::createType(), scope);
//    SgExpression* fncall = buildOverloadFn("bounds_check", str_addr, index_exp, SgTypeVoid::createType(), scope, str_array_name);
    SgExpression* fncall = buildMultArgOverloadFn("bounds_check", SB::buildExprListExp(str_addr, index_exp), SgTypeVoid::createType(), scope, str_array_name);

    return fncall;

}

SgExpression* ArithCheck::buildPntrArrRefOverload(SgPntrArrRefExp* array_ref, SgType* retType, SgExpression* new_lhs, SgExpression* new_rhs) {

    printf("SB::buildPntrArrRefExpOverload\n");
    // use the array to find the return type, scope and name
    //SgType* retType = array_ref->get_type();
    //SgType* retType = isSgArrayType(array_type)->get_base_type();
    
    printf("retType\n");
    Util::printNode(retType);

    // We could use the original expression's types to make up the
    // function name -- we retain the types from the original
    // we simply change the rhs operand to use a var ref, rather
    // than the earlier expression
    SgName fn_name(Util::getStringForOp(array_ref, retType));
    
    SgScopeStatement* scope = Util::getScopeForExp(array_ref);

    // use the new_lhs and new_rhs as operands
    #ifdef OLD_NAME_GEN
//    SgExpression* fncall = buildOverloadFn(fn_name, new_lhs, new_rhs, retType, scope);
//    SgExpression* fncall = buildOverloadFn(fn_name, new_lhs, new_rhs, retType, scope, array_ref);
    SgExpression* fncall = buildMultArgOverloadFn(fn_name, SB::buildExprListExp(new_lhs, new_rhs), retType, scope, array_ref);

    #else
//    SgExpression* fncall = buildOverloadFn("PntrArrRef", new_lhs, new_rhs, retType, scope);

//    SgExpression* fncall = buildOverloadFn("PntrArrRef", new_lhs, new_rhs, retType, scope, array_ref);
    SgExpression* fncall = buildMultArgOverloadFn("PntrArrRef", SB::buildExprListExp(new_lhs, new_rhs), retType, scope, array_ref);
    #endif

    return fncall;
}

SgExpression* ArithCheck::skipToOriginalVarRef(SgExpression* replacement) {
    
    SgExpression* orig = findInOriginalVarRef(replacement);
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(orig->get_type(), GEFD(orig))) 
    #else
    if(strDecl::isValidStructType(orig->get_type())) 
    #endif
    {
        return skipToOriginalVarRef(replacement);
    }
    return orig;
}

SgExpression* ArithCheck::createArrayBoundCheckFn(SgExpression* arg1, SgExpression* arg2,
                                                SgExpression* arg3, SgType* arg4, SgScopeStatement* scope, 
                                                SgStatement* insert,
                                                bool array) {
    
    #ifdef STRIP_TYPEDEFS
    // We don't need to the args below since they are all casted
    // before being used.
    // The return type is void. So, nothing to do.
    #endif

    // arg1 is designed to be the metadata lookup using addr... this is not required
    // for arrays...
    // arg2 is the pointer/array var
    // arg3 is the index
    // arg4 is the base type for array/pointer
    // scope gives the scope in which the function needs to be inserted
    // insert specifies where to insert the forward decl/def of the function
    // array specifies if this is an array or not. false by default

    SgExpression* overload;

    if(!array) {
        #if 0
        // For pointers
        #ifdef ARRAY_LOOKUP_USING_ADDR
        // output: array_bound_check_using_lookup(arg1, (sizeof_type)(&arg2[arg3]))
        #else
        // output: array_bound_check_using_lookup(arg1, (sizeof_type)arg2 + arg3*sizeof(arg4))
        #endif
        
        SgExpression* arg2_casted = SB::buildCastExp(arg2, Util::getSizeOfType(), SgCastExp::e_C_style_cast);
    
        #ifdef ARRAY_LOOKUP_USING_ADDR
        // (sizeof_type)(&arg2[arg3])
        
        // This one casts the addresses into their base form, and that somehow causes
        // bugs in benchmarks... case in point: art: (unsigned long)&(struct ...)(f1_layer[])
        // The (struct ...) part screws the actaul value transmitted by the address of operator
        // This might be a bug in gcc that we are hitting, or something else, but 
        // instead of that, we'll simply create address of op for this here.
        //SgExpression* addr = Util::createAddressOfOpFor(SB::buildPntrArrRefExp(arg2, arg3));
        
        SgExpression* array_ref = SB::buildPntrArrRefExp(arg2, arg3);
        array_ref->set_lvalue(false);
        SgExpression* addr = SB::buildAddressOfOp(array_ref);

        SgExpression* total = SB::buildCastExp(addr, Util::getSizeOfType(), SgCastExp::e_C_style_cast);
        #else
        // index*sizeof(base_type)
        SgExpression* sizeof_exp = SB::buildSizeOfOp(arg4);
        //SgExpression* offset = SB::buildMultiplyOp(arg3, sizeof_exp);
        // arg3 * sizeof(arg4)/sizeof(arg3)
        // We need the sizeof(arg3) since arg3 already increments by a particular
        // count -- typically integer
        SgExpression* offset = buildDivideOp(SB::buildMultiplyOp(arg3, sizeof_exp), SB::buildSizeOfOp(arg3));
    
        // (sizeof_type)param.ptr + index*sizeof(base_type)
        SgExpression* total = SB::buildAddOp(arg2_casted, offset);
        #endif
    
        // Insert the function call
        SgExprListExp* param_list = SB::buildExprListExp(arg1, total);
        #else
        SgExprListExp* param_list = Util::getArrayBoundCheckUsingLookupParams(arg1, arg2, arg3, arg4);
        #endif
        overload = buildMultArgOverloadFn("array_bound_check_using_lookup", param_list, 
                                                SgTypeVoid::createType(), scope, insert);
    }
    else {
        #if 0
        // For arrays        
        // output: array_bound_check(sizeof(arg2)/sizeof(arg4), arg3)

        SgExpression* upper_bound_bytes = SB::buildSizeOfOp(SI::copyExpression(arg2));
    
        SgExpression* upper_bound = buildDivideOp(upper_bound_bytes, SB::buildSizeOfOp(arg4));

        #ifdef INT_ARRAY_INDEX
        SgExpression* index_casted = SB::buildCastExp(arg3, 
                                        upper_bound_bytes->get_type(), SgCastExp::e_C_style_cast);
        #else
        SgExpression* index_casted = arg3;
        #endif

        SgExprListExp* param_list = SB::buildExprListExp(upper_bound, index_casted);
        #else
        SgExprListExp* param_list = Util::getArrayBoundCheckParams(arg1, arg2, arg3, arg4);
        #endif
    
        overload = buildMultArgOverloadFn("array_bound_check", param_list, SgTypeVoid::createType(), 
                                    scope, insert);
    }

    return overload;
}

void ArithCheck::handlePntrArrRefExp(SgPntrArrRefExp* array_ref) {

    #ifdef PNTRARRREF_DEBUG
    printf("handlePntrArrRefExp... \n");
    #endif

    // lhs could be a var ref here.
    // 1. Check if its part of the params
    // 2. If so, replace the var ref with an array bound
    // check using the struct from the params
    // 3. Else, do a normal array bound check for it

    // In both cases, we need to copy over the rhs (index)
    // to a local variable.
    // Here, we first create that variable
    SgExpression* rhs = isSgPntrArrRefExp(array_ref)->get_rhs_operand();
    SgType* rhs_type = rhs->get_type();

    #ifdef STRIP_TYPEDEFS
    rhs_type = Util::getType(rhs_type);
    #endif

    SgName s_name(rhs_type->get_mangled() + "_index_" + boost::lexical_cast<std::string>(Util::VarCounter++)); 

    SgVariableDeclaration* array_index = Util::createLocalVariable(s_name, rhs_type, NULL, Util::getScopeForExp(isSgExpression(array_ref)));
    
    #ifdef PNTRARRREF_DEBUG
    SgStatement* encl_stmt = SI::getEnclosingStatement(array_ref);
    printf("encl_stmt\n");
    Util::printNode(encl_stmt);
    #endif
                
    SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(array_ref));
    SI::insertStatementBefore(stmt, array_index);            

    SgExpression* comma_op;
    SgExpression* overload;
    SgExpression* pntr_arr_ref;

    // array_index = rhs
    SgExpression* assign_op = SB::buildAssignOp(SB::buildVarRefExp(array_index), SI::copyExpression(rhs));

    SgExpression* lhs = array_ref->get_lhs_operand();
    SgType* lhs_type = lhs->get_type();

    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif

    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(lhs_type, GEFD(lhs))) 
    #else
    if(strDecl::isValidStructType(lhs_type)) 
    #endif
    {
        // the pntr arr ref looks like: str[index]
        // We should change that to array_index = index; Ptr_check(str, array_index)[array_index]
        // where Ptr_check does a array bound check using lookup, and then returns str.ptr
        // Maybe we should also do a temporal check in Ptr_check

        // We should insert the array_index in a *statement* before the pntr arr ref
        // This makes sure that we don't replace an operation with a comma op. With this we can make
        // sure that we don't have to handle comma ops in the next higher operation... hence it doesn't
        // complicate the hiearchy
        
        #ifdef STRIP_TYPEDEFS
        // No need to check if lhs/rhs is typedef type. lhs is ValidStruct
        // and rhs can't be typedef type (thanks to the rhs_type above)
        #endif

        SgExprListExp* param_list = SB::buildExprListExp(SI::copyExpression(lhs), 
                            SB::buildVarRefExp(array_index));

        SgExpression* overload = buildMultArgOverloadFn("ptr_check", param_list, 
                                strDecl::findInUnderlyingType(lhs_type),
                                SI::getScope(array_ref),
                                GEFD(SI::getEnclosingStatement(array_ref)));

        
        #ifdef ARRAY_CHECK_PLACEMENT
        // Since we could be changing array_ref itself in the function
        // we can't do this before the replaceExpression below... we
        // need to atleast return the new array_ref to use below
        // or, do this after that...
        insertCheckCodeForArray(array_ref, assign_op);
        #else
        // insert the assign op before the current statement.
        SgExprStatement* assign_stmt = SB::buildExprStatement(assign_op);

        SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(array_ref));
        SI::insertStatementBefore(stmt, assign_stmt);            
        #endif


        // build a new pntr arr ref to take the place of the original one
        SgExpression* pntr_arr_ref = SB::buildPntrArrRefExp(overload, SB::buildVarRefExp(array_index));
        SI::replaceExpression(array_ref, pntr_arr_ref);
    }
    else {
        // Case of var ref...
        if(isSgVarRefExp(lhs)) {
            SgVarRefExp* lhs_var_ref = isSgVarRefExp(lhs);

            #ifndef RETAIN_FUNC_PROTOS
            // check in varRemap
            SgVariableSymbol* param = checkInVarRemap(lhs_var_ref->get_symbol());
            if(param != NULL) {
                // This was a param, which has been converted to struct
                // This is a pointer. Arrays don't go through fncalls
                // directly. They need to be inserted within structs
                ROSE_ASSERT(isSgPointerType(lhs_type));

                // input: ptr[index]
                // output: array_bound_check_using_lookup(param.addr, param.ptr + index*sizeof(base_type)), param.ptr[index]

                // 1. Do an array_bound_check_using_lookup here, and then
                // 2. insert an actual pntr arr ref

                // 1. Array bound check using lookup


                // param.addr
                SgExpression* param_addr = Util::castToAddr(Util::createDotExpFor(param->get_declaration(), "addr"));

                // (sizeof_type)param.ptr
                SgExpression* param_ptr = Util::createDotExpFor(param->get_declaration(), "ptr");
                //            SgExpression* param_ptr_casted = SB::buildCastExp(param_ptr, Util::getSizeOfType(), SgCastExp::e_C_style_cast);

                // index*sizeof(base_type)
                SgType* base_type = isSgPointerType(param_ptr->get_type())->get_base_type();
                //            SgExpression* sizeof_exp = SB::buildSizeOfOp(base_type);
                //            SgExpression* offset = SB::buildMultiplyOp(SB::buildVarRefExp(array_index), sizeof_exp);

                // By default, the function assumes pointers... which is true in this case.
                overload = createArrayBoundCheckFn(param_addr, param_ptr, SB::buildVarRefExp(array_index),
                        base_type, SI::getScope(array_ref), 
                        GEFD(SI::getEnclosingStatement(array_ref)));


#if 0
                // (sizeof_type)param.ptr + index*sizeof(base_type)
                SgExpression* total = SB::buildAddOp(param_ptr_casted, offset);

                // Insert the function call

                SgExprListExp* param_list = SB::buildExprListExp(param_addr, total);

                SgExpression* overload = buildMultArgOverloadFn("array_bound_check_using_lookup", param_list, 
                        SgTypeVoid::createType(), SI::getScope(array_ref), 
                        GEFD(SI::getEnclosingStatement(array_ref)));
#endif

                pntr_arr_ref = SB::buildPntrArrRefExp(SI::copyExpression(param_ptr), SB::buildVarRefExp(array_index));

                //comma_op = SB::buildCommaOpExp(overload, pntr_arr_ref);
            }
            else {
            #endif
                // This is the case where the lhs is not from the param list
                // So, we build an array_bound_check_using_lookup if its a pointer
                // or just an array_bound_check if its an array
                
                #ifdef USE_ARRAY_LOOKUP_FOR_ARGV
                if(strcmp(isSgVarRefExp(lhs)->get_symbol()->get_name().str(), "argv") == 0) {
                    SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
                    
                    SgType* base_type = isSgArrayType(lhs_type)->get_base_type();
                    
                    #ifdef STRIP_TYPEDEFS
                    base_type = Util::getType(base_type);
                    #endif

                    overload = createArrayBoundCheckFn(ptr_addr, SI::copyExpression(lhs), SB::buildVarRefExp(array_index),
                            base_type,
                            SI::getScope(array_ref),
                            GEFD(SI::getEnclosingStatement(array_ref)));

                }
                else if(isSgArrayType(lhs_type)) 
                #else
                if(isSgArrayType(lhs_type)) 
                #endif
                {

                    SgType* base_type = isSgArrayType(lhs_type)->get_base_type();

                    #ifdef STRIP_TYPEDEFS // Inserted with ENUM_TO_INT_HACK
                    base_type = Util::getType(base_type);
                    #endif


                    overload = createArrayBoundCheckFn(NULL, SI::copyExpression(lhs),
                            SB::buildVarRefExp(array_index), 
                            base_type,
                            SI::getScope(array_ref),
                            GEFD(SI::getEnclosingStatement(array_ref)),
                            true);
                }
                else {
                    ROSE_ASSERT(isSgPointerType(lhs_type));

                    SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));

                    SgType* base_type = isSgPointerType(lhs_type)->get_base_type();

                    #ifdef STRIP_TYPEDEFS // Inserted with ENUM_TO_INT_HACK
                    base_type = Util::getType(base_type);
                    #endif

                    overload = createArrayBoundCheckFn(ptr_addr, SI::copyExpression(lhs), SB::buildVarRefExp(array_index),
                            base_type,
                            SI::getScope(array_ref),
                            GEFD(SI::getEnclosingStatement(array_ref)));
                }

                pntr_arr_ref = SB::buildPntrArrRefExp(SI::copyExpression(lhs), SB::buildVarRefExp(array_index));
                //comma_op = SB::buildCommaOpExp(overload, pntr_arr_ref);
            #ifndef RETAIN_FUNC_PROTOS
            }
            #endif

            // We insert the array_bound_check and assignment as a previous statement, rather than
            // putting it in a comma op with the pntr arr ref. This should work in most cases.

            SgExprStatement* assign_ovl = SB::buildExprStatement(SB::buildCommaOpExp(assign_op, overload));
            SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(array_ref));
            SI::insertStatementBefore(stmt, assign_ovl);            

            //SgExpression* comma_op2 = SB::buildCommaOpExp(assign_op, comma_op);

            //SI::replaceExpression(array_ref, comma_op2);
            // Only diff between original array_ref and the new pntr arr ref is that the
            // array index is the local variable, rather than the original expression
            SI::replaceExpression(array_ref, pntr_arr_ref);

        }
        else {
            // This is the case of a dot/arrow (with var oper) expression on the lhs.

            if(isSgArrayType(lhs_type)) {
                overload = createArrayBoundCheckFn(NULL, SI::copyExpression(lhs),
                            SB::buildVarRefExp(array_index),
                            isSgArrayType(lhs_type)->get_base_type(),
                            SI::getScope(array_ref),
                            GEFD(SI::getEnclosingStatement(array_ref)),
                            true);
            }
            else {
                ROSE_ASSERT(isSgPointerType(lhs_type));

                SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));

                overload = createArrayBoundCheckFn(ptr_addr, SI::copyExpression(lhs), SB::buildVarRefExp(array_index),
                            isSgPointerType(lhs_type)->get_base_type(),
                            SI::getScope(array_ref),
                            GEFD(SI::getEnclosingStatement(array_ref)));
            }

            pntr_arr_ref = SB::buildPntrArrRefExp(SI::copyExpression(lhs), SB::buildVarRefExp(array_index));

            SgExprStatement* assign_ovl = SB::buildExprStatement(SB::buildCommaOpExp(assign_op, overload));
            SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(array_ref));
            SI::insertStatementBefore(stmt, assign_ovl);

            SI::replaceExpression(array_ref, pntr_arr_ref);

        }

    }
}




SgExpression* ArithCheck::handleCastExp(SgCastExp* cast_exp) {
    
    #ifdef CAST_DEBUG
    printf("handleCastExp\n");
    #endif

    // This function handles four different types of casts
    // 1. struct to struct    - already implemented
    // 2. value_exp to struct - already implemented
    // 3. qual_type to struct - NEW!
    // 4. struct to value     - already implemented, possibly broken!
    // 5. exp to struct - This handles the case where the expression is not a value
    // exp but rather a computed one.

    SgType* cast_type = cast_exp->get_type();
    #ifdef STRIP_TYPEDEFS
    cast_type = Util::getType(cast_type);
    #endif

    SgExpression* oper = cast_exp->get_operand();
    SgType* oper_type = oper->get_type();

    #ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
    #endif

    SgExpression* oper_copy = SI::copyExpression(oper);

    #ifdef STRIP_TYPEDEFS
    oper_copy = Util::castAwayTypedef(oper_copy);
    #endif

    oper_copy->set_lvalue(false);

    SgExprListExp* param_list;
    SgType* retType;

    // 1. struct to struct
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(oper_type, GEFD(oper)) &&
        Util::isQualifyingType(cast_type))
    #else
    if(strDecl::isValidStructType(oper_type) &&
        Util::isQualifyingType(cast_type))
    #endif
         
    {
        
        param_list = SB::buildExprListExp(oper_copy);
        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_type),
                        GEFD(SI::getEnclosingStatement(cast_exp)),
                        true);
    }
    // 2. value_exp to struct
    else if(isSgValueExp(oper) && Util::isQualifyingType(cast_type)) {
        
        // Lets do something slightly different for a value cast... instead
        // of just converted the value to a pointer, lets do how a traditional
        // compiler would handle... capture the value in a global variable
        // and then we will create a struct out of it.
        // input: cast("abc")
        // output: global_var = "abc"
        // output: cast(&global_var, &global_ptr, sizeof(global_var), dummy_lock)
        // Using global_ptr to get a global address

        std::string global_var_name_string = "global_var" + boost::lexical_cast<std::string>(Util::VarCounter++);
        #ifdef FILE_SPECIFIC_VARNAMES
        global_var_name_string = global_var_name_string + "_" + boost::lexical_cast<std::string>(oper->get_file_info()->get_line()) 
                                                + "_" + boost::lexical_cast<std::string>(oper->get_file_info()->get_col());
        #endif

        std::string global_ptr_name_string = "global_ptr" + boost::lexical_cast<std::string>(Util::VarCounter++);
        #ifdef FILE_SPECIFIC_VARNAMES
        global_ptr_name_string = global_ptr_name_string + "_" + boost::lexical_cast<std::string>(oper->get_file_info()->get_line()) 
                                                + "_" + boost::lexical_cast<std::string>(oper->get_file_info()->get_col());
        #endif
        
        #ifdef CPP_SCOPE_MGMT
        // This could insert the variable as a static variable within a class... lets not do that... 
        // instead of using the "SI::getGlobalScope()".. lets do it slightly differently, to make sure
        // that the variable is inserted in the global scope... in all cases (even member func cases)
        SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(GEFD(SI::getEnclosingStatement(cast_exp)));
        SgStatement* pos;
        // Now, if its a member func decl, find the location before the class..
        if(SgMemberFunctionDeclaration* mem = isSgMemberFunctionDeclaration(fn_decl)) {
            SgClassDefinition* def = isSgClassDefinition(mem->get_scope());    
            pos = isSgStatement(isSgClassDeclaration(def->get_parent()));
        }
        else {
            pos = fn_decl;
        }
        ROSE_ASSERT(isSgGlobal(pos->get_scope()));

        SgVariableDeclaration* global_var = SB::buildVariableDeclaration(
                                            SgName(global_var_name_string),
                                            oper_type,
                                            SB::buildAssignInitializer(SI::copyExpression(oper_copy), oper_type),
                                            pos->get_scope());    
        SI::insertStatementBefore(pos, global_var);

        #else
        SgVariableDeclaration* global_var = SB::buildVariableDeclaration(
                                            SgName(global_var_name_string),
                                            oper_type,
                                            SB::buildAssignInitializer(SI::copyExpression(oper_copy), oper_type),
                                            SI::getGlobalScope(cast_exp));
        
        SI::insertStatementBefore(GEFD(SI::getEnclosingStatement(cast_exp)), global_var);
        #endif

        // Added to make sure that each file has its own set of generated global variables
        Util::makeDeclStatic(global_var);

        // We could've used a lookup ticket... right?
        #ifdef INSERT_GLOBAL_PTR_FOR_ADDR
        SgVariableDeclaration* global_ptr = SB::buildVariableDeclaration(
                                            SgName(global_ptr_name_string),
                                            oper_type,
                                            NULL,
                                            SI::getGlobalScope(cast_exp));

        SI::insertStatementBefore(GEFD(SI::getEnclosingStatement(cast_exp)), global_ptr);
        #endif


        // global_var
        SgExpression* global_var_casted;

        // This handles cases where NULL is converted to a void pointer
        if(Util::getNameForDecl(global_var)->get_type() != cast_type) {
            global_var_casted = SB::buildCastExp(SB::buildVarRefExp(global_var), cast_type, SgCastExp::e_C_style_cast);
        }
        else {
            global_var_casted = SB::buildVarRefExp(global_var);
        }

        // &global_var
        SgExpression* addr_global_var = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(global_var)));

        #ifdef INSERT_GLOBAL_PTR_FOR_ADDR
        // &global_ptr
        SgExpression* addr_global_ptr = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(global_ptr)));
        #endif
        
        // sizeof(global_var)
        SgExpression* sizeof_global_var = SB::buildCastExp(SB::buildSizeOfOp(SB::buildVarRefExp(global_var)),
                                                        Util::getSizeOfType(), SgCastExp::e_C_style_cast);

        // lock
        SgExpression* lock = Util::castToAddr(Util::getDummyLock());

        #ifdef INSERT_GLOBAL_PTR_FOR_ADDR
        // the global ptr supplies the address -- using address of
        // the global variable supplies the pointer -- using address of
        // the size is given by sizeof(global_var)
        // lock is a dummy lock for now
        param_list = SB::buildExprListExp(addr_global_var, addr_global_ptr, sizeof_global_var, lock);
        #else
        param_list = SB::buildExprListExp(global_var_casted, addr_global_var, sizeof_global_var, lock);
        #endif

        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_type),
                        GEFD(SI::getEnclosingStatement(cast_exp)),
                        true);

        #if 0
        param_list = SB::buildExprListExp(oper_copy);
        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_exp->get_type()),
                        GEFD(SI::getEnclosingStatement(cast_exp)),
                        true);
        #endif
    }
    // 3. qual_type to struct - NEW!
    else if(Util::isQualifyingType(oper_type) && 
            Util::isQualifyingType(cast_type)) {
        

        #ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the arg, if its
        // array type.
        if(isSgArrayType(oper_type)) {
            insertCreateEntry(SI::copyExpression(oper_copy), Util::getScopeForExp(cast_exp),
                        SI::getEnclosingStatement(cast_exp));
        }
        #endif


        SgExpression* ptr = oper_copy;
        SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper_copy)));

        param_list = SB::buildExprListExp(ptr, addr);
        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_type),
                        GEFD(SI::getEnclosingStatement(cast_exp)),
                        true);
    }
    // 4. struct to value
    #ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(oper_type, GEFD(oper)) &&
            !(Util::isQualifyingType(cast_type) ||
              strDecl::isValidStructType(cast_type, GEFD(oper)))) 
    #else
    else if(strDecl::isValidStructType(oper_type) &&
            !(Util::isQualifyingType(cast_type) ||
              strDecl::isValidStructType(cast_type)))
    #endif
    {
        param_list = SB::buildExprListExp(oper_copy);
        retType = cast_type;
    }
    // 5. primitive/original class types to qualifying type -- unlike value exps above
    // these are not known statically, they are computed at runtime (like ret vals from fns)
    else if(!Util::isQualifyingType(oper_type) && Util::isQualifyingType(cast_type)) {
        // For this case, we don't have any metadata... so, we create a struct... and we don't
        // create an entry in the TrackingDB. This will make sure that any check_entry occuring
        // by dereferencing this value will fail...
        // If we wanted to allow a deref, we could always use a dummy entry instead.

        // create_struct((ptr_type) oper_copy, (addr_type)Util::getLookupTicket())
        SgExpression* casted_oper_copy = SB::buildCastExp(oper_copy, cast_type, 
                                                        SgCastExp::e_C_style_cast);
        SgExpression* casted_lookup_ticket = Util::castToAddr(Util::getLookupTicket());

        param_list = SB::buildExprListExp(casted_oper_copy, casted_lookup_ticket);
        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_type),
                        GEFD(SI::getEnclosingStatement(cast_exp)),
                        true);
        
        SgExpression* overload = buildMultArgOverloadFn("create_struct", param_list, retType,
                                                    Util::getScopeForExp(cast_exp),
                                                    GEFD(SI::getEnclosingStatement(cast_exp)));
        SI::replaceExpression(cast_exp, overload);
        return overload;
    }
    else {
        printf("Unsupported case in Cast Exp\n");
        printf("cast_exp");
        Util::printNode(cast_exp);
        printf("cast_exp->get_type(): %s", cast_type->unparseToString().c_str());
        printf("cast_exp->get_operand()->get_type(): %s", 
                oper_type->unparseToString().c_str());
        ROSE_ASSERT(0);
    }

    SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(cast_exp->variantT())),
                                                    param_list,
                                                    retType,
                                                    Util::getScopeForExp(cast_exp),
                                                    GEFD(SI::getEnclosingStatement(cast_exp)));
    
    SI::replaceExpression(cast_exp, overload);

    return overload;
}

SgExpression* ArithCheck::updatePointerUsingAddr(SgVarRefExp* var_ref) {

    SgVarRefExp* var_ref_copy = isSgVarRefExp(SI::copyExpression(var_ref));

    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(var_ref_copy->get_type(), GEFD(var_ref)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(var_ref_copy->get_type()));
    #endif
    // Basically, this function uses the address in the struct, to update
    // the original pointer.

    SgVariableSymbol* var_symbol = var_ref_copy->get_symbol();
    
    // str.ptr
    SgExpression* ptr_exp = Util::createDotExpFor(getNameForSymbol(var_symbol), "ptr");
    
    // str.addr
    SgExpression* addr_exp = Util::createDotExpFor(getNameForSymbol(var_symbol), "addr");
    // Cast the addr to the correct type -- i.e pointer of the ptr type
    //SgExpression* addr_dbl_ptr_exp = SB::buildCastExp(addr_exp, SgPointerType::createType(ptr_exp->get_type()), SgCastExp::e_reinterpret_cast);
    SgExpression* addr_dbl_ptr_exp = SB::buildCastExp(addr_exp, SgPointerType::createType(ptr_exp->get_type()), CAST_TYPE);


    // First deref the addr
    // *(reinterpret_cast<ptr(ptr_type)>str.addr)
    SgExpression* deref_exp = SB::buildPointerDerefExp(addr_dbl_ptr_exp);
    // This final cast seems to create the error "invalid lvalue in assignment"
//    SgExpression* casted_addr = SB::buildCastExp(deref_exp, getUnderlyingTypeForStruct(var_ref->get_type()), SgCastExp::e_C_style_cast);


    // *(reinterpret_cast<ptr(ptr_type)>str.addr) = str.ptr
//    SgExpression* update = SB::buildAssignOp(casted_addr, ptr_exp);
    SgExpression* update = SB::buildAssignOp(deref_exp, ptr_exp);

    return update;

}

void ArithCheck::handleBinaryOverloadOps(SgNode* node) {
    
    if(isSgAssignOp(node)) {
        handleAssignOverloadOps(node);
    }
    else if(isSgAddOp(node) || isSgSubtractOp(node)) {
        handleAddSubtractOverloadOps(node);
    }

}

void ArithCheck::handleAddSubtractOverloadOps(SgNode* node) {
    // Cases:
    // 1. var +/- exp
    // 2. struct +/- exp
    // 3. *ptr +/- exp

    SgBinaryOp* bop = isSgBinaryOp(node);
    SgType* bop_type = bop->get_type();
    #ifdef STRIP_TYPEDEFS
    bop_type = Util::getType(bop_type);
    #endif

    
    SgExpression* bop_lhs = bop->get_lhs_operand();
    SgType* bop_lhs_type = bop_lhs->get_type();
    #ifdef STRIP_TYPEDEFS
    bop_lhs_type = Util::getType(bop_lhs_type);
    #endif

    SgExpression* bop_rhs = bop->get_rhs_operand();
    SgType* bop_rhs_type = bop_rhs->get_type();
    #ifdef STRIP_TYPEDEFS
    bop_rhs_type = Util::getType(bop_rhs_type);
    #endif
    


    if(isSgArrayType(bop_lhs_type)) {
        insertCreateEntry(SI::copyExpression(bop_lhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
    }

    if(isSgArrayType(bop_rhs_type)) {
        insertCreateEntry(SI::copyExpression(bop_rhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
    }

    
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(bop_lhs_type, GEFD(bop_lhs)) ||
                    strDecl::isValidStructType(bop_rhs_type, GEFD(bop_rhs))) 
    #else
    if(strDecl::isValidStructType(bop_lhs_type) ||
                    strDecl::isValidStructType(bop_rhs_type)) 
    #endif
    {

        // input: struct +/- exp
        // output: add/sub(struct, exp);

        SgExpression* lhs;
        SgExpression* rhs;

        SgExpression* lhs_copy = SI::copyExpression(bop_lhs);
        #ifdef STRIP_TYPEDEFS
        lhs_copy = Util::castAwayTypedef(lhs_copy);
        #endif

        SgExpression* rhs_copy = SI::copyExpression(bop_rhs);
        #ifdef STRIP_TYPEDEFS
        rhs_copy = Util::castAwayTypedef(rhs_copy);
        #endif

        SgExprListExp* param_list = SB::buildExprListExp(lhs_copy, rhs_copy);
        SgType* retType;

        #ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(bop_lhs_type, GEFD(bop_lhs))) 
        #else
        if(strDecl::isValidStructType(bop_lhs_type)) 
        #endif
        {
            retType = bop_lhs_type;
        }
        else {
            retType = bop_rhs_type;
        }

        SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(node->variantT())), 
                                param_list, 
                                retType, Util::getScopeForExp(bop), 
                                GEFD(SI::getEnclosingStatement(bop)));

        SI::replaceExpression(bop, overload);


    }
    else if(Util::isVarRefOrDotArrow(bop_lhs) ||
        Util::isVarRefOrDotArrow(bop_rhs)) {
        // input: var +/- exp
        // output: create_struct(var +/- exp, &var)
        
        #if 0
        Util::printNode(bop);
        Util::printNode(bop->get_type());
        #endif

        SgExpression* arg1;
        if(isSgArrayType(bop_type)) {
            // this needs to be converted to the corresponding pointer type
            arg1 = SB::buildCastExp(SI::copyExpression(bop), Util::getTypeForPntrArr(bop_type), 
                    SgCastExp::e_C_style_cast);
        }
        else {
            arg1 = SI::copyExpression(bop);
        }

        SgExpression* arg2;
        if(Util::isVarRefOrDotArrow(bop_lhs)) {
            arg2 = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(bop_lhs)));

            // done above
            #if 0
            #ifdef ARRAY_CREATE_ENTRY
            // Also, we need to insert create_entry for the lhs, if its
            // array type.
            if(isSgArrayType(bop_lhs->get_type())) {
                insertCreateEntry(SI::copyExpression(bop_lhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
            }
            #endif
            #endif

        }
        else {
            arg2 = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(bop_rhs)));

            // Done above
            #if 0
            #ifdef ARRAY_CREATE_ENTRY
            // Also, we need to insert create_entry for the rhs, if its
            // array type.
            if(isSgArrayType(bop_rhs->get_type())) {
                insertCreateEntry(SI::copyExpression(bop_rhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
            }
            #endif
            #endif

        }

        SgExprListExp* param_list = SB::buildExprListExp(arg1, arg2);
        
        #if 0
        printf("typename: %s\n", Util::getNameForType(bop->get_type()).getString().c_str());
        printf("PntrArr - typename: %s\n", Util::getNameForType(Util::getTypeForPntrArr(bop->get_type())).getString().c_str());
        #endif

        
        #if 0
        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(bop_lhs->get_type()),
                                    GEFD(SI::getEnclosingStatement(bop)),
                                    true);
        #endif
        
        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(bop_type),
                                    GEFD(SI::getEnclosingStatement(bop)),
                                    true);

        #if 0
        printf("ret typename: %s\n", Util::getNameForType(retType).getString().c_str());
        #endif

        SgExpression* overload = buildMultArgOverloadFn("create_struct", param_list,
                                                    retType, Util::getScopeForExp(bop),
                                                    GEFD(SI::getEnclosingStatement(bop)));
        
        SI::replaceExpression(bop, overload);
    }
    #ifdef DOUBLE_DIM_ENABLED
    else if(isSgPointerDerefExp(bop_lhs) || isSgPointerDerefExp(bop_rhs)) {
        // input: *ptr +/- exp
        // output: add/sub(ptr, exp)
        
        SgExprListExp* p_list;

        // array create entry done above

        if(isSgPointerDerefExp(bop_lhs)) {

            SgExpression* arg1 = SI::copyExpression(Util::stripDeref(bop_lhs));
            #ifdef STRIP_TYPEDEFS
            arg1 = Util::castAwayTypedef(arg1);
            #endif

            SgExpression* arg2 = SI::copyExpression(bop_rhs);
            #ifdef STRIP_TYPEDEFS
            arg2 = Util::castAwayTypedef(arg2);
            #endif

            p_list = SB::buildExprListExp(arg1, arg2);
        }
        else {
            
            SgExpression* arg1 = SI::copyExpression(bop_lhs);
            #ifdef STRIP_TYPEDEFS
            arg1 = Util::castAwayTypedef(arg1);
            #endif

            SgExpression* arg2 = SI::copyExpression(Util::stripDeref(bop_rhs));
            #ifdef STRIP_TYPEDEFS
            arg2 = Util::castAwayTypedef(arg2);
            #endif


            p_list = SB::buildExprListExp(arg1, arg2);
        }

        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(bop_type),
                                    GEFD(SI::getEnclosingStatement(bop)),
                                    true);

        std::string op_name = Util::getStringForVariantT(node->variantT()) ;

        SgExpression* overload = buildMultArgOverloadFn(SgName(op_name + "_deref"),
                                                p_list,
                                                retType, Util::getScopeForExp(bop),
                                                GEFD(SI::getEnclosingStatement(bop)));

        SI::replaceExpression(bop, overload);

    }
    #endif
    else {
        printf("Case not supported in handleAddSubtractOverloadOps\n");
        printf("node\n");
        Util::printNode(node);
        ROSE_ASSERT(0);
    }
}

SgExpression* ArithCheck::stripDeref(SgExpression* exp) {

    ROSE_ASSERT(isSgPointerDerefExp(exp));
    SgExpression* oper = isSgPointerDerefExp(exp)->get_operand();
    #ifdef STRIP_TYPEDEFS
    oper = Util::castAwayTypedef(oper);
    #endif
    return oper;
}

SgExpression* ArithCheck::handleAssignOverloadOps(SgNode* node) {

    #ifdef BINARY_OVLOP_DEBUG
    printf("handleBinaryOverloadOps\n");
    #endif

    SgBinaryOp* bop = isSgBinaryOp(node);

    // += and -= are broken down to assign and +/-
    // Cases: 
    // 1. var_ref/dotarrow = var_ref/dotarrow
    // 2. struct  = var_ref/dotarrow
    // 3. var_ref/dotarrow = struct
    // 4. struct = struct
    // 5. *ptr = struct
    // 6. struct = *ptr
    // 7. var_ref/dotarrow = *ptr
    // 8. *ptr = var_ref/dotarrow
    // 9. *ptr = *ptr2

    SgExpression* lhs = bop->get_lhs_operand();
    SgType* lhs_type = lhs->get_type();
    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif

    SgExpression* rhs = bop->get_rhs_operand();
    
    #ifdef CONVERT_ARG_STRUCTS_TO_PTR
    // Convert the lhs from struct of a variable (typically, a variable in the
    // param list) to a dot exp so that its updated accordingly.
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(lhs_type, GEFD(lhs)) && isSgVarRefExp(lhs)) 
    #else
    if(strDecl::isValidStructType(lhs_type) && isSgVarRefExp(lhs)) 
    #endif
    {
        SgInitializedName* var_name = isSgVarRefExp(lhs)->get_symbol()->get_declaration();
        SgExpression* dot_exp = Util::createDotExpFor(var_name, "ptr");
        SI::replaceExpression(lhs, dot_exp);
        ROSE_ASSERT(bop->get_lhs_operand() == dot_exp);
        return handleAssignOverloadOps(bop);
    }
    #endif

    


    // If the rhs is a value exp, cast it to the 
    // qualifying type on the lhs
    if(isSgValueExp(rhs)) {
        
        SgType* lhs_type_local;
        SgCastExp* casted_rhs;
        if(Util::isQualifyingType(lhs_type)) {
            lhs_type_local = lhs_type;
            // Nothing to do
        }
        #ifdef CPP_SCOPE_MGMT
        else if(strDecl::isValidStructType(lhs_type, GEFD(lhs))) 
        #else
        else if(strDecl::isValidStructType(lhs_type)) 
        #endif
        {
            lhs_type_local = strDecl::findInUnderlyingType(lhs_type);
        }
        else {
            printf("AssignOverloadOps: lhs is not qual/valid struct type\n");
            printf("lhs\n");
            Util::printNode(lhs);
        }
        
        casted_rhs = SB::buildCastExp(SI::copyExpression(rhs), lhs_type_local, SgCastExp::e_C_style_cast);    

        // replace rhs with the casted_rhs
        SI::replaceExpression(rhs, casted_rhs);

        // Now, call the handleCastExp function with the casted_rhs
        SgExpression* transf_rhs = handleCastExp(casted_rhs);

        // Now, assign transf_rhs to rhs... 
        rhs = transf_rhs;
            
    }
        

    SgExprListExp* param_list;
    SgExpression* overload;
    SgType* retType;

    // We should get the rhs_type here since we could have
    // changed the rhs to transf_rhs above
    SgType* rhs_type = rhs->get_type();
    #ifdef STRIP_TYPEDEFS
    rhs_type = Util::getType(rhs_type);
    #endif

    // Create entry for arrays... on rhs
    // lhs doesn't need metadata entry since it will be overwritten
    // by the metadata from rhs anyway.
    #ifdef ARRAY_CREATE_ENTRY
    // Also, we need to insert create_entry for the RHS, if its
    // array type.
    if(isSgArrayType(rhs_type)) {
        insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
                    SI::getEnclosingStatement(node));
    }                        
    #endif

    SgName fn_name;
    
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(lhs_type, GEFD(lhs)) && strDecl::isValidStructType(rhs_type, GEFD(rhs))) 
    #else
    if(strDecl::isValidStructType(lhs_type) && strDecl::isValidStructType(rhs_type)) 
    #endif
    {
        // This is the struct = struct case
        // input: struct_L = struct_R
        // output: assign_copy(struct_L, struct_R)
        
        SgExpression* lhs_copy = SI::copyExpression(lhs);
        SgExpression* rhs_copy = SI::copyExpression(rhs);

        param_list = SB::buildExprListExp(lhs_copy, rhs_copy);

        retType = lhs_type;    
    }
    #ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(rhs_type, GEFD(rhs)) && Util::isVarRefOrDotArrow(lhs)) 
    #else
    else if(strDecl::isValidStructType(rhs_type) && Util::isVarRefOrDotArrow(lhs)) 
    #endif
    {
        // This is the var_ref/dotarrow = struct case

        // input: var_ref_L = struct_R
        // output: assign_copy(&var_ref_L, struct_R)

        //SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
        SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
        SgExpression* rhs_copy = SI::copyExpression(rhs);

        param_list = SB::buildExprListExp(lhs_addr, rhs_copy);

        retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
                                GEFD(SI::getEnclosingStatement(node)), true);        
    }
    #ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(lhs_type, GEFD(lhs)) && Util::isVarRefOrDotArrow(rhs)) 
    #else
    else if(strDecl::isValidStructType(lhs_type) && Util::isVarRefOrDotArrow(rhs)) 
    #endif
    {
        // This is the struct = var_ref/dotarrow case

        // input: struct_L = var_ref_R
        // output: assign_copy(struct_L, var_ref_R, &var_ref_R);

        #if 0
        #ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the RHS, if its
        // array type.
        if(isSgArrayType(rhs->get_type())) {
            insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
        }                        
        #endif
        #endif

        
        SgExpression* lhs_copy = SI::copyExpression(lhs);
        //SgExpression* rhs_copy = SI::copyExpression(rhs);
        SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs_type), SgCastExp::e_C_style_cast);
        SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

        param_list = SB::buildExprListExp(lhs_copy, rhs_copy, rhs_addr);

        retType = lhs_type;        
        
    }
    else if(Util::isVarRefOrDotArrow(lhs) && Util::isVarRefOrDotArrow(rhs)) {
        // This is the var_ref/dotarrow = var_ref/dotarrow

        // input: var_ref_L = var_ref_R
        // output: assign_copy(&var_ref_L, var_ref_R, &var_ref_R);

        #if 0
        #ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the RHS, if its
        // array type.
        if(isSgArrayType(rhs->get_type())) {
            insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
        }
        #endif
        #endif
                        

        //SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
        SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
        SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs_type), SgCastExp::e_C_style_cast);
        SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

        param_list = SB::buildExprListExp(lhs_addr, rhs_copy, rhs_addr);

        retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type), 
                        GEFD(SI::getEnclosingStatement(node)), true);

    }
    #ifdef DOUBLE_DIM_ENABLED
    else if(isSgPointerDerefExp(lhs) && Util::isVarRefOrDotArrow(rhs)) {
        // This is the *ptr = var_ref/dotarrow
    
        // input: *ptr_L = var_ref_R
        // output: lhs_deref_assign_and_copy(ptr_L, var_ref_R, &var_ref_R)

        #if 0
        #ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the RHS, if its
        // array type.
        if(isSgArrayType(rhs->get_type())) {
            insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
        }
        #endif
        #endif
        
        SgExpression* lhs_strip = SI::copyExpression(Util::stripDeref(lhs));

        #ifdef STRIP_TYPEDEFS
        lhs_strip = Util::castAwayTypedef(lhs_strip);
        #endif

        SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs_type),
                                                SgCastExp::e_C_style_cast);
        SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

        param_list = SB::buildExprListExp(lhs_strip, rhs_copy, rhs_addr);

        retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
                            GEFD(SI::getEnclosingStatement(node)), true);
    }
    else if(Util::isVarRefOrDotArrow(lhs) && isSgPointerDerefExp(rhs)) {
        // This is the var_ref/dotarrow = *ptr

        // input: var_ref_L = *ptr_R
        // output: rhs_deref_assign_and_copy(&var_ref_L, ptr_R)

        SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
        SgExpression* rhs_strip = SI::copyExpression(Util::stripDeref(rhs));

        #ifdef STRIP_TYPEDEFS
        rhs_strip = Util::castAwayTypedef(rhs_strip);
        #endif

        param_list = SB::buildExprListExp(lhs_addr, rhs_strip);

        retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
                        GEFD(SI::getEnclosingStatement(node)), true);
    }
    #ifdef CPP_SCOPE_MGMT
    else if(isSgPointerDerefExp(lhs) && strDecl::isValidStructType(rhs->get_type(), GEFD(rhs))) 
    #else
    else if(isSgPointerDerefExp(lhs) && strDecl::isValidStructType(rhs->get_type())) 
    #endif
    {
        // This is the *ptr = struct case

        // input: *ptr = str_R
        // output: lhs_deref_assign_and_copy(ptr, str_R)

        SgExpression* lhs_strip = SI::copyExpression(Util::stripDeref(lhs));

        #ifdef STRIP_TYPEDEFS
        lhs_strip = Util::castAwayTypedef(lhs_strip);
        #endif
            
        SgExpression* rhs_copy = SI::copyExpression(rhs);

        param_list = SB::buildExprListExp(lhs_strip, rhs_copy);

        retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
                        GEFD(SI::getEnclosingStatement(node)), true);
    }
    #ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(lhs->get_type(), GEFD(lhs)) && isSgPointerDerefExp(rhs)) 
    #else
    else if(strDecl::isValidStructType(lhs->get_type()) && isSgPointerDerefExp(rhs)) 
    #endif
    {
        // This is the str = *ptr case
        
        // input: str_L = *ptr_R
        // output: rhs_deref_assign_and_copy(str_L, ptr_R)

        SgExpression* lhs_copy = SI::copyExpression(lhs);
        SgExpression* rhs_strip = SI::copyExpression(Util::stripDeref(rhs));

        #ifdef STRIP_TYPEDEFS
        rhs_strip = Util::castAwayTypedef(rhs_strip);
        #endif

        param_list = SB::buildExprListExp(lhs_copy, rhs_strip);

        retType = rhs_type;
    }
    else if(isSgPointerDerefExp(lhs) && isSgPointerDerefExp(rhs)) {
        // This is the *ptr = *ptr2 case

        // input: *ptr = *ptr2
        // output: deref_assign_and_copy(ptr, ptr2)

        SgExpression* lhs_strip = SI::copyExpression(Util::stripDeref(lhs));
        #ifdef STRIP_TYPEDEFS
        lhs_strip = Util::castAwayTypedef(lhs_strip);
        #endif
            
        SgExpression* rhs_strip = SI::copyExpression(Util::stripDeref(rhs));
        #ifdef STRIP_TYPEDEFS
        rhs_strip = Util::castAwayTypedef(rhs_strip);
        #endif

        param_list = SB::buildExprListExp(lhs_strip, rhs_strip);

        retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
                    GEFD(SI::getEnclosingStatement(node)), true);
    }
    #endif
    else {
        printf("Case not supported in handleAssignOverload Ops\n");
        printf("assign_op\n");
        printf("lhs_type\n");
        Util::printNode(lhs->get_type());
        printf("rhs_type\n");
        Util::printNode(rhs->get_type());
        Util::printNode(node);
        ROSE_ASSERT(0);
    }

    #if 0

    if(Util::isVarRefOrDotArrow(lhs) && Util::isVarRefOrDotArrow(rhs)) {
        // This is the var_ref/dotarrow = var_ref/dotarrow

        // input: var_ref_L = var_ref_R
        // output: assign_copy(&var_ref_L, var_ref_R, &var_ref_R);

        #ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the RHS, if its
        // array type.
        if(isSgArrayType(rhs->get_type())) {
            insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
        }
        #endif
                        

        //SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
        SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
        SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs->get_type()), SgCastExp::e_C_style_cast);
        SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

        param_list = SB::buildExprListExp(lhs_addr, rhs_copy, rhs_addr);

        retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs->get_type()), 
                        GEFD(SI::getEnclosingStatement(node)), true);

    }
    else if(Util::isVarRefOrDotArrow(lhs)) {
        ROSE_ASSERT(strDecl::isValidStructType(rhs->get_type()));
        // This is the var_ref/dotarrow = struct case

        // input: var_ref_L = struct_R
        // output: assign_copy(&var_ref_L, struct_R)

        //SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
        SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
        SgExpression* rhs_copy = SI::copyExpression(rhs);

        param_list = SB::buildExprListExp(lhs_addr, rhs_copy);

        retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs->get_type()),
                                GEFD(SI::getEnclosingStatement(node)), true);

    }
    else if(Util::isVarRefOrDotArrow(rhs)) {
        ROSE_ASSERT(strDecl::isValidStructType(lhs->get_type()));
        // This is the struct = var_ref/dotarrow case

        // input: struct_L = var_ref_R
        // output: assign_copy(struct_L, var_ref_R, &var_ref_R);

        #ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the RHS, if its
        // array type.
        if(isSgArrayType(rhs->get_type())) {
            insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
                        SI::getEnclosingStatement(node));
        }                        
        #endif

        
        SgExpression* lhs_copy = SI::copyExpression(lhs);
        //SgExpression* rhs_copy = SI::copyExpression(rhs);
        SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs->get_type()), SgCastExp::e_C_style_cast);
        SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

        param_list = SB::buildExprListExp(lhs_copy, rhs_copy, rhs_addr);

        retType = lhs->get_type();
    }
    else {
        ROSE_ASSERT(strDecl::isValidStructType(lhs->get_type()) &&
                    strDecl::isValidStructType(rhs->get_type()));
        // This is the struct = struct case
        // input: struct_L = struct_R
        // output: assign_copy(struct_L, struct_R)
        
        SgExpression* lhs_copy = SI::copyExpression(lhs);
        SgExpression* rhs_copy = SI::copyExpression(rhs);

        param_list = SB::buildExprListExp(lhs_copy, rhs_copy);

        retType = lhs->get_type();
    }
    #endif
    

    // 1. Modify dot/arrow to not transform when their parent is 
    // an assign op. 
    // 2. Def for assign_and_copy
    #ifdef DOUBLE_DIM_ENABLED
    if(isSgPointerDerefExp(lhs) && isSgPointerDerefExp(rhs)) {
        overload = buildMultArgOverloadFn("deref_assign_and_copy", param_list,
                                    retType,
                                    Util::getScopeForExp(isSgExpression(node)), 
                                    GEFD(SI::getEnclosingStatement(node)));
    }    
    else if(isSgPointerDerefExp(lhs)) {
        overload = buildMultArgOverloadFn("lhs_deref_assign_and_copy", param_list,
                                    retType,
                                    Util::getScopeForExp(isSgExpression(node)), 
                                    GEFD(SI::getEnclosingStatement(node)));
    }
    else if(isSgPointerDerefExp(rhs)) {
        overload = buildMultArgOverloadFn("rhs_deref_assign_and_copy", param_list,
                                    retType,
                                    Util::getScopeForExp(isSgExpression(node)), 
                                    GEFD(SI::getEnclosingStatement(node)));
    }
    else {
        overload = buildMultArgOverloadFn("assign_and_copy", param_list,
                                    retType,
                                    Util::getScopeForExp(isSgExpression(node)), 
                                    GEFD(SI::getEnclosingStatement(node)));
    }
    #else
    overload = buildMultArgOverloadFn("assign_and_copy", param_list,
                                    retType,
                                    Util::getScopeForExp(isSgExpression(node)), 
                                    GEFD(SI::getEnclosingStatement(node)));
    #endif


    SI::replaceExpression(isSgExpression(node), overload);

    return overload;
        
}

void ArithCheck::handleUnaryOverloadOps(SgNode* node) {

    #ifdef UNARY_OVLOP_DEBUG
    printf("handleUnaryOverloadOps\n");
    #endif

    SgUnaryOp* uop = isSgUnaryOp(node);    
    SgType* uop_type = uop->get_type();
    #ifdef STRIP_TYPEDEFS
    uop_type = Util::getType(uop_type);
    #endif

    SgExpression* oper = uop->get_operand();
    SgType* oper_type = oper->get_type();
    #ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
    #endif

    // For a var ref/ dot / arrow, overload the op
    // and update the varible in the overloading fn
    #ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(oper_type, GEFD(node))) 
    #else
    if(strDecl::isValidStructType(oper_type)) 
    #endif
    {

        #ifdef CONVERT_ARG_STRUCTS_TO_PTR
        // Its possible that the struct type exp is actually a reference to a variable
        // . one case is when the variable is a parameter to the function, and has been
        // converted to a struct. Here, we should actually manipulate the ptr within the 
        // struct, not just pass the struct by value. So, this is what we do. We'll 
        // create a dot exp in place of the struct type exp of the variable. We can then simply
        // call this function again, so that it handles the expression appropriately.
        if(isSgVarRefExp(oper)) {
            // This is the case we are looking for. So, lets create a dot exp to replace the
            // the oper. 
            SgVarRefExp* var_ref_exp = isSgVarRefExp(oper);
            SgInitializedName* var_name = var_ref_exp->get_symbol()->get_declaration();
            SgExpression* dot_exp = Util::createDotExpFor(var_name, "ptr");
            SI::replaceExpression(oper, dot_exp);
            ROSE_ASSERT(uop->get_operand() == dot_exp);
            // Now call handleUnaryOverloadOps and it should be able to handle the dot exp correctly
            handleUnaryOverloadOps(uop);
            return;
        }
        #endif
        
        
        // Its a struct. Overload the operation
        // and return the updated struct
        // The struct needs to be passed by reference
        // since we would -- its a struct .. so its a value
        // so no need to pass by ref. we only need the value out... RIGHT?
        SgExprListExp* param_list = SB::buildExprListExp(SI::copyExpression(oper));
        SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(node->variantT())), 
                                                param_list, oper_type, 
                                                Util::getScopeForExp(uop), 
                                                GEFD(SI::getEnclosingStatement(uop)));
        
        SI::replaceExpression(uop, overload);
    }
    else if(Util::isVarRefOrDotArrow(oper)) {
        
        // We might also need to do a create_entry if the arg is
        // of array type
        #ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the arg, if its
        // array type.
        if(isSgArrayType(oper_type)) {
            insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(uop),
                        SI::getEnclosingStatement(uop));
        }
        #endif

        // input: oper++, oper--
        // output: plusplus(&oper), minusminus(&oper)

        SgExprListExp* param_list = SB::buildExprListExp(Util::createAddressOfOpFor(SI::copyExpression(oper)));

        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(oper_type), 
                                        GEFD(SI::getEnclosingStatement(uop)),
                                        true);
        
        SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(node->variantT())),
                                        param_list, retType,
                                        Util::getScopeForExp(uop),
                                        GEFD(SI::getEnclosingStatement(uop)));

        SI::replaceExpression(uop, overload);

        // Nothing to do
        #ifdef UNARY_OVLOP_DEBUG
        printf("Var ref on unary op. Nothing to do\n");
        #endif
    }
    #ifdef DOUBLE_DIM_ENABLED
    else if(isSgPointerDerefExp(oper)) {
        // input: *ptr++ / *ptr--
        // output: deref_incr_decr(ptr)

        // 1. strip the deref
        // 2. call deref_incr_decr(ptr)
        
        SgExpression* ptr = SI::copyExpression(Util::stripDeref(oper));
        #ifdef STRIP_TYPEDEFS
        ptr = Util::castAwayTypedef(ptr);
        #endif
        
        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(oper_type),
                                    GEFD(SI::getEnclosingStatement(uop)),
                                    true);

        std::string op_name = Util::getStringForVariantT(node->variantT()) ;

        SgExpression* overload = buildMultArgOverloadFn(SgName(op_name + "_deref"),
                                                SB::buildExprListExp(ptr), retType,
                                                Util::getScopeForExp(uop),
                                                GEFD(SI::getEnclosingStatement(uop)));

        SI::replaceExpression(uop, overload);
    }
    #endif
    else {
        printf("Case not supported in UnaryOverloadOps\n");
        printf("unaryop\n");
        Util::printNode(uop);
        ROSE_ASSERT(0);
    }
}

void ArithCheck::handleOverloadOps(SgNode* node) {
    // The operations listed here would be
    //AddOp:
    //SubtractOp:
    //PlusPlusOp:
    //MinusMinusOp:
    //AssignOp: 
    //PlusAssignOp:
    //MinusAssignOp:

    // There are two objectives with each of these operations:
    // 1. Perform the actual operation on the pointer
    // 2. Copy metadata from RHS to LHS - in the binary operations

    // The fundamental difference between these operations and ones like
    // dot/arrow, deref, ptnr arr ref is that they don't change the value of
    // the pointer. So, its okay to copy the pointer over to another local copy
    // (i.e. that in a struct) and perform the check/operation on that local
    // pointer. For example: *ptr is same as ptr2 = ptr; *ptr2
    // But ptr++ is not the same as ptr2 = ptr; ptr2++
    // Here, ptr is not updated with the correct value.
    // So, an interesting point here... 
    // On the rhs, we can always make a copy since we are reading out of that
    // location
    // On the lhs, if its a var ref/dot/arrow -- need to include dot or arrow to
    // the DELAYED_INSTR so that we don't output structs --
    // then, we need to write to that memory location. If its just a pointer value,
    // then it can be copied since its just a write to a given address. For example,
    // int **ptr; *ptr = ptr2; -- we could take a local copy of *ptr, and that will give 
    // us the address of the location that needs to be written. And we could simply
    // write to that location. Tricky part is to figure out how we could update the
    // metadata at that location.
    // On the other hand, int* ptr; ptr = ptr2; will simply copy the metadata from
    // ptr2 to ptr, along with the actual value.
    
    // There are four cases for binary ops, two cases for unary ops
    // binary ops: var_ref = var_ref, struct = var_ref, var_ref = struct
    // struct = struct
    // unary ops: var_ref, struct
    
    // NOTE: None of these operations require checks to be performed.

    if(isSgBinaryOp(node)) {
        handleBinaryOverloadOps(node);
    }
    else {
        ROSE_ASSERT(isSgUnaryOp(node));
        handleUnaryOverloadOps(node);
    }
    
}

SgScopeStatement* ArithCheck::getScopeThruParent(SgNode* node) {
    if(node->get_parent() != NULL) {
        SgNode* parent = node->get_parent();
        Util::printNode(parent);
        if(isSgScopeStatement(parent)) {
            return isSgScopeStatement(parent);
        }
        else {
            return getScopeThruParent(parent);
        }
    }
    else {
        return NULL;
    }
}

SgScopeStatement* ArithCheck::getScopeForVarRef(SgVarRefExp* var_ref) {

    SgVariableSymbol* var_symbol = var_ref->get_symbol();
    SgInitializedName* init_name = getNameForSymbol(var_symbol);
    return init_name->get_scope();
}

bool ArithCheck::findInVarRemap(SgVarRefExp* var_ref) {
    #ifdef VAR_REMAP_DEBUG
    printf("findInVarRemap\n");
    #endif

    SgVariableSymbol* var_symbol = var_ref->get_symbol();
    ROSE_ASSERT(var_symbol != NULL);
    VariableSymbolMap_t::iterator iter = ArithCheck::varRemap.find(var_ref->get_symbol());

    if(iter == ArithCheck::varRemap.end()) {
        return false;
    }

    ROSE_ASSERT(iter != ArithCheck::varRemap.end());

    return true;
}


SgExpression* ArithCheck::createStructForVarRef(SgVarRefExp* var_ref) {

    // Essentially, this variable reference, which could have been an lvalue
    // is not, no longer one, because we are using it in as an argument to a 
    // function.
    SgExpression* var_ref_copy = SI::copyExpression(var_ref);
    var_ref_copy->set_lvalue(false);

    SgExpression* ptr_value = var_ref_copy;

    SgExpression* addr_ptr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(var_ref_copy))); 
    
    // We will be returning a struct variable based on the type
    // of this variable.
    // The struct might need to be created 

    // From the variable's type, remove the typdefs, and find the original type.
    #ifdef REMOVE_TYPEDEFS_FROM_VARS
    SgType* var_ref_type = Util::skip_Typedefs(var_ref->get_type());
    #ifdef CREATE_STRUCT_VAR_REF_DEBUG
    printf("var_ref_type\n");
    Util::printNode(var_ref_type);
    #endif
    SgType* type_pntr_arr = Util::getTypeForPntrArr(var_ref_type);
    #ifdef CREATE_STRUCT_VAR_REF_DEBUG
    printf("type_pntr_arr\n");
    Util::printNode(type_pntr_arr);
    #endif
    SgType* retType = strDecl::getStructType(type_pntr_arr, GEFD(SI::getEnclosingStatement(var_ref)), true);
    #else
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(var_ref->get_type()), GEFD(SI::getEnclosingStatement(var_ref)), true);
    #endif

    #ifdef CREATE_STRUCT_VAR_REF_DEBUG
    printf("retType\n");
    Util::printNode(retType);
    #endif

    #ifdef CREATE_STRUCT_VAR_REF_DEBUG
    printf("Util::getScopeForExp:var_ref\n");
    Util::printNode(var_ref);
    #endif

    SgScopeStatement* scope = Util::getScopeForVarRef(var_ref);
    #if 0
    #ifdef CREATE_STRUCT_VAR_REF_DEBUG
    printf("Got scope:\n");
    Util::printNode(scope);
    #endif
    #endif

    SgExpression* createfn;
    
    if(isSgArrayType(var_ref->get_type())) {
        
        #if 1
        // create_struct(var_ref, &var_ref, sizeof(var_ref))  
        // we don't need to do a divide at the sizeof(var_ref) since we need the size in bytes... 
        // for metadatacalculations. We won't use this size for additions/subtractions.
        #ifdef REMOVE_TYPEDEFS_FROM_VARS
        SgExpression* ce1 = SB::buildCastExp(ptr_value, Util::getTypeForPntrArr(var_ref_type), SgCastExp::e_C_style_cast);
        #else
        SgExpression* ce1 = SB::buildCastExp(ptr_value, Util::getTypeForPntrArr(var_ref->get_type()), SgCastExp::e_C_style_cast);
        #endif
        SgExpression* ce2 = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(var_ref))); 
        SgExpression* ce3 = SB::buildSizeOfOp(SI::copyExpression(var_ref));
        
//        createfn = build3ArgOverloadFn("create_struct", ce1, ce2, ce3, retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));
        createfn = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(ce1, ce2, ce3), retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));

        #else
        // Old working one    
        // Change the first argument to a pointer... its currently
        // of array variable type ... something like unsigned int[bounds].
        // We'll transform that into unsigned int*
        SgExpression* casted_ptr_value = SB::buildCastExp(ptr_value, Util::getTypeForPntrArr(var_ref->get_type()), SgCastExp::e_C_style_cast);

        // Insert third arg as bounds of the array
        SgExpression* array_index = getArrayIndex(isSgVarRefExp(var_ref_copy)); 
        SgExpression* third_arg = SI::copyExpression(array_index);


        #ifdef OLD_NAME_GEN
        SgName fn_name(Util::getStringForFn("create_struct", casted_ptr_value->get_type(), addr_ptr->get_type(), third_arg->get_type(), retType));
//        createfn = build3ArgOverloadFn(fn_name, casted_ptr_value, addr_ptr, third_arg, retType, scope);

//        createfn = build3ArgOverloadFn(fn_name, casted_ptr_value, addr_ptr, third_arg, retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));
        createfn = buildMultArgOverloadFn(fn_name, SB::buildExprListExp(casted_ptr_value, addr_ptr, third_arg), retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));
        #else
//        createfn = build3ArgOverloadFn("create_struct", casted_ptr_value, addr_ptr, third_arg, retType, scope);

//        createfn = build3ArgOverloadFn("create_struct", casted_ptr_value, addr_ptr, third_arg, retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));
        createfn = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(casted_ptr_value, addr_ptr, third_arg), retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));
        #endif
        #endif
    }
    else {
        #ifdef REMOVE_TYPEDEFS_FROM_VARS
        SgName fn_name(Util::getStringForFn("create_struct", var_ref_type, addr_ptr->get_type(), NULL, retType));
        #else
        SgName fn_name(Util::getStringForFn("create_struct", ptr_value->get_type(), addr_ptr->get_type(), NULL, retType));
        #endif

        #ifdef OLD_NAME_GEN
//        createfn = buildOverloadFn(fn_name, ptr_value, addr_ptr, retType, scope);
        //createfn = buildOverloadFn(fn_name, ptr_value, addr_ptr, retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));
        createfn = buildMultArgOverloadFn(fn_name, SB::buildExprListExp(ptr_value, addr_ptr), retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));

        #else
//        createfn = buildOverloadFn("create_struct", ptr_value, addr_ptr, retType, scope);
//        createfn = buildOverloadFn("create_struct", ptr_value, addr_ptr, retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));        

        createfn = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(ptr_value, addr_ptr), retType, scope, GEFD(SI::getEnclosingStatement(var_ref)));
        #endif

    }

    #ifdef CREATE_STRUCT_VAR_REF_DEBUG
    printf("createfn\n");
    Util::printNode(createfn);
    #endif
    
    return createfn;
}

void ArithCheck::mapExprToOriginalType(SgExpression* replacement, SgType* original_type) {
    ROSE_ASSERT(Util::isQualifyingType(original_type));
    ArithCheck::OriginalVarType.insert(ExprTypeMap_t::value_type(replacement, original_type));
}

SgType* ArithCheck::findInOriginalVarType(SgExpression* replacement) {
    ExprTypeMap_t::iterator etm;

    etm = ArithCheck::OriginalVarType.find(replacement);

    if(etm != ArithCheck::OriginalVarType.end()) {
        SgType* orig_type = etm->second;
        return orig_type;
    }

    ROSE_ASSERT(0);
}

void ArithCheck::mapExprToOriginalVarRef(SgExpression* replacement, SgExpression* original) {
    
    SgType* orig_type = original->get_type();
    #ifdef STRIP_TYPEDEFS
    orig_type = Util::getType(orig_type);
    #endif

    ROSE_ASSERT(Util::isQualifyingType(orig_type));
    ExprMap_t::iterator em = OriginalVarRef.find(replacement);
    if(em != OriginalVarRef.end()) {
        OriginalVarRef.erase(em);
    }

    ArithCheck::OriginalVarRef.insert(ExprMap_t::value_type(replacement, SI::copyExpression(original)));
}

SgExpression* ArithCheck::findInOriginalVarRef(SgExpression* replacement) {
    ExprMap_t::iterator em;

    em = ArithCheck::OriginalVarRef.find(replacement);

    if(em != ArithCheck::OriginalVarRef.end()) {
        SgExpression* orig = em->second;
        return orig;
    }

    ROSE_ASSERT(0);
}

SgExpression* ArithCheck::checkInOriginalVarRef(SgExpression* replacement) {
    ExprMap_t::iterator em;

    em = ArithCheck::OriginalVarRef.find(replacement);

    if(em != ArithCheck::OriginalVarRef.end()) {
        SgExpression* orig = em->second;
        return orig;
    }

    return NULL;
}

// This is a more general version of the OriginalVarRef above. This handles cases
// where the underlying expression was replaced with another exp... rather than
// the restricted vase of replacing the underlying *var ref* with another exp
void ArithCheck::mapExprToOriginalExpr(SgExpression* repl, SgExpression* orig) {
    
    SgType* orig_type = orig->get_type();
    #ifdef STRIP_TYPEDEFS
    orig_type = Util::getType(orig_type);
    #endif
    
    ROSE_ASSERT(Util::isQualifyingType(orig_type));
    ExprMap_t::iterator em = OriginalExp.find(repl);
    if(em != OriginalExp.end()) {
        OriginalExp.erase(em);
    }

    ArithCheck::OriginalExp.insert(ExprMap_t::value_type(repl, SI::copyExpression(orig)));
        
}

SgExpression* ArithCheck::findInOriginalExp(SgExpression* repl) {
    ExprMap_t::iterator em;

    em = ArithCheck::OriginalExp.find(repl);

    if(em != ArithCheck::OriginalExp.end()) {
        SgExpression* orig = em->second;
        return orig;
    }

    ROSE_ASSERT(0);
}

SgExpression* ArithCheck::checkInOriginalExp(SgExpression* repl) {
    ExprMap_t::iterator em;

    em = ArithCheck::OriginalExp.find(repl);

    if(em != ArithCheck::OriginalExp.end()) {
        SgExpression* orig = em->second;
        return orig;
    }

    return NULL;
}

    

void ArithCheck::handlePointerVarRefs(SgVarRefExp* var_ref) {
    
    #ifdef POINTER_VAR_REF_DEBUG
    printf("handlePointerVarRefs\n");
    #endif
    // Here, we call create_struct function, which will
    // create a struct variable using the pointer value
    // and the pointer variable's address
    // Any initialization data for this pointer would already be
    // in the TrackingDB if this pointer was initialized.

    // do the old trick for cases where the symbol is found 
    // in the varRemap -- these are variables in the parameter
    // list that have been converted to structs. 
    // If we retain function prototypes, original params stay in
    // place
    #ifndef RETAIN_FUNC_PROTOS
    if(findInVarRemap(var_ref)) {
        handleVarRefs(var_ref);
        return;
    }
    #endif



    // If the parent to this var_ref is an arrow/dot expression, don't
    // touch it... this variable is not a local/global variable... its
    // address and metadata is given by the result of the dot/arrow exp
    if(Util::isDotOrArrow(var_ref->get_parent())) {
        #ifdef POINTER_VAR_REF_DEBUG
        printf("Parent is dot/arrow exp\n");
        Util::printNode(var_ref->get_parent());
        printf("returning\n");
        #endif
        return;
    }


    

    #ifdef DELAYED_INSTR
    if(isSgPntrArrRefExp(var_ref->get_parent())) {
        // This should be the LHS operand of the Pntr Arr Ref
        ROSE_ASSERT(isSgPntrArrRefExp(var_ref->get_parent())->get_lhs_operand()
                    == var_ref);
        // Do nothing, we'll handle it Pntr Arr Ref level
        return;
    }
    if(isSgSizeOfOp(var_ref->get_parent())) {
        // Do nothing. sizeof needs no handling
        return;
    }
    if(isSgPointerDerefExp(var_ref->get_parent())) {
        // Do nothing. Handle this in PointerDeref
        return;
    }
    if(Trav::NeedsToBeOverloaded(var_ref->get_parent())) {
        // Do nothing. Handle this in handleOverloadOps
        return;
    }
    #endif

    // Why do we need this?
    #ifndef NO_INSTR_AT_VAR_REFS    
    // This part is not active but is being updated with
    // STRIP_TYPEDEFS for later use... if necessary
    
    SgExpression* createfn = createStructForVarRef(var_ref);

    SgType* var_ref_type = var_ref->get_type();
    #ifdef STRIP_TYPEDEFS
    var_ref_type = Util::getType(var_ref_type);
    #endif

    mapExprToOriginalType(createfn, var_ref_type);
    #ifndef ONLY_ADD_ARRAYS_TO_OVR
    mapExprToOriginalVarRef(createfn, var_ref);
    #ifdef POINTER_VAR_REF_DEBUG
    printf("handlePointerVarRefs\n");
    printf("mapping\n");
    printf("createfn\n");
    Util::printNode(createfn);
    printf("key: %llu\n", (unsigned long long)createfn);
    printf("var_ref\n");
    Util::printNode(var_ref);
    #endif
    #endif

    SI::replaceExpression(var_ref, createfn);
    #endif
}

SgExpression* ArithCheck::getArrayIndex(SgExpression* exp) {
    
    SgType* exp_type = exp->get_type();
    SgArrayType* exp_arr_type = isSgArrayType(exp_type);
    return exp_arr_type->get_index();
}

SgType* ArithCheck::getArrayBaseType(SgExpression* exp) {
    
    SgType* exp_type = exp->get_type();
    SgArrayType* exp_arr_type = isSgArrayType(exp_type);
    return exp_arr_type->get_base_type();
}

void ArithCheck::handleArrayVarRefs(SgVarRefExp* var_ref) {

    #ifndef RETAIN_FUNC_PROTOS
    // Array variables can be passed without turning into pointers at the other end...
    if(findInVarRemap(var_ref)) {
        handleVarRefs(var_ref);
        return;
    }
    #endif


    // If the parent to this var_ref is an arrow/dot expression, don't
    // touch it... this variable is not a local/global variable... its
    // address and metadata is given by the result of the dot/arrow exp
    if(Util::isDotOrArrow(var_ref->get_parent())) {
        #ifdef ARRAY_VAR_REF_DEBUG
        printf("Parent is dot/arrow exp\n");
        Util::printNode(var_ref->get_parent());
        printf("returning\n");
        #endif
        return;
    }

    #ifdef DELAYED_INSTR
    if(isSgPntrArrRefExp(var_ref->get_parent())) {
        // This should be the LHS operand of the Pntr Arr Ref
        ROSE_ASSERT(isSgPntrArrRefExp(var_ref->get_parent())->get_lhs_operand()
                    == var_ref);
        // Do nothing, we'll handle it Pntr Arr Ref level
        return;
    }
    if(isSgSizeOfOp(var_ref->get_parent())) {
        // Do nothing. sizeof needs no handling
        return;
    }
    if(isSgPointerDerefExp(var_ref->get_parent())) {
        // Do nothing. Handle this in PointerDeref
        return;
    }
    if(Trav::NeedsToBeOverloaded(var_ref->get_parent())) {
        // Do nothing. Handle this in handleOverloadOps
        return;
    }
    #endif

    // WHY IS THIS STUFF NECESSARY?
    #ifndef NO_INSTR_AT_VAR_REFS
    // This part is not used right now... but updating it with
    // STRIP_TYPEDEFS if it is used later...

    SgExpression* createfn = createStructForVarRef(var_ref);

    #ifdef ARRAY_VAR_REF_DEBUG
    printf("handleArrayVarRefs\n");
    #endif

    SgType* var_ref_type = var_ref->get_type();
        
    #ifdef STRIP_TYPEDEFS
    var_ref_type = Util::getType(var_ref_type);
    #endif

    mapExprToOriginalType(createfn, var_ref_type);    
    mapExprToOriginalVarRef(createfn, var_ref);
    #ifdef ARRAY_VAR_REF_DEBUG
    printf("mapping\n");
    printf("createfn\n");
    Util::printNode(createfn);
    printf("key: %llu\n", (unsigned long long)createfn);
    printf("var_ref\n");
    Util::printNode(var_ref);
    #endif


    SI::replaceExpression(var_ref, createfn);
    
    #endif
}

SgExpression* ArithCheck::createDotArrowOverloadExp(SgBinaryOp* dotarrow_exp) {

    ROSE_ASSERT(isSgDotExp(dotarrow_exp) || isSgArrowExp(dotarrow_exp));

    // Here, we know that its a dot exp with a variable operand
    // If its a pointer, do the following:
    // create_struct(dotarrow_exp, &dotarrow_exp);

    // If its an array, do the following:
    // create_struct(dotarrow_exp, &dotarrow_exp, array_size)


    SgExpression* dotarrow_exp_copy = SI::copyExpression(dotarrow_exp);
    dotarrow_exp_copy->set_lvalue(false);
    SgExpression* ptr_exp = dotarrow_exp_copy;
    ptr_exp->set_lvalue(false);
    SgExpression* addr_exp = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(dotarrow_exp_copy)));
    addr_exp->set_lvalue(false);
    ROSE_ASSERT(addr_exp->get_lvalue() == false);

    SgExpression* oper = dotarrow_exp->get_rhs_operand();
    SgType* oper_type = oper->get_type();

    // The return type is the struct version of the type of the operand.
    // Since the operand can be an array or pointer, we use the generic function to get the correct type

    // Lets remove the typedefs from the vars before creating a struct out of them.
    #ifdef REMOVE_TYPEDEFS_FROM_VARS
    SgType* oper_var_type = Util::skip_Typedefs(oper_type);
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(oper_var_type), GEFD(SI::getEnclosingStatement(dotarrow_exp)), true);
    #else
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(oper_type), GEFD(SI::getEnclosingStatement(dotarrow_exp)), true);
    #endif

    SgName fn_name(Util::getStringForOp(dotarrow_exp, retType));

    SgExpression* overload;
        

    if(isSgArrayType(oper->get_type())) {

        SgArrayType* array = isSgArrayType(oper->get_type());
        SgExpression* array_size = SI::copyExpression(array->get_index());
        
        #ifdef OLD_NAME_GEN
//        overload = build3ArgOverloadFn(fn_name, ptr_exp, addr_exp, array_size, retType, Util::getScopeForExp(dotarrow_exp)); 

//        overload = build3ArgOverloadFn(fn_name, ptr_exp, addr_exp, array_size, retType, Util::getScopeForExp(dotarrow_exp), GEFD(SI::getEnclosingStatement(dotarrow_exp))); 
        overload = buildMultArgOverloadFn(fn_name, SB::buildExprListExp(ptr_exp, addr_exp, array_size), retType, Util::getScopeForExp(dotarrow_exp), GEFD(SI::getEnclosingStatement(dotarrow_exp))); 


        #else
        // OLD
        //overload = build3ArgOverloadFn(SgName(Util::getStringForVariantT(dotarrow_exp->variantT())), ptr_exp, addr_exp, array_size, retType, Util::getScopeForExp(dotarrow_exp));
        // Working one. Commented for adding node at the end of the list
//        overload = build3ArgOverloadFn("create_struct", ptr_exp, addr_exp, array_size, retType, Util::getScopeForExp(dotarrow_exp));

//        overload = build3ArgOverloadFn("create_struct", ptr_exp, addr_exp, array_size, retType, Util::getScopeForExp(dotarrow_exp), GEFD(SI::getEnclosingStatement(dotarrow_exp)));

        overload = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(ptr_exp, addr_exp, array_size), retType, Util::getScopeForExp(dotarrow_exp), GEFD(SI::getEnclosingStatement(dotarrow_exp)));
        #endif
    }
    else {
        #ifdef OLD_NAME_GEN
//        overload = buildOverloadFn(fn_name, ptr_exp, addr_exp, retType, Util::getScopeForExp(dotarrow_exp));

//        overload = buildOverloadFn(fn_name, ptr_exp, addr_exp, retType, Util::getScopeForExp(dotarrow_exp), GEFD(SI::getEnclosingStatement(dotarrow_exp)));
        overload = buildMultArgOverloadFn(fn_name, SB::buildExprListExp(ptr_exp, addr_exp), retType, Util::getScopeForExp(dotarrow_exp), GEFD(SI::getEnclosingStatement(dotarrow_exp)));


        #else
        //overload = buildOverloadFn(SgName(Util::getStringForVariantT(dotarrow_exp->variantT())), ptr_exp, addr_exp, retType, Util::getScopeForExp(dotarrow_exp));
        // Working one... Commented out to add dotarrow_exp
//        overload = buildOverloadFn("create_struct", ptr_exp, addr_exp, retType, Util::getScopeForExp(dotarrow_exp));


//        overload = buildOverloadFn("create_struct", ptr_exp, addr_exp, retType, Util::getScopeForExp(dotarrow_exp), GEFD(SI::getEnclosingStatement(dotarrow_exp)));

        overload = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(ptr_exp, addr_exp), retType, Util::getScopeForExp(dotarrow_exp), GEFD(SI::getEnclosingStatement(dotarrow_exp)));
        #endif
    }        
    
    return overload;

}

void ArithCheck::handleDotExpVarOper(SgDotExp* dot_exp) {
}


void ArithCheck::handleDotExpFuncOper(SgDotExp* dot_exp) {

}

void ArithCheck::handleDotExp(SgDotExp* dot_exp) {

    // So, we get wrap the dot exp in a create struct... right?
    // But that doesn't always work because if its an array we would need
    // initialization data... We need to create a struct which uses
    // the dot exp's addr, and location it points to.
    // If this is an array type, then we might need to send in the initialization
    // data for that array, since we may not have recorded it.

    // Two cases here... var operand... function operand
    if(isSgVarRefExp(dot_exp->get_rhs_operand())) {
        handleDotExpVarOper(dot_exp);
    }
    else if(isSgFunctionCallExp(dot_exp->get_rhs_operand())) {
        handleDotExpFuncOper(dot_exp);
    }
    else {
        // Nothing to do in other cases... This must be
        // a class/struct var
    }
}

SgExpression* ArithCheck::createDerefOvlForStruct(SgExpression* str) {

    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(str->get_type(), GEFD(str)));
    #else
    ROSE_ASSERT(strDecl::isValidStructType(str->get_type()));
    #endif
    
    SgType* retType = strDecl::findInUnderlyingType(str->get_type());
    
    SgScopeStatement* scope = Util::getScopeForExp(str);

    SgExpression* arg = SI::copyExpression(str);
    
    #if OLD_NAME_GEN
    SgName fn_name(Util::getStringForFn("Deref", arg->get_type(), NULL, NULL, retType));
//    SgExpression* dereffn = buildOverloadFn(fn_name, arg, NULL, retType, scope);
//    SgExpression* dereffn = buildOverloadFn(fn_name, arg, NULL, retType, scope, GEFD(SI::getEnclosingStatement(str)));
    SgExpression* dereffn = buildMultArgOverloadFn(fn_name, SB::buildExprListExp(arg), retType, scope, GEFD(SI::getEnclosingStatement(str)));


    #else
//    SgExpression* dereffn = buildOverloadFn("Deref", arg, NULL, retType, scope);

//    SgExpression* dereffn = buildOverloadFn("Deref", arg, NULL, retType, scope, GEFD(SI::getEnclosingStatement(str)));

    SgExpression* dereffn = buildMultArgOverloadFn("Deref", SB::buildExprListExp(arg), retType, scope, GEFD(SI::getEnclosingStatement(str)));
    #endif

    return dereffn;    

}

SgExpression* ArithCheck::derefLHSOfArrowExp(SgArrowExp* arrow_exp) {
    // Now, deref it...
    #ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(arrow_exp->get_lhs_operand()->get_type(), GEFD(arrow_exp)));    
    #else
    ROSE_ASSERT(strDecl::isValidStructType(arrow_exp->get_lhs_operand()->get_type()));
    #endif

    SgExpression* deref_ovl = createDerefOvlForStruct(arrow_exp->get_lhs_operand());

    // Now, create an arrow expr using the deref_ovl and copy of rhs.
    SgExpression* rhs_copy = SI::copyExpression(arrow_exp->get_rhs_operand());

    // create the arrow exp
    SgExpression* new_arrow_exp = SB::buildArrowExp(deref_ovl, rhs_copy);

    return new_arrow_exp;
}


SgExpression* ArithCheck::handlePntrArrLHSArrowExp(SgArrowExp* arrow_exp) {

    #ifdef PNTRARRREF_LHS_ARROW_DEBUG
    printf("arrow_exp: \n");
    Util::printNode(arrow_exp);
    printf("arrow_exp->lhs: \n");
    Util::printNode(arrow_exp->get_lhs_operand());
    printf("arrow_exp->rhs: \n");
    Util::printNode(arrow_exp->get_rhs_operand());
    #endif


    // Assert that we are looking at a var ref here.
    ROSE_ASSERT(isSgVarRefExp(arrow_exp->get_lhs_operand()));
    // Assert that its the qualifying type
    ROSE_ASSERT(Util::isQualifyingType(arrow_exp->get_lhs_operand()->get_type()));

    SgArrowExp* rep_arrow_exp;
    
    #ifndef RETAIN_FUNC_PROTOS
    if(findInVarRemap(isSgVarRefExp(arrow_exp->get_lhs_operand()))) {
        handleVarRefs(isSgVarRefExp(arrow_exp->get_lhs_operand()));
        rep_arrow_exp = arrow_exp;
        #ifdef PNTRARRREF_LHS_ARROW_DEBUG
        printf("Found arrow->lhs_operand in var-remap -- lhs oper--\n");
        Util::printNode(arrow_exp->get_lhs_operand());
        #endif
    }
    else {
    #endif
        // Now, call the function to create the struct version of this var ref
        SgExpression* createfn = createStructForVarRef(isSgVarRefExp(arrow_exp->get_lhs_operand()));
    
        // replace original one with this one... in fact, be careful and create a new
        // arrow exp.
        rep_arrow_exp = SB::buildArrowExp(createfn, SI::copyExpression(arrow_exp->get_rhs_operand()));

        SI::replaceExpression(arrow_exp, rep_arrow_exp);
        
        #ifdef PNTRARRREF_LHS_ARROW_DEBUG
        printf("created rep arrow rep\n");
        Util::printNode(rep_arrow_exp);
        #endif
    #ifndef RETAIN_FUNC_PROTOS
    }
    #endif
    
    // Now, deref it...

    SgExpression* new_arrow_exp = derefLHSOfArrowExp(rep_arrow_exp);

    SI::replaceExpression(rep_arrow_exp, new_arrow_exp);

    #ifdef PNTRARRREF_LHS_ARROW_DEBUG
    printf("Created new arrow exp\n");
    Util::printNode(new_arrow_exp);
    #endif

    return new_arrow_exp;
}

SgExpression* ArithCheck::handleStructLHSArrowExp(SgArrowExp* arrow_exp) {

    SgExpression* new_arrow_exp = derefLHSOfArrowExp(arrow_exp);

    SI::replaceExpression(arrow_exp, new_arrow_exp);

    return new_arrow_exp;
}


SgExpression* ArithCheck::handlePntrArrRHSArrowExp(SgArrowExp* arrow_exp) {
    

    SgExpression* overload = createDotArrowOverloadExp(isSgBinaryOp(arrow_exp));

    SI::replaceExpression(arrow_exp, overload);

    #ifdef PNTRARRREF_RHS_ARROW_DEBUG
    printf("Created overload to replace arrow exp\n");
    Util::printNode(overload);
    #endif

    return overload;
}

void ArithCheck::handleArrowExpVarOper(SgArrowExp* arrow_exp) {

    // With arrow, we first create a deref function for
    // the struct on lhs. Then, we use createDotArrowOverloadExp
    // to overload the arrow exp
    printf("handleArrowExpVarOper\n");

    // The lhs of the arrow_exp is still a pointer/array type. Need to handle
    // it here -- by converting into the respective struct.
    // ... we only do this if it isn't in the varRemap

    SgExpression* new_arrow_exp = handlePntrArrLHSArrowExp(arrow_exp); 


    // Now, we need createDotArrowOverloadExp, to create the overloaded function
    SgExpression* overload = createDotArrowOverloadExp(isSgBinaryOp(new_arrow_exp));

    // Now, replace the new_arrow_exp, with overload
    SI::replaceExpression(new_arrow_exp, overload);

    printf("Created overload to replace new arrow exp\n");
    Util::printNode(overload);
}


void ArithCheck::handleArrowExpFuncOper(SgArrowExp* arrow_exp) {

    // create a deref function for the lhs struct, and
    // the function already returns a struct... so we should be
    // good?
    // Yes...

    handlePntrArrLHSArrowExp(arrow_exp);
    // No need to overload... the RHS since its a function returning
    // a struct... so, nothing else to do...
}

void ArithCheck::handleArrowExp(SgArrowExp* arrow_exp) {
    
    if(isSgVarRefExp(arrow_exp->get_rhs_operand())) {
        handleArrowExpVarOper(arrow_exp);
    }
    else if(isSgFunctionCallExp(arrow_exp->get_rhs_operand())) {
        handleArrowExpFuncOper(arrow_exp);
    }
    else {
        printf("Not a variable/func in arrow exp?\n");
        Util::printNode(arrow_exp);
        ROSE_ASSERT(0);
    }
}

void ArithCheck::handlePointerDeref(SgPointerDerefExp* deref_exp) {
    handleDerefExp(deref_exp);
}

void ArithCheck::handleArrayDeref(SgPointerDerefExp* deref_exp) {
    // For now, we use handleDerefExp to handle array derefs
    // handleDerefExp returns the underlying type
    handleDerefExp(deref_exp);
}

void ArithCheck::handlePointerArrRefs(SgPntrArrRefExp* pntr_arr_ref) {
    // Using handlePntrArrRefExp on pointers as well as array types
    // since the operation is similar on both variables
    handlePntrArrRefExp(pntr_arr_ref);
}

void ArithCheck::handleArrayArrRefs(SgPntrArrRefExp* pntr_arr_ref) {
    handlePntrArrRefExp(pntr_arr_ref);
}

void ArithCheck::handlePointerDotExp(SgDotExp* dot_exp) {
    handleDotExp(dot_exp);
}

void ArithCheck::handleArrayDotExp(SgDotExp* dot_exp) {
    handleDotExp(dot_exp);
}

void ArithCheck::handlePointerArrowExp(SgArrowExp* arrow_exp) {
    handleArrowExp(arrow_exp);
}

void ArithCheck::handleArrayArrowExp(SgArrowExp* arrow_exp) {
    handleArrowExp(arrow_exp);
}

void ArithCheck::handlePointerCastExp(SgCastExp* cast_exp) {
    // Cast expressions typically go from one struct to another
    // or one value to a struct, or vice versa.
    // This behavior is regardless of whether    
    // its a pointer/array type
    handleCastExp(cast_exp);
}

void ArithCheck::handleArrayCastExp(SgCastExp* cast_exp) {
    handleCastExp(cast_exp);
}

void ArithCheck::handlePointerAddrOfOp(SgAddressOfOp* addr_of) {
    // FIXME: Not checking whether the address of op exactly
    // fits in here well... 
    handleAddressOfOp(addr_of);
}

void ArithCheck::handleArrayAddrOfOp(SgAddressOfOp* addr_of) {
    // FIXME: Not checking whether the address of op exactly
    // fits in here well... 
    handleAddressOfOp(addr_of);

}

SgExpression* ArithCheck::buildCreateEntryFnForArrays(SgExpression* array, SgScopeStatement* scope,
                                        SgStatement* pos) {
    
    #ifdef STRIP_TYPEDEFS
    // No need to cast out typedefs.. since all the args are casted to the appropriate types
    #endif

    // create_entry(&array, array, sizeof(array), dummy_lock)
    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(array));
    SgExpression* ptr = Util::castToAddr(SI::copyExpression(array));
    SgExpression* sizeof_array = SB::buildCastExp(SB::buildSizeOfOp(SI::copyExpression(array)),
                                            Util::getSizeOfType(),
                                            SgCastExp::e_C_style_cast);

    SgExpression* dummy_lock = Util::castToAddr(Util::getDummyLock());

    SgExprListExp* param_list = SB::buildExprListExp(addr, ptr, sizeof_array, dummy_lock);

    return buildMultArgOverloadFn("create_entry", param_list,
                                SgTypeVoid::createType(),
                                scope, pos);
}


void ArithCheck::insertCreateEntry(SgExpression* array, SgScopeStatement* scope, 
                                    SgStatement* pos) {

    SgExpression* overload = buildCreateEntryFnForArrays(array, scope, GEFD(pos));
    SI::insertStatementBefore(Util::getSuitablePrevStmt(pos), SB::buildExprStatement(overload));

}


void ArithCheck::handleClassStructVars(SgVariableDeclaration* var_decl) {

    // Here, we use a technique similar to assign and copy to transfer metadata from rhs to lhs. Since we
    // don't want to increase the number of uses of rhs, and we need the actual variable from the rhs, we'll
    // take both the lhs and rhs by reference into cs_assign_and_copy. Inside cs_assign_and copy, we copy
    // rhs to lhs (the actual operation) and then perform an rtc_copy.
    // input: str_type lhs_str = rhs_str
    // output: str_type lhs_str; cs_assign_and_copy(&lhs_str, &rhs_str)

    // Lets remove the initializer for the var_decl
    // Only assign initializers supported    
    SgInitializedName* var_name = Util::getNameForDecl(var_decl);
    SgAssignInitializer* assign_init = isSgAssignInitializer(var_name->get_initializer());
    ROSE_ASSERT(assign_init != NULL);
    
    // Copy the initializer
    SgExpression* init_copy = SI::copyExpression(assign_init->get_operand());

    // Set the initializer in the var decl to NULL.
    var_name->set_initializer(NULL);
    
    // Create an assign op.. which will be replaced by the appropriate
    // operation by the handleAssignOverloadOps function below.
    SgAssignOp* var_assign = SB::buildAssignOp(SB::buildVarRefExp(var_name), init_copy);

    SI::insertStatementAfter(var_decl, SB::buildExprStatement(var_assign));

    // cs_assign_and_copy(ptr...)
    SgExpression* overload = handleCSAssignOverloadOps(var_assign);
}


SgExpression* ArithCheck::handleCSAssignOverloadOps(SgExpression* assign_op) {

    ROSE_ASSERT(isSgAssignOp(assign_op));

    // input: lhs_str = rhs_str
    // output: cs_assign_and_copy(&lhs_str, &rhs_str)

    // In cs_assign_and_copy: *lhs_str = *rhs_str; rtc_copy(lhs_str, rhs_str); return lhs_str
    
    SgExpression* rhs_copy = SI::copyExpression(isSgAssignOp(assign_op)->get_rhs_operand());
    #ifdef STRIP_TYPEDEFS
    rhs_copy = Util::castAwayTypedef(rhs_copy);
    #endif

    SgExpression* lhs_copy = SI::copyExpression(isSgAssignOp(assign_op)->get_lhs_operand());
    #ifdef STRIP_TYPEDEFS
    lhs_copy = Util::castAwayTypedef(lhs_copy);
    #endif

    SgExpression* lhs_addr = Util::createAddressOfOpFor(lhs_copy);
    SgExpression* rhs_addr = Util::createAddressOfOpFor(rhs_copy);

    SgExprListExp* param_list = SB::buildExprListExp(lhs_addr, rhs_addr);

    SgType* retType = lhs_copy->get_type();
    ROSE_ASSERT(!isSgTypedefType(retType));

    SgExpression* overload = buildMultArgOverloadFn("cs_assign_and_copy", param_list, retType,
                                                    Util::getScopeForExp(assign_op),
                                                    GEFD(SI::getEnclosingStatement(assign_op)));
    
    SI::replaceExpression(assign_op, overload);

    return overload;
                        
}

void ArithCheck::handleVarDecls4(SgVariableDeclaration* var_decl) {

    SgInitializedName* var_name = Util::getNameForDecl(var_decl);

    SgType* var_type = var_name->get_type();

    #ifdef STRIP_TYPEDEFS
    var_type = Util::getType(var_type);
    #endif

    if(isSgPointerType(var_type)) {
        #ifdef VAR_DECL_DEBUG
        printf("variable is a pointer\n");
        #endif
        handlePointerVars(var_decl);
    }
    #if 1
    else if(isSgArrayType(var_type)) {
        #ifdef VAR_DECL_DEBUG
        printf("variable is an array\n");
        #endif
        handleArrayVars(var_decl);
    }
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(var_type)) {
        handleClassStructVars(var_decl);
    }
    #endif
    else {
        printf("What is this variable decl doing here?\n");
        Util::printNode(var_decl);
        ROSE_ASSERT(0);
    }
    #endif
}

SgVariableSymbol* ArithCheck::checkInClassStructVarRemap(SgVariableSymbol* var_sym) {
    ROSE_ASSERT(var_sym != NULL);
    VariableSymbolMap_t::iterator iter = ArithCheck::ClassStructVarRemap.find(var_sym);
    if(iter == ArithCheck::ClassStructVarRemap.end()) {
        return NULL;
    }
    
    return iter->second;
}



void ArithCheck::handleClassStructVarRefs(SgVarRefExp* var_ref) {
    // This function replaces class object var refs/struct var refs
    // with the correct version.. since, the object/var might have been
    // replaced in the func param list by a ref... this replacement
    // is done so that we can copy over the pointers... The actual 
    // pointer copy occurs when the func param is replaced. At that
    // place, we insert into the ClassStructVarRemap. Here, we check that
    // map to see if we need to replace the current var ref with a new one

    #ifdef CLASS_STRUCT_VAR_REF_DEBUG
    printf("handleClassStructVarRefs\n");
    #endif

    SgVariableSymbol* var_symbol = var_ref->get_symbol();
    ROSE_ASSERT(var_symbol != NULL);
    VariableSymbolMap_t::iterator iter = ArithCheck::ClassStructVarRemap.find(var_ref->get_symbol());
    // FIXME: Should not be equal to NULL... correct?
    if(iter == ArithCheck::ClassStructVarRemap.end()) {
        // This var ref need not be replaced
        return;
    }

    SgVariableSymbol* str_symbol = iter->second;
        
    ROSE_ASSERT(str_symbol != NULL);
    SgVarRefExp* str_var_ref = SB::buildVarRefExp(isSgVariableSymbol(str_symbol));
    ROSE_ASSERT(str_var_ref != NULL);
    SI::replaceExpression(var_ref, str_var_ref);
    #ifdef CLASS_STRUCT_VAR_REF_DEBUG
    printf("Done\n");
    #endif

    
}

// For normal (non-pointer) variables, check if the variable
// has been initialized before this use.
void ArithCheck::insertInitCheckAtVarRef(SgVarRefExp* var_ref) {

    // 1. Find the scope lock for this variable
    // 2. If var is LVal, insert update.
    // 3. If var is not LVal, insert check.
    
    // 1. Find the scope for this variable
    SgInitializedName* name = var_ref->get_symbol()->get_declaration();
    // FIXME: The scope for function parameters is different from
    // the variable in function body. Lock and key are in the function body
    // so, we might not find the lock and key for the function body here
    // if the "name" is a parameter
    // Simple check for this would be to see the parent of the name, and 
    // if its a param list, then just use the scope of the function body
    // to find the lock and key.
    SgScopeStatement* scope = name->get_scope();
    LockKeyPair lock_key = findInSLKM(scope);
    SgVariableDeclaration* lock = lock_key.first;

    // check/update(lock, &var)
    SgExpression* ce1 = SB::buildVarRefExp(lock);
    SgExpression* ce2 = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(var_ref)));
    SgExprListExp* param_list = SB::buildExprListExp(ce1, ce2);

    SgExpression* overload;
    // 2. If the var is LVal, insert update
    if(var_ref->get_lvalue()) {
        // LValues write to the location, so they update initialization info.
        overload = buildMultArgOverloadFn("update_initinfo", param_list, SgTypeVoid::createType(), 
                                    SI::getScope(var_ref), GEFD(SI::getEnclosingStatement(var_ref)));
    }
    else {
        // Check if location is initialized before accessing it.
        overload = buildMultArgOverloadFn("check_initinfo", param_list, SgTypeVoid::createType(), 
                                    SI::getScope(var_ref), GEFD(SI::getEnclosingStatement(var_ref)));
    }
    
    // original: var
    // transformed: check/update(lock, &var), var
    SgCommaOpExp* comma_op = SB::buildCommaOpExp(overload, SI::copyExpression(var_ref));
    
    SI::replaceExpression(var_ref, comma_op);
        
}

void ArithCheck::handleNormalVarRefs(SgVarRefExp* var_ref) {

    if(Util::isDotOrArrow(var_ref->get_parent())) {
        #ifdef NORMAL_VAR_REF_DEBUG
        printf("Parent is dot/arrow exp\n");
        Util::printNode(var_ref->get_parent());
        printf("returning\n");
        #endif
        return;
    }

    insertInitCheckAtVarRef(var_ref);
}

void ArithCheck::handleVarRefs4(SgVarRefExp* var_ref) {
    // FIXME: Skip cases where parent is 
    // arrow or dot exp... and this is the rhs!
    #ifdef VAR_REF_DEBUG
    printf("handleVarRefs4\n");
    #endif
    ROSE_ASSERT(var_ref->get_symbol() != NULL);
    
    SgType* var_type = var_ref->get_type();

    #ifdef STRIP_TYPEDEFS
    var_type = Util::getType(var_type);
    #endif

    // Removing these checks since parameter inputs are 
    // removed from the function, and replaced by structs.
    // using get_type() seg faults.
    #if 1
    #ifdef SUPPORT_REFERENCES
    if(isSgPointerType(var_type) || isSgReferenceType(var_type)) 
    #else
    if(isSgPointerType(var_type))
    #endif
    {
        handlePointerVarRefs(var_ref);
    }
    else if(isSgArrayType(var_type)) {
        handleArrayVarRefs(var_ref);
    }
    #ifdef CLASS_STRUCT_COPY
    else if(isSgClassType(var_type)) {
        handleClassStructVarRefs(var_ref);
    }
    #endif
    else {
        #ifdef INIT_CHECKS
        handleNormalVarRefs(var_ref);
        #else
        printf("What is this var ref doing here?\n");
        Util::printNode(var_ref);
        ROSE_ASSERT(0);
        #endif
    }
    #endif


}

void ArithCheck::handleFuncCalls4(SgFunctionCallExp* fncall) {
    handleFuncCalls(fncall);
}

void ArithCheck::handleDerefs4(SgPointerDerefExp* deref_exp) {
    handleDerefExp(deref_exp);
}

void ArithCheck::handlePntrArrRefs4(SgPntrArrRefExp* pntr_arr_ref) {
    handlePntrArrRefExp(pntr_arr_ref);
}

void ArithCheck::handleDotExp4(SgDotExp* dot_exp) {

    SgType* dot_exp_type = dot_exp->get_type();
    #ifdef STRIP_TYPEDEFS
    dot_exp_type = Util::getType(dot_exp_type);
    #endif

    if(Util::isQualifyingType(dot_exp_type)) {
        handleDotExp(dot_exp);
    }
    else {
        printf("What is this dot exp?");
        Util::printNode(dot_exp);
        ROSE_ASSERT(0);
    }

}

SgExpression* ArithCheck::handleLHSArrowExp(SgArrowExp* arrow_exp) {
    #ifdef LHSARROW_DEBUG
    printf("handleLHSArrowExp\n");    
    #endif

    SgExpression* lhs = arrow_exp->get_lhs_operand();
    SgType* lhs_type = lhs->get_type();

    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif

    // If this is a struct/var ref/another arrow/dot expression
    // we simply call deref_check/deref_check_with_str

    if(isSgThisExp(lhs)) {
        // this->val... nothing to do...
        return arrow_exp;
    }
    else {

        SgExprListExp* param_list;
        SgExpression* overload;

        // We'll be using deref_check for the var_ref/dot/arrow
        // case
        #ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(lhs_type, GEFD(arrow_exp))) 
        #else
        if(strDecl::isValidStructType(lhs_type))
        #endif
        {
            
            param_list = SB::buildExprListExp(SI::copyExpression(lhs));

            overload = buildMultArgOverloadFn("deref_check_with_str", param_list,
                        strDecl::findInUnderlyingType(lhs_type),
                        Util::getScopeForExp(arrow_exp),
                        GEFD(SI::getEnclosingStatement(arrow_exp)));
        }
        else if(Util::isVarRefOrDotArrow(lhs)) {
            
            SgExpression* lhs_ptr = SI::copyExpression(lhs);
            
            #ifdef STRIP_TYPEDEFS
            lhs_ptr = Util::castAwayTypedef(lhs_ptr);
            #endif

            SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));

            param_list = SB::buildExprListExp(lhs_ptr, lhs_addr);
            
            overload = buildMultArgOverloadFn("deref_check", param_list,
                        lhs_type, 
                        Util::getScopeForExp(arrow_exp),
                        GEFD(SI::getEnclosingStatement(arrow_exp)));
            
        }
        else {
            printf("Arrow LHS not supported\n");
            printf("arrowlhs\n");
            Util::printNode(lhs);
            ROSE_ASSERT(0);
        }

        SgExpression* new_arrow_exp = SB::buildArrowExp(overload, SI::copyExpression(arrow_exp->get_rhs_operand()));    

        SI::replaceExpression(arrow_exp, new_arrow_exp);
        return new_arrow_exp;
    }
}
            
SgExpression* ArithCheck::handleRHSArrowExp(SgArrowExp* arrow_exp) {

    #ifdef RHSARROW_DEBUG
    printf("handleRHSArrowExp\n");    
    #endif

    SgExpression* rhs = arrow_exp->get_rhs_operand();

    // Here, there are two cases... 
    // either the rhs is a var_ref/function call
    // If its a var ref, we don't handle it here.. it'll be handled
    // at the next higher op.
    // If its a func call, its already converted to a struct
    // So, nothing to do in either case
    return arrow_exp;
}

void ArithCheck::handleArrowExp4(SgArrowExp* arrow_exp) {

    // Handle LHS of Arrow exp... and then handle
    // the RHS...
    // This function replaces the original arrow exp with the
    // new one.
    SgExpression* new_arrow_exp = handleLHSArrowExp(arrow_exp);

    // Now to handle the RHS...
    SgExpression* final_arrow_exp = handleRHSArrowExp(isSgArrowExp(new_arrow_exp));

}

void ArithCheck::handleCastExp4(SgCastExp* cast_exp) {


    SgType* cast_type = cast_exp->get_type();
    
    #ifdef STRIP_TYPEDEFS
    cast_type = Util::getType(cast_type);
    #endif

    SgExpression* oper = cast_exp->get_operand();
    SgType* oper_type = oper->get_type();

    #ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
    #endif

    #if 1
    #ifdef CPP_SCOPE_MGMT
    if(Util::isQualifyingType(cast_type) || 
        strDecl::isValidStructType(oper_type, GEFD(cast_exp))) 
    #else
    if(Util::isQualifyingType(cast_type) || 
        strDecl::isValidStructType(oper_type))
    #endif
    {
        handleCastExp(cast_exp);
    }
    else {
        printf("What is this cast exp?");
        Util::printNode(cast_exp);
        ROSE_ASSERT(0);
    }
    #endif
}

void ArithCheck::handleAddressOfOp4(SgAddressOfOp* addr_of) {
    handleAddressOfOp(addr_of);
}

SgExpression* ArithCheck::getRHSClassStruct(SgExpression* rhs) {
    
    if(isSgVarRefExp(rhs)) {
        ROSE_ASSERT(strDecl::isOriginalClassType(rhs->get_type()));
        return rhs;
    }
    else if(isSgCommaOpExp(rhs)) {
        SgExpression* comma_rhs = isSgCommaOpExp(rhs)->get_rhs_operand();
        ROSE_ASSERT(isSgVarRefExp(comma_rhs) && strDecl::isOriginalClassType(comma_rhs->get_type()));
        return comma_rhs;
    }
    else if(isSgAssignInitializer(rhs)) {
        SgExpression* oper = isSgAssignInitializer(rhs)->get_operand();
        // Inside the assign initializer, we can handle any of the given cases.
        return getRHSClassStruct(oper);
    }
    else if(isSgConstructorInitializer(rhs)) {
        //FIXME: constructor initializer is not supported yet
        return NULL;        
    }
    else {
        printf("getRHSClassStruct: incompatible rhs: ");
        Util::printNode(rhs);
        ROSE_ASSERT(0);
    }
}


void ArithCheck::handleClassStructAssigns(SgAssignOp* assign_op) {
    
    // lhs is original class type
    ROSE_ASSERT(strDecl::isOriginalClassType(assign_op->get_lhs_operand()->get_type()));
    

    SgExpression* rhs = getRHSClassStruct(assign_op->get_rhs_operand());
    if(rhs == NULL) {
        // Must be unsupported RHS
        return;
    }

    SgExpression* ce1 = Util::createAddressOfOpFor(SI::copyExpression(assign_op->get_lhs_operand())); 
    SgExpression* ce2 = Util::createAddressOfOpFor(SI::copyExpression(rhs));
    
    #if 0
    SgExpression* overload = buildOverloadFn("rtc_copy", ce1, ce2, SgTypeVoid::createType(), 
                                SI::getScope(assign_op), GEFD(SI::getEnclosingStatement(assign_op)));
    #endif
    SgExpression* overload = buildMultArgOverloadFn("rtc_copy", SB::buildExprListExp(ce1, ce2), SgTypeVoid::createType(), 
                                SI::getScope(assign_op), GEFD(SI::getEnclosingStatement(assign_op)));

    SgExpression* comma_op = SB::buildCommaOpExp(SI::copyExpression(assign_op), overload);

    // original: s1 = s2;
    // transformed: s1 = s2, rtc_copy(&s1, &s2);
    SI::replaceExpression(assign_op, comma_op);

    return;
}

bool ArithCheck::existsInSLKM(SgScopeStatement* scope) {

    ScopeLockKeyMap::iterator it = ArithCheck::SLKM.find(scope);
    return (it != SLKM.end());
}

LockKeyPair ArithCheck::findInSLKM(SgScopeStatement* scope) {
    ScopeLockKeyMap::iterator it = ArithCheck::SLKM.find(scope);
    ROSE_ASSERT(it != SLKM.end());
    return it->second;
}

SgType* ArithCheck::getLockType() {
    return SgTypeLongLong::createType();
}

SgType* ArithCheck::getKeyType() {
    return SgTypeLongLong::createType();
}

void ArithCheck::handleGlobalScope(SgGlobal* global) {

    // Create the lock and key variables in global scope.
    // In main: 
    // EnterScope();
    // lock = getTopLock();
    // key = getTopKey();
    // .... rest of main
    // ExitScope();
    // return;
    // FIXME: Add case where we handle arbitrary exits from main
    // This can be handled similar to the way returns are handled
    // for basic blocks.

    SgScopeStatement* scope = isSgScopeStatement(global);        

    // Insert lock and key variables at the top of the global scope
    // lock variable
    SgName lock_name("lock_var" + boost::lexical_cast<std::string>(Util::VarCounter));
    SgVariableDeclaration* lock_var = Util::createLocalVariable(lock_name, getLockType(), NULL, scope);
    // Add declaration at the top of the scope
    scope->prepend_statement(lock_var);

    // key variable
    // **** IMPORTANT: Using same counter value for lock and key
    SgName key_name("key_var" + boost::lexical_cast<std::string>(Util::VarCounter++));
    SgVariableDeclaration* key_var = Util::createLocalVariable(key_name, getKeyType(), NULL, scope);
    // Insert this key decl after the lock decl
    SI::insertStatementAfter(lock_var, key_var);


    // Now, find the main function and insert... 
    // EnterScope();
    // lock = getTopLock();
    // key = getTopKey();
    // .... rest of main
    // ExitScope()
    // return; -- this already exists...
    // see FIXME above

    // find main function...
    SgFunctionDeclaration* MainFn = SI::findMain(global);
    if(!MainFn) {
        #ifdef HANDLE_GLOBAL_SCOPE_DEBUG
        printf("Can't find Main function. Not inserting Global Enter and Exit Scopes\n");
        #endif
        return;
    }

    SgBasicBlock *bb = Util::getBBForFn(MainFn);
        
    // insert EnterScope()    
    #if 0
    SgExpression* overload = buildOverloadFn("EnterScope", NULL, NULL, SgTypeVoid::createType(), scope,
                                            GEFD(bb));
    #endif
    SgExpression* overload = buildMultArgOverloadFn("EnterScope", SB::buildExprListExp(), SgTypeVoid::createType(), scope,
                                            GEFD(bb));

    SgStatement* enter_scope = SB::buildExprStatement(overload);
    Util::insertAtTopOfBB(bb, enter_scope);

    // insert lock = getTopLock();
    //overload = buildOverloadFn("getTopLock", NULL, NULL, getLockType(), scope, GEFD(bb));
    overload = buildMultArgOverloadFn("getTopLock", SB::buildExprListExp(), getLockType(), scope, GEFD(bb));
    SgStatement* lock_assign = SB::buildExprStatement(SB::buildAssignOp(SB::buildVarRefExp(lock_var), overload));
    SI::insertStatementAfter(enter_scope, lock_assign);

    // insert key = getTopKey();
    // overload = buildOverloadFn("getTopKey", NULL, NULL, getKeyType(), scope, GEFD(bb));
    overload = buildMultArgOverloadFn("getTopKey", SB::buildExprListExp(), getKeyType(), scope, GEFD(bb));
    SgStatement* key_assign = SB::buildExprStatement(SB::buildAssignOp(SB::buildVarRefExp(key_var), overload));
    SI::insertStatementAfter(lock_assign, key_assign);

    // add to scope -> lock and key map... SLKM
    LockKeyPair lock_key = std::make_pair(lock_var, key_var);
    ArithCheck::SLKM[scope] = lock_key;

    ROSE_ASSERT(existsInSLKM(scope));

    // Insert ExitScope if last stmt is not return.
    SgStatementPtrList& stmts = bb->get_statements();
    SgStatementPtrList::iterator it = stmts.begin();
    it += (stmts.size() - 1);

    // A little iffy on the scope part here... lets check that.
    if(!isSgReturnStmt(*it)) {
        // Last statement is not return. So, add exit scope... 
        // If its a break/continue statement, insert statement before,
        // otherwise, add exit_scope afterwards.
        //SgExpression* overload = buildOverloadFn("ExitScope", NULL, NULL, SgTypeVoid::createType(), scope, GEFD(bb));
        SgExpression* overload = buildMultArgOverloadFn("ExitScope", SB::buildExprListExp(), SgTypeVoid::createType(), scope, GEFD(bb));

        // check if its break/continue
        if(isSgBreakStmt(*it) || isSgContinueStmt(*it)) {
            SI::insertStatementBefore(*it, SB::buildExprStatement(overload));
        }
        else {
            SI::insertStatementAfter(*it, SB::buildExprStatement(overload));
        }
    }

}

void ArithCheck::handleBasicBlocks(SgBasicBlock* bb) {

    // 1. Add Enter Scope -- This will be removed when RTED ScopeGuard is used
    // 2. Insert lock and key variables at the top of stack.
    // 3. Create a map for lock and key with each scope
    // 4. Insert ExitScope statement if the last statement in the list isn't
    // a return stmt. Return stmts are handled in handleReturnStmts

    SgScopeStatement* scope = isSgScopeStatement(bb);

    // 1. Add Enter Scope
    #if 0
    SgExpression* overload = buildOverloadFn("EnterScope", NULL, NULL, SgTypeVoid::createType(), scope,
                                            GEFD(bb));
    #endif
    SgExpression* overload = buildMultArgOverloadFn("EnterScope", SB::buildExprListExp(), SgTypeVoid::createType(), scope,
                                            GEFD(bb));

    SgStatement* enter_scope = SB::buildExprStatement(overload);
    Util::insertAtTopOfBB(bb, enter_scope);

    // 2. Insert lock and key variables
    // lock calls "getTopLock"
    // key calls "getTopKey"
    #if 0
    overload = buildOverloadFn("getTopLock", NULL, NULL, getLockType(), scope,
                                            GEFD(bb));
    #endif
    overload = buildMultArgOverloadFn("getTopLock", SB::buildExprListExp(), getLockType(), scope,
                                            GEFD(bb));
    // **** IMPORTANT: Using same counter value for lock and key
    SgName lock_name("lock_var" + boost::lexical_cast<std::string>(Util::VarCounter));
    SgVariableDeclaration* lock_var = Util::createLocalVariable(lock_name, getLockType(), overload, scope);
    // Insert this lock declaration after the EnterScope
    SI::insertStatementAfter(enter_scope, lock_var);

    // For the key...
    #if 0
    overload = buildOverloadFn("getTopKey", NULL, NULL, getKeyType(), scope,
                                GEFD(bb));
    #endif
    overload = buildMultArgOverloadFn("getTopKey", SB::buildExprListExp(), getKeyType(), scope,
                                GEFD(bb));

    // **** IMPORTANT: Using same counter value for lock and key
    SgName key_name("key_var" + boost::lexical_cast<std::string>(Util::VarCounter++));
    SgVariableDeclaration* key_var = Util::createLocalVariable(key_name, getKeyType(), overload, scope);
    // Insert this key decl after the lock decl
    SI::insertStatementAfter(lock_var, key_var);

    // Add to scope -> lock and key map
    LockKeyPair lock_key = std::make_pair(lock_var, key_var);
    ArithCheck::SLKM[scope] = lock_key;

    ROSE_ASSERT(existsInSLKM(scope));

    // 4. Insert ExitScope if last stmt is not return.
    SgStatementPtrList& stmts = bb->get_statements();
    SgStatementPtrList::iterator it = stmts.begin();
    it += (stmts.size() - 1);

    if(!isSgReturnStmt(*it)) {
        // Last statement is not return. So, add exit scope... 
        // If its a break/continue statement, insert statement before,
        // otherwise, add exit_scope afterwards.
        //SgExpression* overload = buildOverloadFn("ExitScope", NULL, NULL, SgTypeVoid::createType(), scope, GEFD(bb));
        SgExpression* overload = buildMultArgOverloadFn("ExitScope", SB::buildExprListExp(), SgTypeVoid::createType(), scope, GEFD(bb));
        // check if its break/continue
        if(isSgBreakStmt(*it) || isSgContinueStmt(*it)) {
            SI::insertStatementBefore(*it, SB::buildExprStatement(overload));
        }
        else {
            SI::insertStatementAfter(*it, SB::buildExprStatement(overload));
        }
    }

    return;    
}


void ArithCheck::handleNormalReturnStmts(SgReturnStmt* retstmt) {

    // We handle normal return statements (int/char returns... this also handles returns for main... for global scope)
    // So, add one extra ExitScope if the current function is main... this ExitScope removes the global scope from the 
    // scope stack...
    //SgExpression* overload = buildOverloadFn("ExitScope", NULL, NULL, SgTypeVoid::createType(), SI::getScope(retstmt), GEFD(retstmt));
    SgExpression* overload = buildMultArgOverloadFn("ExitScope", SB::buildExprListExp(), SgTypeVoid::createType(), SI::getScope(retstmt), GEFD(retstmt));
    SI::insertStatementBefore(retstmt, SB::buildExprStatement(overload));

    // if the current function is main...
    SgFunctionDeclaration* parent_fn = Util::getFnFor(retstmt);            
    if(SI::isMain(parent_fn)) {
        // insert ExitScope again... to remove the global scope lock and key
        SgExpression* exit_scope_2 = SI::copyExpression(overload);
        SI::insertStatementBefore(retstmt, SB::buildExprStatement(exit_scope_2));
    }
}

void ArithCheck::handleQualReturnStmts(SgReturnStmt* retstmt) {

    SgExpression* exp = retstmt->get_expression();
    SgType* exp_type = exp->get_type();
    #ifdef STRIP_TYPEDEFS
    exp_type = Util::getType(exp_type);
    #endif

    if(isSgArrayType(exp_type)) {
        // Need to insert create entry as well...
        insertCreateEntry(SI::copyExpression(exp), SI::getScope(retstmt),
                        SI::getEnclosingStatement(retstmt));
    }

    // call create_struct
    SgExpression* ptr = SI::copyExpression(exp);
    #ifdef STRIP_TYPEDEFS
    ptr = Util::castAwayTypedef(ptr);
    #endif

    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(exp)));
    

    #ifdef RETAIN_FUNC_PROTOS
    // transform from: return ptr to: return (push(addr), ptr)
    // 1. push(addr)
    SgExpression* pushcall = buildMultArgOverloadFn("push_to_stack", SB::buildExprListExp(addr),
                                SgTypeVoid::createType(), SI::getScope(retstmt),
                                GEFD(retstmt));
    
    // 2. push(addr), ptr
    SgExpression* ret_exp = SB::buildCommaOpExp(pushcall, ptr);
    // 3. replace exp with ret_exp
    SI::replaceExpression(exp, ret_exp);

    #else
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp_type),
                                GEFD(SI::getEnclosingStatement(retstmt)),
                                true);
    
    SgExprListExp* param_list = SB::buildExprListExp(ptr, addr);

    SgExpression* fncall = buildMultArgOverloadFn("create_struct", param_list,
                                                retType, SI::getScope(retstmt),
                                                GEFD(SI::getEnclosingStatement(retstmt)));
    
    SI::replaceExpression(exp, fncall);
    #endif
}

void ArithCheck::unwrapStructAndPush(SgStatement* retstmt) {

    SgExpression* exp = isSgReturnStmt(retstmt)->get_expression();

    // transform from: str_var to: str temp; return ((temp = exp, push(temp.addr)), str.ptr)

    // read the struct into a variable...
    // 1. create a local varible... argvar
    SgName var_name("temp_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    SgVariableDeclaration* temp = SB::buildVariableDeclaration(var_name, exp->get_type(), NULL, SI::getScope(retstmt));
    SI::insertStatementBefore(retstmt, temp);    

    // 2. temp = exp;
    SgExpression* temp_assign = SB::buildAssignOp(SB::buildVarRefExp(temp), SI::copyExpression(exp));

    // 3. push(temp.addr)
    SgExprListExp* cs_p = SB::buildExprListExp(Util::createDotExpFor(Util::getNameForDecl(temp), "addr"));
    SgExpression* pushcall = buildMultArgOverloadFn("push_to_stack", cs_p,
                                            SgTypeVoid::createType(), SI::getScope(retstmt),
                                            GEFD(retstmt));
    
    // 4. temp=exp, push(temp.addr)
    SgExpression* comma_op = SB::buildCommaOpExp(temp_assign, pushcall);

    // 5. ((temp=exp, push(temp.addr)), str.ptr)
    comma_op = SB::buildCommaOpExp(comma_op, Util::createDotExpFor(Util::getNameForDecl(temp), "ptr"));

    // 6. replace exp with comma_op
    SI::replaceExpression(exp, comma_op);
}

void ArithCheck::handleReturnStmts(SgReturnStmt* retstmt) {
    
    #ifdef RETURN_STMT_DEBUG
    printf("handleReturnStmts\n");
    Util::printNode(retstmt);
    #endif

    SgExpression* exp = retstmt->get_expression();
    SgType* exp_type = exp->get_type();
    #ifdef STRIP_TYPEDEFS
    exp_type = Util::getType(exp_type);
    #endif

    if(strDecl::isOriginalClassType(exp_type)) {
        // This would need to be handled slightly differently... 
        #ifdef RETURN_STMT_DEBUG
        printf("exp is orig class type\n");
        #endif
        handleClassStructReturnStmts(retstmt);
    }
    else if(Util::isQualifyingType(exp_type)) {
        // This needs to be converted to a struct
        #ifdef RETURN_STMT_DEBUG
        printf("exp is qual type\n");
        #endif
        handleQualReturnStmts(retstmt);
    }
    #ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(exp_type, GEFD(retstmt))) 
    #else
    else if(strDecl::isValidStructType(exp_type)) 
    #endif
    {
        #ifdef RETAIN_FUNC_PROTOS
        // Should return the pointer, and push the
        // metadata onto the stack
        unwrapStructAndPush(retstmt);    
        #endif
        // The exp has been converted to valid struct type
        // No need to convert
        #ifdef LOCK_KEY_INSERT
        handleNormalReturnStmts(retstmt);
        #endif
    }
    #ifdef LOCK_KEY_INSERT
    else {
        // Here, we simply insert exit_scope
        handleNormalReturnStmts(retstmt);
    }
    #else
    else {
        // We shouldn't be here if we are not inserting exit scope
        printf("handleReturnStmts: retstmt: ");
        Util::printNode(retstmt);
        printf("What are we doing here?");
        ROSE_ASSERT(0);
    }
    #endif
    return;
}

void ArithCheck::setProject(SgProject* proj) {
    ArithCheck::ACProject = proj;
}

void ArithCheck::handleThisExp(SgThisExp* exp) {
    // create an entry for "this"...
    // create_entry(&this, this, sizeof(*this), Util::getDummyLock())
    // FIXME: Can't use create_entry since it returns void. And in the fn call,
    // we actually deref "this", which itself could fail. Instead, lets do a dummy
    // entry on this for now.
    SgType* exp_type = exp->get_type();
    #ifdef STRIP_TYPEDEFS
    exp_type = Util::getType(exp_type);
    #endif
    
    #if 0
    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(exp)));
    SgExpression* ptr = SI::copyExpression(exp);
    SgExpression* size = SB::buildSizeOfOp(SB::buildPointerDerefExp(SI::copyExpression(exp)));
    SgExpression* lock = Util::getDummyLock();
    SgExprListExp* params = SB::buildExprListExp(addr, ptr, size, lock);

    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp_type),
                                GEFD(SI::getEnclosingStatement(exp)),
                                true);

    SgExpression* ovl = buildMultArgOverloadFn("create_entry", params, 
                                retType,
                                Util::getScopeForExp(exp),
                                GEFD(SI::getEnclosingStatement(exp)));

    SI::replaceExpression(exp, ovl);
    #endif

    // Create a dummy entry prior to this use... 
    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(exp)));
    SgExpression* ovl = buildMultArgOverloadFn("create_dummy_entry", SB::buildExprListExp(addr),
                                SgTypeVoid::createType(),
                                Util::getScopeForExp(exp),
                                GEFD(SI::getEnclosingStatement(exp)));    
    SI::insertStatementBefore(Util::getSuitablePrevStmt(SI::getEnclosingStatement(exp)), SB::buildExprStatement(ovl));

    // Now, simply create struct(this, &this)...
    SgExpression* ptr = SI::copyExpression(exp);
    SgExpression* addr_copy = SI::copyExpression(addr);
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp_type),
                                GEFD(SI::getEnclosingStatement(exp)),
                                true);

    ovl = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(ptr, addr_copy),
                                            retType,
                                            Util::getScopeForExp(exp),
                                            GEFD(SI::getEnclosingStatement(exp)));
    
    SI::replaceExpression(exp, ovl);
                                    

}

void ArithCheck::handleNewExp(SgNewExp* ne) {
    
    #ifdef NEW_EXP_DEBUG
    printf("Begin handleNewExp\n");
    printf("new_exp\n");
    Util::printNode(ne);
    #endif

    // Original:
    // new class A;
    // Transformed
    // class A* newvar;
    // newvar = new class A, create_entry_with_new_lock_gen(&newvar, newvar, newvar + alloc_size)
    // where alloc_size = sizeof(class A) if its a single object..
    // and alloc_size = array_size*sizeof(class A) if its an array type

    SgType* nty = ne->get_type();
    printf("new_exp->get_type()\n");
    Util::printNode(nty);

    // Converting it into a single pointer... stripping out array types
    // For the case: class A* ptr = new class A [5];
    // Here, the RHS is of type A (*)[5], whereas the LHS is of type A (*)
    #ifdef STRIP_ARRAY_TYPE
    nty = Util::skip_ArrPntrRefTypedefs(nty);
    nty = SgPointerType::createType(nty);
    #endif

    SgType* sty = ne->get_specified_type();
    printf("new_exp->get_specified_type()\n");
    Util::printNode(sty);


    ROSE_ASSERT(isSgPointerType(nty));

    SgConstructorInitializer* ci = ne->get_constructor_args();
    SgExprListExp* explist = ci->get_args();

    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(nty), 
                            GEFD(SI::getEnclosingStatement(ne)), 
                            true); 

    SgExpression* alloc_size;

    if(SgArrayType* arr = isSgArrayType(sty)) {
        // We are allocating an array of objects here... 
        // So, we pass the index for this number of objects
        // to be created into the new_ovl function.
        // Also, there shouldn't be any constructor args... 
        // Can't initialize objects within array
        ROSE_ASSERT(explist->empty());
        
        SgExpression* index = SI::copyExpression(arr->get_index());
        SgType* base = arr->get_base_type();
        SgExpression* sizeof_base = SB::buildSizeOfOp(base);

        // The total allocation size is: index*sizeof(base_type)
        alloc_size = SB::buildMultiplyOp(index, sizeof_base);
    }
    else {
        // This is the case where we are creating a single object... 
        // And there might be constructor args... 
        // The allocation size will be simply the size of the specified type
        alloc_size = SB::buildSizeOfOp(sty);
    }

    // Create a local variable which will hold the value of the new_exp
    // This variable will have the starting value for the new_exp. 
    // The variable will also provide an address at which the metadata will
    // be stored.

    // Create a variable declaration
    // 1. create a local varible... newvar of nty
    SgName var_name("newvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    SgVariableDeclaration* newvar = SB::buildVariableDeclaration(var_name, nty, NULL, SI::getScope(ne));
    SI::insertStatementBefore(SI::getEnclosingStatement(ne), newvar);    

    // assign the new exp to newvar
    SgExpression* newvar_assign = SB::buildAssignOp(SB::buildVarRefExp(newvar), SI::copyExpression(ne)); 

    // build the params for create_entry_with_new_lock_gen
    SgExprListExp* plist = SB::buildExprListExp(Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(newvar))),
                                            SB::buildVarRefExp(newvar), 
                                            SB::buildCastExp(alloc_size, Util::getSizeOfType(),SgCastExp::e_C_style_cast));

    // Now, call create_entry_with_new_lock_gen
    SgExpression* ovl = buildMultArgOverloadFn("create_entry_with_new_lock_gen", plist, retType,
                                                Util::getScopeForExp(ne),
                                                GEFD(SI::getEnclosingStatement(ne)));

    // Build a comma op exp with newvar_assign and ovl
    SgExpression* cop = SB::buildCommaOpExp(newvar_assign, ovl);

    // replace new_exp with comma op
    SI::replaceExpression(ne, cop);
    
    #ifdef NEW_EXP_DEBUG
    printf("Done with handleNewExp\n");
    #endif
}

void ArithCheck::handleDeleteExp(SgDeleteExp* del) {

    // original: delete(ptr)
    // final: delete(ptr), remove_entry(&ptr)
    // FIXME: May want to check in remove_entry if the other elements
    // within the class have been deallocated
    
    SgExpression* var = del->get_variable();
    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(var)));

    SgExpression* ovl = buildMultArgOverloadFn("remove_entry", SB::buildExprListExp(addr),
                                                SgTypeVoid::createType(),
                                                Util::getScopeForExp(del),
                                                GEFD(SI::getEnclosingStatement(del)));

    SgExpression* del_copy = SI::copyExpression(del);

    // replace delete(ptr) with delete(ptr), remove_entry(&ptr)
    SI::replaceExpression(del, SB::buildCommaOpExp(del_copy, ovl));
}
        
void ArithCheck::InstrumentNodes4(SgProject* project) {

    #ifdef IN4_DEBUG
    printf("InstrumentNodes4 - begin\n");
    #endif
    
    
    for(NodeContainer::iterator nit = Trav::NodesToInstrument.begin(); nit != Trav::NodesToInstrument.end(); ++nit) {
        
        SgNode* node = *nit;
        Sg_File_Info* File = isSgNode(node)->get_file_info();

        // print statements in SgGlobal
        //Util::printStatements(SI::getGlobalScope(node));

        #ifdef IN4_DEBUG
        printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
        printf("File: (%s, %d, %d) %s = %s\n", File->get_filenameString().c_str(),
                                            File->get_line(),
                                            File->get_col(),
                                            isSgNode(node)->sage_class_name(),
                                            isSgNode(node)->unparseToString().c_str());
        #endif


        #ifndef IN4_ITERATE_ONLY

        #ifdef IN4_DEBUG
        printf("is variable declaration?");
        #endif
        if(isSgVariableDeclaration(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleVarDecls4(isSgVariableDeclaration(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }
            
        #if 1
        #ifdef IN4_DEBUG
        printf("is variable reference?");
        #endif
        if(isSgVarRefExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleVarRefs4(isSgVarRefExp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef INSTR_THIS_EXP
        #ifdef IN4_DEBUG
        printf("is this exp?");
        #endif
        if(isSgThisExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleThisExp(isSgThisExp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }
        #endif
        
        #ifdef IN4_DEBUG
        printf("is func call?");
        #endif
        if(isSgFunctionCallExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleFuncCalls4(isSgFunctionCallExp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef IN4_DEBUG
        printf("is deref exp?");
        #endif
        if(isSgPointerDerefExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleDerefs4(isSgPointerDerefExp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef IN4_DEBUG
        printf("is pntr arr ref?");
        #endif
        if(isSgPntrArrRefExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handlePntrArrRefs4(isSgPntrArrRefExp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef IN4_DEBUG
        printf("is dot exp?");
        #endif
        if(isSgDotExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleDotExp4(isSgDotExp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef IN4_DEBUG
        printf("is arrow exp?");
        #endif
        if(isSgArrowExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleArrowExp4(isSgArrowExp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef IN4_DEBUG
        printf("is cast exp?");
        #endif
        if(isSgCastExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleCastExp4(isSgCastExp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef IN4_DEBUG
        printf("is address of exp?");
        #endif
        if(isSgAddressOfOp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleAddressOfOp4(isSgAddressOfOp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef IN4_DEBUG
        printf("is comma op?");
        #endif
        if(isSgCommaOpExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            printf("Nothing to do.\n");
            #endif
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        // Conditional ops return bool... and its
        // a simple overload -- irrespective of whether
        // its a pointer or array type -- just use the
        // struct available
        #ifdef IN4_DEBUG
        printf("is conditional op?");
        #endif
        if(Util::isConditionalOp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleConditionals(isSgBinaryOp(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        // if the return type is of qualifying type..
        // change it to struct type... works the same for
        // pointers and arrays.
        #ifdef IN4_DEBUG
        printf("is function decl?");
        #endif
        if(isSgFunctionDeclaration(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleFuncDecls(isSgFunctionDeclaration(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        // We will modify handleFuncParams to handle pointers
        // and arrays differently. No need to create different
        // calls at the top level.
        #ifdef IN4_DEBUG
        printf("is param list?");
        #endif
        if(isSgFunctionParameterList(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleFuncParams(isSgFunctionParameterList(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #if defined(CLASS_STRUCT_COPY) || defined(RETAIN_FUNC_PROTOS)
        // Finding return stmts which need to be converted to assign
        // and void returns..
        #ifdef IN4_DEBUG
        printf("is return stmt?");
        #endif
        if(isSgReturnStmt(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleReturnStmts(isSgReturnStmt(node));
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }
        #endif

        #ifdef CLASS_STRUCT_COPY
        #ifdef IN4_DEBUG
        printf("is it an assign stmt?");
        #endif
        if(isSgAssignOp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            SgExpression* lhs = isSgAssignOp(node)->get_lhs_operand();
            SgType* lhs_type = lhs->get_type();
            #ifdef STRIP_TYPEDEFS
            lhs_type = Util::getType(lhs_type);
            #endif
            if(strDecl::isOriginalClassType(lhs_type)) {
                //handleClassStructAssigns(isSgAssignOp(node));    
                handleCSAssignOverloadOps(isSgAssignOp(node));
            }
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }
        #endif

        #ifdef LOCK_KEY_INSERT
        #ifdef IN4_DEBUG
        printf("LOCK_KEY_INSERT: is it a basic block?");
        #endif
        if(isSgBasicBlock(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleBasicBlocks(isSgBasicBlock(node));
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #ifdef IN4_DEBUG
        printf("LOCK_KEY_INSERT: is it global scope?");
        #endif
        if(isSgGlobal(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleGlobalScope(isSgGlobal(node));
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }

        #endif

        #ifdef NEW_OVL_ENABLED
        if(isSgNewExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleNewExp(isSgNewExp(node));
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }
        #endif

        #ifdef DELETE_OVL_ENABLED
        if(isSgDeleteExp(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleDeleteExp(isSgDeleteExp(node));
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }
        #endif


        // Changed needs to be overloaded to check for arith ops
        // only -- so this would be used to handle those ops which
        // don't change the type of pointer/array, or create 
        // a pointer/array type 
        // they simply
        // perform arithmetic operations on the given type.
        // should work the same for both pointer and array types
        #ifdef IN4_DEBUG
        printf("is it an overloadable op?");
        #endif
        if(Trav::NeedsToBeOverloaded(node)) {
            #ifdef IN4_DEBUG
            printf("Yes\n");
            #endif
            handleOverloadOps(node);
            continue;
        }
        else {
            #ifdef IN4_DEBUG
            printf("No\n");
            #endif
        }
        #endif
        // Casts from those expressions which output VoidStr should be removed -- or we could simply
        // put a Cast_Overload. Removing an expression might require clean up. Putting a Cast_Overload for
        // now would be a good idea.

        #endif

    }
    #ifdef IN4_DEBUG
    printf("InstrumentNodes4 - end\n");
    #endif

}

void ArithCheck::handleArgvInitialization(SgProject* project) {

    // Adding at the top of the main function:
    // create_dummy_entry(&argv)
    // argc_type index;
    // for(index = 0; index < argc; index++) 
    //      create_dummy_entry(&argv[index]);

    SgFunctionDeclaration* MainFn = SI::findMain(isSgNode(project));
    
    if(!MainFn) {
        #ifdef ARGV_INIT
        printf("handleArgvInit:Can't find main\n");
        #endif
        return;
    }

    SgScopeStatement* scope = MainFn->get_scope();
    
    SgFunctionDefinition* MainFnDef = MainFn->get_definition();

    SgBasicBlock* BB = MainFnDef->get_body();

    SgStatementPtrList& Stmts = BB->get_statements();

    // Now, check if it has args... and if so, there should be two
    SgInitializedNamePtrList& args = MainFn->get_args();

    ROSE_ASSERT((args.size() == 0) || (args.size() == 2));

    if(args.size() == 0) {
        // Nothing to do in this case
        return;
    }

    // Get the argc
    SgInitializedName* argc_var = *args.begin();

    // Get the argv
    SgInitializedName* argv_var = *(args.begin() + 1);

    // insert: create_dummy_entry(&argv)
    SgExpression* addr_argv = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(argv_var)));
    SgExprListExp* p_list = SB::buildExprListExp(addr_argv);

    SgExpression* argv_ovl = buildMultArgOverloadFn("create_dummy_entry", p_list, 
                            SgTypeVoid::createType(),
                            SI::getScope(BB),
                            GEFD(SI::getEnclosingStatement(BB)));
    
    SgExprStatement* argv_ovl_stmt = SB::buildExprStatement(argv_ovl);
    Util::insertAtTopOfBB(BB, argv_ovl_stmt);


    // Create an index variable to iterator over argv
    // argc_type index;
    SgVariableDeclaration* index_var = SB::buildVariableDeclaration(SgName("index_argc"),
                                    argc_var->get_type(),
                                    NULL,
                                    SI::getScope(BB));

    SI::insertStatementAfter(argv_ovl_stmt, index_var);

    // Now, create a for loop which will iterate over the argv and create dummy entries

    // for init stmt
    // index = 0
    SgAssignOp* init_assign = SB::buildAssignOp(SB::buildVarRefExp(index_var), SB::buildIntVal(0));    
    //SgForInitStatement* init_stmt = buildForInitStatement(init_assign);

    // test statement
    // index < argc
    SgExpression* test_exp = SB::buildLessThanOp(SB::buildVarRefExp(index_var), SB::buildVarRefExp(argc_var));
    SgStatement* test_stmt = SB::buildExprStatement(test_exp);

    // increment
    SgExpression* incr_exp = SB::buildPlusPlusOp(SB::buildVarRefExp(index_var));

    // Loop body
    // create_dummy_entry(&argv[index])
    SgExpression* argv_index = SB::buildPntrArrRefExp(SB::buildVarRefExp(argv_var), SB::buildVarRefExp(index_var)); 
    SgExpression* addr_argv_index = Util::castToAddr(Util::createAddressOfOpFor(argv_index));

    SgExprListExp* param_list = SB::buildExprListExp(addr_argv_index);

    SgExpression* overload = buildMultArgOverloadFn("create_dummy_entry", param_list, SgTypeVoid::createType(),
                            SI::getScope(BB), GEFD(SI::getEnclosingStatement(BB)));

    SgStatement* ovl_stmt = SB::buildExprStatement(overload);

    //SgForStatement* for_stmt = SB::buildForStatement(init_stmt, test_stmt, incr_exp, ovl_stmt);
    SgForStatement* for_stmt = SB::buildForStatement(SB::buildExprStatement(init_assign), test_stmt, incr_exp, ovl_stmt);

    SI::insertStatementAfter(index_var, for_stmt);
}

void ArithCheck::insertDummyEntries(SgProject* project) {
    SgFunctionDeclaration* MainFn = SI::findMain(isSgNode(project));
    
    if(!MainFn) {
        #ifdef ARGV_INIT
        printf("handleArgvInit:Can't find main\n");
        #endif
        return;
    }

    SgScopeStatement* scope = MainFn->get_scope();
    
    SgFunctionDefinition* MainFnDef = MainFn->get_definition();

    SgBasicBlock* BB = MainFnDef->get_body();

    SgStatementPtrList& Stmts = BB->get_statements();

    // stderr
    SgExpression* var_ref = SB::buildVarRefExp("__stderrp", SI::getGlobalScope(project));

    SgExprListExp* param_list = SB::buildExprListExp(Util::castToAddr(Util::createAddressOfOpFor(var_ref)));

    SgExpression* overload = buildMultArgOverloadFn("create_dummy_entry", param_list,
                                                SgTypeVoid::createType(), SI::getScope(BB),
                                                GEFD(SI::getEnclosingStatement(BB)));
    Util::insertAtTopOfBB(BB, SB::buildExprStatement(overload));

}


// This function transforms for and while loops to work with the instrumentation.
// while(cond) { } is transformed to while(1) { if(!(cond)) { break; } }
// We also split the cond to leave just the conditional expression, and remove 
// the other expressions and place them before the "if" after transformation.
// For example: while(exp1, cond) { } is transformed to
// while(1) { exp1; if(cond) { break; } }
// for(init; cond; incr) { } is transformed to for(init;;incr) { if(!(cond)) { break; } }
#if 0
void ArithCheck::modifyForAndWhileLoops(SgProject* project) {

    Rose_STL_Container <SgWhileStmt*> while_stmts = SI::querySubTree<SgWhileStmt>(isSgNode(project), V_SgWhileStmt);

    for(unsigned int index = 0; index < while_stmts.size(); index++) {
        
        SgWhileStmt* whilestmt = while_stmts[index];

        SgStatement* condstmt = whilestmt->get_condition();

        ROSE_ASSERT(isSgExprStatement(condstmt));

        SgExpression* cond_exp = isSgExprStatement(condstmt)->get_expression();
        // If its a comma op exp, split the lhs and rhs.
        // lhs becomes a statement in the body. rhs goes into the if stmt
        SgExpression* comma_lhs = NULL;
        SgExpression* cond = NULL;
        if(isSgCommaOpExp(cond_exp)) {
            comma_lhs = SI::copyExpression(isSgCommaOpExp(cond_exp)->get_lhs_operand());
            cond = SI::copyExpression(isSgCommaOpExp(cond_exp)->get_rhs_operand());
        }
        else {
            cond = SI::copyExpression(cond_exp);
        }

        // cond -> !cond
        SgExpression* not_cond = buildNotOp(cond);
        SgStatement* not_cond_stmt = SB::buildExprStatement(not_cond);

        // if(!cond) { break; }
        SgStatement* if_true_body = buildBreakStmt();
        SgStatement* if_stmt = buildIfStmt(not_cond_stmt, if_true_body, NULL);
        
        // insert if stmt into while    
        SgStatement* while_body = whilestmt->get_body();
        if(isSgBasicBlock(while_body)) {
            // prepend prepend this statement to the basic block
            isSgBasicBlock(while_body)->prepend_statement(if_stmt);
            // if its a comma op, comma_lhs won't be NULL, so prepend
            // it as well
            if(comma_lhs) {
                isSgBasicBlock(while_body)->prepend_statement(SB::buildExprStatement(comma_lhs));
            }
        }
        else {
            // insert a basic block in the place of the old
            // statement and insert if_stmt in that basic block.
            // if the comma_lhs is not NULL, insert it before the if_stmt
            SgBasicBlock* bb;

            if(comma_lhs) {
                bb = buildBasicBlock(SB::buildExprStatement(comma_lhs), if_stmt, copyStatement(while_body));
            }
            else {
                 bb = buildBasicBlock(if_stmt, copyStatement(while_body));
            }
            SI::deepDelete(while_body);
            whilestmt->set_body(bb);
            bb->set_parent(whilestmt);
        }
    
        // while(cond) -> while(1)
        SgExprStatement* new_cond = SB::buildExprStatement(SB::buildIntVal(1));
        whilestmt->set_condition(new_cond);
        // Remove the old condition
        SI::deepDelete(condstmt);
        // Set the parent for the new cond as the whilestmt
        new_cond->set_parent(whilestmt);
    }

}
#endif

#if 0
void ArithCheck::pushGlobalVarInitsToMain(SgProject* project) {

    SgGlobal* global = SI::getFirstGlobalScope(project);

    Rose_STL_Container <SgVariableDeclaration*> global_var_decls;
    global_var_decls = SI::querySubTree<SgVariableDeclaration>(global, V_SgVariableDeclaration);    

    // find Main
    SgFunctionDeclaration* MainFn = SI::findMain(global);
    if(!MainFn) {
        printf("pushGlobalVarInits: can't find main -- returning");
        return;
    }

    SgBasicBlock *bb = Util::getBBForFn(MainFn);

    for(unsigned int index = 0; index < global_var_decls.size(); index++) {
        
        SgVariableDeclaration* var_decl = global_var_decls[index];

        if(Util::NoInitializer(var_decl) || !isSgGlobal(var_decl->get_scope())) {
            continue;
        }

        // Lets copy the initializer to a statement.. inserted at the top of main
        // input: global_var = init
        // output: global_var; main() { global_var = init; }

        SgInitializedName* var_name = Util::getNameForDecl(var_decl);
        SgAssignInitializer* assign_init = isSgAssignInitializer(var_name->get_initializer());

        // Skip constructor initializers
        if(assign_init == NULL) {
            continue;
        }

        ROSE_ASSERT(assign_init != NULL);
    
        // Copy the initializer
        SgExpression* init_copy = SI::copyExpression(assign_init->get_operand());

        // Set the initializer in the var decl to NULL.
        var_name->set_initializer(NULL);
    
        // Create an assign op.. which will be replaced by the appropriate
        // operation by the handleAssignOverloadOps function below.
        SgAssignOp* var_assign = SB::buildAssignOp(SB::buildVarRefExp(var_name), init_copy);
        SgExprStatement* assign_stmt = SB::buildExprStatement(var_assign);

        Util::insertAtTopOfBB(bb, assign_stmt);

    }
    
}
#endif

#if 0
void ArithCheck::functionCallArgCastHandler(SgProject* project) {

    Rose_STL_Container <SgFunctionCallExp*> fn_calls;
    fn_calls = SI::querySubTree<SgFunctionCallExp>(project, V_SgFunctionCallExp);        

    for(unsigned int index = 0; index < fn_calls.size(); index++) {
        SgFunctionCallExp* cf = fn_calls[index];

        SgExprListExp* args = cf->get_args();
        // This is an indexable structure (like a vector)
        SgExpressionPtrList& exprs = args->get_expressions();
        
        // Now get the declaration for this function... and
        // if available the definition
        // If we can't find the definition, we won't be changing
        // the function signature (through handleFuncParams), so we might as well leave this
        // as it is...

        // Associated Function Declaration.. param list types
        SgFunctionDeclaration* fn_decl = cf->getAssociatedFunctionDeclaration();
        // fn_decl may be NULL if its a function pointer being deref'ed
        // If that is the case, then we don't do anything... 
        if(!fn_decl) {
            continue;
        }

        SgFunctionDeclaration* fn_def_decl = isSgFunctionDeclaration(fn_decl->get_definingDeclaration());
        
        if(!fn_def_decl) {
            #ifdef PANIC_DEBUG
            printf("fnArgCastHandler: Can't find defining decl for %s. Continuing\n", 
                    fn_decl->get_name().getString().c_str());
            #endif
            continue;
        }

        SgInitializedNamePtrList& decl_args = fn_def_decl->get_args();

        // Now, lets see if there is cast from string val to char*
        for(unsigned int exp_index = 0; exp_index < exprs.size(); exp_index++) {
            // exprs -> func_call args
            SgExpression* ce = exprs[exp_index];
            // decl_args -> defining decl args
            SgInitializedName* ci = decl_args[exp_index];

            // If they don't match, replace ce with casted_ce... (although we are traversing
            // the structure, it should be okay to replace the member, I guess
            SgType* ce_type = ce->get_type();
            #ifdef STRIP_TYPEDEFS
            ce_type = Util::getType(ce_type);
            #endif

            SgType* ci_type = ci->get_type();
            #ifdef STRIP_TYPEDEFS
            ci_type = Util::getType(ci_type);
            #endif

            if(isSgStringVal(ce) && !Util::compareTypes(ce_type, ci_type)) {
                // There is a cast from string to a pointer type (char* mostly)...
                ROSE_ASSERT(isSgPointerType(ci_type));
                SgExpression* casted_ce = SB::buildCastExp(SI::copyExpression(ce), ci_type, 
                                                    SgCastExp::e_C_style_cast);
                SI::replaceExpression(ce, casted_ce);
            }
        }
    }
                
}
#endif


void ArithCheck::cloneFunctions(SgProject* project) {
    
    #ifdef CLONE_FN_DEBUG
    printf("cloneFunctions - begin\n");
    #endif
    //for(NodeContainer::iterator nit = ArithCheck::NodesToInstrument.begin(); nit != ArithCheck::NodesToInstrument.end(); ++nit) {
    for(NodeContainer::iterator nit = Trav::DeclsToClone.begin(); nit != Trav::DeclsToClone.end(); ++nit) {
        
        SgNode* node = *nit;
        Sg_File_Info* File = isSgNode(node)->get_file_info();
        #ifdef CLONE_FN_DEBUG
        printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
        printf("File: (%s, %d, %d) %s = %s\n", File->get_filenameString().c_str(),
                                            File->get_line(),
                                            File->get_col(),
                                            isSgNode(node)->sage_class_name(),
                                            isSgNode(node)->unparseToString().c_str());
        #endif

        // Only func delcs in DeclsToClone 
        if(!isSgFunctionDeclaration(node)) {
            ROSE_ASSERT(0);
        }

        SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(node);
    
        

        SgName fn_name = fn_decl->get_name();
        SgFunctionParameterList* param_list = fn_decl->get_parameterList();
        SgType* ret_type = fn_decl->get_type()->get_return_type();
        SgScopeStatement* scope = fn_decl->get_scope();
        SgExprListExp* dec_list = fn_decl->get_decoratorList();

        SgSymbol* fn_decl_sym;
//        if(strcmp(fn_name.getString().c_str(), "panic") == 0) {
            fn_decl_sym = fn_decl->search_for_symbol_from_symbol_table();
            ROSE_ASSERT(fn_decl_sym);
//        }

        // Rename the current function to fn_name to include "_mod"
        // This will make sure that when we create a clone, both the functions
        // can exist in C (C doesn't allow two functions with the same name)
        #if 0
        std::string clone_name_string = fn_name.getString() + "_mod";
        SgName clone_name(clone_name_string);
        fn_decl->set_name(clone_name);
        #endif
    

        SgSymbolTable* sym_tab = scope->get_symbol_table();


//        if(strcmp(fn_name.getString().c_str(), "panic") == 0) {
            ROSE_ASSERT(sym_tab->exists(fn_decl_sym));
            sym_tab->remove(fn_decl_sym);

            #if 1
            std::string clone_name_string = fn_name.getString() + "_mod";
            SgName clone_name(clone_name_string);
            fn_decl->set_name(clone_name);
            #endif

            sym_tab->insert(clone_name, fn_decl_sym);
//        }


        #if 1
        // Now, build a copy of the original function
        // copy the param_list and decorator list
        //SgFunctionParameterList* new_param_list = isSgFunctionParameterList(SI::deepCopyNode(param_list));
        SgFunctionParameterList* new_param_list = SB::buildFunctionParameterList();
        
        
        SgExprListExp* new_dec_list = isSgExprListExp(SI::deepCopyNode(isSgNode(dec_list)));

        // Depending on whether this is a defining/non-defining decl, insert
        // the corresponding function declaration.
        SgFunctionDefinition* fndef = fn_decl->get_definition();
        bool defining_decl = (fndef != NULL);

//        if(strcmp(fn_name.getString().c_str(), "panic") == 0)
            ROSE_ASSERT(fn_decl_sym);

        SgFunctionDeclaration* new_fn_decl;
        if(defining_decl) {
            // its a defining func decl... 
            new_fn_decl = SB::buildDefiningFunctionDeclaration(fn_name, ret_type, new_param_list, scope, new_dec_list);

            // Get the function definition...
            SgFunctionDefinition* fndef = getFuncDef(new_fn_decl);
            
            // copy the def from the orig function to the new one.
            SgFunctionDefinition* new_fndef = isSgFunctionDefinition(SI::deepCopyNode(isSgNode(fndef)));
            
            // link the new def to the new fn
            new_fn_decl->set_definition(new_fndef);
            new_fndef->set_declaration(new_fn_decl);

            // now copy the BB and set it to the new_fndef
            SgBasicBlock* bb = Util::getBBForFn(fn_decl);
            SgBasicBlock* new_bb = isSgBasicBlock(SI::deepCopyNode(bb));
            new_fndef->set_body(new_bb);
            new_bb->set_parent(new_fndef);

        }
        else {
            // non defining decl
            new_fn_decl = SB::buildNondefiningFunctionDeclaration(fn_name, ret_type, new_param_list, scope, new_dec_list);
        }

//        if(strcmp(fn_name.getString().c_str(), "panic") == 0)
            ROSE_ASSERT(fn_decl_sym);

        SI::insertStatementBefore(fn_decl, new_fn_decl);

//        if(strcmp(fn_name.getString().c_str(), "panic") == 0)
            ROSE_ASSERT(fn_decl_sym);

        SgInitializedNamePtrList& args = param_list->get_args();
        for(SgInitializedNamePtrList::iterator iter = args.begin(); iter != args.end(); iter++) {
            SgInitializedName* arg_name = *iter;
            //SgInitializedName* new_arg_name = isSgInitializedName(SI::deepCopyNode(isSgNode(arg_name)));
            SgInitializer* init = arg_name->get_initptr();
            SgInitializedName* new_arg_name;
            if(!init) {
                new_arg_name = SB::buildInitializedName(arg_name->get_name(), arg_name->get_type(), NULL);
            }
            else {
                new_arg_name = SB::buildInitializedName(arg_name->get_name(), arg_name->get_type(), 
                                                                    isSgInitializer(SI::copyExpression(init)));
            }

            // If its a defining decl, scope is fn def
            // else its the scope of the fn_decl - in this case, SgGlobal

            if(defining_decl) {
                ROSE_ASSERT(isSgFunctionDefinition(arg_name->get_scope()));
                new_arg_name->set_scope(fndef);
            }
            else {
                ROSE_ASSERT(isSgGlobal(arg_name->get_scope()) && isSgGlobal(fn_decl->get_scope()));
                new_arg_name->set_scope(fn_decl->get_scope());
            }

            new_arg_name->set_parent(new_param_list);
            new_param_list->append_arg(new_arg_name);
        }

        // Check if fn_decl and new_fn_decl exist in the symbol table...
        ROSE_ASSERT(scope == new_fn_decl->get_scope());
        
//        if(strcmp(fn_name.getString().c_str(), "panic") == 0) {
            // Get the symbol for each of the fn decls...
            SgSymbol* new_fn_decl_sym = new_fn_decl->search_for_symbol_from_symbol_table();
            ROSE_ASSERT(new_fn_decl_sym);
            ROSE_ASSERT(fn_decl_sym);
            
            // Now, check that the symbol we retrieved is in fact, in the 
            // sym_tab
            ROSE_ASSERT(sym_tab->exists(fn_decl_sym));
            ROSE_ASSERT(sym_tab->exists(new_fn_decl_sym));
//        }

        // Marking that the new_fn_decl is the clone
        ArithCheck::Clones.push_back(new_fn_decl);
        // Create a mapping from the fn_decl_sym to the new_fn_decl_sym
        // Remember: new_fn_decl_sym is the clone of the original function
        // fn_decl_sym corresponds to the mod version of the function (which will
        // be modified by our instrumentation. We are trying to fix references
        // in the original function (clone) to point to other original functions
        // rather than the mod versions of the functions
        ArithCheck::CloneSymbolMap.insert(SymbolMap_t::value_type(fn_decl_sym, new_fn_decl_sym));

        printf("mod fn:\n");
        Util::printNode(fn_decl);
        
        // print statements in SgGlobal
        //ROSE_ASSERT(isSgGlobal(fn_decl->get_scope()));
        //Util::printStatements(isSgGlobal(fn_decl->get_scope()));
        #endif
    }
}

void ArithCheck::fixCallsWithinClones(SgProject* project) {

    // Iterator over clones
    for(NodeContainer::iterator iter = Clones.begin(); iter != Clones.end(); iter++) {
        SgFunctionDeclaration* clone_decl = isSgFunctionDeclaration(*iter);
        ROSE_ASSERT(clone_decl);

        // Now, in this decl, find all the fn calls, and then see if they are pointing
        // to the mod version of the function. If so, make them point to the clone version
        Rose_STL_Container <SgFunctionCallExp*> fncalls = 
                                        SI::querySubTree<SgFunctionCallExp>(clone_decl, V_SgFunctionCallExp);

        for(unsigned int index = 0; index < fncalls.size(); index++) {

            SgFunctionCallExp* fn_call = fncalls[index];
            SgSymbol* fn_sym = fn_call->getAssociatedFunctionSymbol();

            SymbolMap_t::iterator sym_it = CloneSymbolMap.find(fn_sym);
            if(sym_it != CloneSymbolMap.end()) {
                // This fn call points to the mod version. Make it point to the clone
                SgFunctionSymbol* clone_sym = isSgFunctionSymbol(sym_it->second);
                ROSE_ASSERT(clone_sym);

                // copy the expr list exp
                SgExprListExp* new_param_list = isSgExprListExp(SI::copyExpression(fn_call->get_args()));

                // replace the function call with a new one
                SgFunctionCallExp* new_fn_call = SB::buildFunctionCallExp(clone_sym, new_param_list);

                SI::replaceExpression(fn_call, new_fn_call);
            }
        }
    }

}

#if 0
void ArithCheck::replacePlusMinusAssigns(SgBinaryOp* op, bool plus) {

    SgExpression* lhs = op->get_lhs_operand();
    SgExpression* rhs = op->get_rhs_operand();

    SgType* lhs_type = lhs->get_type();
    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif

    if(!Util::isQualifyingType(lhs_type)) {
        return;
    }

    SgBinaryOp* plus_minus;
    if(plus) {
        plus_minus = SB::buildAddOp(SI::copyExpression(lhs), SI::copyExpression(rhs));
    }
    else {
        plus_minus = SB::buildSubtractOp(SI::copyExpression(lhs), SI::copyExpression(rhs));
    }

    SgBinaryOp* assign_op = SB::buildAssignOp(SI::copyExpression(lhs), plus_minus);
        
    SI::replaceExpression(op, assign_op);

}

void ArithCheck::splitPlusMinusAssigns(SgProject* project) {

    Rose_STL_Container <SgPlusAssignOp*> plus_assigns = 
                                        SI::querySubTree<SgPlusAssignOp>(isSgNode(project), V_SgPlusAssignOp);

    for(unsigned int index = 0; index < plus_assigns.size(); index++) {
        
        SgPlusAssignOp* ce = plus_assigns[index];

        replacePlusMinusAssigns(ce);
    }

    Rose_STL_Container <SgMinusAssignOp*> minus_assigns = 
                                        SI::querySubTree<SgMinusAssignOp>(isSgNode(project), V_SgMinusAssignOp);

    for(unsigned int index = 0; index < minus_assigns.size(); index++) {
        
        SgMinusAssignOp* ce = minus_assigns[index];

        replacePlusMinusAssigns(ce, false);
    }

}
#endif

void ArithCheck::getStructDeclsInHeaders(SgProject* project) {
    ArithCheck::str_decls = SI::querySubTree<SgClassDeclaration>(isSgNode(project), V_SgClassDeclaration);
}

#if 0

SgFunctionDeclaration* ArithCheck::findDeclParent(SgNode* node) {
    if(!node) {
        return NULL;
    }
    else if(isSgFunctionDeclaration(node)) {
        return isSgFunctionDeclaration(node);
    }
    else if(isSgGlobal(node)) {
        return NULL;
    }
    else {
        return findDeclParent(node->get_parent());
    }
}


SgClassDeclaration* ArithCheck::duplicateStruct(SgClassDeclaration* orig, SgName new_name, SgScopeStatement* scope, SgStatement* pos) {
    SgClassDeclaration* str_decl = 
                        buildStructDeclaration(new_name,scope);

    SgClassDefinition* str_def = buildClassDefinition(str_decl);

    SI::insertStatementBefore(pos, str_decl);

    SgDeclarationStatementPtrList& members = orig->get_definition()->get_members();

    SgDeclarationStatementPtrList::iterator iter = members.begin();

    for(; iter != members.end(); ++iter) {

        SgDeclarationStatement* decl_stmt = *iter;    

        // Function calls within structs/classes in C++...
        if(!isSgVariableDeclaration(decl_stmt)) {
                ROSE_ASSERT(0 && "Member function inside a class/struct located within a function!");
                continue;
        }

        SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl_stmt);
        SgInitializedName* var_name = Util::getNameForDecl(var_decl);

        SgVariableDeclaration* new_var_decl = SB::buildVariableDeclaration(var_name->get_name(), var_name->get_type(),
                        isSgInitializer(SI::deepCopyNode(var_name->get_initializer())), 
                        str_def);

        str_def->append_member(new_var_decl);
    }

    return str_decl;

}

void ArithCheck::evictStructsFromFunctions(SgProject* project) {
    // Get all the struct declarations... then, from that list, figure out which ones
    // are *defined within functions*

    Rose_STL_Container <SgClassDeclaration*> str_decls =
                                        SI::querySubTree<SgClassDeclaration>(isSgNode(project), V_SgClassDeclaration);
    
    typedef std::map<SgClassDeclaration*, SgClassDeclaration*> ClassMap;
    ClassMap replacedStructs;

    for(unsigned int i = 0; i < str_decls.size(); i++) {
        
        SgClassDeclaration* cd = str_decls[i];
        if(cd->isForward()) {
            // This is a forward declaration, no need to hoist this.
            continue;
        }

        // Find if the parent of this node ever hits a function declaration or class declaration
        SgFunctionDeclaration* parent = findDeclParent(cd);
        if(!parent) {
            // This struct is not within a function/class, nothing to do.
            continue;
        }

        SgClassDeclaration* str_decl = duplicateStruct(cd, SgName(isSgFunctionDeclaration(parent)->get_name().getString() + "_" +
                                                            cd->get_name().getString()), parent->get_scope(), parent);
        
        // Now, we need to make sure that the variables of the original struct, and now variables of the new struct
        // For that, we create a map from the orig decl to the new decl.
        replacedStructs[cd] = str_decl;
    }

    // One way to go about fixing the variable declarations and references is as follows:
    // 1. Get the *relevant* var decls, replace them with new var decls. Create a map which
    // tracks the orig to new var decl. 
    // 2. Get the *relevant* var refs, replace them with new var refs.
    // Here *relevant* refers to how a map is used to figure out if this is a var decl we are interested in
    
}
#endif

#if 0
void ArithCheck::castReturnExps(SgProject* project) {

    Rose_STL_Container <SgReturnStmt*> rets = SI::querySubTree<SgReturnStmt>(isSgNode(project), V_SgReturnStmt);

    for(unsigned int i = 0; i < rets.size(); i++) {
        
        SgReturnStmt* r = rets[i];
        if(r->get_expression()) {
            // get the function declaration
            SgFunctionDefinition* fn_def = getEnclosingProcedure(r);
            SgFunctionDeclaration* fn_decl = fn_def->get_declaration();
            SgType* ty = fn_decl->get_type()->get_return_type();
            #ifdef STRIP_TYPEDEFS
            ty = Util::getType(ty);
            #endif
            if(!Util::compareTypes(ty, r->get_expression()->get_type())) {
                // need to cast it... 
                SgExpression* casted = SB::buildCastExp(SI::copyExpression(r->get_expression()), ty, CAST_TYPE);
                SI::replaceExpression(r->get_expression(), casted);
            }
        }
    }
}
#endif

#if 0
SgVariableDeclaration* ArithCheck::pushToLocalVar(SgExpression* exp, SgScopeStatement* scope) {
    SgName temp("temp_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    return SB::buildVariableDeclaration(temp, exp->get_type(), SB::buildAssignInitializer(exp, exp->get_type()), 
                                    scope);
}

SgVariableDeclaration* ArithCheck::takeRefAndPushToLocalVar(SgExpression* exp) {

    ROSE_ASSERT(exp->get_type());

    
    SgExpression* addr = Util::createAddressOfOpFor(SI::copyExpression(exp));
    
    #if 0
    SgName temp("temp_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    return SB::buildVariableDeclaration(temp, buildReferenceType(exp->get_type()), SB::buildAssignInitializer(addr, addr->get_type()), 
                                    Util::getScopeForExp(exp)); 
    #endif
    // assign this to local variable and push to prev stmt
    return pushToLocalVar(addr, Util::getScopeForExp(exp));    
}

void ArithCheck::simplifyDotExp(SgDotExp* dot) {

    SgVariableDeclaration* tempvar;
    SgExpression* subst;
    // If the rhs operand is a variable operand, then we take its reference
    // Need to take it as a reference since it could be an lval/rval
    if(isSgVarRefExp(dot->get_rhs_operand())) {
        // rhs is var ref... move it to expression statement prior to the current stmt...
        tempvar = takeRefAndPushToLocalVar(dot);
        subst = SB::buildPointerDerefExp(SB::buildVarRefExp(tempvar));
    }
    else if(isSgFunctionCallExp(dot->get_rhs_operand())) {
        // If its a function, then we simply take its value/ref in a variable
        // rhs is a func call... move it to the expression stmt prior to the current stmt..
        tempvar = pushToLocalVar(SI::copyExpression(dot), Util::getScopeForExp(dot));
        subst = SB::buildVarRefExp(tempvar);
    }
    else if(isSgMemberFunctionRefExp(dot->get_rhs_operand())) {
        // Nothing to do... 
        return;
    }
    else {
        printf("Unsupported case in simplifyDotExp\n");
        Util::printNode(dot->get_rhs_operand());
        ROSE_ASSERT(0);
    }
    SI::insertStatementBefore(SI::getEnclosingStatement(dot), tempvar);
    SI::replaceExpression(dot, subst);
}

void ArithCheck::simplifyArrowExp(SgArrowExp* arrow) {
    // Deref the lhs... and convert to dot...
    SgExpression* lhs = arrow->get_lhs_operand();
    SgExpression* lhs_deref = SB::buildPointerDerefExp(SI::copyExpression(lhs));
    
    // Now, build the dot expression...
    SgDotExp* dot = buildDotExp(lhs_deref, SI::copyExpression(arrow->get_rhs_operand()));

    // replace the original one with this one...
    SI::replaceExpression(arrow, dot);

    // simplify the dot
    simplifyDotExp(dot);
}

void ArithCheck::pushToLocalVarAndReplace(SgExpression* exp) {
    SgVariableDeclaration* tempvar = pushToLocalVar(SI::copyExpression(exp), Util::getScopeForExp(exp));
    SI::insertStatementBefore(SI::getEnclosingStatement(exp), tempvar);
    SI::replaceExpression(exp, SB::buildVarRefExp(tempvar));
}

void ArithCheck::SimplifyNodes(SgProject* project) {
    
    #ifdef SN_DEBUG
    printf("InstrumentNodes4 - begin\n");
    #endif
    for(NodeContainer::iterator nit = ArithCheck::NodesToInstrument.begin(); nit != ArithCheck::NodesToInstrument.end(); ++nit) {
        
        SgNode* node = *nit;
        Sg_File_Info* File = isSgNode(node)->get_file_info();

        // print statements in SgGlobal
        //Util::printStatements(SI::getGlobalScope(node));

        #ifdef SN_DEBUG
        printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
        printf("File: (%s, %d, %d) %s = %s\n", File->get_filenameString().c_str(),
                                            File->get_line(),
                                            File->get_col(),
                                            isSgNode(node)->sage_class_name(),
                                            isSgNode(node)->unparseToString().c_str());
        #endif

        if(isSgDotExp(node)) {
            simplifyDotExp(isSgDotExp(node));
        }
        else if(isSgArrowExp(node)) {
            simplifyArrowExp(isSgArrowExp(node));
        }
        else {
            // Must be a child node... push to prev stmt...
            pushToLocalVarAndReplace(isSgExpression(node));
        }
    }
        
}
#endif

#if 0
void ArithCheck::convertArrowThisToDot(SgProject* project) {
    
    // expressions of the form this->var or this->func will be converted
    // to (*this).var and (*this).func
    Rose_STL_Container <SgArrowExp*> arrows =
                                        SI::querySubTree<SgArrowExp>(isSgNode(project), V_SgArrowExp);
    
    for(unsigned int i = 0; i < arrows.size(); i++) {
        SgArrowExp* arr = arrows[i];
        if(isSgThisExp(arr->get_lhs_operand())) {
            // this arrow needs to be converted to dot... 
            SgExpression* lhs = arr->get_lhs_operand();
            SgExpression* deref_lhs = SB::buildPointerDerefExp(SI::copyExpression(lhs));
            SgExpression* rhs_copy = SI::copyExpression(arr->get_rhs_operand());
            SgDotExp* new_dot = buildDotExp(deref_lhs, rhs_copy);
            SI::replaceExpression(arr, new_dot);
        }
    }
}
#endif

void ArithCheck::HackyPtrCheck7() {

    printf("HackyPtrCheck7\n");
    
    //SgProject* project = ArithCheck::ACProject;
    //project = ArithCheck::ACProject;

    #if 0

    // Push global var inits into main -- This was required since we ended up placing the
    // create_entry function ahead of the main function in handlePointerVars. There is another
    // elegant solution where in we could simply put the create_entry before the pointer var decl
    // itself. That makes sure that we put the create_entry before the first use of it, and all uses
    // of create_entry see that forward decl.
    // Earlier:
    // input: int* ptr = NULL; 
    //        fn(int* ptr1)
    //        main() {
    //          }
    // output: int* ptr;
    //        fn(int_str ptr1)
    //        struct int_str {create_entry }
    //          create_entry();
    //          assign_and_copy(ptr) {... create_entry}
    //        main() { 
    //            assign_and_copy(ptr, NULL);
    //          }
    // Now:
    // output: int* ptr;
    //          struct int_str {}
    //          create_entry();
    //          fn(int_str ptr1)
    //          assign_and_copy(ptr) {... create_entry}
    //          main() {
    //            assign_and_copy(ptr, NULL);
    //          }
    // We could have also solved this problem by simply making
    // sure that we placed the int_str and create_entry before
    // int* ptr rather than before the main() function.. however
    // that would've required us to tweak the handleAssignOverloadOps
    // which would've been messy!
    pushGlobalVarInitsToMain(project);


    // Modify for and while loops to work with our
    // instrumentation
    modifyForAndWhileLoops(project);

    // Split plus and minus assign ops to plus/minus and
    // then assign ops
    splitPlusMinusAssigns(project);

    // This will remove all the original expression trees -
    // generated through constant propagation/folding
    removeAllOriginalExpressionTrees(isSgNode(project));

    // Convert arrow exps where lhs is "this" to dot exps...
    convertArrowThisToDot(project);

    // Make string val to char* conversions (through function call args)
    // explicit
    #ifdef STR_TO_CHARPTR_ARG_CASTS
    functionCallArgCastHandler(project);
    #endif

    // Move structs defined within functions, out into the class/global scope
    // so that the instrumentation functions can see the definition
    #ifdef EVICT_STRUCTS_FROM_FUNCTIONS
    evictStructsFromFunctions(project);
    #endif

    #ifdef EXPLICIT_RETURN_STMT_CASTS
    castReturnExps(project);
    #endif

    #if 0
    #ifdef RETAIN_FUNC_PROTOS
    moveArgsOutOfFuncCalls(project);
    #endif
    #endif


    #ifdef SIMPLIFY_EXPRESSIONS
    TopBotTrack3 TB3;
    Util::nodeType inh3Attr(UNKNOWN, Util::INH_UNKNOWN);
    TB3.traverseInputFiles(project, inh3Attr);

    SimplifyNodes(project);

    ArithCheck::NodesToInstrument.clear();
    #endif

    #endif

    #if 0
    #ifndef TURN_OFF_INSTR
    TopBotTrack2 TB2;
    Util::nodeType inhAttr(Util::UNKNOWN, Util::INH_UNKNOWN);
    #ifdef JUST_TRAVERSE
    TB2.traverse(project, inhAttr);
    #elif defined(TRAVERSE_INPUT_FILE_TOP)
    SgFilePtrList file_list = ArithCheck::ACProject->get_fileList();
    ROSE_ASSERT(file_list.size() == 1);
            
    SgSourceFile* cur_file = isSgSourceFile(*file_list.begin());
    SgGlobal* global_scope = cur_file->get_globalScope();    
    TB2.traverse(global_scope, inhAttr);
    #else
    TB2.traverseInputFiles(project, inhAttr);
    #endif
    #endif
    #endif

    //createTrackingStruct(project);

    getFirstStatementInScope(SI::getFirstGlobalScope(ArithCheck::ACProject));

    #ifdef CLONE_FUNCTIONS
    // Before we instrument the nodes, the functions need to be 
    // cloned so that we have the original version of the function
    // and this is necessary for two reasons:
    // 1. satisifies the forward decl in the header with a def decl
    // 2. for external calls to these functions
    cloneFunctions(ArithCheck::ACProject);


    // After clone functions, the function calls within the cloned functions...
    // i.e. the original function copies... will point to the "_mod" versions
    // of the functions... thanks to the change in symbol from original function
    // name to the new function name ("name" -> "name_mod").
    // Here, we change the function calls within the cloned functions to point
    // to the "name" function rather than the "name_mod". The mapping between
    // both is maintained in the CloneSymbolMap
    // The set of cloned functions is stored in a vector... Clones
    fixCallsWithinClones(ArithCheck::ACProject);
    #endif

    #ifdef CHECK_HDR_FILES
    //getStructDeclsInHeaders(project);
    ClassDeclsVisitor visitor_A;
    traverseMemoryPoolVisitorPattern(visitor_A);
    #endif

    InstrumentNodes4(ArithCheck::ACProject);

    // Handle argv initialization... by creating dummy entries
    handleArgvInitialization(ArithCheck::ACProject);

    // Insert other dummy entries... for stderr, stdout type of variables
    //insertDummyEntries(project);
}

void ArithCheck::insertPreamble() {
    #if 0
    StaticSingleAssignment SSA(ArithCheck::ACProject);
    SSA.run(true, true);
    SSA.toDOT("dot");
    StaticSingleAssignment::LocalDefUseTable& def_table = SSA.getOriginalDefTable();
    printf("Starting def table\n");
    SSA.printLocalDefUseTable(def_table);
    printf("Done with def table\n");
    printf("Starting use table\n");
    StaticSingleAssignment::LocalDefUseTable& use_table = SSA.getLocalUsesTable();
    SSA.printLocalDefUseTable(use_table);
    printf("Done with use table\n");
    #endif

    // Shouldn't be using this CFG. Instead, we should use the virtualCFG
    #if 0
    DominatorTreesAndDominanceFrontiers::ControlFlowGraph CFG(ArithCheck::ACProject);
    CFG.outputCFGImpl();
    #endif

    #if 0
    SgFilePtrList file_list = ArithCheck::ACProject->get_fileList();
    SgFilePtrList::iterator iter;
    
    for(iter = file_list.begin(); iter!=file_list.end(); iter++) {
        SgSourceFile* cur_file = isSgSourceFile(*iter);
        SSA.toDOT(cur_file->get_file_info()->get_raw_filename());
        #if 0
        std::ofstream outFile;
        outFile.open("example.txt", std::ios::out | std::ios::app);
        SSA.printToDOT(cur_file, outFile);
        outFile.close();
        #endif
    }
    #endif
    
    #ifdef IDENTITY_TRANSLATOR
    return;
    #endif

    //insertLibHeader();
    
    HackyPtrCheck7();

    insertExecFunctions();
    
    //insertTimingSupport(ArithCheck::ACProject);

}

void ArithCheck::insertExecFunctions() {

    SgNode* TopNode = isSgNode(ArithCheck::ACProject);

    SgFunctionDeclaration* MainFn = SI::findMain(TopNode);

    if(!MainFn) {
        #ifdef INSERT_EXECFN_DEBUG
        printf("Can't find Main function. Not inserting execAtFirst and execAtLast\n");
        #endif
        return;
    }

    SgScopeStatement* scope = MainFn->get_scope();
    
    SgFunctionDefinition* MainFnDef = MainFn->get_definition();

    SgBasicBlock* FirstBlock = MainFnDef->get_body();

    SgStatementPtrList& Stmts = FirstBlock->get_statements();

    #if 0
    SgExpression* overload = buildOverloadFn(SgName("execAtFirst"), NULL, NULL, SgTypeVoid::createType(),
                         scope, MainFn); 
    #endif
    SgExpression* overload = buildMultArgOverloadFn(SgName("execAtFirst"), SB::buildExprListExp(), SgTypeVoid::createType(),
                         scope, MainFn); 
    SI::insertStatementBefore(*(Stmts.begin()), SB::buildExprStatement(overload)); 

    #if 0
    overload = buildOverloadFn(SgName("execAtLast"), NULL, NULL, SgTypeVoid::createType(),
                         scope, MainFn);
    #endif
    overload = buildMultArgOverloadFn(SgName("execAtLast"), SB::buildExprListExp(), SgTypeVoid::createType(),
                         scope, MainFn);

    SgStatementPtrList::iterator i = Stmts.begin();
    i += (Stmts.size() - 1);

    SI::insertStatementBefore(*i, SB::buildExprStatement(overload));

}

void ArithCheck::instrument() {
    
    insertPreamble();

    // FIXME: Removed actual overflow checks to test pointer checking
    #if 0
    for(BopContainer::iterator bpit = SelectedBops.begin(); bpit != SelectedBops.end();
            ++bpit) {

        SgBinaryOp* Bop = *bpit;
        instrument(Bop);
    }
    #endif
}






bool checkDontInstrumentList(SgBinaryOp* Parent, SgExpression* LHS, SgExpression* RHS) {

    // Unsigned Subtract Ops - cannot overflow
    if(Parent->variantT() == V_SgSubtractOp &&
        (LHS->get_type()->isUnsignedType() || RHS->get_type()->isUnsignedType())) {
        return true;
    }
    // Pointer Types are not currently handled - Return type or Op types shouldn't be pointer types
    else if((Parent->get_type()->variantT() == V_SgPointerType) ||
        (LHS->get_type()->variantT() == V_SgPointerType) ||
        (RHS->get_type()->variantT() == V_SgPointerType)) {
        return true;
    }
    // Array Types are not currently handled - Return type or Op types shouldn't be array types
    else if((Parent->get_type()->variantT() == V_SgArrayType) ||
        (LHS->get_type()->variantT() == V_SgArrayType) ||
        (RHS->get_type()->variantT() == V_SgArrayType)) {
        return true;
    }

    // We can handle all other cases
    return false;

}

void ArithCheck::instrument(SgBinaryOp* bop) {

    printf("insertArithCheck\n");
    
    printf("bop: %lu\n", &(*bop));

        // Seg faulting here. Possibly due to the fact that the bop
        // has been "removed" somehow... --- can't seem to get a 
        // read on it

    SgExpression* LHS = bop->get_lhs_operand();
    SgExpression* RHS = bop->get_rhs_operand();
    
    
    printf("LHS: %lu\n", &(*LHS));
    printf("RHS: %lu\n", &(*RHS));


        
    if(checkDontInstrumentList(bop, LHS, RHS)) {
        printf("Part of dontInstrumentList\n");
        return;
    }

    printInfo(bop, LHS, RHS);

    SgStatement* stmt = Util::getSurroundingStatement(*isSgExpression(bop));
    SgScopeStatement* scope = stmt->get_scope();

    ROSE_ASSERT(scope);

    SgExpression* LHS_Copy = SI::copyExpression(LHS);
    SgExpression* RHS_Copy = SI::copyExpression(RHS);

    SgType* LHSTy = Util::resolveTypedefsAndReferencesToBaseTypes(LHS->get_type());
    SgType* RHSTy = Util::resolveTypedefsAndReferencesToBaseTypes(RHS->get_type());
    SgType* ParTy = Util::resolveTypedefsAndReferencesToBaseTypes(bop->get_type());

    VariantT ParV = bop->variantT();
    VariantT ParV_Ty = ParTy->variantT();
    VariantT LHSV = LHSTy->variantT();
    VariantT RHSV = RHSTy->variantT();

    ROSE_ASSERT(LHSV != V_SgTypedefType);
    ROSE_ASSERT(RHSV != V_SgTypedefType);

    char* FnName = getString(ParV_Ty, ParV, LHSV, RHSV);

    SgName s_name(FnName);
    SgExprListExp* parameter_list = SB::buildExprListExp();

    SI::appendExpression(parameter_list, LHS_Copy);
    SI::appendExpression(parameter_list, RHS_Copy);

    SgExpression* NewFunction = SB::buildFunctionCallExp(s_name,
                                                                bop->get_type(),
                                                                parameter_list,
                                                                scope);

    SI::replaceExpression(bop, NewFunction);
    

}


void ArithCheck::instrument(SgNode* Node) {
    printf("ArithCheck:process\n");
    if(isSgBinaryOp(Node)) {
        instrument(isSgBinaryOp(Node));
    }
}



#endif
