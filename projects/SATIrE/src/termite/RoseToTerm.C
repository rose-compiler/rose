// -*- mode: C++ -*-
/* Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
        2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#include <satire_rose.h>
#include <list>
#include <cctype>
#include "termite.h" 
// GB (2009-02-25): Use the term printer without DFI stuff.
#undef HAVE_PAG
#define HAVE_SATIRE_ICFG 0
#include "TermPrinter.h"
#include "RoseToTerm.h"
#include "satire.h"

using namespace std;

/* Helper Macro to attach the preprocessing information to each node's
   annotation */
#define PPI(NODE) \
  getPreprocessingInfo((NODE)->getAttachedPreprocessingInfo())

/**
 * get node specific info for a term.
 * This function, depending on the type of the node, uses private helper functions.
 * No actual work is done here.
 */
PrologTerm*
RoseToTerm::getSpecific(SgNode* astNode) {
  string cname = astNode->class_name();
  if (SgValueExp* n = dynamic_cast<SgValueExp*>(astNode)) {
    return getValueExpSpecific(n);
  } else if (SgUnaryOp* n = dynamic_cast<SgUnaryOp*>(astNode)) {
    return getUnaryOpSpecific(n);
  } else if (SgBinaryOp* n = dynamic_cast<SgBinaryOp*>(astNode)) {
    return getBinaryOpSpecific(n);
    /*be careful with logic: SgMemberFunctionDeclaration is a subtype of SgFunctionDeclaration*/
  } else if (SgMemberFunctionDeclaration* n = dynamic_cast<SgMemberFunctionDeclaration*>(astNode)) {
    return getMemberFunctionDeclarationSpecific(n);
  } else if (SgFunctionDeclaration* n = dynamic_cast<SgFunctionDeclaration*>(astNode)) {
    return getFunctionDeclarationSpecific(n);
  } else if (SgInitializedName* n = dynamic_cast<SgInitializedName*>(astNode)) {
    return getInitializedNameSpecific(n);
  } else if (SgVarRefExp* n = dynamic_cast<SgVarRefExp*>(astNode)) {
    return getVarRefExpSpecific(n);
  } else if (SgAssignInitializer* n = dynamic_cast<SgAssignInitializer*>(astNode)) {
    return getAssignInitializerSpecific(n);
  } else if (SgClassDeclaration* n = isSgClassDeclaration(astNode)) {
    return getClassDeclarationSpecific(n);
  } else if (SgClassDefinition* n = isSgClassDefinition(astNode)) {
    return getClassDefinitionSpecific(n);
  } else if (SgLabelStatement* n = isSgLabelStatement(astNode)) {
    return getLabelStatementSpecific(n);
  } else if (SgGotoStatement* n = isSgGotoStatement(astNode)) {
    return getGotoStatementSpecific(n);
  } else if (SgConditionalExp* n = isSgConditionalExp(astNode)) {
    return getConditionalExpSpecific(n);
  } else if (SgEnumDeclaration* n = isSgEnumDeclaration(astNode)) {
    return getEnumDeclarationSpecific(n);
  } else if (SgDeleteExp* n = isSgDeleteExp(astNode)) {
    return getDeleteExpSpecific(n);
  } else if (SgRefExp* n = isSgRefExp(astNode)) {
    return getRefExpSpecific(n);
  } else if (SgExpression* n = isSgVarArgOp(astNode)) {
    return getVarArgSpecific(n);
  } else if (SgExpression* n = isSgVarArgCopyOp(astNode)) {
    return getVarArgSpecific(n);
  } else if (SgExpression* n = isSgVarArgEndOp(astNode)) {
    return getVarArgSpecific(n);
  } else if (SgExpression* n = isSgVarArgStartOp(astNode)) {
    return getVarArgSpecific(n);
  } else if (SgExpression* n = isSgVarArgStartOneOperandOp(astNode)) {
    return getVarArgSpecific(n);
  } else if (SgFunctionRefExp* n = isSgFunctionRefExp(astNode)) {
    return getFunctionRefExpSpecific(n);
  } else if (SgFunctionCallExp* n = isSgFunctionCallExp(astNode)) {
    return getFunctionCallExpSpecific(n);
  } else if (SgMemberFunctionSymbol* n = isSgMemberFunctionSymbol(astNode)) {
    return getMemberFunctionSymbolSpecific(n);
  } else if (SgMemberFunctionRefExp* n = isSgMemberFunctionRefExp(astNode)) {
    return getMemberFunctionRefExpSpecific(n);
  } else if (SgNamespaceDeclarationStatement* n = isSgNamespaceDeclarationStatement(astNode)) {
    return getNamespaceDeclarationStatementSpecific(n);
  } else if (SgVariableDeclaration* n = isSgVariableDeclaration(astNode)) {
    return getVariableDeclarationSpecific(n);
  } else if (SgTypedefDeclaration* n = isSgTypedefDeclaration(astNode)) {
    return getTypedefDeclarationSpecific(n);
  } else if (SgSizeOfOp* n = isSgSizeOfOp(astNode)) {
    return getSizeOfOpSpecific(n);
  } else if (SgConstructorInitializer* n = isSgConstructorInitializer(astNode)) {
    return getConstructorInitializerSpecific(n);
  } else if (SgNewExp* n = isSgNewExp(astNode)) {
    return getNewExpSpecific(n);
  } else if (SgPragma* n = isSgPragma(astNode)) {
    return getPragmaSpecific(n);
  } else {
    if (SgLocatedNode* n = dynamic_cast<SgLocatedNode*>(astNode)) {
      // add preprocessing info
      return new PrologCompTerm("default_annotation", //2, 
			     new PrologAtom("null"),
			     PPI(n));
    } else {
      return new PrologCompTerm("default_annotation", /*1,*/ new PrologAtom("null"));
    }
  }
  assert(false);
}

PrologCompTerm*
RoseToTerm::getPreprocessingInfo(AttachedPreprocessingInfoType* inf) {
  PrologList* l = new PrologList();
  if (inf != NULL) {
    for (AttachedPreprocessingInfoType::reverse_iterator it = inf->rbegin();
	 it != inf->rend(); ++it) {
      bool escape =
          (*it)->getTypeOfDirective()
          != PreprocessingInfo::CpreprocessorIncludeDeclaration;

      PrologCompTerm* ppd = new
	PrologCompTerm(re.DirectiveTypes[(*it)->getTypeOfDirective()], //3,
		       new PrologAtom((*it)->getString(), escape),
		       getEnum((*it)->getRelativePosition(),
			       re.RelativePositionTypes),
		       getFileInfo((*it)->get_file_info()));
      l->addFirstElement(ppd);
    }
  }
  return new PrologCompTerm("preprocessing_info", /*1,*/ l);
}

/**
 * class: Sg_File_Info
 * term: file_info(file,line,column)
 * arg file: file name
 * arg line: line name
 * arg col: column name
 */
PrologCompTerm*
RoseToTerm::getFileInfo(Sg_File_Info* inf) {		
  ROSE_ASSERT(inf != NULL);
  return new PrologCompTerm("file_info", /*3,*/
			    new PrologAtom(inf->get_filename()),
			    new PrologInt(inf->get_line()),
			    new PrologInt(inf->get_col()));
}

/**
 * convert ZigZagCase to zig_zag_case.
 * (In Prolog, strings starting with uppercase letters are free variables)
 */
string 
RoseToTerm::prologize(string s) {
  string t;
  string::iterator it;
  it = s.begin();
  // remove "^Sg"
  if ((s.length() > 2) && (s[0] == 'S') && (s[1] == 'g')) {
    ++it, ++it;
  }
  //lowercase first char (without prepending underscore)
  if(it != s.end()) {
    if(isupper(*it)) {
      t.push_back(tolower(*it));
    } else {
      t.push_back(*it);
    }
    it++;
  }
  //replace an uppercase letter with an underscore
  //and its lowercase equivalent
  while(it != s.end()) {
    if(isupper(*it)) {
      t.push_back('_');
      t.push_back(tolower(*it));
    } else {
      t.push_back(*it);
    }
    it++;
  }
  return t;
}

/** check whether we have seen the type before, and store it for future
 * queries */
bool
RoseToTerm::typeWasDeclaredBefore(std::string type) {
  /* std::set::insert returns a pair where the second component tells us
   * whether the object actually needed to be inserted (i.e., was *not*
   * present before) */
  return !declaredTypes.insert(type).second;
}

/**
 * class: SgFunctionDeclaration
 * term: function_declaration_annotation(type,name)
 * arg type: type of the declaration
 * arg name: name of the declaration
 * arg dec_mod: declaration modifier (see getDeclarationModifierSpecific)
 */
PrologCompTerm*  
RoseToTerm::getFunctionDeclarationSpecific(SgFunctionDeclaration* decl) {
  /* create annotation term*/
  return new PrologCompTerm
    ("function_declaration_annotation", /*4,*/
     /* add type and name*/
     getTypeSpecific(decl->get_type()),
     new PrologAtom(decl->get_name().getString()),
     getDeclarationModifierSpecific(&(decl->get_declarationModifier())),
     PPI(decl));
}


/**
 * class: SgFunctionType
 * term: function_type(tpe,he,argl)
 * arg tpe: return type
 * arg he: has_ellipses - flag
 * arg argl: argument type list (PrologList of SgType - Annotations
 * */
PrologCompTerm*  
RoseToTerm::getFunctionTypeSpecific(SgType* mytype) {
  /*let ROSE do casting and testing*/
  SgFunctionType* ftype = isSgFunctionType(mytype);
  ROSE_ASSERT(ftype != NULL);
  /*this is a nested type*/
  return new PrologCompTerm
    ("function_type", /*3,*/
     /*recurse with getTypeSpecific*/
     getTypeSpecific(ftype->get_return_type()),
     /*we need to know wether it has ellipses to unparse the constructor*/
     getEnum(ftype->get_has_ellipses(), re.ellipses_flags),
     /*arguments*/
     getTypePtrListSpecific(ftype->get_arguments()));
}



/**
 * class: SgMemberFunctionType
 * term: member_function_type(tpe,he,argl,mfs)
 * arg tpe: return type
 * arg he: has_ellipses - flag
 * arg argl: argument type list (PrologList of SgType - Annotations
 * arg mfs: mfunc_specifier of type
 * */
PrologCompTerm*  
RoseToTerm::getMemberFunctionTypeSpecific(SgType* mytype) {
  /*let ROSE do casting and testing*/
  SgMemberFunctionType* ftype = isSgMemberFunctionType(mytype);
  ROSE_ASSERT(ftype != NULL);
  /*this is a nested type*/
  return new PrologCompTerm
    ("member_function_type", /*4,*/
     /*recurse with getTypeSpecific*/
     getTypeSpecific(ftype->get_return_type()),
    /*we need to know wether it has ellipses for the constructor for unparsing*/
     getEnum(ftype->get_has_ellipses(), re.ellipses_flags),
     /*arguments*/
     getTypePtrListSpecific(ftype->get_arguments()),
     /* mfunc_specifier*/
     getEnum(ftype->get_mfunc_specifier(), re.declaration_modifiers));
}


/**
 * class: SgPointerType
 * term: pointer_type(type)
 * arg type: base type
 */
PrologCompTerm* 
RoseToTerm::getPointerTypeSpecific(SgType* mytype) {
  /* let rose do type testing and casting*/
  SgPointerType* ptype = isSgPointerType(mytype);
  ROSE_ASSERT(ptype != NULL);
  /* nested type */
  return new PrologCompTerm
    ("pointer_type", /*1,*/
     /* get base type with recursion*/
     getTypeSpecific(ptype->get_base_type()));
}


/**
 * class: SgEnumType
 * term: enum_type(declaration)
 * arg declaration: the term representation of the declaration
 */
PrologCompTerm* 
RoseToTerm::getEnumTypeSpecific(SgType* mtype) {
  /*make sure we are actually dealing with a class type*/
  SgEnumType* ctype = isSgEnumType(mtype);
  ROSE_ASSERT(ctype != NULL);
  /*add base type*/
  string id = ctype->get_name().str();
  if (id == "") {
    /* nameless enum declarations can occur in typedefs */
    SgTypedefDeclaration *td;
    if (td = isSgTypedefDeclaration(ctype->get_declaration()->get_parent())) {
      id = td->get_mangled_name().str();
    }
  }
  ROSE_ASSERT(id != "" && id != "''");

  /*nested type -> nested term*/
  return new PrologCompTerm("enum_type", /*1,*/ new PrologAtom(id));
}


/** 
 * class: SgClassType
 * term: class_type(name,type,scope)
 * arg name: name of the class
 * arg type: type enum of the class (class/struct/union)
 * arg scope: name of the scope 
 */
PrologCompTerm* 
RoseToTerm::getClassTypeSpecific(SgType* mtype) {
  /*make sure we are actually dealing with a class type*/
  SgClassType* ctype = isSgClassType(mtype);
  ROSE_ASSERT(ctype != NULL);

  SgClassDeclaration* d = isSgClassDeclaration(ctype->get_declaration());
  ROSE_ASSERT(d != NULL);

  return new PrologCompTerm
    ("class_type", //3, 
     /*add base type*/
     new PrologAtom(ctype->get_name().str()),
     /* what kind of class is this?*/
     getEnum(d->get_class_type(), re.class_types),
     /* add qualified name of scope*/
     new PrologAtom
     (d->get_scope()->get_scope()->get_qualified_name().getString()));
}

/**
 * class: SgTypedefType
 * term: typedef_type(name, base)
 * arg name: name of the new type
 * arg base: basetype
 */
PrologCompTerm* 
RoseToTerm::getTypedefTypeSpecific(SgType* mtype) {
  /* make sure this is actually a SgTypedefType*/
  SgTypedefType* tp = isSgTypedefType(mtype);
  ROSE_ASSERT(tp != NULL);

  /* create term and add name*/
  return new PrologCompTerm
    ("typedef_type", //2,
     new PrologAtom(tp->get_name().getString()),
     (tp->get_base_type() != NULL /* add base type */
      ? getTypeSpecific(tp->get_base_type())
      : new PrologAtom("null")));
}

/**
 * class: SgConstructorInitializer
 * term: constructor_initializer_annotiation(name)
 * arg name: qualified class name
 */
PrologCompTerm* 
RoseToTerm::getConstructorInitializerSpecific(SgConstructorInitializer* ci) {
  ROSE_ASSERT(ci != NULL);
  /* get name from class declaration*/
  SgClassDeclaration* dec = ci->get_class_decl();
  ROSE_ASSERT(dec != NULL);
  return new PrologCompTerm
    ("constructor_initializer_annotation", //2,
     new PrologAtom(dec->get_qualified_name().getString()),
     PPI(ci));
}

/**
 * class: SgNewExp
 * term: new_exp_annotation(type)
 * arg type: type of the expression
 */
PrologCompTerm* 
RoseToTerm::getNewExpSpecific(SgNewExp* ne) {
  ROSE_ASSERT(ne != NULL);
  /*create annot term*/
  return new PrologCompTerm
    ("new_exp_annotation", //2,
     getTypeSpecific(ne->get_specified_type()), /* add type term*/
     PPI(ne));
}



/** 
 * class: SgArrayType
 * term: array_type(nested,index)
 * arg nested: nested type
 * arg index: index (a SgExpression)
 */
PrologCompTerm* 
RoseToTerm::getArrayTypeSpecific(SgType* mtype) {
  /*make sure we are actually dealing with an array type*/
  SgArrayType* a = isSgArrayType(mtype);
  ROSE_ASSERT(a != NULL);
  return new PrologCompTerm
    ("array_type", //2,
     getTypeSpecific(a->get_base_type()), /* get nested type*/ 
     (a->get_index() /* get expression*/
      ? traverseSingleNode(a->get_index())
      : new PrologAtom("null")));
}

/**
 * class: SgModifierType
 * term: modifier_type(nested,tmod)
 * arg nested: nested type
 * tmod: term representation of type modifier
 */
PrologCompTerm* 
RoseToTerm::getModifierTypeSpecific(SgType* stype) {
  /*make sure we are actually dealing with a modifier type*/
  SgModifierType* m = isSgModifierType(stype);
  ROSE_ASSERT(m != NULL);
  return new PrologCompTerm
    ("modifier_type", //2,
     /* base type*/ getTypeSpecific(m->get_base_type()),
     /* type modifier*/ getTypeModifierSpecific(&(m->get_typeModifier())));
}


/** 
 * create representation for a type.
 * Result is an atom for the primitive types and a nested
 * term for the complex types.
 * */
PrologTerm* 
RoseToTerm::getTypeSpecific(SgType* stype) {
  string cn = stype->class_name();
  /*type is represented by a prolog term*/
  PrologTerm*  t = NULL;
  /* composite types implemented in different functions*/
  if (cn == "SgFunctionType") {
    t = getFunctionTypeSpecific(stype);
  } else if (cn == "SgPointerType") {
    t = getPointerTypeSpecific(stype);
  } else if (cn == "SgClassType") {
    t = getClassTypeSpecific(stype);
  } else if (cn == "SgTypedefType") {
    t = getTypedefTypeSpecific(stype);
  } else if (cn == "SgEnumType") {
    t = getEnumTypeSpecific(stype);
  } else if (cn == "SgArrayType") {
    t = getArrayTypeSpecific(stype);
  } else if (cn == "SgModifierType") {
    t = getModifierTypeSpecific(stype);
  } else if (cn == "SgMemberFunctionType") {
    t = getMemberFunctionTypeSpecific(stype);
  } else if ( /* simple types */
	     cn == "SgTypeBool"  ||
	     cn == "SgTypeChar"  ||
	     cn == "SgTypeDefault"  ||
	     cn == "SgTypeDouble"  ||
	     cn == "SgTypeEllipse"  ||
	     cn == "SgTypeFloat"  ||
	     cn == "SgTypeGlobalVoid"  ||
	     cn == "SgTypeInt"  ||
	     cn == "SgTypeLong"  ||
	     cn == "SgTypeLongDouble"  ||
	     cn == "SgTypeLongLong"  ||
	     cn == "SgTypeShort"  ||
	     cn == "SgTypeSignedChar"  ||
	     cn == "SgTypeSignedInt"  ||
	     cn == "SgTypeSignedLong"  ||
	     cn == "SgTypeSignedShort"  ||
	     cn == "SgTypeString"  ||
	     cn == "SgTypeUnknown"  ||
	     cn == "SgTypeUnsignedChar"  ||
	     cn == "SgTypeUnsignedInt"  ||
	     cn == "SgTypeUnsignedLong"  ||
	     cn == "SgTypeUnsignedLongLong"  ||
	     cn == "SgTypeUnsignedShort"  ||
	     cn == "SgTypeVoid"  ||
	     cn == "SgTypeWchar"
	      ) {  
    t =  new PrologAtom(prologize(cn));
  } else {
    PrologCompTerm* ct  = new PrologCompTerm
      ("not_yet_implemented", /*1,*/ new PrologAtom(stype->class_name()));
    t = ct;
  }
  /*we should have created some type info here*/
  ROSE_ASSERT(t != NULL);
  return t;
}


/**
 * class: SgUnaryOp
 * term: unary_op_annotation(mode,type,throw_kind)
 * arg mode: prefix or postfix
 * arg type: type of the expression
 * arg throw_kind: an integer flag of throw ops
 */
PrologCompTerm* 
RoseToTerm::getUnaryOpSpecific(SgUnaryOp* op) {
  PrologTerm *e3, *e4;
  if(SgThrowOp* thrw = dynamic_cast<SgThrowOp*>(op)) {
    /*Throw Ops also have a 'throw kind'*/
    // GB (2008-08-23): As of ROSE 0.9.3.a-1593, throw ops no longer have a
    // type list. Or was it only removed temporarily? TODO: Check again
    // sometime.
    e3 = getEnum(thrw->get_throwKind(), re.throw_kinds);
#if 0
    SgTypePtrListPtr types = thrw->get_typeList ();
    SgTypePtrList::iterator it = types->begin();
    PrologList* l = PrologList();
    while (it != types->end()) {
      l->addElement(getTypeSpecific(*it));
      it++;
    }
#else
    e4 = new PrologAtom("null");
#endif
  } else if (SgCastExp* cst = dynamic_cast<SgCastExp*>(op)) {
    /*Casts have a cast type*/
    e3 = getEnum(cst->get_cast_type(), re.cast_types);
    /*assure that arity = 4*/
    e4 = new PrologAtom("null");
  } else {
    /*assure that arity = 4*/
    e3 = new PrologAtom("null");
    e4 = new PrologAtom("null");
  }
  return new PrologCompTerm
    ("unary_op_annotation", //5,
     new PrologAtom(op->get_mode() == SgUnaryOp::prefix ? "prefix" : "postfix"),
     getTypeSpecific(op->get_type()),
     e3, 
     e4,
     PPI(op));	    
}

/**
 * class: SgBinaryOp
 * term: binary_op_annotation(type)
 * arg type: the type of the expression
 */
PrologCompTerm* 
RoseToTerm::getBinaryOpSpecific(SgBinaryOp* op) {
  return new PrologCompTerm
    ("binary_op_annotation", //2,
     getTypeSpecific(op->get_type()),
     PPI(op));
}

/**
 * class: SgValueExp
 * term: value_annotation(val)
 * arg val: value of the SgValueExp. The possibilities are integers 
 * (for booleans and the smaller integer types) or quoted strings 
 * using << on ostringstreams for alll other types
 */
PrologCompTerm* 
RoseToTerm::getValueExpSpecific(SgValueExp* astNode) {
  PrologTerm *val;
  /* int and enum types */	
  if(SgIntVal* n = dynamic_cast<SgIntVal*>(astNode)) {
    val = new PrologInt(n->get_value());
  } else if(SgUnsignedIntVal* n = dynamic_cast<SgUnsignedIntVal*>(astNode)) {
    val = new PrologInt(n->get_value());
  } else if(SgShortVal* n = dynamic_cast<SgShortVal*>(astNode)) {
    val = new PrologInt(n->get_value());
  } else if(SgUnsignedShortVal* n = dynamic_cast<SgUnsignedShortVal*>(astNode)) {
    val = new PrologInt(n->get_value());
  } else if (SgLongIntVal* n = dynamic_cast<SgLongIntVal*>(astNode)) {
    val = new PrologInt(n->get_value());
  } else if (SgUnsignedLongVal* n = dynamic_cast<SgUnsignedLongVal*>(astNode)) {
    val = new PrologInt(n->get_value());
  } else if (SgLongLongIntVal* n = dynamic_cast<SgLongLongIntVal*>(astNode)) {
    val = new PrologInt(n->get_value());
  } else if (SgUnsignedLongLongIntVal* n = dynamic_cast<SgUnsignedLongLongIntVal*>(astNode)) {
    val = new PrologInt(n->get_value());
  } else if(SgEnumVal* n = dynamic_cast<SgEnumVal*>(astNode)) { /*FIXME*/
    /* value*/
    val = new PrologInt(n->get_value()); 
    /* name of value*/
    val = new PrologAtom(n->get_name().getString());
    /* name of declaration*/
    SgEnumType *type = isSgEnumDeclaration(n->get_declaration())->get_type();
    ROSE_ASSERT(type != NULL);
    val = getEnumTypeSpecific(type);
  }
  /* float types */
  else if (SgFloatVal* n = dynamic_cast<SgFloatVal*>(astNode)) {
    ostringstream o;
    o << n->get_value();
    string s = o.str();
    val = new PrologAtom(s);
  } else if (SgDoubleVal* n = dynamic_cast<SgDoubleVal*>(astNode)) {
    ostringstream o;
    o << n->get_value();
    string s = o.str();
    val = new PrologAtom(s);
  } else if (SgLongDoubleVal* n = dynamic_cast<SgLongDoubleVal*>(astNode)) {
    ostringstream o;
    o << n->get_value();
    string s = o.str();
    val = new PrologAtom(s);
  } 
  /* boolean type */ 
  else if (SgBoolValExp* n = dynamic_cast<SgBoolValExp*>(astNode)) {
    ostringstream o;
    o << n->get_value();
    string s = o.str();
    val = new PrologAtom(s);
  } 
  /* char and string types */
  else if (SgCharVal* n = dynamic_cast<SgCharVal*>(astNode)) {
    val = new PrologInt((int)n->get_value()); 
  } else if (SgUnsignedCharVal* n = dynamic_cast<SgUnsignedCharVal*>(astNode)) {
    val = new PrologInt((unsigned)n->get_value()); 
  } else if (SgWcharVal* n = dynamic_cast<SgWcharVal*>(astNode)) {
    ostringstream o;
    o << n->get_valueUL();
    string s = o.str();
    val = new PrologAtom(s);
  } else if (SgStringVal* n = dynamic_cast<SgStringVal*>(astNode)) {
    val = new PrologAtom(n->get_value());
  } else {
    val = new PrologAtom("null");
  }
  return new PrologCompTerm("value_annotation", /*2,*/ val, PPI(astNode));
}
/**
 * class: SgAssignInitializer
 * term: assign_initializer_annotation(tpe)
 * arg tpe: type of the initializer
 */
PrologCompTerm*  
RoseToTerm::getAssignInitializerSpecific(SgAssignInitializer* ai) {
  return new PrologCompTerm("assign_initializer_annotation", //2,
			    getTypeSpecific(ai->get_type()),
			    PPI(ai));
}
/**
 * class: SgVarRefExp
 * term: var_ref_exp_annotation(tpe,name,static,scope)
 * arg tpe: type
 * arg name: name
 * arg static: wether the declaration was static
 * arg scope: scope name (either from a namespace, a class or "null")
 */
PrologCompTerm* 
RoseToTerm::getVarRefExpSpecific(SgVarRefExp* vr) {
  SgInitializedName* n = vr->get_symbol()->get_declaration();
  /* type: in general, this can be taken "as is" from the ROSE AST. However,
   * ROSE (up to 0.9.4a-8xxx at least) gets a detail wrong: a declaration
   * like "int arr[]" as a function parameter declares a *pointer*, not an
   * array. Thus we check whether the variable might be of this sort, and if
   * yes, we must make a pointer type for it. */
  PrologTerm* typeSpecific = NULL;
  SgInitializedName* vardecl = vr->get_symbol()->get_declaration();
  SgType* t = vr->get_type()->stripType(SgType::STRIP_MODIFIER_TYPE
                                      | SgType::STRIP_REFERENCE_TYPE
                                      | SgType::STRIP_TYPEDEF_TYPE);
  if (isSgFunctionParameterList(vardecl->get_parent()) && isSgArrayType(t)) {
    SgType* baseType = isSgArrayType(t)->get_base_type();
    PrologTerm* baseTypeSpecific = getTypeSpecific(baseType);
    typeSpecific = new PrologCompTerm("pointer_type", /*1,*/ baseTypeSpecific);
  } else {
    typeSpecific = getTypeSpecific(n->get_typeptr());
  }
  /* static? (relevant for unparsing if scope is a class)*/
  PrologTerm *isStatic;
  SgDeclarationStatement* vdec = n->get_declaration();
  if (vdec != NULL) {
    isStatic = 
      getEnum(vdec->get_declarationModifier().get_storageModifier().isStatic(),
	      re.static_flags);
  } else {
    isStatic = getEnum(0, re.static_flags);
  }
  PrologTerm* scope;
  if (vdec != NULL) {
    /* named scope or irrelevant?*/
    if (SgNamespaceDefinitionStatement* scn = 
	isSgNamespaceDefinitionStatement(vdec->get_parent())) {
      scope = getNamespaceScopeName(scn);
    } else if (SgClassDefinition* scn = 
	       isSgClassDefinition(vdec->get_parent())) {
      scope = getClassScopeName(scn);
    } else {
      scope = new PrologAtom("null");
    }
  } else {
    scope = new PrologAtom("null");
  }

  return new PrologCompTerm
    ("var_ref_exp_annotation", //5,
     typeSpecific,
     /* name*/ new PrologAtom(n->get_name().getString()),
     isStatic,
     scope,
     PPI(vr));
}

/**
 * class: SgInitializedName
 * term: initialized_name_annotation(tpe,name,static,scope)
 * arg tpe: type
 * arg name: name
 * arg static: wether the declaration was static
 * arg scope: scope name (either from a namespace, a class or "null")
 */
PrologCompTerm* 
RoseToTerm::getInitializedNameSpecific(SgInitializedName* n) {
  /* named scope or irrelevant?*/
  PrologTerm* scope;
  if(SgNamespaceDefinitionStatement* scn = 
     isSgNamespaceDefinitionStatement(n->get_scope())) {
    scope = getNamespaceScopeName(scn);
  } else if (SgClassDefinition* scn = isSgClassDefinition(n->get_scope())) {
    scope = getClassScopeName(scn);
  } else {
    scope = new PrologAtom("null");
  }
  return new PrologCompTerm
    ("initialized_name_annotation", //4,
     getTypeSpecific(n->get_typeptr()),
     new PrologAtom(n->get_name().getString()),
     /* static? (relevant for unparsing if scope is a class)*/
     getEnum(n->get_storageModifier().isStatic(), re.static_flags),
     scope);
}

/**
 * class: SgClassDeclaration
 * term: class_declaration_annotation(name,class_type,type)
 * arg name: class name
 * arg class_type: class type as required by ROSE
 * arg type: SgClassType of the class declaration.
 * */
PrologCompTerm* 
RoseToTerm::getClassDeclarationSpecific(SgClassDeclaration* cd) {
  PrologTerm *typet = getTypeSpecific(cd->get_type());
  typeWasDeclaredBefore(typet->getRepresentation());
  return new PrologCompTerm
    ("class_declaration_annotation", //4,
     /* add name and type*/
     new PrologAtom(cd->get_name().str()),
     getEnum(cd->get_class_type(), re.class_types),
     getTypeSpecific(cd->get_type()),
     PPI(cd));
}

/**
 * class: SgClassDefinition
 * term: class_definition_annotation(fileinfo)
 * arg fileinfo: file info information for end of construct
 * */
PrologCompTerm* 
RoseToTerm::getClassDefinitionSpecific(SgClassDefinition* def) {
  return new PrologCompTerm
    ("class_definition_annotation", //2,
     /* add end of construct*/
     getFileInfo(def->get_endOfConstruct()),
     PPI(def));
}

/**
 * class: SgNamespaceDeclarationStatement
 * term: namespace_declaration_statement(name,unnamed)
 * arg name: name of the namespace
 * arg unnamed: unnamed namespace
 */
PrologCompTerm* 
RoseToTerm::getNamespaceDeclarationStatementSpecific
(SgNamespaceDeclarationStatement* dec) {
  return new PrologCompTerm
    ("namespace_declaration_statement", //3,
     /* name*/ new PrologAtom(dec->get_name().getString()),
     /* unnamed?*/ new PrologInt((int) dec->get_isUnnamedNamespace()),
     PPI(dec));
}

/** create a list of atoms from a bit vector*/
PrologTerm* 
RoseToTerm::getBitVector(const SgBitVector &v, const vector<string> &names) {
  PrologList* l = new PrologList;
  reverse_iterator<SgBitVector::const_iterator> it = v.rbegin();
  reverse_iterator<vector<string>::const_iterator> name = names.rbegin();
  
  ROSE_ASSERT(v.size() <= names.size());
  while(it != v.rend()) {
    if (*it == true)
      l->addFirstElement(new PrologAtom(*name));
    it++;
    name++;
  }
  return l;
}

/** create a list of atoms from a bit vector*/
PrologTerm* 
RoseToTerm::getEnum(int enum_val, const vector<string> &names) {
  ROSE_ASSERT(enum_val < names.size());
  return new PrologAtom(names[enum_val]);
}

/**
 * class: SgConditionalExp
 * term: conditional_exp_annotation(type)
 * arg type: type of the expression
 * */
PrologCompTerm* 
RoseToTerm::getConditionalExpSpecific(SgConditionalExp* c) {
  return new PrologCompTerm
    ("conditional_exp_annotation", //2,
     getTypeSpecific(c->get_type()),
     PPI(c));
}

/**
 * class: SgLabelStatement
 * term: label_annotation(label)
 * arg label: name of the label
 * */
PrologCompTerm*  
RoseToTerm::getLabelStatementSpecific(SgLabelStatement* label) {
  //get name of the label
  string s = *(new string(label->get_label().getString()));
  // create a term containing the name;
  return new PrologCompTerm("label_annotation", //2,
			    new PrologAtom(s),
			    PPI(label));
}

/**
 * class: SgGotoStatement
 * term: label_annotation(label)
 * arg label: name of the label associated with the goto
 * */
PrologCompTerm*  
RoseToTerm::getGotoStatementSpecific(SgGotoStatement* sgoto) {
  SgLabelStatement* l = sgoto->get_label();
  ROSE_ASSERT(l != NULL);
  /* we need the information to create the label*/
  return getLabelStatementSpecific(l);	
}

/**
 * class SgEnumDeclaration
 * term: enum_declaration_annotation(name,decl_att)
 * arg name: name of the enum
 * arg decl_att: declaration attributes (see SgDeclarationStatement
 */
PrologCompTerm* 
RoseToTerm::getEnumDeclarationSpecific(SgEnumDeclaration* d) {
  ROSE_ASSERT(d != NULL);
  //get Enum name
  string ename = d->get_name().getString();
  if (ename == "") {
    /* nameless enum declarations can occur in typedefs */
    SgTypedefDeclaration *td;
    if (td = isSgTypedefDeclaration(d->get_parent())) {
      ename = td->get_mangled_name().str();
    }
  }
  ROSE_ASSERT(ename != "");
  PrologTerm *typet = getTypeSpecific(d->get_type());
  typeWasDeclaredBefore(typet->getRepresentation());

  //create term
  return new PrologCompTerm
    ("enum_declaration_annotation", //4,
     new PrologAtom(ename),
     getDeclarationAttributes(d),
     new PrologInt(d->get_embedded()),
     PPI(d));
}

/**
 * class SgDeclarationStatement
 * term decl_attributes(nameonly,forward,externbrace,skipelaboratetype,neednamequalifier
 * arg all: boolean flags common to all declaration statements
 */
PrologCompTerm* 
RoseToTerm::getDeclarationAttributes(SgDeclarationStatement* s) {
  ROSE_ASSERT(s != NULL);
  return new PrologCompTerm
    ("decl_attributes", //5,
     new PrologInt(s->get_nameOnly()),
     new PrologInt(s->get_forward()),
     new PrologInt(s->get_externBrace()),
     new PrologInt(s->get_skipElaborateType()),
     // ROSE 0.8.8a: new PrologInt(s->get_need_name_qualifier()),
     new PrologInt(0)); // set dummy value
}


/**
 * class: SgDeleteExp
 * term: delete_exp_annotation(is_array,need_global_specifier)
 * arg all: short "flags"
 */
PrologCompTerm* 
RoseToTerm::getDeleteExpSpecific(SgDeleteExp* de) {
  return new PrologCompTerm("delete_exp_annotation", //3,
			    new PrologInt(de->get_is_array()),
			    new PrologInt(de->get_need_global_specifier()),
			    PPI(de));
}

/**
 * class: SgRefExp
 * term: ref_exp_annotation(type)
 * arg type: type of the expression
 */
PrologCompTerm* 
RoseToTerm::getRefExpSpecific(SgRefExp* re) {
  return new PrologCompTerm("ref_exp_annotation", //2,
			    getTypeSpecific(re->get_type()),
			    PPI(re));
}

/**
 * class: SgVariableDeclaration
 * term: variable_declaration_specific(dm)
 * arg dm: declaration modifier information (see annotation of SgDeclarationModifier)
 */

PrologCompTerm* 
RoseToTerm::getVariableDeclarationSpecific(SgVariableDeclaration* d) {
  ROSE_ASSERT(d != NULL);
  /* add base type forward declaration */
  SgNode *baseTypeDecl = NULL;
  if (d->get_variableDeclarationContainsBaseTypeDefiningDeclaration()) {
    baseTypeDecl = d->get_baseTypeDefiningDeclaration();
  } else {
    /* The complication is that in the AST, the type declaration member is
     * only set if it is a type definition, not if it is a forward
     * declaration. So we need to check whether the base type (possibly
     * below layers of pointers) is a class type, and whether its first
     * declaration appears to be hidden here in the variable declaration. */
    SgClassType *ctype = isSgClassType(d->get_variables().front()
                                       ->get_type()->findBaseType());
    if (ctype) {
      /* See if the type is declared in the scope where it belongs. If no,
       * then the declaration is apparently inside this variable
       * declaration, so we add it as a subterm. */
      SgDeclarationStatement *cdecl = ctype->get_declaration();
      SgSymbol *symbol = cdecl->get_symbol_from_symbol_table();
      if (!typeWasDeclaredBefore(
           getTypeSpecific(symbol->get_type())->getRepresentation())) {
        baseTypeDecl = cdecl;
      }
    }
  }

  return new PrologCompTerm
    ("variable_declaration_specific", //3,
     getDeclarationModifierSpecific(&(d->get_declarationModifier())),
     (baseTypeDecl != NULL
      ? traverseSingleNode(baseTypeDecl)
      : new PrologAtom("null")),
     PPI(d));
}

/**
 * class: SgVarArgCopyOp, SgVarArgEndOp, SgVarArgOp, SgVarArgStartOp, SgVarArgStartOneOperatorOp
 * term: vararg_annotation(type)
 * arg type: type of the expression
 */
PrologCompTerm* 
RoseToTerm::getVarArgSpecific(SgExpression* e) {
  return new PrologCompTerm("vararg_annotation", //2,
			    getTypeSpecific(e->get_type()),
			    PPI(e));
}

/**
 * traverse single node
 */
PrologTerm* 
RoseToTerm::traverseSingleNode(SgNode* astNode) {
  BasicTermPrinter tempt;
  tempt.traverse(astNode);
  PrologTerm*  rep = tempt.getTerm();
  ROSE_ASSERT(rep != NULL);
  return rep;
}

/**
 * class: SgAccessModifier
 * term: access_modifier(a)
 * arg a: enum value of SgAccessModifier (see ROSE docs!)
 */
// PrologCompTerm* 
// RoseToTerm::getAccessModifierSpecific(SgAccessModifier* a) {
//   PrologCompTerm* t = PrologCompTerm("access_modifier");
//   t->addSubterm(PrologInt((int) a->get_modifier()));
//   return t;
// }

/**
 * class: SgBaseClassModifier
 * term: base_class_modifier(b,a)
 * arg b: enum value of SgBaseClassModifier (see ROSE docs!)
 * arg a: enum value of SgAccessModifier
 */
PrologCompTerm* 
RoseToTerm::getBaseClassModifierSpecific(SgBaseClassModifier* b) {
  return new PrologCompTerm
    ("base_class_modifier", //2,
     new PrologInt((int) b->get_modifier()),
     new PrologInt((int) b->get_accessModifier().get_modifier()));
}

/**
 * class: SgFunctionModifier
 * term: function_modifier(b)
 * arg b: bit vector of SgFunctionModifier as PrologList (true = 1)
 */
PrologCompTerm* 
RoseToTerm::getFunctionModifierSpecific(SgFunctionModifier* f) {
  return new PrologCompTerm
    ("function_modifier", //1,
     /* get bit vector and convert to PROLOG*/
     getBitVector(f->get_modifierVector(), re.function_modifiers));
}

/**
 * class: SgSpecialFunctionModifier
 * term: special_function_modifier(b)
 * arg b: bit vector of SgFunctionModifier as PrologList (true = 1)
 */
PrologCompTerm* 
RoseToTerm::getSpecialFunctionModifierSpecific(SgSpecialFunctionModifier* f) {
  return new PrologCompTerm
    ("special_function_modifier", //1,
     /* get bit vector and convert to PROLOG*/
     getBitVector(f->get_modifierVector(),re.special_function_modifiers));
}

/**
 * class: SgLinkageModifier
 * term: linkage_modifier(a)
 * arg a: enum value of SgLinkageModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getLinkageModifierSpecific(SgLinkageModifier* a) {
  return new PrologCompTerm("linkage_modifier", //1,
			    new PrologInt((int) a->get_modifier()));
}
/**
 * class: SgStorageModifier
 * term: storage_modifier(a)
 * arg a: enum value of SgStorageModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getStorageModifierSpecific(SgStorageModifier* a) {
  return new PrologCompTerm("storage_modifier", //1,
			    getEnum(a->get_modifier(), re.storage_modifiers));
}
/**
 * class: SgElaboratedTypeModifier
 * term: elaborated_type_modifier(a)
 * arg a: enum value of SgElaboratedTypeModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getElaboratedTypeModifierSpecific(SgElaboratedTypeModifier* a) {
  return new PrologCompTerm("elaborated_type_modifier", //1,
			    new PrologInt((int) a->get_modifier()));
}
/**
 * class: SgConstVolatileModifier
 * term: const_volatile_modifier(a)
 * arg a: enum value of SgConstVolatileModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getConstVolatileModifierSpecific(SgConstVolatileModifier* a) {
  return new PrologCompTerm("const_volatile_modifier", //1,
			    getEnum(a->get_modifier(), re.cv_modifiers));
}
/**
 * class: SgUPC_AccessModifier
 * term: upc_access_modifier(a)
 * arg a: enum value of SgUPC_AccessModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getUPC_AccessModifierSpecific(SgUPC_AccessModifier* a) {
  return new PrologCompTerm
    ("upc_access_modifier", //1,
     getEnum(a->get_modifier(), re.upc_access_modifiers));
}

/**
 * class: SgTypeModifier
 * term: type_modifier(b,u,c,e)
 * arg b: bit vector of SgTypeModifier
 * arg u: enum of SgUPC_AccessModifier
 * arg c: enum of SgConstVolatileModifier
 * arg e: enum of SgElaboratedTypeModifier
 */
PrologCompTerm* 
RoseToTerm::getTypeModifierSpecific(SgTypeModifier* a) {
  return new PrologCompTerm
    ("type_modifier", //4,
     /* get bit vector and convert to PROLOG*/
     getBitVector(a->get_modifierVector(), re.type_modifiers),
     /* add enums*/
     getEnum(a->get_upcModifier().get_modifier(), re.upc_access_modifiers),
     getEnum(a->get_constVolatileModifier().get_modifier(), re.cv_modifiers),
     getEnum(a->get_elaboratedTypeModifier().get_modifier(), 		   
	     re.elaborated_type_modifiers));
}

/**
 * class: SgDeclarationModifier
 * term: declaration_modifier(e,t,a,s)
 * arg e: enum of SgDeclarationModifier
 * arg t: term representation of SgTypeModifier
 * arg a: enum of SgAccessModifier
 * arg s: enum of SgStorageModifier
 */
PrologCompTerm* 
RoseToTerm::getDeclarationModifierSpecific(SgDeclarationModifier* dm) {
  return new PrologCompTerm
    ("declaration_modifier", //4,
     getBitVector(dm->get_modifierVector(), re.declaration_modifiers),
     getTypeModifierSpecific(&(dm->get_typeModifier())),
     getEnum(dm->get_accessModifier().get_modifier(), re.access_modifiers),
     getEnum(dm->get_storageModifier().get_modifier(),re.storage_modifiers));
}

/**
 * class: SgFunctionRefExp
 * term: function_ref_exp_annotation(n,ft)
 * arg n: name of the function
 * arg ft: type of the function (via getTypeSpecific)
 */
PrologCompTerm* 
RoseToTerm::getFunctionRefExpSpecific(SgFunctionRefExp* r) {
  ROSE_ASSERT(r != NULL);
  /* get name and type from SgFunctionSymbol that is linked from this node*/
  SgFunctionSymbol* s = r->get_symbol();
  ROSE_ASSERT(s != NULL);
  SgType* tpe = s->get_type();
  ROSE_ASSERT(tpe != NULL);
  /*create Prolog Term*/
  return new PrologCompTerm("function_ref_exp_annotation", //3,
			    new PrologAtom(s->get_name().getString()),
			    getTypeSpecific(tpe),
			    PPI(r));
	
}
/**
 * class: SgMemberFunctionRefExp
 * term: member_function_ref_exp_annotation(sym,vc,ft,nq)
 * arg sym: member function symbol annotation
 * arg vc: wether this is a virtual call
 * arg ft: type of the function (via getTypeSpecific)
 * arg nq: wether a qualifier is needed
 */
PrologCompTerm* 
RoseToTerm::getMemberFunctionRefExpSpecific(SgMemberFunctionRefExp* r) {
  ROSE_ASSERT(r != NULL);
  /* get member function symbol information*/
  SgMemberFunctionSymbol* s = r->get_symbol();
  ROSE_ASSERT(s != NULL);

  return new PrologCompTerm
    ("member_function_ref_exp_annotation", //5,
     getMemberFunctionSymbolSpecific(s),
     new PrologInt(r->get_virtual_call()), // virtual call?
     getTypeSpecific(s->get_type()),   // type
     new PrologInt(r->get_need_qualifier()),   // need qualifier?
     PPI(r));
}

/**
 * class: SgFunctionCallExp
 * term: function_call_exp_annotation(rt) 
 * arg rt: return type (via getTypeSpecific)
 */
PrologCompTerm* 
RoseToTerm::getFunctionCallExpSpecific(SgFunctionCallExp* c) {
  ROSE_ASSERT(c != NULL);
  /* create Prolog Term*/
  return new PrologCompTerm("function_call_exp_annotation", //2,
			    getTypeSpecific(c->get_type()),
			    PPI(c));
}


/**
 * class: SgMemberFunctionDeclaration
 * term: member_function_declaration_annotation(t,name,scope,mod)
 * term t: type
 * term name: name
 * term scope: class scope
 * arg mod: declaration modifier representation
 */

PrologCompTerm* 
RoseToTerm::getMemberFunctionDeclarationSpecific(SgMemberFunctionDeclaration* decl) {
  /* add scope */
  SgClassDefinition* def = decl->get_class_scope();

  /* create term and append type and name */
  return new PrologCompTerm
    ("member_function_declaration_annotation", //5,
     getTypeSpecific(decl->get_type()),
     new PrologAtom(decl->get_name().getString()), /* add the node's name*/
     /* we add the complete class scope name here */
     getClassScopeName(def),
     /* add declaration modifier specific*/
     getDeclarationModifierSpecific(&(decl->get_declarationModifier())),
     PPI(decl));
}

/**
 * class: SgClassDefinition
 * term: class_scope(name,type)
 * arg name: qualified name of class scope
 * arg type: class type enum
 */
PrologCompTerm* 
RoseToTerm::getClassScopeName(SgClassDefinition* def) {
  ROSE_ASSERT(def != NULL);
  /* get qualified name of scope and type of class declaration*/
  SgClassDeclaration* decl = def->get_declaration();
  ROSE_ASSERT(decl != NULL);
  string qname = decl->get_qualified_name().getString();
  /* create a PrologCompTerm*/
  return new PrologCompTerm("class_scope", //3,
			    new PrologAtom(qname),
			    getEnum(decl->get_class_type(), re.class_types),
			    PPI(def));
}

/**
 * class: SgNamespaceDefinition
 * term: namespace_scope(name,unnamed)
 * arg name: qualified name of the namespace
 * arg unnamed: wether the namespace is unnamed
 */

PrologCompTerm* 
RoseToTerm::getNamespaceScopeName(SgNamespaceDefinitionStatement* def) {
  ROSE_ASSERT(def != NULL);
  /* get declaration*/
  SgNamespaceDeclarationStatement* decl = def->get_namespaceDeclaration();
  ROSE_ASSERT(decl != NULL);
  /* create annotation term*/
  return new PrologCompTerm
    ("namespace_scope", //3,
     /* add qualified name*/
     new PrologAtom(decl->get_qualified_name().getString()),
     /* add unnamed */
     new PrologInt((int)decl->get_isUnnamedNamespace()),
     PPI(def));
}


/**
 * class: SgMemberFunctionSymbol
 * term: member_function_symbol_annotation(mf,scope)
 * arg mf: complete PROLOG-Representation of member function declaration without definition
 * arg scope: class scope (see getClassScopeName)
 */
PrologCompTerm* 
RoseToTerm::getMemberFunctionSymbolSpecific(SgMemberFunctionSymbol* sym) {
  SgMemberFunctionDeclaration* orig_decl = sym->get_declaration();
  ROSE_ASSERT(orig_decl != NULL);
  /* save wether original node had declaration*/
  bool orig_decl_has_def = (orig_decl->get_definition() == NULL);
  /* clone node (deep copy) */
  // ROSE 0.8.8a SgMemberFunctionDeclaration* cop_decl = isSgMemberFunctionDeclaration(orig_decl->copy(SgTreeCopy()));
  // GB (2008-03-05): This copy statement produces interesting warnings like:
  // WARNING: Scopes do NOT match! variable = 0x2aaaadaa2760 = n (could this
  // be a static variable, or has the symbol table been setup before the
  // scopes have been set?) 
  // Error: Symbol not found for initializedName_copy = 0x2aaaadaa2760 = n
  SgMemberFunctionDeclaration* cop_decl = isSgMemberFunctionDeclaration(orig_decl->copy(*new SgTreeCopy()));
  ROSE_ASSERT(cop_decl != NULL);
  /* make sure we didn't change the orginal node*/
  ROSE_ASSERT(orig_decl_has_def == (orig_decl->get_definition() == NULL));
  cop_decl->set_definition(NULL);
  // GB (2008-03-05): For some reason, cop_decl has a null parent. This
  // causes problems within the nested traversal. Therefore: Copy the parent
  // pointer manually. (It took me two days to find this bug, BTW.)
  cop_decl->set_parent(orig_decl->get_parent());
  /* add scope*/
  SgClassDefinition* cdef = orig_decl->get_class_scope();
  ROSE_ASSERT(cdef != NULL);
  
  PrologCompTerm* t = new PrologCompTerm
    ("member_function_symbol_annotation", //2, 
     traverseSingleNode(cop_decl),
     getClassScopeName(cdef));

  delete cop_decl;
  return t;
}

/**
 * class: SgSizeOfOp
 * term: size_of_op_annotation(otype,etype)
 * term otype: operand type
 * term etype: expression type
 */
PrologCompTerm* 
RoseToTerm::getSizeOfOpSpecific(SgSizeOfOp* o) {
  PrologTerm* e1, *e2;
  ROSE_ASSERT(o != NULL);
  /* create type info if types are present*/
  SgType* otype = o->get_operand_type();
  if (otype != NULL) {
    e1 = getTypeSpecific(otype);
  } else {
    e1 = new PrologAtom("null");
  }
  SgType* etype = o->get_type();
  if (etype != NULL) {
    e2 = getTypeSpecific(etype);
  } else {
    e2 = new PrologAtom("null");
  }
  return new PrologCompTerm("size_of_op_annotation", /*3,*/ e1, e2, PPI(o));
}


/**
 * class: SgTypedefDeclaration
 * term: typedef_annotation(name,base_type,decl)
 * arg name: qualified name of the typedef
 * arg base_type: base type of the typedef
 * arg decl: declaration statement (unparsed), null if none exists
 */
PrologCompTerm* 
RoseToTerm::getTypedefDeclarationSpecific(SgTypedefDeclaration* d) {
  ROSE_ASSERT(d != NULL);
  /*get name*/
  // FIXME :: t->addSubterm(PrologAtom(d->get_qualified_name().getString()));

  return new PrologCompTerm
    ("typedef_annotation", //3,
     new PrologAtom(d->get_name().getString()),
     /*get base type*/
     getTypeSpecific(d->get_base_type()),
     /* the base type declaration is no longer in the typedef
      * annotation; it is now a child of the typedef declaration
      * itself */
     PPI(d));	
}



/**
 *class: SgTypePtrList
 *term: [T|Ts]
 *arg [T|Ts]: List of SgTypes
 */
PrologTerm* 
RoseToTerm::getTypePtrListSpecific(SgTypePtrList& tl) {
  PrologList* alist = new PrologList();
  if (&tl != NULL) {
    SgTypePtrList::iterator it = tl.begin();
    while(it != tl.end()) {
      alist->addElement(getTypeSpecific(*it));
      it++;
    }
  }
  return alist;
}

/**
 * class: SgPragma
 * term: pragma(name)
 * arg name: name
 */
PrologCompTerm* 
RoseToTerm::getPragmaSpecific(SgPragma* n) {
  // Adrian 2007-11-27:
  // This is to work around a bug in ROSE?/EDG? that inserts whitespaces
  // Hopefully I can remove it in a later revision
  string s = n->get_pragma();
  s.erase(remove_if( s.begin(), s.end(), 
		     bind1st(equal_to<char>(), ' ')), 
	  s.end());
  return new PrologCompTerm("pragma_annotation", /*1,*/ new PrologAtom(s));
}
