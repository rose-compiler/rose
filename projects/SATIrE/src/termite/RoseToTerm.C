// -*- mode: C++ -*-
/* Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
        2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#include <satire_rose.h>
#include <list>
#include "termite.h" 
// GB (2009-02-25): Use the term printer without DFI stuff.
#undef HAVE_PAG
#define HAVE_SATIRE_ICFG 0
#include "TermPrinter.h"
#include "RoseToTerm.h"
#include "satire.h"

using namespace std;

/**
 * add node specific info to a term.
 * This function, depending on the type of the node, uses private helper functions.
 * No actual work is done here.
 */
void
RoseToTerm::addSpecific(SgNode* astNode, PrologCompTerm* t) {
  PrologCompTerm* a = NULL;
  string cname = astNode->class_name();
  if (SgValueExp* n = dynamic_cast<SgValueExp*>(astNode)) {
    a = getValueExpSpecific(n);
  } else if (SgUnaryOp* n = dynamic_cast<SgUnaryOp*>(astNode)) {
    a = getUnaryOpSpecific(n);
  } else if (SgBinaryOp* n = dynamic_cast<SgBinaryOp*>(astNode)) {
    a = getBinaryOpSpecific(n);
    /*be careful with logic: SgMemberFunctionDeclaration is a subtype of SgFunctionDeclaration*/
  } else if (SgMemberFunctionDeclaration* n = dynamic_cast<SgMemberFunctionDeclaration*>(astNode)) {
    a = getMemberFunctionDeclarationSpecific(n);
  } else if (SgFunctionDeclaration* n = dynamic_cast<SgFunctionDeclaration*>(astNode)) {
    a = getFunctionDeclarationSpecific(n);
  } else if (SgInitializedName* n = dynamic_cast<SgInitializedName*>(astNode)) {
    a = getInitializedNameSpecific(n);
  } else if (SgVarRefExp* n = dynamic_cast<SgVarRefExp*>(astNode)) {
    a = getVarRefExpSpecific(n);
  } else if (SgAssignInitializer* n = dynamic_cast<SgAssignInitializer*>(astNode)) {
    a = getAssignInitializerSpecific(n);
  } else if (SgClassDeclaration* n = isSgClassDeclaration(astNode)) {
    a = getClassDeclarationSpecific(n);
  } else if (SgClassDefinition* n = isSgClassDefinition(astNode)) {
    a = getClassDefinitionSpecific(n);
  } else if (SgLabelStatement* n = isSgLabelStatement(astNode)) {
    a = getLabelStatementSpecific(n);
  } else if (SgGotoStatement* n = isSgGotoStatement(astNode)) {
    a = getGotoStatementSpecific(n);
  } else if (SgConditionalExp* n = isSgConditionalExp(astNode)) {
    a = getConditionalExpSpecific(n);
  } else if (SgEnumDeclaration* n = isSgEnumDeclaration(astNode)) {
    a = getEnumDeclarationSpecific(n);
  } else if (SgDeleteExp* n = isSgDeleteExp(astNode)) {
    a = getDeleteExpSpecific(n);
  } else if (SgRefExp* n = isSgRefExp(astNode)) {
    a = getRefExpSpecific(n);
  } else if (SgExpression* n = isSgVarArgOp(astNode)) {
    a = getVarArgSpecific(n);
  } else if (SgExpression* n = isSgVarArgCopyOp(astNode)) {
    a = getVarArgSpecific(n);
  } else if (SgExpression* n = isSgVarArgEndOp(astNode)) {
    a = getVarArgSpecific(n);
  } else if (SgExpression* n = isSgVarArgStartOp(astNode)) {
    a = getVarArgSpecific(n);
  } else if (SgExpression* n = isSgVarArgStartOneOperandOp(astNode)) {
    a = getVarArgSpecific(n);
  } else if (SgFunctionRefExp* n = isSgFunctionRefExp(astNode)) {
    a = getFunctionRefExpSpecific(n);
  } else if (SgFunctionCallExp* n = isSgFunctionCallExp(astNode)) {
    a = getFunctionCallExpSpecific(n);
  } else if (SgMemberFunctionSymbol* n = isSgMemberFunctionSymbol(astNode)) {
    a = getMemberFunctionSymbolSpecific(n);
  } else if (SgMemberFunctionRefExp* n = isSgMemberFunctionRefExp(astNode)) {
    a = getMemberFunctionRefExpSpecific(n);
  } else if (SgNamespaceDeclarationStatement* n = isSgNamespaceDeclarationStatement(astNode)) {
    a = getNamespaceDeclarationStatementSpecific(n);
  } else if (SgVariableDeclaration* n = isSgVariableDeclaration(astNode)) {
    a = getVariableDeclarationSpecific(n);
  } else if (SgTypedefDeclaration* n = isSgTypedefDeclaration(astNode)) {
    a = getTypedefDeclarationSpecific(n);
  } else if (SgSizeOfOp* n = isSgSizeOfOp(astNode)) {
    a = getSizeOfOpSpecific(n);
  } else if (SgConstructorInitializer* n = isSgConstructorInitializer(astNode)) {
    a = getConstructorInitializerSpecific(n);
  } else if (SgNewExp* n = isSgNewExp(astNode)) {
    a = getNewExpSpecific(n);
  } else if (SgPragma* n = isSgPragma(astNode)) {
    a = getPragmaSpecific(n);
  } else {
    a = new PrologCompTerm("default_annotation");
    a->addSubterm(new PrologAtom("null"));
  }

  // add preprocessing info
  if (SgLocatedNode* n = dynamic_cast<SgLocatedNode*>(astNode)) {
    a->addSubterm(getPreprocessingInfo(n->getAttachedPreprocessingInfo()));
  }

  t->addSubterm(a);
}

PrologCompTerm*
RoseToTerm::getPreprocessingInfo(AttachedPreprocessingInfoType* inf) {
  PrologCompTerm* pi = new PrologCompTerm("preprocessing_info");
  PrologList* l = new PrologList();
  if (inf != NULL) {
    for (AttachedPreprocessingInfoType::reverse_iterator it = inf->rbegin();
	 it != inf->rend(); ++it) {
      PrologCompTerm* ppd = new PrologCompTerm(
        re.DirectiveTypes[(*it)->getTypeOfDirective()]);

      bool escapeRepresentation =
          (*it)->getTypeOfDirective()
          != PreprocessingInfo::CpreprocessorIncludeDeclaration;
      ppd->addSubterm(new PrologAtom((*it)->getString(),
                                     escapeRepresentation));
      ppd->addSubterm(getEnum((*it)->getRelativePosition(),
			    re.RelativePositionTypes));

      ppd->addSubterm(getFileInfo((*it)->get_file_info()));
      l->addFirstElement(ppd);
    }
  }
  pi->addSubterm(l);
  return pi;
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
  PrologCompTerm* fi = new PrologCompTerm("file_info");
  fi->addSubterm(new PrologAtom(inf->get_filename()));
  fi->addSubterm(new PrologInt(inf->get_line()));
  fi->addSubterm(new PrologInt(inf->get_col()));
  return fi;
}

/**
 * convert ZigZagCase to zig_zag_case.
 * The reason we need this is that in PROLOG strings
 * starting with uppercase letters are variables.
 */
string 
RoseToTerm::prologize(string s) {
  string t;
  string::iterator it;
  it = s.begin();
  // remove "^Sg"
  if ((SG_PREFIX == "") && (s.length() > 2) && (s[0] == 'S') && (s[1] == 'g')) {
    ++it, ++it;
  }
  //lowercase first char (without prepending underscore)
  if(it != s.end()) {
    if(isUpper(*it)) {
      t.push_back(toLower(*it));
    } else {
      t.push_back(*it);
    }
    it++;
  }
  //replace an uppercase letter with an underscore
  //and its lowercase equivalent
  while(it != s.end()) {
    if(isUpper(*it)) {
      t.push_back('_');
      t.push_back(toLower(*it));
    } else {
      t.push_back(*it);
    }
    it++;
  }
  return t;
}


/** get lower case of upper case letter*/
char 
RoseToTerm::toLower(const char c) {
  return (c - ('A'-'a'));
};

/** is a letter upper case?*/
bool 
RoseToTerm::isUpper(const char c) {
  return (('A' <= c) && ('Z' >= c));
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
  PrologCompTerm* t = new PrologCompTerm("function_declaration_annotation");
  /* add type and name*/
  t->addSubterm(getTypeSpecific(decl->get_type()));
  t->addSubterm(new PrologAtom(decl->get_name().getString()));
  t->addSubterm(getDeclarationModifierSpecific(&(decl->get_declarationModifier())));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("function_type");
  /*recurse with getTypeSpecific*/
  t->addSubterm(getTypeSpecific(ftype->get_return_type()));
  /*we need to know wether it has ellipses for the constructor when unparsing*/
  t->addSubterm(getEnum(ftype->get_has_ellipses(),
			re.ellipses_flags));
  /*arguments*/
  t->addSubterm(getTypePtrListSpecific(ftype->get_arguments()));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("member_function_type");
  /*recurse with getTypeSpecific*/
  t->addSubterm(getTypeSpecific(ftype->get_return_type()));
  /*we need to know wether it has ellipses for the constructor when unparsing*/
  t->addSubterm(getEnum(ftype->get_has_ellipses(),
			re.ellipses_flags));
  /*arguments*/
  t->addSubterm(getTypePtrListSpecific(ftype->get_arguments()));
  /* mfunc_specifier*/
  t->addSubterm(getEnum(ftype->get_mfunc_specifier(), 
			re.declaration_modifiers));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("pointer_type");
  /* get base type with recursion*/
  t->addSubterm(getTypeSpecific(ptype->get_base_type()));
  return t;
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
  /*netsted type -> nested term*/
  PrologCompTerm* t = new PrologCompTerm("enum_type");
  ROSE_ASSERT(t != NULL);
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
  t->addSubterm(new PrologAtom(id));
  return t;
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
  /*netsted type -> nested term*/
  PrologCompTerm* t = new PrologCompTerm("class_type");
  ROSE_ASSERT(t != NULL);
  /*add base type*/
  t->addSubterm(new PrologAtom(ctype->get_name().str()));
  SgClassDeclaration* d = isSgClassDeclaration(ctype->get_declaration());
  ROSE_ASSERT(d != NULL);
  /* what kind of class is this?*/
  t->addSubterm(getEnum(d->get_class_type(), re.class_types));
  /* add qualified name of scope*/
  t->addSubterm(new PrologAtom(d->get_scope()->get_scope()->get_qualified_name().getString()));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("typedef_type");
  t->addSubterm(new PrologAtom(tp->get_name().getString()));

  /* add base type */
  if (tp->get_base_type() != NULL)
    t->addSubterm(getTypeSpecific(tp->get_base_type()));
  else t->addSubterm(new PrologAtom("null"));
  return t;
}

/**
 * class: SgConstructorInitializer
 * term: constructor_initializer_annotiation(name)
 * arg name: qualified class name
 */
PrologCompTerm* 
RoseToTerm::getConstructorInitializerSpecific(SgConstructorInitializer* ci) {
  ROSE_ASSERT(ci != NULL);
  /* create annotation term*/
  PrologCompTerm* t = new PrologCompTerm("constructor_initializer_annotiation");
  /* get name from class declaration*/
  SgClassDeclaration* dec = ci->get_class_decl();
  ROSE_ASSERT(dec != NULL);
  t->addSubterm(new PrologAtom(dec->get_qualified_name().getString()));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("new_exp_annotation");
  /* add type term*/
  t->addSubterm(getTypeSpecific(ne->get_specified_type()));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("array_type");
  ROSE_ASSERT(t != NULL);
  /* get nested type*/
  t->addSubterm(getTypeSpecific(a->get_base_type()));
  /* get expression*/
  if (a->get_index()) {
    t->addSubterm(traverseSingleNode(a->get_index()));
  } else {
    t->addSubterm(new PrologAtom("null"));
  }
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("modifier_type");
  ROSE_ASSERT(t != NULL);
  /* add the base type*/
  t->addSubterm(getTypeSpecific(m->get_base_type()));
  /* add the type modifier*/
  t->addSubterm(getTypeModifierSpecific(&(m->get_typeModifier())));
  return t;
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
    t =  new 
      PrologAtom(prologize(cn));
  } else {
    PrologCompTerm* ct  = new PrologCompTerm("not_yet_implemented");
    ct->addSubterm(new PrologAtom(stype->class_name()));
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
  PrologCompTerm* annot = new PrologCompTerm("unary_op_annotation");
  // GB (2008-12-04): Generating a self-documenting atom instead of an
  // opaque numerical constant.
  // annot->addSubterm(new PrologInt((int) op->get_mode()));
  annot->addSubterm(new PrologAtom(
              op->get_mode() == SgUnaryOp::prefix ? "prefix" : "postfix"));
  annot->addSubterm(getTypeSpecific(op->get_type()));
  if(SgThrowOp* thrw = dynamic_cast<SgThrowOp*>(op)) {
    /*Throw Ops also have a 'throw kind'*/
    // GB (2008-08-23): As of ROSE 0.9.3.a-1593, throw ops no longer have a
    // type list. Or was it only removed temporarily? TODO: Check again
    // sometime.
    annot->addSubterm(getEnum(thrw->get_throwKind(), re.throw_kinds));
#if 0
    SgTypePtrListPtr types = thrw->get_typeList ();
    SgTypePtrList::iterator it = types->begin();
    PrologList* l = new PrologList();
    while (it != types->end()) {
      l->addElement(getTypeSpecific(*it));
      it++;
    }
#endif
  } else if (SgCastExp* cst = dynamic_cast<SgCastExp*>(op)) {
    /*Casts have a cast type*/
    annot->addSubterm(getEnum(cst->get_cast_type(), re.cast_types));
    /*assure that arity = 4*/
    annot->addSubterm(new PrologAtom("null"));
  } else {
    /*assure that arity = 4*/
    annot->addSubterm(new PrologAtom("null"));
    annot->addSubterm(new PrologAtom("null"));
  }
  return annot;
}

/**
 * class: SgBinaryOp
 * term: binary_op_annotation(type)
 * arg type: the type of the expression
 */
PrologCompTerm* 
RoseToTerm::getBinaryOpSpecific(SgBinaryOp* op) {
  PrologCompTerm* annot = new PrologCompTerm("binary_op_annotation");
  /*add the type*/
  annot->addSubterm(getTypeSpecific(op->get_type()));
  return annot;
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
  PrologCompTerm* t = new PrologCompTerm("value_annotation");
  /* int and enum types */	
  if(SgIntVal* n = dynamic_cast<SgIntVal*>(astNode)) {
    t->addSubterm(new PrologInt(n->get_value()));
  } else if(SgUnsignedIntVal* n = dynamic_cast<SgUnsignedIntVal*>(astNode)) {
    t->addSubterm(new PrologInt(n->get_value()));
  } else if(SgShortVal* n = dynamic_cast<SgShortVal*>(astNode)) {
    t->addSubterm(new PrologInt(n->get_value()));
  } else if(SgUnsignedShortVal* n = dynamic_cast<SgUnsignedShortVal*>(astNode)) {
    t->addSubterm(new PrologInt(n->get_value()));
  } else if (SgLongIntVal* n = dynamic_cast<SgLongIntVal*>(astNode)) {
    /*create ostringstream*/
    //ostringstream o;
    /* let the << handle the conversion to string*/
    //o << n->get_value();
    //string s = o.str();
    /* add a new PrologAtom*/
    //t->addSubterm(new PrologAtom(s));
    t->addSubterm(new PrologInt(n->get_value()));
  } else if (SgUnsignedLongVal* n = dynamic_cast<SgUnsignedLongVal*>(astNode)) {
    //ostringstream o;
    //o << n->get_value();
    //string s = o.str();
    //t->addSubterm(new PrologAtom(s));
    t->addSubterm(new PrologInt(n->get_value()));
  } else if (SgLongLongIntVal* n = dynamic_cast<SgLongLongIntVal*>(astNode)) {
    //ostringstream o;
    //o << n->get_value();
    //string s = o.str();
    //t->addSubterm(new PrologAtom(s));
    t->addSubterm(new PrologInt(n->get_value()));
  } else if (SgUnsignedLongLongIntVal* n = dynamic_cast<SgUnsignedLongLongIntVal*>(astNode)) {
    //ostringstream o;
    //o << n->get_value();
    //string s = o.str();
    //t->addSubterm(new PrologAtom(s));
    t->addSubterm(new PrologInt(n->get_value()));
  } else if(SgEnumVal* n = dynamic_cast<SgEnumVal*>(astNode)) { /*FIXME*/
    /* value*/
    t->addSubterm(new PrologInt(n->get_value())); 
    /* name of value*/
    t->addSubterm(new PrologAtom(n->get_name().getString()));
    /* name of declaration*/
    SgEnumType *type = isSgEnumDeclaration(n->get_declaration())->get_type();
    ROSE_ASSERT(type != NULL);
    t->addSubterm(getEnumTypeSpecific(type));
  }
  /* float types */
  else if (SgFloatVal* n = dynamic_cast<SgFloatVal*>(astNode)) {
    ostringstream o;
    o << n->get_value();
    string s = o.str();
    t->addSubterm(new PrologAtom(s));
  } else if (SgDoubleVal* n = dynamic_cast<SgDoubleVal*>(astNode)) {
    ostringstream o;
    o << n->get_value();
    string s = o.str();
    t->addSubterm(new PrologAtom(s));
  } else if (SgLongDoubleVal* n = dynamic_cast<SgLongDoubleVal*>(astNode)) {
    ostringstream o;
    o << n->get_value();
    string s = o.str();
    t->addSubterm(new PrologAtom(s));
  } 
  /* boolean type */ 
  else if (SgBoolValExp* n = dynamic_cast<SgBoolValExp*>(astNode)) {
    ostringstream o;
    o << n->get_value();
    string s = o.str();
    t->addSubterm(new PrologAtom(s));
  } 
  /* char and string types */
  else if (SgCharVal* n = dynamic_cast<SgCharVal*>(astNode)) {
    //t->addSubterm(new PrologAtom(escape_string(std::string(1, (unsigned char)n->get_value()))));
    t->addSubterm(new PrologInt((int)n->get_value())); 
  } else if (SgUnsignedCharVal* n = dynamic_cast<SgUnsignedCharVal*>(astNode)) {
    //t->addSubterm(new PrologAtom(escape_string(std::string(1, n->get_value()))));
    t->addSubterm(new PrologInt((unsigned)n->get_value())); 
  } else if (SgWcharVal* n = dynamic_cast<SgWcharVal*>(astNode)) {
    ostringstream o;
    o << n->get_valueUL();
    string s = o.str();
    t->addSubterm(new PrologAtom(s));
  } else if (SgStringVal* n = dynamic_cast<SgStringVal*>(astNode)) {
    t->addSubterm(new PrologAtom(n->get_value()));
  } else {
    t->addSubterm(new PrologAtom("null"));
  }
  return t;
}
/**
 * class: SgAssignInitializer
 * term: assign_initializer_annotation(tpe)
 * arg tpe: type of the initializer
 */
PrologCompTerm*  
RoseToTerm::getAssignInitializerSpecific(SgAssignInitializer* ai) {
  PrologCompTerm* annot = new PrologCompTerm("assign_initializer_annotation");
  annot->addSubterm(getTypeSpecific(ai->get_type()));
  return annot;
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
  PrologCompTerm* annot = new PrologCompTerm("var_ref_exp_annotation");
  SgInitializedName* n = vr->get_symbol()->get_declaration();
  /* type*/
  annot->addSubterm(getTypeSpecific(n->get_typeptr()));
  /* name*/
  annot->addSubterm(new PrologAtom(n->get_name().getString()));
  /* static? (relevant for unparsing if scope is a class)*/
  SgDeclarationStatement* vdec = n->get_declaration();
  if (vdec != NULL) {
    annot->addSubterm(getEnum(vdec->get_declarationModifier().get_storageModifier().isStatic(),
                              re.static_flags));
  } else {
    annot->addSubterm(getEnum(0, re.static_flags));
  }
  if (vdec != NULL) {
    /* named scope or irrelevant?*/
    if(SgNamespaceDefinitionStatement* scn = isSgNamespaceDefinitionStatement(vdec->get_parent())) {
      annot->addSubterm(getNamespaceScopeName(scn));
    } else if (SgClassDefinition* scn = isSgClassDefinition(vdec->get_parent())) {
      annot->addSubterm(getClassScopeName(scn));
    } else {
      annot->addSubterm(new PrologAtom("null"));
    }
  } else {
    annot->addSubterm(new PrologAtom("null"));
  }

  return annot;
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
  PrologCompTerm* t = new PrologCompTerm("initialized_name_annotation");
  t->addSubterm(getTypeSpecific(n->get_typeptr()));
  t->addSubterm(new PrologAtom(n->get_name().getString()));
  /* static? (relevant for unparsing if scope is a class)*/
  t->addSubterm(getEnum(n->get_storageModifier().isStatic(), 
			re.static_flags));
  /* named scope or irrelevant?*/
  if(SgNamespaceDefinitionStatement* scn = isSgNamespaceDefinitionStatement(n->get_scope())) {
    t->addSubterm(getNamespaceScopeName(scn));
  } else if (SgClassDefinition* scn = isSgClassDefinition(n->get_scope())) {
    t->addSubterm(getClassScopeName(scn));
  } else {
    t->addSubterm(new PrologAtom("null"));
  }
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("class_declaration_annotation");
  /* add name and type*/
  t->addSubterm(new PrologAtom(cd->get_name().str()));
  t->addSubterm(getEnum(cd->get_class_type(), re.class_types));
  PrologTerm *typet = getTypeSpecific(cd->get_type());
  typeWasDeclaredBefore(typet->getRepresentation());
  t->addSubterm(getTypeSpecific(cd->get_type()));
  return t;
}
/**
 * class: SgClassDefinition
 * term: class_definition_annotation(fileinfo)
 * arg fileinfo: file info information for end of construct
 * */
PrologCompTerm* 
RoseToTerm::getClassDefinitionSpecific(SgClassDefinition* def) {
  PrologCompTerm* t = new PrologCompTerm("class_definition_annotation");
  /* add end of construct*/
  Sg_File_Info* eoc = def->get_endOfConstruct();
  ROSE_ASSERT(eoc != NULL);
  t->addSubterm(getFileInfo(eoc));
  return t;
}

/**
 * class: SgNamespaceDeclarationStatement
 * term: namespace_declaration_statement(name,unnamed)
 * arg name: name of the namespace
 * arg unnamed: unnamed namespace
 */
PrologCompTerm* 
RoseToTerm::getNamespaceDeclarationStatementSpecific(SgNamespaceDeclarationStatement* dec) {
  ROSE_ASSERT(dec != NULL);
  PrologCompTerm* t = new PrologCompTerm("namespace_declaration_statement");
  /* name*/
  t->addSubterm(new PrologAtom(dec->get_name().getString()));
  /* unnamed?*/
  t->addSubterm(new PrologInt((int) dec->get_isUnnamedNamespace()));
  return t;
}

/** create a list of atoms from a bit vector*/
PrologTerm* 
RoseToTerm::getBitVector(const SgBitVector &v, const vector<string> &names) {
  PrologList* l = new PrologList;
  SgBitVector::const_iterator it = v.begin();
  vector<string>::const_iterator name = names.begin();
  
  ROSE_ASSERT(v.size() <= names.size());
  while(it != v.end()) {
    if (*it == true)
      l->addElement(new PrologAtom(*name));
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
  SgType* t = c->get_type();
  ROSE_ASSERT(t != NULL);
  PrologTerm*  t_term = getTypeSpecific(t);
  ROSE_ASSERT(t_term != NULL);
  PrologCompTerm* annot = new PrologCompTerm("conditional_exp_annotation");
  annot->addSubterm(t_term);
  return annot;
}

/**
 * class: SgLabelStatement
 * term: label_annotation(label)
 * arg label: name of the label
 * */
PrologCompTerm*  
RoseToTerm::getLabelStatementSpecific(SgLabelStatement* label) {
  //get naem of the label
  string s = *(new string(label->get_label().getString()));
  // create a term containing the name;
  PrologCompTerm* t = new PrologCompTerm("label_annotation");
  t->addSubterm(new PrologAtom(s));
  return t;
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
  //create term
  PrologCompTerm* t = new PrologCompTerm("enum_declaration_annotation");
  ROSE_ASSERT(t != NULL);
  t->addSubterm(new PrologAtom(ename));
  t->addSubterm(getDeclarationAttributes(d));
  t->addSubterm(new PrologInt(d->get_embedded()));
  PrologTerm *typet = getTypeSpecific(d->get_type());
  typeWasDeclaredBefore(typet->getRepresentation());
  return t;
}

/**
 * class SgDeclarationStatement
 * term decl_attributes(nameonly,forward,externbrace,skipelaboratetype,neednamequalifier
 * arg all: boolean flags common to all declaration statements
 */
PrologCompTerm* 
RoseToTerm::getDeclarationAttributes(SgDeclarationStatement* s) {
  PrologCompTerm* t = new PrologCompTerm("decl_attributes");
  t->addSubterm(new PrologInt(s->get_nameOnly()));
  t->addSubterm(new PrologInt(s->get_forward()));
  t->addSubterm(new PrologInt(s->get_externBrace()));
  t->addSubterm(new PrologInt(s->get_skipElaborateType()));
  // ROSE 0.8.8a:	t->addSubterm(new PrologInt(s->get_need_name_qualifier()));
  t->addSubterm(new PrologInt(0)); // set dummy value
  return t;
}


/**
 * class: SgDeleteExp
 * term: delete_exp_annotation(is_array,need_global_specifier)
 * arg all: short "flags"
 */
PrologCompTerm* 
RoseToTerm::getDeleteExpSpecific(SgDeleteExp* de) {
  PrologCompTerm* t = new PrologCompTerm("delete_exp_annotation");
  t->addSubterm(new PrologInt(de->get_is_array()));
  t->addSubterm(new PrologInt(de->get_need_global_specifier()));
  return t;
}

/**
 * class: SgRefExp
 * term: ref_exp_annotation(type)
 * arg type: type of the expression
 */
PrologCompTerm* 
RoseToTerm::getRefExpSpecific(SgRefExp* re) {
  PrologCompTerm* t = new PrologCompTerm("ref_exp_annotation");
  ROSE_ASSERT(t != NULL);
  t->addSubterm(getTypeSpecific(re->get_type()));
  return t;
}

/**
 * class: SgVariableDeclaration
 * term: variable_declaration_specific(dm)
 * arg dm: declaration modifier information (see annotation of SgDeclarationModifier)
 */

PrologCompTerm* 
RoseToTerm::getVariableDeclarationSpecific(SgVariableDeclaration* d) {
  ROSE_ASSERT(d != NULL);
  /* create annotation term*/
  PrologCompTerm* t = new PrologCompTerm("variable_declaration_specific");
  t->addSubterm(getDeclarationModifierSpecific(&(d->get_declarationModifier())));

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
  t->addSubterm(baseTypeDecl != NULL
                  ? traverseSingleNode(baseTypeDecl)
                  : new PrologAtom("null"));

  return t;
}

/**
 * class: SgVarArgCopyOp, SgVarArgEndOp, SgVarArgOp, SgVarArgStartOp, SgVarArgStartOneOperatorOp
 * term: vararg_annotation(type)
 * arg type: type of the expression
 */
PrologCompTerm* 
RoseToTerm::getVarArgSpecific(SgExpression* e) {
  PrologCompTerm* t = new PrologCompTerm("vararg_annotation");
  t->addSubterm(getTypeSpecific(e->get_type()));
  return t;
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
//   PrologCompTerm* t = new PrologCompTerm("access_modifier");
//   t->addSubterm(new PrologInt((int) a->get_modifier()));
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
  PrologCompTerm* t = new PrologCompTerm("base_class_modifier");
  t->addSubterm(new PrologInt((int) b->get_modifier()));
  t->addSubterm(new PrologInt((int) b->get_accessModifier().get_modifier()));
  return t;
}

/**
 * class: SgFunctionModifier
 * term: function_modifier(b)
 * arg b: bit vector of SgFunctionModifier as PrologList (true = 1)
 */
PrologCompTerm* 
RoseToTerm::getFunctionModifierSpecific(SgFunctionModifier* f) {
  PrologCompTerm* t = new PrologCompTerm("function_modifier");
  /* get bit vector and convert to PROLOG*/
  t->addSubterm(getBitVector(f->get_modifierVector(), 
			     re.function_modifiers));
  return t;
}

/**
 * class: SgSpecialFunctionModifier
 * term: special_function_modifier(b)
 * arg b: bit vector of SgFunctionModifier as PrologList (true = 1)
 */
PrologCompTerm* 
RoseToTerm::getSpecialFunctionModifierSpecific(SgSpecialFunctionModifier* f) {
  PrologCompTerm* t = new PrologCompTerm("special_function_modifier");
  /* get bit vector and convert to PROLOG*/
  t->addSubterm(getBitVector(f->get_modifierVector(),
			     re.special_function_modifiers));
  return t;
}
/**
 * class: SgLinkageModifier
 * term: linkage_modifier(a)
 * arg a: enum value of SgLinkageModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getLinkageModifierSpecific(SgLinkageModifier* a) {
  PrologCompTerm* t = new PrologCompTerm("linkage_modifier");
  t->addSubterm(new PrologInt((int) a->get_modifier()));
  return t;
}
/**
 * class: SgStorageModifier
 * term: storage_modifier(a)
 * arg a: enum value of SgStorageModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getStorageModifierSpecific(SgStorageModifier* a) {
  PrologCompTerm* t = new PrologCompTerm("storage_modifier");
  t->addSubterm(getEnum(a->get_modifier(), re.storage_modifiers));
  return t;
}
/**
 * class: SgElaboratedTypeModifier
 * term: elaborated_type_modifier(a)
 * arg a: enum value of SgElaboratedTypeModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getElaboratedTypeModifierSpecific(SgElaboratedTypeModifier* a) {
  PrologCompTerm* t = new PrologCompTerm("elaborated_type_modifier");
  t->addSubterm(new PrologInt((int) a->get_modifier()));
  return t;
}
/**
 * class: SgConstVolatileModifier
 * term: const_volatile_modifier(a)
 * arg a: enum value of SgConstVolatileModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getConstVolatileModifierSpecific(SgConstVolatileModifier* a) {
  PrologCompTerm* t = new PrologCompTerm("const_volatile_modifier");
  t->addSubterm(getEnum(a->get_modifier(), re.cv_modifiers));
  return t;
}
/**
 * class: SgUPC_AccessModifier
 * term: upc_access_modifier(a)
 * arg a: enum value of SgUPC_AccessModifier (see ROSE docs!)
 */
PrologCompTerm* 
RoseToTerm::getUPC_AccessModifierSpecific(SgUPC_AccessModifier* a) {
  PrologCompTerm* t = new PrologCompTerm("upc_access_modifier");
  t->addSubterm(getEnum(a->get_modifier(), re.upc_access_modifiers));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("type_modifier");
  ROSE_ASSERT(t != NULL);
  /* get bit vector and convert to PROLOG*/
  t->addSubterm(getBitVector(a->get_modifierVector(), re.type_modifiers));
  /* add enums*/
  t->addSubterm(getEnum(a->get_upcModifier().get_modifier(), 
			re.upc_access_modifiers));
  t->addSubterm(getEnum(a->get_constVolatileModifier().get_modifier(),
			re.cv_modifiers));
  t->addSubterm(getEnum(a->get_elaboratedTypeModifier().get_modifier(), 
			   re.elaborated_type_modifiers));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("declaration_modifier");
  ROSE_ASSERT(t != NULL);
  t->addSubterm(getBitVector(dm->get_modifierVector(), 
			     re.declaration_modifiers));
  t->addSubterm(getTypeModifierSpecific(&(dm->get_typeModifier())));
  t->addSubterm(getEnum(dm->get_accessModifier().get_modifier(), 
			re.access_modifiers));
  t->addSubterm(getEnum(dm->get_storageModifier().get_modifier(),
			re.storage_modifiers));
  return t;
	
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
  PrologCompTerm* t = new PrologCompTerm("function_ref_exp_annotation");
  ROSE_ASSERT(t != NULL);
  t->addSubterm(new PrologAtom(s->get_name().getString()));
  t->addSubterm(getTypeSpecific(tpe));
  return t;
	
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
  PrologCompTerm* t = new PrologCompTerm("member_function_ref_exp_annotation");
  ROSE_ASSERT(t != NULL);
  /* get member function symbol information*/
  SgMemberFunctionSymbol* s = r->get_symbol();
  ROSE_ASSERT(s != NULL);
  t->addSubterm(getMemberFunctionSymbolSpecific(s));
  // virtual call?
  t->addSubterm(new PrologInt(r->get_virtual_call()));
  // type
  SgType* tpe = s->get_type();
  ROSE_ASSERT(tpe != NULL);
  t->addSubterm(getTypeSpecific(tpe));
  // need qualifier?
  t->addSubterm(new PrologInt(r->get_need_qualifier()));
  /*create Prolog Term*/
  return t;
	
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
  PrologCompTerm* t = new PrologCompTerm("function_call_exp_annotation");
  ROSE_ASSERT(t != NULL);
  t->addSubterm(getTypeSpecific(c->get_type()));
  return t;
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
  /* create term and append type and name */
  PrologCompTerm* t = new PrologCompTerm("member_function_declaration_annotation");
  t->addSubterm(getTypeSpecific(decl->get_type()));
  /* add the nodes name*/
  t->addSubterm(new PrologAtom(decl->get_name().getString()));
  /* add scope */
  SgClassDefinition* def = decl->get_class_scope();
  /* we add the complete class scope name here */
  t->addSubterm(getClassScopeName(def));
  /* add declaration modifier specific*/
  t->addSubterm(getDeclarationModifierSpecific(&(decl->get_declarationModifier())));	
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("class_scope");
  ROSE_ASSERT(t != NULL);
  t->addSubterm(new PrologAtom(qname));
  t->addSubterm(getEnum(decl->get_class_type(), re.class_types));
  return t;
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
  PrologCompTerm* t = new PrologCompTerm("namespace_scope");
  /* add qualified name*/
  t->addSubterm(new PrologAtom(decl->get_qualified_name().getString()));
  /* add unnamed */
  t->addSubterm(new PrologInt((int)decl->get_isUnnamedNamespace()));
  return t;
}


/**
 * class: SgMemberFunctionSymbol
 * term: member_function_symbol_annotation(mf,scope)
 * arg mf: complete PROLOG-Representation of member function declaration without definition
 * arg scope: class scope (see getClassScopeName)
 */
PrologCompTerm* 
RoseToTerm::getMemberFunctionSymbolSpecific(SgMemberFunctionSymbol* sym) {
  PrologCompTerm* t = new PrologCompTerm("member_function_symbol_annotation");
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
  t->addSubterm(traverseSingleNode(cop_decl));
  /* add scope*/
  SgClassDefinition* cdef = orig_decl->get_class_scope();
  ROSE_ASSERT(cdef != NULL);
  t->addSubterm(getClassScopeName(cdef));
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
  ROSE_ASSERT(o != NULL);
  PrologCompTerm* t = new PrologCompTerm("size_of_op_annotation");
  /* create type info if types are present*/
  SgType* otype = o->get_operand_type();
  if (otype != NULL) {
    t->addSubterm(getTypeSpecific(otype));
  } else {
    t->addSubterm(new PrologAtom("null"));
  }
  SgType* etype = o->get_type();
  if (etype != NULL) {
    t->addSubterm(getTypeSpecific(etype));
  } else {
    t->addSubterm(new PrologAtom("null"));
  }
  return t;
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
  /*create annotation term*/
  PrologCompTerm* t = new PrologCompTerm("typedef_annotation");
  /*get name*/
  // FIXME :: t->addSubterm(new PrologAtom(d->get_qualified_name().getString()));
  t->addSubterm(new PrologAtom(d->get_name().getString()));
  /*get base type*/
  t->addSubterm(getTypeSpecific(d->get_base_type()));
  /* the base type declaration is no longer in the typedef annotation; it is
   * now a child of the typedef declaration itself */
  return t;
	
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
  PrologCompTerm* t = new PrologCompTerm("pragma_annotation");
  // Adrian 2007-11-27:
  // This is to work around a bug in ROSE?/EDG? that inserts whitespaces
  // Hopefully I can remove it in a later revision
  string s = n->get_pragma();
  s.erase(remove_if(s.begin(), s.end(), bind1st(equal_to<char>(), ' ')), s.end());
  t->addSubterm(new PrologAtom(s));
  return t;
}
