#include "EqualityTraversal.h"

size_t NodeAddressHash::operator()(const SgNode* s) const {
  return (size_t)s;
};

// the hash/equal functions depends on:
// - the node's variant
// - the node's name, if applicable
// - the node's value, if applicable
// - the number of children, if applicable
// - in the future, perhaps the node's type 

size_t NodeHash::operator()(NodeInfo node) const {
  // is this a good idea? any better solution?
  size_t hash = (size_t)(node.first)->variantT(); 
  
  // TODO: this works at the moment, but it is suboptimal
  switch ((node.first)->variantT()) {
    // variables - also consider the name, TODO
  case V_SgVariableDeclaration:

    break;
  case V_SgVarRefExp:

    break;

    // values - <<10, add to hash, at least for integer values, TODO
    
  default:
    break;
  }

  return hash;

}

bool NodeEqual::operator()(NodeInfo s1, NodeInfo s2) const {
  
  if ((s1.first)->variantT() != (s2.first)->variantT()) return false;
  if (s1.second != s2.second) return false;


  switch ((s1.first)->variantT()) {
    // for the time being, we use the mangled names to define equality
    // mangled names are different for every scope, however they are
    // the same for parameters of a given function.
    // e.g. when comparing two files containing the declaration/definition of
    // foo(int a, int b) and foo(int x, int y) respectively, a and x get
    // the same mangled name, just like b and y. in the declarations of
    // foo(bool a) and foo(int a), the parameters' mangled names are different
   

    // TODO: more such cases? casts?

    // namespaces
    // declarations see below

    // classes
  case V_SgClassNameRefExp:
    // according to the documentation, this node is quite elusive
    // we'll handle it anyway
    return isSgClassNameRefExp(s1.first)->get_symbol()->
      get_declaration()->get_mangled_name() ==
      isSgClassNameRefExp(s2.first)->get_symbol()->
      get_declaration()->get_mangled_name();
    break;

    // declarations see below

    // functions
    // this isn't absolutely necessary (at least not
    // to my knowledge), since every SgFunctionDefinition
    // has a unique SgFunctionDeclaration parent.
    /*case V_SgFunctionDefinition:
    return isSgFunctionDefinition(s1.first)->get_mangled_name() ==
      isSgFunctionDefinition(s2.first)->get_mangled_name();
      break;*/

  case V_SgFunctionRefExp:
    return isSgFunctionRefExp(s1.first)->get_symbol()->
      get_declaration()->get_mangled_name() ==
      isSgFunctionRefExp(s2.first)->get_symbol()->
      get_declaration()->get_mangled_name();
    break;

  case V_SgMemberFunctionRefExp:
    return isSgMemberFunctionRefExp(s1.first)->get_symbol()->
      get_declaration()->get_mangled_name() ==
      isSgMemberFunctionRefExp(s2.first)->get_symbol()->
      get_declaration()->get_mangled_name();
    break;

    // declarations handled below

    // initialized name
  case V_SgInitializedName:
    return isSgInitializedName(s1.first)->get_mangled_name() ==
      isSgInitializedName(s2.first)->get_mangled_name();
    break;

    // TODO: function pointers

    
    // labels and gotos
  case V_SgLabelRefExp:
    return isSgLabelRefExp(s1.first)->get_name() ==
      isSgLabelRefExp(s2.first)->get_name();
    break;

  case V_SgLabelStatement:
    return isSgLabelStatement(s1.first)->get_label() ==
      isSgLabelStatement(s2.first)->get_label();
    break;
  case V_SgGotoStatement:
    return isSgGotoStatement(s1.first)->get_label()->
      get_label() ==
      isSgGotoStatement(s2.first)->get_label()->
      get_label();
    break;


    // variables

  case V_SgVarRefExp:
    return 
      isSgVarRefExp(s1.first)->get_symbol()->get_declaration()->
      get_mangled_name() == 
      isSgVarRefExp(s2.first)->get_symbol()->get_declaration()->
      get_mangled_name();
    break;

    // declarations handled below

    // special case : differentiate pre/post mode for increment/decrement
  case V_SgPlusPlusOp:
  case V_SgMinusMinusOp:
    if (isSgUnaryOp(s1.first)->get_mode() != isSgUnaryOp(s2.first)->get_mode())
      return false;
    break;

    // values

#include "ValueEqualityCases"
    
  default:
    break;
  }
    
  // Every SgVariableDeclaration seems to have a SgInitializedName child
  // so this is redundant for SgVariableDeclarations

  // check for a SgDeclarationStatement, see documentation to get
  // an idea
  if (SgDeclarationStatement* decl1 = isSgDeclarationStatement(s1.first)) {
    SgDeclarationStatement* decl2 = isSgDeclarationStatement(s2.first); 
    return decl1->get_mangled_name() == decl2->get_mangled_name();
  }
  
  if (SgType* decl1 = isSgType(s1.first)) {
    SgType* decl2 = isSgType(s2.first); 
    return decl1->get_mangled() == decl2->get_mangled();
  }

  /*
   * SgTemplateArgument - ?!?
   * SgFunctionDefinition - a FunctionDefinition seems to always be
   *                        encapsuled by a FunctionDeclaration (?)
   * SgNamespaceDefinitionStatement
   * SgClassDefinition - again, encapsulated by declaration?
   * SgType (and all IR nodes derived from SgType) check
   * 
   * double-checked:
   * functions, function calls, classes, enums, namespaces,
   * labels, gotos
   * remaining: templates (?)
   */

  return true;

}

// TODO actually use this - maybe
bool EqualityTraversal::equal_child_ids(SgNode* n, SgNode* m) {
  
  size_t nsize = n->get_numberOfTraversalSuccessors();
  size_t msize = m->get_numberOfTraversalSuccessors();

  size_t i = 0, j = 0;

  while (i < nsize || j < msize) {
    SgNode* nchild = n->get_traversalSuccessorByIndex(i);
    SgNode* mchild = m->get_traversalSuccessorByIndex(i);
    EqualityId nid, mid;

    // find next valid child node for n
    while (!(nid = node_id_map[nchild]) && i < nsize) {

      // check for constant propagation node
      SgValueExp* value = isSgValueExp(nchild);
      if (value == NULL || 
	  value->get_originalExpressionTree() == NULL)
	// no constant propagation, continue searching
	nchild = n->get_traversalSuccessorByIndex(++i);
      else {
	// constant propagation node, get 
	// original expression tree
	nchild = value->get_originalExpressionTree();
      }
      
    }
    
    // same thing for m
    while (!(mid = node_id_map[mchild]) && j < msize) {

      // check for constant propagation node
      SgValueExp* value = isSgValueExp(mchild);
      if (value == NULL || 
	  value->get_originalExpressionTree() == NULL)
	// no constant propagation, continue searching
	mchild = m->get_traversalSuccessorByIndex(++j);
      else {
	// constant propagation node, get 
	// original expression tree
	mchild = value->get_originalExpressionTree();
      }
      
    }
    
    // one node has more children than the other
    if ((i >= nsize) != (j >= msize))
      return false;

    // ids do not match
    if (nid != mid)
      return false;
  }

  return true;
}

EqualityId EqualityTraversal::evaluateSynthesizedAttribute
(SgNode* astNode, SynthesizedAttributesList synList) {
  std::vector<EqualityId > children;

  // pass-through constant propagation nodes
  // since constant propagation nodes only have one
  // child, we can return the child's id directly

  if(SgValueExp* valExp=isSgValueExp(astNode)) {
    if(valExp->get_originalExpressionTree()) {
      assert(synList.size()==1);
    return synList[0];
    }
  }

  // generate a vector that contains only the interesting
  // ids of the children
  for (SynthesizedAttributesList::iterator i = synList.begin();
       i != synList.end(); i++) {
    if (*i > 0)
      children.push_back(*i);
  }

  // create the nodeinfo (SgNode + children id vector)
  NodeInfo node = NodeInfo(astNode, children);

  // return the id if it already exists in the hashtable
  SimpleNodeMap::iterator id = node_map.find(node);
  EqualityId ret;
  if (id != node_map.end())
    ret = (*id).second;
  else {
    // or generate a new entry in the hashtable
    EqualityId i = node_map.size()+1; // start at 1 because 0 is the "invalid id"
    node_map[node] = i;

    id_child_map[i] = children;

    ret = i;
  }

  // add the node to the node list map
  node_list_map[ret].push_back(astNode);
  node_id_map[astNode] = ret;

  return ret;
}

EqualityId EqualityTraversal::defaultSynthesizedAttribute () {
  return 0;
}


// interface functions

void EqualityTraversal::output_tables() {
  // <id>(<children>) = <nodes>

  for (int id = 1; id <= node_list_map.size(); id++) {
    std::cout << id << "( ";
    std::vector<EqualityId> children = id_child_map[id];
    std::vector<SgNode*> nodes = node_list_map[id];
    
    for (std::vector<EqualityId>::iterator j = children.begin();
	 j != children.end(); j++) {
      std::cout << *j << " ";
    }
    std::cout << ") = ";
    
    for (std::vector<SgNode*>::iterator j = nodes.begin();
	 j != nodes.end(); j++) {
      std::cout << (*j) << " ";
    }

    std::string str_rep = get_node_repr(id, FullRepresentation);

    if (str_rep.length() > 50)
      str_rep = str_rep.substr(0,50)+"...";

    std::cout << str_rep << std::endl;
  }

}

EqualityId EqualityTraversal::get_node_id(SgNode* node) {
  // Here we need a way to map from a node to an id.
  // Since I can't think of another simple way of doing that without
  // going through all of the node's children and determining their
  // ids, we'll maintain a node->id map.
  return node_id_map[node];
}

const std::vector<SgNode*>& EqualityTraversal::get_nodes_for_id(EqualityId id) {
  // alternatively, we could do this in linear time by searching node_id_map
  return node_list_map[id];
}


bool EqualityTraversal::add_extra_criteria(std::stringstream& out, 
					   SgNode* node) {

  bool output = false;

  //std::cout << "adding extra criteria for node type " << 
  // node->variantT() << "(" 
  //	    << node->sage_class_name() << ")" << std::endl;

  switch (node->variantT()) {

    // namespaces
  case V_SgNamespaceAliasDeclarationStatement:
    out << "\"" << (std::string)
      isSgNamespaceAliasDeclarationStatement(node)->
      get_name() << "\"";   
    output = true;
    break;

  case V_SgNamespaceDeclarationStatement:
    out << "\"" << (std::string)isSgNamespaceDeclarationStatement(node)->
      get_qualified_name() << "\"";
    output = true;
    break;

    // classes and enums, declarations see below
  case V_SgClassNameRefExp:
    out << "\"" << (std::string)isSgClassNameRefExp(node)->
      get_symbol()->get_declaration()->
      get_qualified_name() << "\"";    
    output = true;
    break;

  case V_SgEnumDeclaration:
    out << "\"" << (std::string)isSgEnumDeclaration(node)->
      get_qualified_name() << "\"";    
    output = true;
    break;

    // functions and member functions, declarations see below
  case V_SgFunctionRefExp:
    out << "\"" << (std::string)isSgFunctionRefExp(node)->
      get_symbol()->get_declaration()->
      get_qualified_name() << "\"";
    output = true;
    break;    

  case V_SgMemberFunctionRefExp:
    out << "\"" << (std::string)isSgMemberFunctionRefExp(node)->
      get_symbol()->get_declaration()->
      get_qualified_name() << "\"";
    output = true;
    break;  


    // labels and gotos
  case V_SgLabelRefExp:
    out << "\"" << (std::string)isSgLabelRefExp(node)->
      get_name() << "\"";
    output = true;
    break;

  case V_SgLabelStatement:
    out << "\"" << (std::string)isSgLabelStatement(node)->
      get_label() << "\"";    
    output = true;
    break;

  case V_SgGotoStatement:
    out << "\"" << (std::string)isSgGotoStatement(node)->
      get_label()->get_label() << "\""; // ...
    output = true;
    break;

    // initlalized name
  case V_SgInitializedName:
    out << "\"" << (std::string)isSgInitializedName(node)->get_name()
	<< "\"";
    output = true;
    break;

    // variables
    
    /*case V_SgVariableDeclaration:
    out << "\"" << (std::string)isSgVariableDeclaration(node)->get_name() 
	<< "\"";
	break;*/

  case V_SgVarRefExp:
    out << "\"" << (std::string)isSgVarRefExp(node)->get_symbol()->get_name() 
	<< "\"";
    output = true;
    break;

    // values
    #include "ValueStringCases"

  default:
    break;

  }

  if (output)
    return output;

  // special cases (see SgDeclarationStmt to get an idea)

  /*if (SgDeclarationStatement* d = isSgDeclarationStatement(node)) {
    out << "\"" << (std::string)d->get_mangled_name()
	<< "\"";
    output = true;
    }*/

  // SgClassDeclaration - display qualified name, use mangled name
  if (SgClassDeclaration* d = isSgClassDeclaration(node)) {
    out << "\"" << (std::string)d->get_qualified_name()
	<< "\"";
    output = true;
  }
  if (SgFunctionDeclaration* d = isSgFunctionDeclaration(node)) {
    out << "\"" << (std::string)d->get_qualified_name()
	<< "\"";
    output = true;
  }

  // TODO finish me

  return output;
}

std::string EqualityTraversal::get_node_specifics(SgNode* node) {
  std::stringstream s;
  add_extra_criteria(s, node);
  return s.str();
}

std::string EqualityTraversal::get_id_specifics(EqualityId id) {
  const std::vector<SgNode*>& nodes = get_nodes_for_id(id);
  if (nodes.size() == 0)
    return "";
  
  std::stringstream s;
  add_extra_criteria(s, nodes[0]);
  return s.str();
}

void EqualityTraversal::get_node_repr_recursive(std::stringstream& out,
						EqualityId id, 
						NodeStringRepresentation r) {

  // fetch the node if needed
  SgNode* node = NULL;

  if (r != IdRepresentation) {
    node = get_nodes_for_id(id)[0];
    if (!node) { 
      out << "(INVALID NODE " << id << ")";
      return;
    }
  }

  // output the node description
  switch (r) {
  case FullRepresentation:
    {
      // assert node initialized
      out << std::string(node->sage_class_name()).substr(2); // remove the "Sg"
      out << "[" << id << "]";
    }
    break;
  case IdRepresentation:
    out << id;
    break;
  case TreeRepresentation:
    {
      // assert node initialized
      out << std::string(node->sage_class_name()).substr(2); // remove the "Sg"
    }
    break;
  }

  // output the children

  // alas, here we need a way to get the children of a given node
  std::vector<EqualityId> children = id_child_map[id];
  bool comma = false;

  if (r != IdRepresentation || children.size() > 0)
    out << "(";

  // add the extra criteria (variable name, value..)
  if (r != IdRepresentation)
    comma = add_extra_criteria(out, node);

  // output all children
  for (std::vector<EqualityId>::iterator i = children.begin(); 
       i != children.end(); i++) { 
    
    if (comma)
      out << ", ";
    comma = true;

    get_node_repr_recursive(out, *i, r);
    
  }

  if (r != IdRepresentation || children.size() > 0)
    out << ")";

}

// get the string representation of a given node
std::string EqualityTraversal::get_node_repr(EqualityId id, NodeStringRepresentation r) {
  std::stringstream ret;
  get_node_repr_recursive(ret, id, r);
  return ret.str();
}

// get all SgExpressions (by iterating through the id->nodes map)
// assert if two nodes have the same id, either both or none are SgExpressions
void EqualityTraversal::get_all_exprs(std::vector<EqualityId>& ids) {
  for (__gnu_cxx::hash_map<EqualityId, std::vector<SgNode*> >::iterator i =
	 node_list_map.begin();
       i != node_list_map.end();
       i++) {
    
    if ((*i).second.size() > 0 && isSgExpression((*i).second[0]))
      ids.push_back((*i).first);

  }
}

// get all SgTypes (by iterating through the id->nodes map)
// assert if two nodes have the same id, either both or none are SgTypes
void EqualityTraversal::get_all_types(std::vector<EqualityId>& ids) {

  for (__gnu_cxx::hash_map<EqualityId, std::vector<SgNode*> >::iterator i =
	 node_list_map.begin();
       i != node_list_map.end();
       i++) {
    
    if ((*i).second.size() > 0 && isSgType((*i).second[0]))
      ids.push_back((*i).first);

  }

}
