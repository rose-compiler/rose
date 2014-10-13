SgNode* convertAtermToDot(ATerm term) 
   {
     Sg_File_Info* fi = getAtermFileInfo(term);
     SgNode* result = 0;
     ATerm temp1, temp2, temp3;
     char* str;
     int tempint;
     double tempdouble;

     if (ATmatch(term, "NULL") || ATmatch(term, "\"NULL\"")) 
        {
          result = 0;
          goto done;
        }

#define DO_BINARY(key, name, t1, t2) \
    if (ATmatch(term, key "(<term>, <term>)", &temp1, &temp2)) { \
      result = new name(fi, is##t1(convertAtermToNode(temp1)), \
			    is##t2(convertAtermToNode(temp2))); \
      goto done; \
    }
    DO_BINARY("Add", SgAddOp, SgExpression, SgExpression)
    DO_BINARY("Multiply", SgMultiplyOp, SgExpression, SgExpression)
    DO_BINARY("Divide", SgDivideOp, SgExpression, SgExpression)
    DO_BINARY("Less", SgLessThanOp, SgExpression, SgExpression)
    DO_BINARY("LessEqual", SgLessOrEqualOp, SgExpression, SgExpression)
    DO_BINARY("Greater", SgGreaterThanOp, SgExpression, SgExpression)
    DO_BINARY("GreaterEqual", SgGreaterOrEqualOp, SgExpression, SgExpression)
    DO_BINARY("Assign", SgAssignOp, SgExpression, SgExpression)
    DO_BINARY("AddAssign", SgPlusAssignOp, SgExpression, SgExpression)
    DO_BINARY("Index", SgPntrArrRefExp, SgExpression, SgExpression)
    DO_BINARY("Call", SgFunctionCallExp, SgExpression, SgExprListExp)
    DO_BINARY("Cast", SgCastExp, SgExpression, SgType)
#undef DO_BINARY

#define DO_BINARY_NOFILEINFO(key, name, t1, t2) \
    if (ATmatch(term, key "(<term>, <term>)", &temp1, &temp2)) { \
      result = new name(is##t1(convertAtermToNode(temp1)), \
			is##t2(convertAtermToNode(temp2))); \
      goto done; \
    }
    DO_BINARY_NOFILEINFO("Array", SgArrayType, SgType, SgExpression)
#undef DO_BINARY_NOFILEINFO

#define DO_UNARY(key, name, t1) \
    if (ATmatch(term, key "(<term>)", &temp1)) { \
      result = new name(fi, is##t1(convertAtermToNode(temp1))); \
      goto done; \
    }
    DO_UNARY("Return", SgReturnStmt, SgExpression)
    DO_UNARY("ExprStmt", SgExprStatement, SgExpression)
    DO_UNARY("AssignInit", SgAssignInitializer, SgExpression)
#undef DO_UNARY

    if (ATmatch(term, "ExprRoot(<term>)", &temp1)) {
      result = convertAtermToNode(temp1);
      goto done;
    }

#define DO_UNARY_NOFILEINFO(key, name, t1) \
    if (ATmatch(term, key "(<term>)", &temp1)) { \
      result = new name(is##t1(convertAtermToNode(temp1))); \
      goto done; \
    }
    DO_UNARY_NOFILEINFO("Pointer", SgPointerType, SgType)
    DO_UNARY_NOFILEINFO("Reference", SgReferenceType, SgType)
#undef DO_UNARY_NOFILEINFO

    if (ATmatch(term, "FunctionType(<term>, [<list>])", &temp1, &temp2)) {
      SgFunctionType* ft =	new SgFunctionType(isSgType(convertAtermToNode(temp1)));
      vector<ATerm> fargs = getAtermList(temp2);
      for (size_t i = 0; i < fargs.size(); ++i)
           ft->get_arguments().push_back(isSgType(convertAtermToNode(fargs[i])));
      result = ft;
      goto done;
    }

#define DO_NULLARY_NOFILEINFO(key, name) \
    if (ATmatch(term, key)) { \
      result = new name(); \
      goto done; \
    }
    DO_NULLARY_NOFILEINFO("Int", SgTypeInt)
    DO_NULLARY_NOFILEINFO("Bool", SgTypeBool)
    DO_NULLARY_NOFILEINFO("Void", SgTypeVoid)
    DO_NULLARY_NOFILEINFO("Float", SgTypeFloat)
    DO_NULLARY_NOFILEINFO("Double", SgTypeDouble)
    DO_NULLARY_NOFILEINFO("Char", SgTypeChar)
    DO_NULLARY_NOFILEINFO("SignedChar", SgTypeSignedChar)
    DO_NULLARY_NOFILEINFO("Short", SgTypeShort)
    DO_NULLARY_NOFILEINFO("Long", SgTypeLong)
    DO_NULLARY_NOFILEINFO("LongLong", SgTypeLongLong)
    DO_NULLARY_NOFILEINFO("LongDouble", SgTypeLongDouble)
    DO_NULLARY_NOFILEINFO("UnsignedLong", SgTypeUnsignedLong)
    DO_NULLARY_NOFILEINFO("UnsignedShort", SgTypeUnsignedShort)
    DO_NULLARY_NOFILEINFO("UnsignedChar", SgTypeUnsignedChar)
    DO_NULLARY_NOFILEINFO("UnsignedInt", SgTypeUnsignedInt)
    DO_NULLARY_NOFILEINFO("UnsignedLongLong", SgTypeUnsignedLongLong)
    DO_NULLARY_NOFILEINFO("DefaultType", SgTypeDefault)
#undef DO_NULLARY_NOFILEINFO

#define DO_NULLARY(key, name) \
    if (ATmatch(term, key)) { \
      result = new name(fi); \
      goto done; \
    }
    DO_NULLARY("Default", SgDefaultOptionStmt)
#undef DO_NULLARY

    if (ATmatch(term, "Project([<list>])", &temp1)) {
      SgProject* proj = new SgProject();
      vector<ATerm> terms = getAtermList(temp1);

   // proj->set_fileList_ptr(new SgFilePtrList());
      ROSE_ASSERT(proj->get_fileList_ptr() != NULL);

      for (size_t i = 0; i < terms.size(); ++i) {
        // proj->get_fileList_ptr()->push_back(isSgFile(convertAtermToNode(terms[i])));
         proj->get_fileList_ptr()->get_listOfFiles().push_back(isSgFile(convertAtermToNode(terms[i])));
      }
      result = proj;
      goto done;
    }

 // DQ (8/12/2004): This does not work because SgFile() constructor is broken.
 // As a result the input Aterm must be edited to remove this node.
    if (ATmatch(term, "File(<str>, <term>)", &str, &temp1)) {
   // SgFile* file = new SgFile();
      SgSourceFile* file = new SgSourceFile();
   // file->set_root(isSgGlobal(convertAtermToNode(temp1)));
      file->set_globalScope(isSgGlobal(convertAtermToNode(temp1)));
      string name = str; // FIXME: use this
      result = file;
      goto done;
    }

    if (ATmatch(term, "Global([<list>])", &temp1)) {
      SgGlobal* global = new SgGlobal(fi);
      vector<ATerm> terms = getAtermList(temp1);
      for (size_t i = 0; i < terms.size(); ++i) {
	global->append_declaration(
	  isSgDeclarationStatement(convertAtermToNode(terms[i])));
      }
      result = global;
      goto done;
    }

    if (ATmatch(term, "ExprList([<list>])", &temp1)) {
      SgExprListExp* exprlist = new SgExprListExp(fi);
      vector<ATerm> terms = getAtermList(temp1);
      for (size_t i = 0; i < terms.size(); ++i) {
	exprlist->get_expressions().push_back(
	  isSgExpression(convertAtermToNode(terms[i])));
      }
      result = exprlist;
      goto done;
    }

    if (ATmatch(term, "Block([<list>])", &temp1)) {
      SgBasicBlock* block = new SgBasicBlock(fi);
      vector<ATerm> terms = getAtermList(temp1);
      for (size_t i = 0; i < terms.size(); ++i) {
	block->get_statements().push_back(
	  isSgStatement(convertAtermToNode(terms[i])));
	// FIXME: add to symbol tables
      }
      result = block;
      goto done;
    }

    if (ATmatch(term, "Function(<str>, <term>, [<list>], <term>)",
		&str, &temp1, &temp2, &temp3)) {
      string funcname = str;
      SgType* return_type = isSgType(convertAtermToNode(temp1));
      ROSE_ASSERT (return_type);
      vector<ATerm> params = getAtermList(temp2);
      SgFunctionParameterList* pl = 
	new SgFunctionParameterList(
	      Sg_File_Info::generateDefaultFileInfoForTransformationNode());
      for (size_t i = 0; i < params.size(); ++i) {
	SgNode* argi = convertAtermToNode(params[i]);
	ROSE_ASSERT (isSgInitializedName(argi));
	pl->append_arg(isSgInitializedName(argi));
      }
      SgFunctionDefinition* def = 
	isSgFunctionDefinition(convertAtermToNode(temp3));
      SgFunctionType* ft = new SgFunctionType(return_type, false);
      for (SgInitializedNamePtrList::iterator i = pl->get_args().begin();
           i != pl->get_args().end(); ++i)
        ft->get_arguments().push_back((*i)->get_type());
      SgFunctionDeclaration* decl =
	new SgFunctionDeclaration(fi, funcname.c_str(), ft, 0);
      for (SgInitializedNamePtrList::iterator i = pl->get_args().begin();
           i != pl->get_args().end(); ++i) {
        (*i)->set_definition(decl);
	decl->get_args().push_back(*i);
      }
      if (def)
	decl->set_definition(def);
      result = decl;
      goto done;
    }

    if (ATmatch(term, "FunctionDefinition(<term>)", &temp1)) {
      SgFunctionDefinition* def = new SgFunctionDefinition(fi);
      def->set_body(isSgBasicBlock(convertAtermToNode(temp1)));
      result = def;
      goto done;
    }

    if (ATmatch(term, "ParameterList([<list>])", &temp1)) {
      SgFunctionParameterList* parameter_list = 
	new SgFunctionParameterList(fi);
      vector<ATerm> terms = getAtermList(temp1);
      for (size_t i = 0; i < terms.size(); ++i) {
	parameter_list->get_args().push_back(
	  isSgInitializedName(convertAtermToNode(terms[i])));
	// FIXME: add to symbol tables
      }
      result = parameter_list;
      goto done;
    }

    if (ATmatch(term, "InitName(<str>, <term>, <term>)",
		&str, &temp1, &temp2)) {
      string varname = str;
      SgType* type = isSgType(convertAtermToNode(temp1));
      SgInitializer* init = isSgInitializer(convertAtermToNode(temp2));
      SgName* name = new SgName(varname.c_str());
      ROSE_ASSERT (name->str());
      result = new SgInitializedName(*name, type, init);
      goto done;
    }

    if (ATmatch(term, "Ellipses")) {
      result = new SgInitializedName("", new SgTypeEllipse());
      goto done;
    }

    if (ATmatch(term, "IntC(<int>)", &tempint)) {
      result = new SgIntVal(fi, tempint);
      goto done;
    }

    if (ATmatch(term, "DoubleC(<real>)", &tempdouble)) {
      result = new SgDoubleVal(fi, tempdouble);
      goto done;
    }

    if (ATmatch(term, "UnsignedLongC(<str>)", &str)) {
      istringstream s(str);
      unsigned long value;
      s >> value;
      ROSE_ASSERT (s.eof() && !s.fail());
      result = new SgUnsignedLongVal(fi, value);
      goto done;
    }

    if (ATmatch(term, "ExprList([<list>])", &temp1)) {
      SgExprListExp* exprlist = new SgExprListExp(fi);
      vector<ATerm> terms = getAtermList(temp1);
      for (size_t i = 0; i < terms.size(); ++i) {
	exprlist->append_expression(
	  isSgExpression(convertAtermToNode(terms[i])));
      }
      result = exprlist;
      goto done;
    }

    if (ATmatch(term, "ClassDefinition([<list>])", &temp1)) {
      SgClassDefinition* class_definition = new SgClassDefinition(fi);
      vector<ATerm> terms = getAtermList(temp1);
      for (size_t i = 0; i < terms.size(); ++i) {
	SgDeclarationStatement* child = 
	  isSgDeclarationStatement(convertAtermToNode(terms[i]));
	ROSE_ASSERT (child);
	class_definition->append_member(child);
      }
      result = class_definition;
      goto done;
    }

    if (ATmatch(term, "Enum(<str>, [<list>])", &str, &temp1)) {
      SgEnumDeclaration* enumdef = new SgEnumDeclaration(fi, str);
      vector<ATerm> terms = getAtermList(temp1);
      for (size_t i = 0; i < terms.size(); ++i) {
	SgInitializedName* child = 
	  isSgInitializedName(convertAtermToNode(terms[i]));
	ROSE_ASSERT (child);
	enumdef->append_enumerator(child);
      }
      result = enumdef;
      goto done;
    }

    if (ATmatch(term, "VarDecl([<list>])", &temp1)) {
      SgVariableDeclaration* vardecl = new SgVariableDeclaration(fi);
      vector<ATerm> terms = getAtermList(temp1);
      for (size_t i = 0; i < terms.size(); ++i) {
	SgInitializedName* child = 
	  isSgInitializedName(convertAtermToNode(terms[i]));
	ROSE_ASSERT (child);
	SgInitializer* init = child->get_initializer();
	vardecl->append_variable(child, init);
      }
      result = vardecl;
      goto done;
    }

    if (ATmatch(term, "Func(<str>)", &str)) {
      SgFunctionDeclaration* decl = 
	isSgFunctionDeclaration(targetLocations[str]);
      ROSE_ASSERT (decl);
      SgFunctionSymbol* sym = new SgFunctionSymbol(decl);
      result = new SgFunctionRefExp(fi, sym);
      goto done;
    }

    if (ATmatch(term, "Var(<str>)", &str)) {
      SgInitializedName* decl = 
	isSgInitializedName(targetLocations[str]);
      ROSE_ASSERT (decl);
      SgVariableSymbol* sym = new SgVariableSymbol(decl);
      result = new SgVarRefExp(fi, sym);
      goto done;
    }

    if (ATmatch(term, "Type(id(<str>))", &str)) {
      cout << "Type ref to " << str << endl;
      SgNode* body = targetLocations[str];
      ROSE_ASSERT (body);
      SgClassDeclaration* classdecl = isSgClassDeclaration(body);
      SgTypedefDeclaration* typedefdecl = isSgTypedefDeclaration(body);
      SgEnumDeclaration* enumdecl = isSgEnumDeclaration(body);
      if (classdecl)
	result = new SgClassType(classdecl);
      else if (typedefdecl)
	result = new SgTypedefType(typedefdecl);
      else if (enumdecl)
	result = new SgEnumType(enumdecl);
      else
	ROSE_ASSERT (!"Invalid type reference for id");
      goto done;
    }

    if (ATmatch(term, "Type(<term>)", &temp1)) {
       // For things like "typedef struct foo {int x;} bar;"
      SgClassType* nt = 
	new SgClassType(isSgClassDeclaration(convertAtermToNode(temp1)));
      // FIXME add typedef and enum cases
      result = nt;
      goto done;
    }

    if (ATmatch(term, "Class(<str>, <term>)", &str, &temp1)) {
      SgClassDeclaration* decl = new SgClassDeclaration(fi, str);
      decl->set_definition(isSgClassDefinition(convertAtermToNode(temp1)));
      result = decl;
      goto done;
    }

    if (ATmatch(term, "ClassFwd(<str>)", &str)) {
      SgClassDeclaration* decl = new SgClassDeclaration(fi, str);
      result = decl;
      goto done;
    }

    if (ATmatch(term, "Typedef(<str>, <term>)", &str, &temp1)) {
      SgType* base = isSgType(convertAtermToNode(temp1));
      if (!base->get_typedefs())
	base->set_typedefs(new SgTypedefSeq());
      SgTypedefDeclaration* decl = 
	new SgTypedefDeclaration(fi, str, base);
      SgTypedefType* tt = new SgTypedefType(decl);
      base->get_typedefs()->get_typedefs().push_back(tt);
      decl->set_type(tt);
      result = decl;
      goto done;
    }

    if (ATmatch(term, "Const(<term>)", &temp1)) {
      SgModifierType* mt = 
	new SgModifierType(isSgType(convertAtermToNode(temp1)));
      mt->get_typeModifier().get_constVolatileModifier().setConst();
      result = mt;
      goto done;
    }

    if (ATmatch(term, "Volatile(<term>)", &temp1)) {
      SgModifierType* mt = 
	new SgModifierType(isSgType(convertAtermToNode(temp1)));
      mt->get_typeModifier().get_constVolatileModifier().setVolatile();
      result = mt;
      goto done;
    }

    cerr << "Unrecognized term " << ATwriteToString(term) << endl;
    ROSE_ASSERT (false);

    done:
    ATerm idannot = ATgetAnnotation(term, ATmake("id"));
    if (idannot) {
      char* id;
      if (ATmatch(idannot, "<str>", &id)) {
	targetLocations[id] = result;
      } else {
	ROSE_ASSERT (!"id annotation has incorrect format");
      }
    }

#ifdef DEBUG
    if (result)
      FixSgTree(result);
    cout << "Resulting in " << (result ? result->sage_class_name() : "NULL") << ": " << /* myUnparse(result) << */ endl;
#endif
    return result;
  }
