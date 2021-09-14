
#if 0
    std::int32_t repairVarRefExpAccessList(std::list<SgVarRefExp*>& l, std::string accessName); // workaround

// workaround function, no longer necessary
  int32_t VariableIdMappingExtended::repairVarRefExpAccessList(list<SgVarRefExp*>& accesses, string accessName) {
    int32_t numErrors=0;
    // check if all symbols of VarRefExp in structure accesses are represented in gathered class data members' symbols
    for(auto v:accesses) {
      VariableId varId=variableId(v);
      if(!varId.isValid()) {
        // report error, and record error
        numErrors++;
        SgVariableSymbol* varSym=v->get_symbol();
        SgInitializedName* initName=varSym->get_declaration();
        SgVariableDeclaration* decl=isSgVariableDeclaration(initName->get_declaration());
        if(decl && isMemberVariableDeclaration(decl)) {
          if(SgClassDefinition* cdef=isSgClassDefinition(decl->get_parent())) {
            //cout<<"Repairing AST symbol inconsistency #"<<numErrors<<": found class of unregistered symbol:"<<cdef->get_qualified_name ()<<" defined in:"<<SgNodeHelper::sourceFilenameLineColumnToString(cdef)<<endl;
            SgClassDeclaration* classDecl=cdef->get_declaration();
            ROSE_ASSERT(classDecl);
            SgClassType* classType=classDecl->get_type();
            ROSE_ASSERT(classType);
            //cout<<": registering class members. of unregistered symbol:"<<cdef->get_qualified_name ()<<" defined in:"<<SgNodeHelper::sourceFilenameLineColumnToString(cdef)<<endl;
            // register class members based on definition (not SgType)
            std::list<SgVariableDeclaration*> memberList;
            auto classMemberList=cdef->get_members();
            for( auto classMember : classMemberList) {
              if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(classMember)) {
                memberList.push_back(varDecl);
              }
            }
            bool repairMode=true;
            registerClassMembers(classType, memberList , 0, repairMode); // this is *not* the classType through which this definition can be found (BUG), but an equivalent one
          } else {
            //cout<<": could not find class of unregistered symbol!";
          }
        }
      }
    }
    return numErrors;
  }
#endif
  
