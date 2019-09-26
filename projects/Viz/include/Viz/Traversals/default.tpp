
namespace Rose {

namespace Viz {

namespace Traversals {

template <class InheritedAttr_, class SynthesizedAttr_>
InheritedAttr_ Default<InheritedAttr_, SynthesizedAttr_>::evaluateInheritedAttribute(SgNode * node, InheritedAttr_ attr) {
  InheritedAttr_ res;
  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
SynthesizedAttr_ Default<InheritedAttr_, SynthesizedAttr_>::evaluateSynthesizedAttribute(SgNode * node, InheritedAttr_ attr, StackFrameVector<SynthesizedAttr_> attrs) {
  SynthesizedAttr_ res;
  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
bool Default<InheritedAttr_, SynthesizedAttr_>::stopAfter(SgNode * node) {
  return false;
}

template <class InheritedAttr_, class SynthesizedAttr_>
bool Default<InheritedAttr_, SynthesizedAttr_>::skip(SgNode * node) {
  if (isSgForInitStatement(node)) return true;
  return false;
}

template <class InheritedAttr_, class SynthesizedAttr_>
Objects::graph_t * Default<InheritedAttr_, SynthesizedAttr_>::startOn(SgNode *) {
  return NULL;
}

template <class InheritedAttr_, class SynthesizedAttr_>
Objects::graph_t * Default<InheritedAttr_, SynthesizedAttr_>::getSubgraph(SgNode * node, Objects::graph_t * graph) {
  SgExpression * expr_node = isSgExpression(node);

  SgExpression * expr_parent = isSgExpression(node->get_parent());

  if (expr_node != NULL && expr_parent == NULL) {
    std::ostringstream oss; oss << "expr_" << node;
    graph = graph->subgraph(oss.str());
  }

  if (isSgDeclarationStatement(node)) {
    switch (node->variantT()) {
      case V_SgVariableDeclaration:
      case V_SgFunctionDeclaration:
      case V_SgEnumDeclaration:
      case V_SgClassDeclaration:
      case V_SgMemberFunctionDeclaration:
      case V_SgTemplateInstantiationDecl:
      case V_SgTemplateInstantiationFunctionDecl:
      case V_SgTemplateInstantiationMemberFunctionDecl:
      case V_SgTemplateClassDeclaration:
      case V_SgTemplateFunctionDeclaration:
      case V_SgTemplateMemberFunctionDeclaration:
      case V_SgNamespaceDeclarationStatement:
      case V_SgTemplateInstantiationDirectiveStatement:
      case V_SgTypedefDeclaration:
      case V_SgFunctionParameterList:
      {
        std::ostringstream oss; oss << "decl_" << node;
        graph = graph->subgraph(oss.str());
        break;
      }
      default:
        break;
    }
  }

  if (isSgExprStatement(node)) {
    std::ostringstream oss; oss << "expr_stmt_" << node;
    graph = graph->subgraph(oss.str());
  }

//  if (isSgBinaryOp(node)) {
//    std::ostringstream oss; oss << "binop_" << node;
//    graph = graph->subgraph(oss.str());
//  }

  return graph;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void Default<InheritedAttr_, SynthesizedAttr_>::edit(
  SgNode * node, Objects::node_desc_t & desc, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr
) const {
  // Default init
  desc.label = "";
  desc.color = "black";
  desc.style = "";
  desc.shape = "box";
  desc.fillcolor = "";
  desc.penwidth = 3;
  desc.url="";

  // Case for Expressions
  if (isSgExpression(node)) {
    desc.shape="oval";
    desc.color="green";

    std::ostringstream oss;
    switch (node->variantT()) {
      case V_SgVarRefExp:      oss << /*"variable(" << */(     (SgVarRefExp *)node)->get_symbol()->get_name().getString()/* << ")"*/; break;
      case V_SgFunctionRefExp: oss << /*"function(" << */((SgFunctionRefExp *)node)->get_symbol()->get_name().getString()/* << ")"*/; break;
      case V_SgLabelRefExp:    oss << /*"label("    << */(   (SgLabelRefExp *)node)->get_symbol()->get_name().getString()/* << ")"*/; break;

      case V_SgBoolValExp: oss << /*"bool(" << */((SgBoolValExp *)node)->get_value()/* << ")"*/; break;

      case V_SgCharVal:        oss << /*   "char(" << */(       (SgCharVal *)node)->get_value()/* << ")"*/; break;
      case V_SgShortVal:       oss << /*  "short(" << */(      (SgShortVal *)node)->get_value()/* << ")"*/; break;
      case V_SgIntVal:         oss << /*    "int(" << */(        (SgIntVal *)node)->get_value()/* << ")"*/; break;
      case V_SgLongIntVal:     oss << /*   "lint(" << */(    (SgLongIntVal *)node)->get_value()/* << ")"*/; break;
      case V_SgLongLongIntVal: oss << /*  "llint(" << */((SgLongLongIntVal *)node)->get_value()/* << ")"*/; break;
      case V_SgFloatVal:       oss << /*  "float(" << */(      (SgFloatVal *)node)->get_value()/* << ")"*/; break;
      case V_SgDoubleVal:      oss << /* "double(" << */(     (SgDoubleVal *)node)->get_value()/* << ")"*/; break;
      case V_SgLongDoubleVal:  oss << /*"ldouble(" << */( (SgLongDoubleVal *)node)->get_value()/* << ")"*/; break;

      case V_SgUnsignedCharVal:        oss << /* "uchar(" << */(       (SgUnsignedCharVal *)node)->get_value()/* << ")"*/; break;
      case V_SgUnsignedShortVal:       oss << /*"ushort(" << */(      (SgUnsignedShortVal *)node)->get_value()/* << ")"*/; break;
      case V_SgUnsignedIntVal:         oss << /*  "uint(" << */(        (SgUnsignedIntVal *)node)->get_value()/* << ")"*/; break;
      case V_SgUnsignedLongVal:        oss << /* "ulint(" << */(       (SgUnsignedLongVal *)node)->get_value()/* << ")"*/; break;
      case V_SgUnsignedLongLongIntVal: oss << /*"ullint(" << */((SgUnsignedLongLongIntVal *)node)->get_value()/* << ")"*/; break;

      case V_SgEnumVal:   oss << /*  "enum(" << */(  (SgEnumVal *)node)->get_value()/* << ")"*/; break;
      case V_SgStringVal: oss << /*"string(" << */((SgStringVal *)node)->get_value()/* << ")"*/; break;
      case V_SgWcharVal:  oss << /* "wchar(" << */( (SgWcharVal *)node)->get_value()/* << ")"*/; break;

      case V_SgCallExpression: oss << "call"; break;

      case V_SgExprListExp: oss << "expression\\nlist"; break;

      case V_SgAssignInitializer:      oss << "assign\\ninitializer"; break;
      case V_SgAggregateInitializer:   oss << "aggregate\\ninitializer"; break;
      case V_SgCompoundInitializer:    oss << "compound\\ninitializer"; break;
      case V_SgConstructorInitializer: oss << "constructor\\ninitializer"; break;
      case V_SgDesignatedInitializer:  oss << "designated\\ninitializer"; break;

      case V_SgNewExp:           oss << "new";    break;
      case V_SgDeleteExp:        oss << "delete"; break;
      case V_SgThisExp:          oss << "this";   break;
      case V_SgSizeOfOp:         oss << "sizeof"; break;
      case V_SgTypeIdOp:         oss << "typeid"; break;

      case V_SgPlusPlusOp:       oss << "++"; break;
      case V_SgMinusMinusOp:     oss << "--"; break;

      case V_SgUnaryAddOp:       oss << "+"; break;
      case V_SgMinusOp:          oss << "-"; break;

      case V_SgNotOp:            oss << "!"; break;

      case V_SgPointerDerefExp:  oss << "*";    break;
      case V_SgAddressOfOp:      oss << "&";    break;
      case V_SgCastExp:          oss << "cast"; break;

      case V_SgRealPartOp:       oss << "real";      break;
      case V_SgImagPartOp:       oss << "imaginary"; break;

      case V_SgThrowOp:          oss << "throw"; break;

      case V_SgAddOp:            oss << "+"; break;
      case V_SgSubtractOp:       oss << "-"; break;
      case V_SgMultiplyOp:       oss << "*"; break;
      case V_SgDivideOp:         oss << "/"; break;
      case V_SgModOp:            oss << "%"; break;
      case V_SgAssignOp:         oss << "="; break;

      case V_SgEqualityOp:       oss << "=="; break;
      case V_SgNotEqualOp:       oss << "!="; break;
      case V_SgGreaterOrEqualOp: oss << ">="; break;
      case V_SgLessOrEqualOp:    oss << "<="; break;
      case V_SgGreaterThanOp:    oss << ">";  break;
      case V_SgLessThanOp:       oss << "<";  break;

      case V_SgAndOp:            oss << "&&";  break;
      case V_SgOrOp:             oss << "||";  break;
      case V_SgBitXorOp:         oss << "xor"; break;

      case V_SgLshiftOp:         oss << "<<"; break;
      case V_SgRshiftOp:         oss << ">>"; break;
      case V_SgBitAndOp:         oss << "&";  break;
      case V_SgBitOrOp:          oss << "|";  break;

      case V_SgPntrArrRefExp:    oss << "[]";  break;
      case V_SgDotExp:           oss << ".";   break;
      case V_SgArrowExp:         oss << "->";  break;
      case V_SgArrowStarOp:      oss << "->*"; break;
      case V_SgDotStarOp:        oss << ".*";  break;

      case V_SgPlusAssignOp:     oss << "+="; break;
      case V_SgMinusAssignOp:    oss << "-="; break;
      case V_SgMultAssignOp:     oss << "*="; break;
      case V_SgDivAssignOp:      oss << "/="; break;
      case V_SgModAssignOp:      oss << "%="; break;

      case V_SgAndAssignOp:      oss << "&="; break;
      case V_SgIorAssignOp:      oss << "|="; break;
      case V_SgXorAssignOp :     oss << "^="; break;

      case V_SgLshiftAssignOp:   oss << "<<="; break;
      case V_SgRshiftAssignOp:   oss << ">>="; break;

      case V_SgScopeOp:          oss << "::"; break;
      case V_SgCommaOpExp:       oss << ",";    break;

      case V_SgConditionalExp:   oss << "c ? t : f"; break;
      default: break;
    }
    desc.label = oss.str();
  }
  else if (isSgStatement(node)) {
    if (isSgGlobal(node)) {
      desc.shape = "diamond";
      desc.style = "filled";
      desc.fillcolor = "black";
    }
    else if (isSgForInitStatement(node)) {
      desc.shape = "point";
    } 
    else if (isSgScopeStatement(node)) {
      desc.color="orange";
      if (isSgBasicBlock(node)) {
        desc.shape = "point";
      }
      else if (isSgNamespaceDefinitionStatement(node)) {
        desc.label = "namespace";
        desc.shape = "diamond";
      }
      else if (isSgClassDefinition(node)) {
        desc.label = "class";
        desc.shape = "diamond";
        if (isSgTemplateClassDefinition(node)) {
          desc.label += "\\ntemplate";
        }
        else if (isSgTemplateInstantiationDefn(node)) {
          desc.label += "\\ntemplate\\ninstantiation";
        }
      }
      else if (isSgFunctionDefinition(node)) {
        desc.label = "function";
        desc.shape = "diamond";
        if (isSgTemplateFunctionDefinition(node)) {
          desc.label += "\\ntemplate";
        }
      }
      else {
        desc.color="red";
        desc.shape = "octagon";
        switch (node->variantT()) {
          case V_SgIfStmt:          desc.label = "if"; break;
          case V_SgForStatement:    desc.label = "for"; break;
          case V_SgWhileStmt:       desc.label = "while"; break;
          case V_SgDoWhileStmt:     desc.label = "do\\nwhile"; break;
          case V_SgSwitchStatement: desc.label = "switch"; break;

          case V_SgForAllStatement:     desc.label = "forall"; break;
          case V_SgFortranDo:           desc.label = "do"; break;
          case V_SgFortranNonblockedDo: desc.label = "do\\n(non-blocked)"; break;

          case V_SgCatchOptionStmt: desc.label = "catch"; break;
          default: break;
        }
      }
    }
    else if (isSgDeclarationStatement(node)) {
      switch (node->variantT()) {
        case V_SgVariableDeclaration:
        case V_SgFunctionDeclaration:
        case V_SgEnumDeclaration:
        case V_SgClassDeclaration:
        case V_SgMemberFunctionDeclaration:
        case V_SgTemplateInstantiationDecl:
        case V_SgTemplateInstantiationFunctionDecl:
        case V_SgTemplateInstantiationMemberFunctionDecl:
        case V_SgTemplateClassDeclaration:
        case V_SgTemplateFunctionDeclaration:
        case V_SgTemplateMemberFunctionDeclaration:
        case V_SgNamespaceDeclarationStatement:
        case V_SgTemplateInstantiationDirectiveStatement:
        case V_SgTypedefDeclaration:
        case V_SgFunctionParameterList:
          desc.color="blue";
          desc.shape = "Mdiamond";
          break;
        case V_SgNamespaceAliasDeclarationStatement:
        case V_SgCtorInitializerList:
        case V_SgPragmaDeclaration:
        case V_SgUsingDeclarationStatement:
        case V_SgUsingDirectiveStatement:
          desc.shape = "hexagon";
          break;
        default: break;
      }

      std::ostringstream oss;
      switch (node->variantT()) {
        case V_SgVariableDeclaration:       oss << "variable"; break;

        case V_SgEnumDeclaration:
          if (((SgEnumDeclaration *)node)->get_isUnNamed())
            oss << "enum";
          else
            oss << "enum:\\n" << ((SgEnumDeclaration *)node)->get_name().getString();
          break;
        case V_SgClassDeclaration:
          if (((SgClassDeclaration *)node)->get_isUnNamed())
            oss << "class";
          else
            oss << "class:\\n" << ((SgClassDeclaration *)node)->get_name().getString();
          break;
        case V_SgFunctionDeclaration:       oss << "function:\\n" << (      (SgFunctionDeclaration *)node)->get_name().getString(); break;
        case V_SgMemberFunctionDeclaration: oss << "method:\\n"   << ((SgMemberFunctionDeclaration *)node)->get_name().getString(); break;

        case V_SgTemplateInstantiationDecl:               oss << "template class\\ninstantiation:\\n"    <<               ((SgTemplateInstantiationDecl *)node)->get_name().getString(); break;
        case V_SgTemplateInstantiationFunctionDecl:       oss << "template function\\ninstantiation:\\n" <<       ((SgTemplateInstantiationFunctionDecl *)node)->get_name().getString(); break;
        case V_SgTemplateInstantiationMemberFunctionDecl: oss << "template method\\ninstantiation:\\n"   << ((SgTemplateInstantiationMemberFunctionDecl *)node)->get_name().getString(); break; 

        case V_SgTemplateClassDeclaration:          oss << "template class:\\n"    <<          ((SgTemplateClassDeclaration *)node)->get_name().getString(); break;
        case V_SgTemplateFunctionDeclaration:       oss << "template function:\\n" <<       ((SgTemplateFunctionDeclaration *)node)->get_name().getString(); break;
        case V_SgTemplateMemberFunctionDeclaration: oss << "template method:\\n"   << ((SgTemplateMemberFunctionDeclaration *)node)->get_name().getString(); break;

        case V_SgCtorInitializerList: oss << "constructor\\ninitialization"; break;

        case V_SgFunctionParameterList: oss << "parameters"; break;

        case V_SgNamespaceDeclarationStatement:      oss << "namespace"; break;
        case V_SgNamespaceAliasDeclarationStatement: oss << "namespace\\nallias"; break;

        case V_SgPragmaDeclaration: oss << "pragma"; break;

        case V_SgTemplateInstantiationDirectiveStatement: oss << "template\\ninstantiation\\ndirective"; break;

        case V_SgTypedefDeclaration: oss << "typedef"; break;

        case V_SgUsingDeclarationStatement: oss << "using\\ndeclaration"; break;
        case V_SgUsingDirectiveStatement:   oss << "using\\ndirective"; break;
        default: break;
      }
      desc.label = oss.str();
    }
    else if (isSgExprStatement(node)) {
      desc.color="green";
      desc.shape = "triangle";
    }
  }
  else if (isSgInitializedName(node)) {
    desc.color="blue";
    desc.label = ((SgInitializedName *)node)->get_name().getString();
  }
}

template <class InheritedAttr_, class SynthesizedAttr_>
void Default<InheritedAttr_, SynthesizedAttr_>::edit(SgNode * node, SgNode * parent, Objects::edge_desc_t & desc) const {
  desc.label="";
  desc.color="";
  desc.style="";
  desc.minlen=1;
  desc.penwidth=3;
  desc.constraint=true;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void Default<InheritedAttr_, SynthesizedAttr_>::edit(Objects::graph_t * graph, Objects::cluster_desc_t & desc) const {
  desc.label="";
  desc.color="white";
  desc.style="";
  desc.penwidth=1;

  if (graph->tag.find("expr_") == 0) {
    desc.color="lightgreen";
    desc.style="filled";
  }

  if (graph->tag.find("decl_") == 0) {
    desc.color="black";
    desc.style="";
  }
}

template <class InheritedAttr_, class SynthesizedAttr_>
void Default<InheritedAttr_, SynthesizedAttr_>::edit(Objects::graph_desc_t & desc) const {
  desc.label="";
  desc.color="white";
  desc.style="";
  desc.rankdir="TB";
  desc.ranksep=1;
}

}

}

}

