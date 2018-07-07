
#include "strDecl.h"
#include "util.h"

#define GET_STRUCT_TYPE_DEBUG 1

namespace strDecl
{
  SgType* createStruct(std::string newStructname, SgType* ptrType, SgNode* pos);

  SgType* getStructType(SgType* type, SgNode* pos, bool /*create*/)
  {
#ifdef GET_STRUCT_TYPE_DEBUG
    std::cerr << "getStructType\ntype\n"
              << "node: " << type->sage_class_name()
              << " = " << type->unparseToString()
              << std::endl;
#endif /* GET_STRUCT_TYPE_DEBUG */

#ifdef STRIP_TYPEDEFS
    ROSE_ASSERT(!isSgTypedefType(type));
#endif

    // Expect a pointer type since RelevantStructType has pointer types
    // within it, and createStruct expects pointer type
#ifdef SUPPORT_REFERENCES
    ROSE_ASSERT(isSgPointerType(type) || isSgReferenceType(type));
#else
    ROSE_ASSERT(isSgPointerType(type));
#endif

    pos = getCorrectScopeForType(type, pos);

    SgScopeStatement* scope = isSgStatement(pos)->get_scope();
    std::string       strChkName = Util::generateNameForType(type);

    //~ if (strChkName == "__Pb__v__Pe___Type")
    //~ {
      // \pp    void pointer metadata struct is part of metadata/metadata_alt.h
      // \todo  either
      //        (1) parse the header file before, then look up the struct from there
      //        (2) generate structs and helper functions in order

      //~ return SB::buildOpaqueType(strChkName, scope);
    //~ }

    if (!scope->symbol_exists(strChkName))
    {
      createStruct(strChkName, type, pos);
      // Make sure that we have inserted the struct in the right scope
      ROSE_ASSERT(scope->symbol_exists(strChkName));
#ifdef GET_STRUCT_TYPE_DEBUG
      std::cerr << "created new str... " << strChkName << std::endl;
#endif
    }
    else
    {
#ifdef GET_STRUCT_TYPE_DEBUG
      printf("str exists\n");
#endif
    }

    SgSymbol* sym = scope->lookup_symbol(strChkName);
    SgClassSymbol* classSym = isSgClassSymbol(sym);

#ifdef GET_STRUCT_TYPE_DEBUG
    printf("returning str_type\n");
    printf("node: %s = %s\n", classSym->get_type()->sage_class_name(), classSym->get_type()->unparseToString().c_str());
    //printNode(classSym->get_type());
#endif
    return classSym->get_type();
  }

  SgDeclarationStatement* getCorrectScopeForType(SgType* type, SgNode* pos) {

    if(SgStatement* ty_d = type->getAssociatedDeclaration()) {
      if(Util::isNamespaceScope(ty_d->get_scope(), "std")) {
        // Instead of a globally visible decl, we'll return the first decl...
        return Util::FirstDeclStmtInFile;

      }
      else {
        return isSgDeclarationStatement(pos);
      }
    }
    // If we can't find the decl statement, then it must be a primitive data type right?
    else {
      // Instead of a globally visible decl (fn_decl), we'll return the first decl...
      return Util::FirstDeclStmtInFile;
    }
  }


  SgType* createStruct(std::string newStructname, SgType* ptrType, SgNode* pos) {
    #ifdef CREATE_STRUCT_DEBUG
    printf("creating struct of type: ");
    printf("node: %s = %s\n", ptrType->sage_class_name(), ptrType->unparseToString().c_str());
    //printNode(ptrType);
    #endif

    SgName strName(newStructname);

    // Input type has to be pointer type -- obviously
    #if 0
    #ifdef SUPPORT_REFERENCES
    ROSE_ASSERT(isSgPointerType(ptrType) || isSgReferenceType(ptrType));
    #else
    ROSE_ASSERT(isSgPointerType(ptrType));
    #endif
    #endif
    ROSE_ASSERT(isSgPointerType(ptrType));

    // Addresses can be stored in unsigned long long
    SgType* addrType = Util::getAddrType();

    //ROSE_ASSERT(isSgFunctionDeclaration(pos));
    ROSE_ASSERT(isSgFunctionDeclaration(pos) || isSgClassDeclaration(pos) || isSgVariableDeclaration(pos));
    //SgScopeStatement* scope = isSgFunctionDeclaration(pos)->get_scope();
    // pos could be a (member) func decl or a class decl...

    SgScopeStatement* scope = isSgDeclarationStatement(pos)->get_scope();

    SgClassDeclaration* decl = SB::buildStructDeclaration(strName, scope);

    SgType* strType = decl->get_type();

    SgClassDefinition* def = SB::buildClassDefinition(decl);

    ROSE_ASSERT(decl->get_definition() != NULL);

    SgVariableDeclaration* ptrVar = SB::buildVariableDeclaration("ptr", ptrType, NULL, def);
    def->append_member(ptrVar);
    SgVariableDeclaration* addrVar = SB::buildVariableDeclaration("addr", addrType, NULL, def);
    def->append_member(addrVar);

    // if its a member func decl, then this str decl can be inserted in the class
    if (newStructname == "__Pb__v__Pe___Type")
    {
      // do nothing; it is already in the inserted header file
    }
    else if (isSgMemberFunctionDeclaration(pos))
    {
      ROSE_ASSERT(isSgClassDefinition(scope));
      scope->insertStatementInScope(decl, false);
    }
    else
    {
      // otherwise, its a simple func decl.. insert before
      ROSE_ASSERT(isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope));
      // The scope of the type might be another namespace, and we could be inserting
      // it in the current file.. using a namespace qualification...
      // So pos and scope need not be related... i.e. pos needn't be in scope..


      //RMM: We ran into a problem where a struct declaration can be embedded inside a typedef, which is not a scope.
      //We have to handle that situation differently.
      if (isSgScopeStatement(pos->get_parent())){
             SI::insertStatementBefore(isSgStatement(pos), decl);
      } else {
             ROSE_ASSERT(isSgStatement(pos->get_parent()));
             SI::insertStatementBefore(isSgStatement(pos->get_parent()),decl);
      }
    }

    #ifdef CREATE_STRUCT_DEBUG
    printf("Inserting struct in scope\n");
    printf("node: %s = %s\n", scope->sage_class_name(), scope->unparseToString().c_str());
    //printNode(scope);
    #endif
    // assert that the symbol exists in the scope...
    ROSE_ASSERT(scope->symbol_exists(strName));

    // Insert this in the RelevantStructType... which now uses a scope argument
    // to get the correct struct in the current scope...
    // Instead of using str_type, use the namespace qualified version of str_type...
    // we can do that by assigning str_type to scope lookup symbol
    SgSymbol* sym = scope->lookup_symbol(strName);
    SgClassSymbol* classSym = isSgClassSymbol(sym);
    strType = classSym->get_type();

    TypePair ty_p = std::make_pair(ptrType, strType);
    RelevantStructType[scope] = ty_p;
    TypesInScope[scope].insert(strType);
    #ifdef CREATE_STRUCT_DEBUG
    printf("created str type: ");
    printf("node: %s = %s\n", strType->sage_class_name(), strType->unparseToString().c_str());
    //printNode(strType);
    #endif

    return strType;

  }

  // findInUnderlyingType provides the pointer
  // type for a given structure type. We could
  // do the same thing.. by simply getting the declaration
  // for the given struct type, and getting the
  // type of the first member from that declaration -- ptr

  SgType* findInUnderlyingType(SgType* type) {
    #ifdef UNDERLYING_TYPE_DEBUG
    printf("findInUnderlyingType\n");
    printNode(type);
    #endif
    if(SgDeclarationStatement* declStmt = type->getAssociatedDeclaration()) {
      // Now that we have a decl...
      // make sure that its a class type...
      if(SgClassDeclaration* decl = isSgClassDeclaration(declStmt)) {
        // Find the first member of this class

        ROSE_ASSERT(decl->get_firstNondefiningDeclaration()!= NULL);

        ROSE_ASSERT(isSgClassDeclaration(decl->get_firstNondefiningDeclaration()) == decl);

        decl = isSgClassDeclaration(decl->get_definingDeclaration());

        SgClassDefinition* def = decl->get_definition();

        ROSE_ASSERT(def != NULL);

        SgDeclarationStatementPtrList& members = def->get_members();

        SgDeclarationStatement* firstMember = isSgDeclarationStatement(*(members.begin()));

        ROSE_ASSERT(isSgVariableDeclaration(firstMember));

        SgType* varType = Util::getNameForDecl(isSgVariableDeclaration(firstMember))->get_type();
        #if 0
        #ifdef SUPPORT_REFERENCES
        ROSE_ASSERT(isSgPointerType(varType) || isSgReferenceType(varType));
        #else
        ROSE_ASSERT(isSgPointerType(varType));
        #endif
        #endif
        ROSE_ASSERT(isSgPointerType(varType));

        return varType;
      }
      else {
        #ifdef UNDERLYING_TYPE_DEBUG
        printf("findInUnderlyingType: Not class decl. NULL return\n");
        #endif
        return NULL;
      }
    }
    else {
      #ifdef UNDERLYING_TYPE_DEBUG
      printf("findInUnderlyingType: Can't find decl. NULL return\n");
      #endif
      return NULL;
    }
  }

  bool isValidStructType(SgType* type, SgStatement* stmt) {
    //SgScopeStatement* scope = isSgStatement(fn_decl)->get_scope();
    SgScopeStatement* scope = stmt->get_scope();

    // Check in current scope... if you can't find it, check the global scope...
    if(TypesInScope[scope].find(type) == TypesInScope[scope].end()) {

      if(isSgGlobal(scope)) {
        // in global scope... can't find it. this is not a valid struct type...
        return false;
      }
      else {
        // We can't find it in the current scope... so, lets look for it in the global scope..
        // We do this because for std namespace types, we insert the structs in the global scope...
        SgFilePtrList file_list = Util::project->get_fileList();
        ROSE_ASSERT(file_list.size() == 1);

        SgSourceFile* cur_file = isSgSourceFile(*file_list.begin());
        SgGlobal* global_scope = cur_file->get_globalScope();
        return (TypesInScope[global_scope].find(type) != TypesInScope[global_scope].end());
      }
    }
    else {
      return true;
    }

  }

  #ifdef CPP_SCOPE_MGMT
  bool isOriginalClassType(SgType* type) {

    if(!isSgClassType(type) || Util::isQualifyingType(type)) {
      return false;
    }

    #ifdef STRIP_TYPEDEFS
    ROSE_ASSERT(!isSgTypedefType(type));
    #endif
    // Get global scope... since classes are in the global scope...
    // There should only be one source file... get that...
    SgFilePtrList file_list = Util::project->get_fileList();
    ROSE_ASSERT(file_list.size() == 1);

    SgSourceFile* cur_file = isSgSourceFile(*file_list.begin());
    SgGlobal* global_scope = cur_file->get_globalScope();

    // Have we generated any types in this scope yet?
    if(TypesInScope.find(global_scope) != TypesInScope.end()) {
      return (TypesInScope[global_scope].find(type) == TypesInScope[global_scope].end());
    }
    else {
      if(isSgClassType(type)) {
        return true;
      }
      else {
        return false;
      }
    }
    //return (isSgClassType(type) && global_scope->symbol_exists(generateNameForType(type)));
  }
  #else
  bool isOriginalClassType(SgType* type) {
    #ifdef STRIP_TYPEDEFS
    ROSE_ASSERT(!isSgTypedefType(type));
    #endif
    // Get global scope... since classes are in the global scope...

    return (isSgClassType(type) && !strDecl::isValidStructType(type));
  }
  #endif

  bool functionInteractsWithStdOrPrimitives(SgExprListExp* params, SgType* ret, SgNode* pos) {

    if(strDecl::isValidStructType(ret, isSgStatement(pos))) {
      ret = strDecl::findInUnderlyingType(ret);
    }

    if(!Util::isStdOrPrimitiveType(ret)) {
      return false;
    }

    SgExpressionPtrList& exprs = params->get_expressions();
    for(SgExpressionPtrList::iterator i = exprs.begin(); i != exprs.end(); i++) {
      SgExpression* e = *i;
      // Check if this is a member of a class...

      SgExpression* copy = e;
      if(isSgAddressOfOp(copy)) {
        // get the variable underneath the address of op...
        copy = isSgAddressOfOp(copy)->get_operand();

      }

      if(isSgVarRefExp(copy)) {
        // get the declaration for this variable
        SgVariableSymbol* var_sym = isSgVarRefExp(copy)->get_symbol();
        SgInitializedName* init_name = var_sym->get_declaration();
        // If its scope if a class def, then its a member var
        if(isSgClassDefinition(init_name->get_scope())) {
          return false;
        }
      }

      SgType* ty = e->get_type();
      if(strDecl::isValidStructType(e->get_type(), isSgStatement(pos))) {
        ty = strDecl::findInUnderlyingType(e->get_type());
      }
      if(!Util::isStdOrPrimitiveType(ty)) {
        return false;
      }
    }
    return true;
  }
} // namespace strDecl
