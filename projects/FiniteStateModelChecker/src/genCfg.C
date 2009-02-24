#include <rose.h>
#include <stdio.h>

#include <genCfg.h>

#include <java/lang/String.h>
#include <java/lang/Throwable.h>
#include <java/util/Iterator.h>
#include <java/util/Vector.h>

#include <mops/Cfg.h>
#include <mops/CfgFunction.h>
#include <mops/Ast.h>
#include <mops/Node.h>
#include <mops/Edge.h>
#include <mops/MetaFsa.h>
#include <mops/Fsa.h>
#include <mops/Pda.h>

#include <CFG_ROSE.C>
#include <CFG_OA.C>

#include <defaultFunctionGenerator.h>
#include <shortCircuitingTransformation.h>
#include <destructorCallAnnotator.h>

using namespace std;

struct MopsAstListAttr : AstAttribute {
	list<mops::Ast *> astList;
};

struct EndScopeListAttr : AstAttribute {
	vector<mops::Ast *> astList;
};

static list<mops::Ast *> &AstListFor(SgNode *n) {
	MopsAstListAttr *attr;
	if (n->attributeExists("MopsAstList")) {
		attr = dynamic_cast<MopsAstListAttr *>(n->getAttribute("MopsAstList"));
	} else {
		attr = new MopsAstListAttr;
		n->addNewAttribute("MopsAstList", attr);
		//cout << "AstFor: had to create attribute for " << n->sage_class_name() <<  n->unparseToString() << endl;
	}
	ROSE_ASSERT(attr);
	return attr->astList;
}

static vector<mops::Ast *> &EndScopeListFor(SgNode *n) {
	EndScopeListAttr *attr;
	if (n->attributeExists("MopsEndScopeList")) {
		attr = dynamic_cast<EndScopeListAttr *>(n->getAttribute("MopsEndScopeList"));
	} else {
		attr = new EndScopeListAttr;
		n->addNewAttribute("MopsEndScopeList", attr);
		//cout << "AstFor: had to create attribute for " << n->sage_class_name() <<  n->unparseToString() << endl;
	}
	ROSE_ASSERT(attr);
	return attr->astList;
}

struct CfgNode {

     CfgNode(mops::Node *_node, mops::Ast *_ast, SgNode *_sageNode) : node(_node), ast(_ast), sageNode(_sageNode) {}

     mops::Node *node;
     mops::Ast *ast;
     SgNode *sageNode;

};

class MopsBuildCFGConfig : public BuildCFGConfig<list<CfgNode>, mops::Edge> {

private:

	typedef list<CfgNode> Node;
	typedef mops::Edge Edge;

	mops::Ast *defaultAst;
	jstring strUnknown;

	CfgNode CreateDummyNode() {
		static int addr = 1;
		mops::Node *n = new mops::Node;
		n->setLabel(strUnknown);
		n->setAddress(addr++);
		return CfgNode(n, defaultAst, 0);
	}

	list<Node *> nodes;

#if 0
	SgNode *ControlFlowNode(SgNode *node) {
	     while (1) {
		  cout << "ControlFlowNode for " << node->class_name() << endl;
		  switch (node->variantT()) {
		       case V_SgCastExp:
			  node = isSgCastExp(node)->get_operand();
			  break;
		       case V_SgExprStatement:
			  node = isSgExprStatement(node)->get_expression_root()->get_operand();
			  break;
		       default:
		          cout << "returning a " << node->class_name() << endl;
			  return node;
		  }
	     }
	}
#else
	// The purpose of this function is to make the creation of CFG nodes consistent
	// with the behavior observed of the MOPS cc1 program
	mops::Ast *ControlFlowNode(mops::Ast *node) {
	     switch (node->getKind()) {
		  case mops::Ast::kind_expression_stmt:
		     return (mops::Ast *)node->getChildren()->get(0);
		  default:
		     return node;
	     }
	}
#endif

public:

	list<Node *> entryNodes, exitNodes;

	MopsBuildCFGConfig() {
	     defaultAst = new mops::Ast;
	     defaultAst->setKind(mops::Ast::kind_dummy);
	     defaultAst->setAddress(1);
		strUnknown = JvNewStringLatin1("UNKNOWN:0");
	}

	virtual ~MopsBuildCFGConfig() {
		for (list<Node *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
			delete *i;
		}
	}

	Node *CreateNode() {
		Node *n = new Node;
		//printf("CreateNode 0x%x\n", n);
		nodes.push_back(n);
		entryNodes.push_back(n);
		exitNodes.push_back(n);
		return n;
	}

	Edge *CreateEdge(Node *n1, Node *n2, EdgeType condval) {
		//printf("CreateEdge 0x%x 0x%x\n", n1, n2);
		entryNodes.remove(n2);
		exitNodes.remove(n1);
		if (n1->size() == 0) {
			n1->push_back(CreateDummyNode());
		}
		if (n2->size() == 0) {
			n2->push_back(CreateDummyNode());
			n2->back().sageNode = n1->back().sageNode;
		}
		list<mops::Ast *> exitAsts = nodesLeft(n1, n2);
		mops::Node *node = n1->back().node;
		mops::Ast *ast = n1->back().ast;

		cout << "Between " << n1->back().sageNode << " and " << n2->back().sageNode << "(" << n1->size() << " and " << n2->size() << "):" << endl;
		for (list<mops::Ast *>::iterator i = exitAsts.begin(); i != exitAsts.end(); ++i) {
		     int q = (*i)->getAddress();
		     cout << " destructor " << q << " called." << endl;
		     mops::Node *newNode = CreateDummyNode().node;
		     Edge *e = new Edge;
		     e->setNodes(node, newNode);
		     e->setLabel(ControlFlowNode(ast));
		     node = newNode;
		     ast = *i;
		}

		Edge *e = new Edge;
		e->setNodes(node, n2->front().node);
		e->setLabel(ControlFlowNode(ast));
		return e;
	}

	list<SgScopeStatement *> getScopeTrail(SgNode *n) {
	     list<SgScopeStatement *> trail;
	     cout << "getScopeTrail got a " << n->class_name() << ", " << n << endl;
	     do {
		  cout << "getScopeTrail : iteration " << n << n->class_name() << endl;
		  SgScopeStatement *s;
		  if ((s = isSgScopeStatement(n))) {
		       trail.push_front(s);
		       if (isSgFunctionDefinition(s)) {
			    cout << "getScopeTrail : returning trail" << endl;
	                  return trail;
		       }
		  }
	     } while ((n = n->get_parent()));
	     cout << "getScopeTrail : returning empty" << endl;
	     return list<SgScopeStatement *>();
	}

	template<typename T>
	static pair<typename list<T>::const_iterator, typename list<T>::const_iterator> findFirstDifference(const list<T> &li, const list<T> &lj) {
	     for (typename list<T>::const_iterator i = li.begin(), j = lj.begin(); i != li.end() || j != lj.end(); i++, j++) {
		  if (i == li.end() || j == lj.end() || *i != *j) {
		       return make_pair(i, j);
		  }
	     }
	     return make_pair(li.end(), lj.end());
	}

	template<typename T> // T must implement Iteratable List.
	static void dump(T& list) {
	     cout << "[";
	     for (typename T::const_iterator i = list.begin(); i != list.end(); ++i) {
		  cout << *i << ",";
	     }
	     cout << "]" << endl;
	}
	list<SgScopeStatement *> scopesLeft(const list<SgScopeStatement *> &li, const list<SgScopeStatement *> &lj) {
	     cout << "scopesLeft: li is "; dump(li);
	     cout << "lj is "; dump(lj);
	     pair<list<SgScopeStatement *>::const_iterator, list<SgScopeStatement *>::const_iterator> i = findFirstDifference(li, lj);
	     list<SgScopeStatement *> l;
	     l.insert(l.end(), i.first, li.end());
	     l.reverse();
	     return l;
	}

	list<SgScopeStatement *> scopesLeft(Node *li, Node *lj) {
	     cout << "scopesLeft : li is " << (li->empty() ? (SgNode *)1 : li->back().sageNode) << endl;
	     cout << "scopesLeft : lj is " << (lj->empty() ? (SgNode *)1 : lj->back().sageNode) << endl;
	     return scopesLeft(li->empty() || !li->back().sageNode ? list<SgScopeStatement *>() : getScopeTrail(li->back().sageNode),
			       lj->empty() || !lj->back().sageNode ? list<SgScopeStatement *>() : getScopeTrail(lj->back().sageNode));
	}

	list<mops::Ast *> nodesLeft(Node *li, Node *lj) {
	     list<SgScopeStatement *> scopes = scopesLeft(li, lj);
	     list<mops::Ast *> nodes;
	     for (list<SgScopeStatement *>::iterator i = scopes.begin(); i != scopes.end(); ++i) {
		  vector<mops::Ast *> &endScopeList = EndScopeListFor(*i);
		  nodes.insert(nodes.end(), endScopeList.begin(), endScopeList.end());
	     }
	     return nodes;
	}

	void AddNodeStmt(Node *n, const AstNodePtr &s) {
		//printf("AddNodeStmt 0x%x\n", n);
		stringstream label;
		label << s->get_file_info()->get_filenameString() << ":" << s->get_file_info()->get_line();
		
		list<mops::Ast *> &astList = AstListFor(s);
		int count = 0;
		for (list<mops::Ast *>::iterator i = astList.begin(); i != astList.end(); ++i) {
			mops::Node *newNode = new mops::Node;
			newNode->setLabel(JvNewStringLatin1(label.str().c_str()));
			newNode->setAddress((int)(SgNode *)s+(count++));
			if (n->size() != 0) {
				Edge *e = new Edge;
				e->setNodes(n->back().node, newNode);
				e->setLabel(ControlFlowNode(n->back().ast));
			}
			//cout << "AddNodeStmt: pushing back" << endl;
			n->push_back(CfgNode(newNode, *i, s));
			//cout << "AddNodeStmt: pushed back" << endl;
		}
	}

	mops::Node *entryNode() {
		if (entryNodes.size() == 1) {
			return entryNodes.front()->front().node;
		}
		/* If no entry nodes were found in the usual way, this means
		 * that the entry node must have at least one in-edge.  MOPS
		 * cannot deal with this case so we create a fake entry node.
		 */
		list<Node *>::iterator i = nodes.begin();
		++i;
		mops::Node *oldEntry = (*i)->front().node;
		mops::Node *newEntry = new mops::Node;
		newEntry->setLabel(strUnknown);
		newEntry->setAddress(-1);
		mops::Edge *edge = new mops::Edge;
		edge->setNodes(newEntry, oldEntry);
		edge->setLabel(defaultAst);
		return newEntry;
	}

	mops::Node *exitNode() {
		if (exitNodes.size() > 1) {
			mops::Node *x = CreateDummyNode().node;
			for (list<Node *>::iterator i = exitNodes.begin(); i != exitNodes.end(); ++i) {
				Edge *e = new Edge;
				e->setNodes((*i)->back().node, x);
				e->setLabel(defaultAst);
			}
			return x;
		}
		return exitNodes.front()->back().node;
	}

};

class MopsBuildAst : public SgBottomUpProcessing<pair<list<mops::Ast *> *, vector<mops::Ast *> > > {

	pair<list<mops::Ast *> *, vector<mops::Ast *> > defaultSynthesizedAttribute() {
		return make_pair((list<mops::Ast *> *)0, vector<mops::Ast *>());
	}

	/*
	mops::Node *buildConstructorCallStmt(SgConstructorInitializer *ci) {
	     mops::Ast *constructorRef = new mops::Ast;
	     constructorRef->setAddress(((int) ci)+3);
	     constructorRef->setChildren(new java::util::Vector);
	     constructorRef->setKind(mops::Ast::kind_identifier);
	     constructorRef->getChildren()->add(JvNewStringLatin1(ci->get_declaration()->get_mangled_name().str()));

	     mops::Ast *callExpr = new mops::Ast;
	     callExpr->setAddress(((int) ci)+4);
	     callExpr->setChildren(new java::util::Vector);
	     callExpr->setKind(mops::Ast::kind_function_call);
	     callExpr->getChildren()->add(constructorRef);
	     FOR_EACH_KID({
		    callExpr->getChildren()->add(kid);
	     })

	     mops::Ast *callStmt = new mops::Ast;
	     callStmt->setAddress(((int) ci)+5);
	     callStmt->setChildren(new java::util::Vector);
	     callStmt->setKind(mops::Ast::kind_expression_stmt);
	     callStmt->getChildren()->add(callExpr);

	     return callStmt;
	}
	*/

	pair<list<mops::Ast *> *, vector<mops::Ast *> > evaluateSynthesizedAttribute(SgNode *node, vector<pair<list<mops::Ast *> *, vector<mops::Ast *> > > subnodes) {
		mops::Ast *newAst = new mops::Ast;
		java::util::Vector *jSubnodes = new java::util::Vector;
		newAst->setAddress((int) node);
		newAst->setChildren(jSubnodes);
		list<mops::Ast *> &astList = AstListFor(node);
		astList.push_back(newAst);

		list<mops::Ast *> childAstList;
		for (vector<pair<list<mops::Ast *> *, vector<mops::Ast *> > >::iterator i = subnodes.begin(); i != subnodes.end(); ++i) {
		     list<mops::Ast *> *lst = i->first;
		     if (lst) {
			  for (list<mops::Ast *>::iterator j = lst->begin(); j != lst->end(); ++j) {
			       mops::Ast *kid = *j;
			       if (kid) {
				    childAstList.push_back(kid);
			       }
			  }
		     }
		}

		vector<mops::Ast *> endScopeList;
               	for (vector<pair<list<mops::Ast *> *, vector<mops::Ast *> > >::iterator i = subnodes.begin(); i != subnodes.end(); ++i) {
		     endScopeList.insert(endScopeList.begin(), i->second.begin(), i->second.end());
		}
		switch (node->variantT()) {
			case V_SgFunctionDeclaration:
			case V_SgMemberFunctionDeclaration: {
				newAst->setKind(mops::Ast::kind_function_decl);
#if 0
				mops::Ast *declarator = new mops::Ast;
				declarator->setKind(mops::Ast::kind_declarator);
				declarator->setAddress(((int) node)+1);
				java::util::Vector *declSub = new java::util::Vector;
#endif
				childAstList.front()->getChildren()->add(0, JvNewStringLatin1(isSgFunctionDeclaration(node)->get_mangled_name().str())); // first subnode is the declarator, which does not know th function name
#if 0
				declarator->setChildren(declSub);
				jSubnodes->add(declarator);
#endif
				break;
			}
			case V_SgDotExp: {
#if 0
			    SgDotExp *de = isSgDotExp(node);
			    if (isSgMemberFunctionRefExp(de->get_rhs_operand())) {
#endif
				 astList = list<mops::Ast *>(1, childAstList.back());
				 return make_pair(&astList, endScopeList);
#if 0
			    } else {
				 newAst->setKind(mops::Ast::kind_field_ref);
				 jSubnodes->add(subnodes[0]);
				 return newAst;
			    }
#endif
		       }	break;
			case V_SgArrowExp: {
			    SgArrowExp *de = isSgArrowExp(node);
			    if (isSgMemberFunctionRefExp(de->get_rhs_operand())) {
				 SgType *t = de->get_lhs_operand()->get_type()->dereference();
				 cout << "SgArrowExp member function reference: lhs is " << t->get_mangled().getString() << endl;
				 // TODO: handle virtual functions
			    } else {
				 astList = list<mops::Ast *>(1, childAstList.back());
				 return make_pair(&astList, endScopeList);
#if 0
				 newAst->setKind(mops::Ast::kind_field_ref);
				 jSubnodes->add(subnodes[0]);
				 return newAst;
#endif
			    }
		       }	break;
			case V_SgConstructorInitializer: {
				mops::Ast *constructorRef = new mops::Ast;
				constructorRef->setAddress(((int) node)+1);
				constructorRef->setChildren(new java::util::Vector);
				constructorRef->setKind(mops::Ast::kind_identifier);

				SgMemberFunctionDeclaration *theConstructor = isSgConstructorInitializer(node)->get_declaration();
				if (!theConstructor) {
				     cout << "No constructor reference, possibly an array" << endl;
				     SgType *type = 0;
				     if (SgNewExp *nx = isSgNewExp(node->get_parent())) {
					  type = nx->get_type();
				     } else if (SgInitializedName *in = isSgInitializedName(node->get_parent())) {
					  type = in->get_type();
				     }

				     if (type) {
					  cout << "type is a " << type->class_name() << endl;
				     }
				} else
				constructorRef->getChildren()->add(JvNewStringLatin1(theConstructor->get_mangled_name().str()));

				newAst->setKind(mops::Ast::kind_function_call);
				newAst->getChildren()->add(constructorRef);
			}	break;

#define SIMPLE_KIND(cls, kind) \
			case V_Sg##cls: \
				newAst->setKind(mops::Ast::kind_##kind); \
				break;

			SIMPLE_KIND(FunctionParameterList, declarator)
			SIMPLE_KIND(ExprStatement, expression_stmt)
			SIMPLE_KIND(FunctionCallExp, function_call)
			SIMPLE_KIND(FunctionRefExp, identifier)
			SIMPLE_KIND(MemberFunctionRefExp, identifier)
			SIMPLE_KIND(VarRefExp, identifier)
			SIMPLE_KIND(BasicBlock, compound_stmt)
			SIMPLE_KIND(InitializedName, compound_stmt) // note: not correct, but will be corrected by parent treraversal
			SIMPLE_KIND(ExprListExp, compound_stmt) // nodetype for this node does not matter either as its children are used directly
			SIMPLE_KIND(VariableDeclaration, data_decl)
			SIMPLE_KIND(ReturnStmt, return_stmt)
			SIMPLE_KIND(NullStatement, empty_stmt)
			SIMPLE_KIND(IfStmt, if_stmt)
			SIMPLE_KIND(WhileStmt, while_stmt)
			SIMPLE_KIND(ForStatement, for_stmt)
			SIMPLE_KIND(DoWhileStmt, dowhile_stmt)
			SIMPLE_KIND(SwitchStatement, switch_stmt)
			SIMPLE_KIND(CaseOptionStmt, labeled_stmt)
			SIMPLE_KIND(DefaultOptionStmt, labeled_stmt)
			SIMPLE_KIND(ConditionalExp, conditional)
			SIMPLE_KIND(BreakStmt, break_stmt)
			SIMPLE_KIND(ContinueStmt, continue_stmt)
#undef SIMPLE_KIND

#define CONST_KIND(a, cls, z) \
			case V_Sg##cls: { \
				newAst->setKind(mops::Ast::kind_lexical_cst); \
				stringstream ss; \
				ss << a << isSg##cls(node)->get_value() << z; \
				jSubnodes->add(JvNewStringLatin1(ss.str().c_str())); \
			}	return make_pair(&astList, endScopeList);

			CONST_KIND("",BoolValExp,"")
			CONST_KIND("'",CharVal,"'")
			CONST_KIND("",DoubleVal,"")
			CONST_KIND("",EnumVal,"")
			CONST_KIND("",FloatVal,"")
			CONST_KIND("",IntVal,"")
			CONST_KIND("",LongDoubleVal,"")
			CONST_KIND("",LongIntVal,"")
			CONST_KIND("",LongLongIntVal,"")
			CONST_KIND("",ShortVal,"")
			CONST_KIND("\"",StringVal,"\"")
			CONST_KIND("'",UnsignedCharVal,"'")
			CONST_KIND("",UnsignedIntVal,"")
			CONST_KIND("",UnsignedLongLongIntVal,"")
			CONST_KIND("",UnsignedLongVal,"")
			CONST_KIND("",UnsignedShortVal,"")
#undef CONST_KIND

#define STR_PREFIX_KIND(cls, kind, str) \
			case V_Sg##cls: { \
				static jstring prefix = 0; \
				if (!prefix) prefix = JvNewStringLatin1(str); \
				newAst->setKind(mops::Ast::kind_##kind); \
				jSubnodes->add(prefix); \
			}	break;

#define UNARY_KIND(cls, str) STR_PREFIX_KIND(cls, unary, str)
#define BINARY_KIND(cls, str) STR_PREFIX_KIND(cls, binary, str)

			UNARY_KIND(AddressOfOp, "&")
			UNARY_KIND(BitComplementOp, "~")
			UNARY_KIND(MinusMinusOp, "--")
			UNARY_KIND(MinusOp, "-")
			UNARY_KIND(NotOp, "!")
			UNARY_KIND(PlusPlusOp, "++")
			UNARY_KIND(PointerDerefExp, "\\*")
			UNARY_KIND(UnaryAddOp, "+")

			BINARY_KIND(AddOp, "+")
			BINARY_KIND(AndAssignOp, "&&=")
			BINARY_KIND(AndOp, "&&")
			BINARY_KIND(AssignOp, "=")
			BINARY_KIND(BitAndOp, "&")
			BINARY_KIND(BitOrOp, "|")
			BINARY_KIND(BitXorOp, "^")
			BINARY_KIND(CommaOpExp, ",")
			BINARY_KIND(DivAssignOp, "/=")
			BINARY_KIND(DivideOp, "/")
			//BINARY_KIND(DotExp, ".")
			BINARY_KIND(EqualityOp, "==")
			BINARY_KIND(GreaterOrEqualOp, ">=")
			BINARY_KIND(GreaterThanOp, ">")
			BINARY_KIND(IntegerDivideOp, "/")
			BINARY_KIND(IorAssignOp, "||=")
			BINARY_KIND(LessOrEqualOp, "<=")
			BINARY_KIND(LessThanOp, "<")
			BINARY_KIND(LshiftAssignOp, "<<=")
			BINARY_KIND(LshiftOp, "<<")
			BINARY_KIND(MinusAssignOp, "-=")
			BINARY_KIND(ModAssignOp, "%=")
			BINARY_KIND(ModOp, "%")
			BINARY_KIND(MultAssignOp, "\\*=")
			BINARY_KIND(MultiplyOp, "\\*")
			BINARY_KIND(NotEqualOp, "!=")
			BINARY_KIND(OrOp, "||")
			BINARY_KIND(PlusAssignOp, "+=")
			BINARY_KIND(PntrArrRefExp, "\\*")
			BINARY_KIND(RshiftAssignOp, ">>=")
			BINARY_KIND(RshiftOp, ">>")
			BINARY_KIND(ScopeOp, "::")
			BINARY_KIND(SubtractOp, "-")
			BINARY_KIND(XorAssignOp, "^^=")

#undef STR_PREFIX_KIND
#undef UNARY_KIND
#undef BINARY_KIND

			case V_SgFunctionDefinition:
			case V_SgExpressionRoot:
			case V_SgCastExp:
			case V_SgNewExp:
//			case V_SgConstructorInitializer:
				 astList = childAstList;
				 return make_pair(&astList, endScopeList);
			case V_SgNullExpression:
				 astList = list<mops::Ast *>();
				 return make_pair(&astList, endScopeList);
			default:
				newAst->setKind(mops::Ast::kind_compound_stmt);
				jSubnodes->add(JvNewStringLatin1(node->sage_class_name()));
		}

#if 0
		jSubnodes->add(JvNewStringLatin1(node->sage_class_name()));
#endif

#define FOR_EACH_KID(block)	for (list<mops::Ast *>::iterator i = childAstList.begin(); i != childAstList.end(); ++i) { \
				     mops::Ast *kid = *i; \
     			             block \
				}

		switch (node->variantT()) {
			case V_SgVariableDeclaration:
			        // This unusual construction is to allow any constructors of the variable to be called at the correct time.
                        	for (vector<pair<list<mops::Ast *> *, vector<mops::Ast *> > >::iterator i = subnodes.begin(); i != subnodes.end(); ++i) {
				     list<mops::Ast *> *lst = i->first;
				     if (lst) {
					     list<mops::Ast *>::iterator j = lst->begin();
					     if (j != lst->end()) {
						  (*j)->setKind(mops::Ast::kind_variable_decl);
						  jSubnodes->add(*j);
					          ++j;
				             }
					     for (; j != lst->end(); ++j) {
						  mops::Ast *kid = *j;
						  if (kid) {
						       astList.push_back(kid);
					          }
					     }
				     }
				}
				break;
			case V_SgFunctionParameterList:
				FOR_EACH_KID({
					kid->setKind(mops::Ast::kind_parameter);
					jSubnodes->add(kid);
				})
				break;
			/*
			case V_SgFunctionCallExp: {
				jSubnodes->add(subnodes[0]->front());
				jSubnodes->addAll((java::util::Collection *) subnodes[1]->front()->getChildren());
			}	break;
			*/
			case V_SgFunctionRefExp: {
				jSubnodes->add(JvNewStringLatin1(isSgFunctionRefExp(node)->get_symbol()->get_declaration()->get_mangled_name().str()));
			}	break;
			case V_SgMemberFunctionRefExp: {
				jSubnodes->add(JvNewStringLatin1(isSgMemberFunctionRefExp(node)->get_symbol_i()->get_declaration()->get_mangled_name().str()));
			}	break;
			case V_SgVarRefExp: {
				SgVarRefExp *var = isSgVarRefExp(node);
				printf("adding VarRefExp : %s\n", var->get_symbol()->get_declaration()->get_qualified_name().str());
				jSubnodes->add(JvNewStringLatin1(var->get_symbol()->get_declaration()->get_mangled_name().str()));
				jSubnodes->add(AstListFor(var->get_symbol()->get_declaration()).front()->getChildren()->get(0));
			}	break;
			case V_SgCaseOptionStmt:
			case V_SgDefaultOptionStmt: {
				 // ignore valeu of the case stmt
				jSubnodes->add(subnodes[1].first->front());
			}	break;
			case V_SgPntrArrRefExp: {
			        mops::Ast *plus = new mops::Ast;
				plus->setAddress(((int)node)+1);
				plus->setChildren(new java::util::Vector);
				plus->setKind(mops::Ast::kind_binary);
				plus->getChildren()->add(JvNewStringLatin1("+"));

				FOR_EACH_KID({
					plus->getChildren()->add(kid);
				})

				jSubnodes->add(plus);
			}	break;
			case V_SgInitializedName: {
				SgInitializedName *name = isSgInitializedName(node);
				jstring qualName = JvNewStringLatin1(name->get_mangled_name().str());
				newAst->setAddress(((int) node)+1);
				mops::Ast *dataDecl = new mops::Ast;
				dataDecl->setKind(mops::Ast::kind_data_declaration);
				dataDecl->setAddress((int) node);
				mops::Ast *declarator = new mops::Ast;
				declarator->setKind(mops::Ast::kind_declarator);
				declarator->setAddress(((int) node)+2);
				declarator->setChildren(new java::util::Vector);
				declarator->getChildren()->add(qualName);
				jSubnodes->add(dataDecl);
				jSubnodes->add(declarator);

				if (name->get_initializer()) {
				     SgInitializer *init = name->get_initializer();
				     cout << "init is a " << init->sage_class_name() << endl;
				     switch (init->variantT()) {
					  case V_SgAssignInitializer: {
						static jstring eqStr = 0;
						if (eqStr == 0) eqStr = JvNewStringLatin1("=");

						mops::Ast *varRef = new mops::Ast;
						varRef->setAddress(((int) node)+3);
						varRef->setChildren(new java::util::Vector);
						varRef->setKind(mops::Ast::kind_identifier);
						varRef->getChildren()->add(qualName);
						varRef->getChildren()->add(dataDecl);

						mops::Ast *initializerExpr = new mops::Ast;
						initializerExpr->setAddress(((int) node)+4);
						initializerExpr->setChildren(new java::util::Vector);
						initializerExpr->setKind(mops::Ast::kind_binary);
						initializerExpr->getChildren()->add(eqStr);
						initializerExpr->getChildren()->add(varRef);
						initializerExpr->getChildren()->add(subnodes[0].first->front()->getChildren()->get(1));

						mops::Ast *initializerStmt = new mops::Ast;
						initializerStmt->setAddress(((int) node)+5);
						initializerStmt->setChildren(new java::util::Vector);
						initializerStmt->setKind(mops::Ast::kind_expression_stmt);
						initializerStmt->getChildren()->add(initializerExpr);

						astList.push_back(initializerStmt);
					}	break;
					  case V_SgConstructorInitializer: {
						SgConstructorInitializer *cinit = isSgConstructorInitializer(init);
						ROSE_ASSERT(cinit != 0);

						mops::Ast *callStmt = new mops::Ast;
						callStmt->setAddress(((int) node)+5);
						callStmt->setChildren(new java::util::Vector);
						callStmt->setKind(mops::Ast::kind_expression_stmt);
						callStmt->getChildren()->add(subnodes[0].first->front());

						astList.push_back(callStmt);

#if 0
						/* TODO: handle case where object has no destructor.  If object has members with destructors then those should be called */
						if (cinit->get_class_decl() && cinit->get_declaration()) {
							SgFunctionSymbol *dtorSymbol = cinit->get_class_decl()->get_definition()->lookup_function_symbol("~"+cinit->get_declaration()->get_name().getString());
							if (dtorSymbol) {
							     mops::Ast *constructorRef = new mops::Ast;
							     constructorRef->setAddress(((int) node)+6);
							     constructorRef->setChildren(new java::util::Vector);
							     constructorRef->setKind(mops::Ast::kind_identifier);
							     constructorRef->getChildren()->add(JvNewStringLatin1(dtorSymbol->get_declaration()->get_mangled_name().str()));

							     mops::Ast *callExpr = new mops::Ast;
							     callExpr->setAddress(((int) node)+7);
							     callExpr->setChildren(new java::util::Vector);
							     callExpr->setKind(mops::Ast::kind_function_call);
							     callExpr->getChildren()->add(constructorRef);

							     mops::Ast *callStmt = new mops::Ast;
							     callStmt->setAddress(((int) node)+8);
							     callStmt->setChildren(new java::util::Vector);
							     callStmt->setKind(mops::Ast::kind_expression_stmt);
							     callStmt->getChildren()->add(callExpr);

							     endScopeList.push_back(callStmt);
							     printf("%p : %p dtorSymbol found!\n", node, callStmt);
							} 
						}
#endif
					  }	break;
					  default:
						cout << "Initializer: default case reached" << endl;
				     }
				}
			}	break;
		       /* commented out because if/while/dowhile stmts can have a condition statement
			case V_SgIfStmt:
			case V_SgWhileStmt: {
				jSubnodes->add(subnodes[0].first->front()->getChildren()->get(0));
				jSubnodes->add(subnodes[1].first->front());
			}	break;
			case V_SgDoWhileStmt: {
				jSubnodes->add(subnodes[0].first->front());
				jSubnodes->add(subnodes[1].first->front()->getChildren()->get(0));
			}	break;
			*/
			case V_SgForInitStatement:
			case V_SgExprListExp: {
				astList.clear();
				
				FOR_EACH_KID({
					astList.push_back(kid);
				})
			}	break;
			default:
				FOR_EACH_KID({
					jSubnodes->add(kid);
				})
		}
		if (isSgBasicBlock(node)) { /* will not appear in AST */
		     for (vector<mops::Ast *>::iterator i = endScopeList.begin(); i != endScopeList.end(); ++i) {
			  printf("%p MopsBuildAst : appending end-scope node %p to basic block\n", node, *i);
			  jSubnodes->add(*i);
		     }
		     EndScopeListFor(node) = endScopeList;
		     endScopeList.clear();
		}
		return make_pair(&astList, endScopeList);
	}

};

mops::CfgFunction *MopsBuildFunction(SgFunctionDeclaration *decl) {
	MopsBuildAst mba;
	mops::Ast *ast = mba.traverse(decl).first->front();

	SgFunctionDefinition *def = decl->get_definition();
	MopsBuildCFGConfig mbc;
	AstInterface fa(def);
#if 1
	ROSE_Analysis::BuildCFG(fa, def, mbc);
#else
	OpenAnalysis::BuildCFG(fa, def, mbc);
#endif

	mops::CfgFunction *fn = new mops::CfgFunction;
	//printf("entryNodes size = %zu exitNodes size = %zu\n", mbc.entryNodes.size(), mbc.exitNodes.size());
#if 0
	fn->entry = mbc.entryNodes.front()->front().first;
	fn->exit = mbc.exitNodes.front()->front().first;
#else
	fn->entry = mbc.entryNode();
	fn->exit = mbc.exitNode();
#endif
	// The logbal "main" function must be handled specially
	if (isSgGlobal(decl->get_scope()) && (decl->get_name() == "main"))
	   {
	     fn->label = JvNewStringLatin1("main");
	   }
	else
	   {
	     fn->label = JvNewStringLatin1(decl->get_mangled_name().str());
	   }
	fn->ast = ast;
	return fn;
}

mops::Ast *MopsBuildVariable(SgVariableDeclaration *decl) {
     MopsBuildAst mba;
     mops::Ast *ast = mba.traverse(decl).first->front();

     return ast;
}

static void DoScope(mops::Cfg *cfg, SgScopeStatement *scope);

static void DoDeclaration(mops::Cfg *cfg, SgDeclarationStatement *decl) {
     SgVariableDeclaration *vd = isSgVariableDeclaration(decl);
     SgFunctionDeclaration *fd = isSgFunctionDeclaration(decl);
     SgNamespaceDeclarationStatement *nd = isSgNamespaceDeclarationStatement(decl);
#if 0
     SgClassDeclaration *cd = isSgClassDeclaration(decl);
     SgTypedefDeclaration *td = isSgTypedefDeclaration(decl);
#endif
     if (vd) {
	  SgInitializedName *var = *(vd->get_variables().begin());
	  if (var->get_storageModifier().isExtern()) return;
	  cfg->getDataDecls()->add(MopsBuildVariable(vd));
     } else if (fd) {
#if 1
	  if (!fd->get_definition()) return;
#else
	  fd = isSgFunctionDeclaration(fd->get_definingDeclaration());
	  if (!fd) return;
#endif
	  cfg->getFunctions()->add(MopsBuildFunction(fd));
     } else if (nd) {
	  if (nd->get_definition()) DoScope(cfg, nd->get_definition());
#if 0
     } else if (cd) {
	  if (cd->get_definition()) DoScope(cfg, cd->get_definition());
     } else if (td) {
	  if (td->get_declaration()) {
	       DoDeclaration(cfg, td->get_declaration()); // note: this won't handle the various other places one might have squirreled away a declaration... should be handled in full visit traversal below
#if 0
	       AstPDFGeneration pdf;
	       static int count = 0;
	       stringstream ss;
	       ss << count++;
	       pdf.generate(ss.str(), td->get_declaration());
#endif
	  }
#endif
     } else {
	  cout << "unrecognised " << decl->class_name() << endl;
     }
}

static void DoScope(mops::Cfg *cfg, SgScopeStatement *scope) {
	  SgDeclarationStatementPtrList &decls = scope->getDeclarationList();
#if 0
	  // Variables may be used before they are declared in a class
	  for (SgDeclarationStatementPtrList::iterator i = decls.begin(); i != decls.end(); ++i) {
	       if (isSgVariableDeclaration(*i)) DoDeclaration(cfg, *i);
	  }
#endif
	  for (SgDeclarationStatementPtrList::iterator i = decls.begin(); i != decls.end(); ++i) {
	       if (!isSgVariableDeclaration(*i)) DoDeclaration(cfg, *i);
	  }
}
     
mops::Cfg *MopsBuildProject(SgProject *project) {

     class VisitVariableDecls : public ROSE_VisitTraversal {

	  mops::Cfg *cfg;

	public:

	  VisitVariableDecls(mops::Cfg *_cfg) : cfg(_cfg) {}

	  void visit(SgNode *node) {
	       SgVariableDeclaration *vd = isSgVariableDeclaration(node);
	       if (vd) {
		    SgScopeStatement *scope = vd->get_scope();
		    printf("VisitVariableDecls : visiting SgVariableDeclaration by the name of %p\n", vd);
		    if (isSgGlobal(scope) || isSgClassDefinition(scope) || isSgNamespaceDefinitionStatement(scope)) {
			 printf("scope ok (is a %s)\n", scope->sage_class_name());
			 DoDeclaration(cfg, vd);
		    }
	       }
	  }

     };

     defaultFunctionGenerator(project);

     shortCircuitingTransformation(project);

     destructorCallAnnotator(project);

     mops::Cfg *cfg = new mops::Cfg;

     VisitVariableDecls vvd(cfg);
     SgVariableDeclaration::traverseMemoryPoolNodes(vvd);
     puts("VisitVariableDecls done");

     for (SgFilePtrList::iterator j = project->get_fileList()->begin(); j != project->get_fileList()->end(); ++j) {
	  SgFile *file = *j;
	  DoScope(cfg, file->get_globalScope());
     }
     return cfg;
}
