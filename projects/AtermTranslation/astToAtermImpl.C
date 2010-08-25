#include "rose.h"
#include <stdio.h>
#include "aterm1.h"
#include "aterm2.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "atermTranslation.h"
#include "MergeUtils.h"

using namespace std;

string uniqueId(SgNode* n) {
  // return SageInterface::generateUniqueName(n, false);
  return intToHex(n);
}

ATerm convertFileInfoToAterm(Sg_File_Info* fi) {
  ATerm term =  ATmake("Position(<str>, <int>, <int>)", fi->get_filename(), fi->get_line(), fi->get_col());
  return term;
}

ATerm convertVectorToAterm(const vector<ATerm>& v, int start, int len) {
  if (len == -1)
    len = v.size() - start;
  ATermList ls = ATmakeList0();
  for (int i = start; i < start + len; ++i)
    ls = ATappend(ls, v[i]);
  return ATmake("<term>", ls);
}

template <typename Iter>
ATerm convertRangeToAterm(Iter b, Iter e) {
  ATermList ls = ATmakeList0();
  for (; b != e; ++b) {
    ls = ATappend(ls, *b);
  }
  return ATmake("<term>", ls);
}

template <typename Iter>
ATerm convertSgNodeRangeToAterm(Iter b, Iter e) {
  ATermList ls = ATmakeList0();
  for (; b != e; ++b) {
    ls = ATappend(ls, convertNodeToAterm(*b));
  }
  return ATmake("<term>", ls);
}

class NullChecker: public AstBottomUpProcessing<bool> {
  public:
  virtual bool evaluateSynthesizedAttribute(SgNode* node, vector<bool> ls) {
    assert (std::accumulate(ls.begin(), ls.end(), true, std::logical_and<bool>()) ||
	    ((cout << node->sage_class_name() << endl), false));
    return true;
  }
};

string getShortVariantName(VariantT var) {
  switch (var) {
    case V_SgAddOp: return "Add";
    case V_SgMultiplyOp: return "Multiply";
    case V_SgDivideOp: return "Divide";
    case V_SgBitOrOp: return "BitOr";
    case V_SgBitXorOp: return "BitXor";
    case V_SgBitAndOp: return "BitAnd";
    case V_SgLshiftOp: return "LeftShift";
    case V_SgRshiftOp: return "RightShift";
    case V_SgModOp: return "Mod";
    case V_SgBitComplementOp: return "Complement";
    case V_SgLessThanOp: return "Less";
    case V_SgLessOrEqualOp: return "LessEqual";
    case V_SgGreaterThanOp: return "Greater";
    case V_SgGreaterOrEqualOp: return "GreaterEqual";
    case V_SgEqualityOp: return "Equal";
    case V_SgNotEqualOp: return "NotEqual";
    case V_SgAssignOp: return "Assign";
    case V_SgPlusAssignOp: return "AddAssign";
    case V_SgPlusPlusOp: return "Increment";
    case V_SgPntrArrRefExp: return "Index";
    case V_SgFunctionCallExp: return "Call";
    case V_SgVariableDeclaration: return "VarDecl";
    case V_SgForStatement: return "For";
    case V_SgIfStmt: return "If";
    case V_SgWhileStmt: return "While";
    case V_SgReturnStmt: return "Return";
    case V_SgSwitchStatement: return "Switch";
    case V_SgCaseOptionStmt: return "Case";
    case V_SgDefaultOptionStmt: return "Default";
    case V_SgExprStatement: return "ExprStmt";
    case V_SgTypeInt: return "Int";
    case V_SgTypeLong: return "Long";
    case V_SgTypeLongLong: return "LongLong";
    case V_SgTypeShort: return "Short";
    case V_SgTypeChar: return "Char";
    case V_SgTypeSignedChar: return "SignedChar";
    case V_SgTypeUnsignedChar: return "UnsignedChar";
    case V_SgTypeUnsignedInt: return "UnsignedInt";
    case V_SgTypeUnsignedLong: return "UnsignedLong";
    case V_SgTypeUnsignedLongLong: return "UnsignedLongLong";
    case V_SgTypeUnsignedShort: return "UnsignedShort";
    case V_SgTypeBool: return "Bool";
    case V_SgTypeVoid: return "Void";
    case V_SgTypeFloat: return "Float";
    case V_SgTypeDouble: return "Double";
    case V_SgTypeLongDouble: return "LongDouble";
    case V_SgTypeDefault: return "DefaultType";
    case V_SgTypeEllipse: return "EllipsesType";
    case V_SgAssignInitializer: return "AssignInit";
    case V_SgCastExp: return "Cast";
    case V_SgGlobal: return "Global";
    case V_SgFunctionParameterList: return "ParameterList";
    case V_SgProject: return "Project";
    case V_SgForInitStatement: return "ForInit";
    case V_SgExprListExp: return "ExprList";
    case V_SgClassDefinition: return "ClassDefinition";
    case V_SgFunctionDefinition: return "FunctionDefinition";
    case V_SgBasicBlock: return "Block";
    case V_SgNullStatement: return "NullStmt";
    default: return getVariantName(var);
  }
}

ATerm getTraversalChildrenAsAterm(SgNode* n) {
  vector<SgNode*> children = n->get_traversalSuccessorContainer();
  return convertSgNodeRangeToAterm(children.begin(), children.end());
}

ATerm convertNodeToAterm(SgNode* n) {
  if (!n) return ATmake("NULL");
  ATerm term;
  switch (n->variantT()) {
    case V_SgFile:
    // Special case needed to include file name
    term = ATmake("File(<str>, <term>)", isSgFile(n)->getFileName(), convertNodeToAterm(isSgFile(n)->get_root()));
    break;

    case V_SgPlusPlusOp:
    case V_SgMinusMinusOp:
    // Special cases needed to include prefix/postfix status
    term = ATmake("<appl(<appl>, <term>)>",
                  getShortVariantName((VariantT)(n->variantT())).c_str(),
                  (isSgUnaryOp(n)->get_mode() == SgUnaryOp::prefix ? "Prefix" :
                   isSgUnaryOp(n)->get_mode() == SgUnaryOp::postfix ? "Postfix" :
                   "Unknown"),
                  convertNodeToAterm(isSgUnaryOp(n)->get_operand()));
    break;

    case V_SgExpressionRoot:
    // Special case to remove this node
    term = convertNodeToAterm(isSgExpressionRoot(n)->get_operand());
    break;

    case V_SgCastExp:
    // Special case needed to include type
    term = ATmake("Cast(<term>, <term>)>",
	    convertNodeToAterm(isSgUnaryOp(n)->get_operand()),
	    convertNodeToAterm(isSgCastExp(n)->get_type()));
    break;

    case V_SgVarRefExp:
    // Special case needed to include id
    term = ATmake("Var(<str>)", 
		  uniqueId(isSgVarRefExp(n)->get_symbol()->get_declaration()).c_str());
    break;

    case V_SgFunctionRefExp:
    // Special case needed to include id
    term = ATmake(
                  "Func(<str>)", 
                  uniqueId(isSgFunctionRefExp(n)->get_symbol()->get_declaration()).c_str());
    break;

    case V_SgIntVal:
    // Special case needed to include value
    term = ATmake("IntC(<int>)", isSgIntVal(n)->get_value());
    break;

    case V_SgUnsignedIntVal:
    term = ATmake("UnsignedIntC(<int>)", isSgUnsignedIntVal(n)->get_value());
    break;

    case V_SgUnsignedLongVal: {
      ostringstream s;
      s << isSgUnsignedLongVal(n)->get_value();
      term = ATmake("UnsignedLongC(<str>)", s.str().c_str());
    }
    break;

    case V_SgUnsignedLongLongIntVal: {
      ostringstream s;
      s << isSgUnsignedLongLongIntVal(n)->get_value();
      term = ATmake("UnsignedLongLongC(<str>)", s.str().c_str());
    }
    break;

    case V_SgDoubleVal:
    term = ATmake("DoubleC(<real>)", isSgDoubleVal(n)->get_value());
    break;

    case V_SgInitializedName: {
      // Works around double initname problem
      SgInitializer* initializer = 
	isSgInitializedName(n)->get_initializer();
      const SgName& name = isSgInitializedName(n)->get_name();
      SgType* type = isSgInitializedName(n)->get_type();
      // Works around fact that ... is not really an initname and shouldn't be
      // a type either
      if (isSgTypeEllipse(type))
	term = ATmake("Ellipses");
      else {
	term = ATmake("InitName(<str>, <term>, <term>) {[id, <str>]}", 
		      (name.str() ? name.str() : ""), 
		      convertNodeToAterm(type), 
		      convertNodeToAterm(initializer),
		      uniqueId(n).c_str());
	term = ATsetAnnotation(term, ATmake("id"),
                               ATmake("<str>", uniqueId(n).c_str()));
      }
    }
    break;

    case V_SgFunctionDeclaration: {
      // Special case needed to include name
      SgFunctionDeclaration* fd = isSgFunctionDeclaration(n);
      term = ATmake("Function(<str>, <term>, <term>, <term>)", 
		    fd->get_name().str(), 
		    convertNodeToAterm(fd->get_orig_return_type()),
		    convertSgNodeRangeToAterm(fd->get_args().begin(),
					      fd->get_args().end()),
		    convertNodeToAterm(fd->get_definition()));
      term = ATsetAnnotation(term, ATmake("id"),
                             ATmake("<str>", uniqueId(n).c_str()));
    }
    break;

    case V_SgClassDeclaration: {
      // Special case needed to distinguish forward/full definitions and to
      // include class name
      SgClassDeclaration* decl = isSgClassDeclaration(n);
      assert (decl);
      SgName sname = decl->get_name();
      char* name = sname.str();
      // Suggestion: have a field named local_definition in each class
      // declaration that is 0 whenever the current declaration doesn't
      // have a definition attached, even if there is another declaration
      // which does have a definition attached.
      SgClassDefinition* defn = decl->get_definition();
      // cout << "defn = 0x" << hex << defn << endl << dec;
      if (decl->isForward())
	defn = 0;
      if (defn)
	term = ATmake("Class(<str>, <term>)", 
		      (name ? name : ""), // Will be simpler when SgName
		      // becomes string
		      convertNodeToAterm(defn));
      else
	term = ATmake("ClassFwd(<str>)", (name ? name : ""));
      term = ATsetAnnotation(term, ATmake("id"),
                             ATmake("<str>", uniqueId(n).c_str()));
    }
    break;

    case V_SgEnumDeclaration: {
      // Special case to include enum name and enumerator names which are not
      // traversal children
      SgName sname = isSgEnumDeclaration(n)->get_name();
      const char* name = sname.str();
      const SgInitializedNamePtrList& enumerators = 
	isSgEnumDeclaration(n)->get_enumerators();
      term = ATmake("Enum(<str>, <term>)",
		    (name ? name : "{anonymous}"), 
		    convertSgNodeRangeToAterm(enumerators.begin(),
					      enumerators.end()));
      term = ATsetAnnotation(term, ATmake("id"),
                             ATmake("<str>", uniqueId(n).c_str()));
    }
    break;

    case V_SgPointerType: {
      // Special case because types can't be traversed yet
      SgType* type = isSgPointerType(n)->get_base_type();
      ATerm t = convertNodeToAterm(type);
      term = ATmake("Pointer(<term>)", t);
    }
    break;

    case V_SgReferenceType: {
      // Special case because types can't be traversed yet
      SgType* type = isSgReferenceType(n)->get_base_type();
      ATerm t = convertNodeToAterm(type);
      term = ATmake("Reference(<term>)", t);
    }
    break;

    case V_SgModifierType: {
      // Special case for type traversal and to prettify modifier names
      SgType* type = isSgModifierType(n)->get_base_type();
      SgTypeModifier& modifier = isSgModifierType(n)->get_typeModifier();
      SgConstVolatileModifier& cvmod = modifier.get_constVolatileModifier();
      term = convertNodeToAterm(type);
      if (cvmod.isConst())
	term = ATmake("Const(<term>)", term);
      if (cvmod.isVolatile())
	term = ATmake("Volatile(<term>)", term);
    }
    break;

    case V_SgArrayType: {
      // Special case because types can't be traversed yet, and to get length
      SgType* type = isSgArrayType(n)->get_base_type();
      ATerm t = convertNodeToAterm(type);
      term = ATmake("Array(<term>, <term>)", t, (isSgArrayType(n)->get_index() ? convertNodeToAterm((n->get_traversalSuccessorContainer())[4]) : ATmake("<str>", "NULL")));
      assert (term);
    }
    break;

    case V_SgFunctionType: {
      // Special case to allow argument list to be traversed
      SgFunctionType* ft = isSgFunctionType(n);
      ATerm ret = convertNodeToAterm(ft->get_return_type());
      ATerm args_list = convertSgNodeRangeToAterm(ft->get_arguments().begin(),
						  ft->get_arguments().end());
      term = ATmake("FunctionType(<term>, <term>)", ret, args_list);
    }
    break;

    case V_SgEnumType:
    case V_SgClassType: 
    case V_SgTypedefType: {
      // Special cases to optionally put in type definition instead of
      // reference
      SgNamedType* nt = isSgNamedType(n);
      assert (nt);
      SgName sname = nt->get_name();
   // char* name = sname.str();
      SgDeclarationStatement* decl = nt->get_declaration();
      assert (decl);
      SgClassDefinition* defn = isSgClassDeclaration(decl) ?
				isSgClassDeclaration(decl)->get_definition() :
				0;
      term = ATmake("Type(<term>)",
		    (nt->get_autonomous_declaration() || !defn ? 
                     ATmake("id(<str>)", uniqueId(decl).c_str()) :
		     convertNodeToAterm(nt->get_declaration())));
    }
    break;

    case V_SgLabelStatement: {
      // Special case to put in label id
      char* name = isSgLabelStatement(n)->get_name().str();
      term = ATmake("Label(<str>)", (name ? name : ""));
      term = ATsetAnnotation(term, ATmake("id"),
                             ATmake("<str>", uniqueId(n).c_str()));
    }
    break;

    case V_SgGotoStatement: {
      // Special case to put in label id
      term = ATmake("Goto(<str>)", 
                    uniqueId(isSgGotoStatement(n)->get_label()).c_str());
    }
    break;

    case V_SgTypedefDeclaration: {
      // Special case to put in typedef name
      const SgName& name = isSgTypedefDeclaration(n)->get_name();
      SgType* type = isSgTypedefDeclaration(n)->get_base_type();
      term = ATmake("Typedef(<str>, <term>)", (name.str() ? name.str() : ""), 
		      convertNodeToAterm(type));
      term = ATsetAnnotation(term, ATmake("id"),
                             ATmake("<str>", uniqueId(n).c_str()));
    }
    break;

    case V_SgTemplateDeclaration: {
      // Traversal doesn't work for these
      SgTemplateDeclaration* td = isSgTemplateDeclaration(n);
      ROSE_ASSERT (td);
      SgTemplateParameterPtrListPtr paramsPtr = td->get_templateParameters();
      SgTemplateParameterPtrList params = 
	paramsPtr ? *paramsPtr : SgTemplateParameterPtrList();
      string templateKindString;
      switch (td->get_template_kind()) {
	case SgTemplateDeclaration::e_template_none:
	  templateKindString = "None"; break;
	case SgTemplateDeclaration::e_template_class:
	  templateKindString = "Class"; break;
	case SgTemplateDeclaration::e_template_m_class:
	  templateKindString = "MemberClass"; break;
	case SgTemplateDeclaration::e_template_function:
	  templateKindString = "Function"; break;
	case SgTemplateDeclaration::e_template_m_function:
	  templateKindString = "MemberFunction"; break;
	case SgTemplateDeclaration::e_template_m_data:
	  templateKindString = "MemberData"; break;
	default: templateKindString = "Unknown"; break;
      }
      term = ATmake("TemplateDeclaration(<appl>, <str>, <term>, <str>)",
		    templateKindString.c_str(),
		    td->get_name().str(),
		    convertSgNodeRangeToAterm(params.begin(), params.end()),
		    td->get_string().str());
    }
    break;

    case V_SgTemplateInstantiationDecl: {
      // Traversal doesn't work for these
      SgTemplateInstantiationDecl* td = isSgTemplateInstantiationDecl(n);
      ROSE_ASSERT (td);
      SgTemplateArgumentPtrListPtr argsPtr = td->get_templateArguments();
      SgTemplateArgumentPtrList args = 
	argsPtr ? *argsPtr : SgTemplateArgumentPtrList();
      term = ATmake("TemplateInstantiationDecl(<str>, <term>)",
		    td->get_templateDeclaration()->get_name().str(),
		    convertSgNodeRangeToAterm(args.begin(), args.end()));
    }
    break;

    case V_SgTemplateParameter: {
      // Traversal doesn't work for these
      SgTemplateParameter* tp = isSgTemplateParameter(n);
      ROSE_ASSERT (tp);
      switch (tp->get_parameterType()) {
	case SgTemplateParameter::parameter_undefined: {
	  term = ATmake("Undefined");
	}
	break;

	case SgTemplateParameter::type_parameter: {
	  term = ATmake("Type(<term>)",
			convertNodeToAterm(tp->get_defaultTypeParameter()));
	}
	break;

	case SgTemplateParameter::nontype_parameter: {
	  term = ATmake("Nontype(<term>, <term>)",
			convertNodeToAterm(tp->get_type()),
			convertNodeToAterm(tp->get_defaultExpressionParameter()));
	}
	break;

	case SgTemplateParameter::template_parameter: {
	  term = ATmake("Template");
	}
	break;

	default: term = ATmake("Unknown"); break;
      }
    }
    break;

    case V_SgTemplateArgument: {
      // Traversal doesn't work for these
      SgTemplateArgument* ta = isSgTemplateArgument(n);
      ROSE_ASSERT (ta);
      switch (ta->get_argumentType()) {
	case SgTemplateArgument::argument_undefined:
	  term = ATmake("Undefined");
	  break;
	case SgTemplateArgument::type_argument:
	  term = ATmake("Type(<term>)", 
			convertNodeToAterm(ta->get_type()));
	  break;
	case SgTemplateArgument::nontype_argument:
	  term = ATmake("Nontype(<term>)", 
			convertNodeToAterm(ta->get_expression()));
	  break;
	// case SgTemplateArgument::template_argument:
	  // term = ATmake("Template");
	  // break;
	default: term = ATmake("Unknown"); break;
      }
    }
    break;

    default: {
      bool isContainer = 
	(AstTests::numSuccContainers(n) == 1) ||
	(!isSgType(n) && (n->get_traversalSuccessorContainer().size() == 0));
      term = ATmake((isContainer ? "<appl(<term>)>" : "<appl(<list>)>"), 
                    getShortVariantName((VariantT)(n->variantT())).c_str(),
                    (isSgType(n) ? ATmake("[]") : getTraversalChildrenAsAterm(n)));
               // Special case for types is because of traversal problems
    }
    break;
  }

  assert (term);

  term = ATsetAnnotation(term, ATmake("ptr"), pointerAsAterm(n));

#if 1
  if (n->get_file_info())
    term = ATsetAnnotation(term, ATmake("location"),
                           convertFileInfoToAterm(n->get_file_info()));

  if (isSgExpression(n))
    term = ATsetAnnotation(term, ATmake("type"),
                           convertNodeToAterm(isSgExpression(n)->get_type()));
#endif

  // cout << n->sage_class_name() << " -> " << ATwriteToString(term) << endl;
  return term;
}
