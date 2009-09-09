// EqualityTraversal, the heart of the AST matcher mechanism
// Author: Mihai Ghete, 2008
//         with performance tweaks by Gergo Barany

#include <cstdlib>
#include "EqualityTraversal.h"

// If DEBUG_EQUALITY_TRAVERSAL is defined to a true value, the equality
// traversal dumps some interesting information onto stdout. This
// information is a sort of profile that shows where the bottlenecks are.
// #define DEBUG_EQUALITY_TRAVERSAL 42

// If BE_PICKY_ABOUT_EMPTY_MANGLED_NAMES is defined to a true value, the
// equality traversal will complain whenever an object with an empty mangled
// name is encountered; this should never happen. Otherwise, the nonmangled
// names are compared and we hope for the best.
// #define BE_PICKY_ABOUT_EMPTY_MANGLED_NAMES 42

size_t NodeAddressHash::operator()(const SgNode* s) const {
  return (size_t)s;
};

// the hash/equal functions depends on:
// - the node's variant
// - the node's name, if applicable
// - the node's value, if applicable
// - the number of children, if applicable
// - in the future, perhaps the node's type 

#if DEBUG_EQUALITY_TRAVERSAL
unsigned int stringHashes[0xFFFF];
#endif

// GB (2008-05-06): Compute a 16-bit hash of the string.
size_t NodeHash::hashString(const std::string &str)
{
    size_t hash = 1;
    std::string::size_type len = str.length();
    for (std::string::size_type i = 0; i < len; i++)
    {
     // Simple but apparently not completely horrible hash.
        if (str[i] != '\0' && str[i] != ' ')
            hash *= str[i];
        if (hash == 0)
            hash = len + 1;
    }
    size_t tmp = 0;
 // Get the higher-order bits into the final hash, too.
    for (size_t i = 0; i < sizeof (size_t); i += 2)
    {
        tmp ^= hash & 0xFFFF;
        hash >>= 16;
    }
    hash = tmp;
#if DEBUG_EQUALITY_TRAVERSAL
    stringHashes[hash & 0xFFFF]++;
#endif
    return hash & 0xFFFF;
}

#if DEBUG_EQUALITY_TRAVERSAL
unsigned int intHashes[0xFFFF];
#endif

// GB (2008-05-19): Added this function as int constants are also a little
// problematic, in particular ones with lots of zero low-order bits.
size_t NodeHash::hashInt(int val)
{
 // unsigned integer types are better for bit-fiddling
    unsigned int uval = val;
    size_t lowerOrderBits = uval & 0xFFFF;
    // Hash the zero value to something that probably won't cause many
    // collisions.
    if (val == 0)
        lowerOrderBits = 0xABCD;
#if DEBUG_EQUALITY_TRAVERSAL
    intHashes[lowerOrderBits & 0xFFFF]++;
#endif
    return lowerOrderBits;
}

// Compute a 16-bit hash of the node's children's variants.
size_t NodeHash::hashChildren(SgNode *node)
{
    size_t children = node->get_numberOfTraversalSuccessors();
    size_t childVariants = 0;
    for (size_t i = 0; i < children; i++)
    {
        SgNode *n = node->get_traversalSuccessorByIndex(i);
        if (n != NULL)
        {
            size_t shiftedVariant = n->variantT();
         // Want to fill 14 bits with variant values, so shift every
         // other variant by 7. This is crude, but crude is good.
            if (i % 2 == 0)
                shiftedVariant <<= 7;
            childVariants ^= shiftedVariant;
        }
    }
 // Two bits for the number of children, 14 bits for child variants.
    childVariants = ((children & 0x04) << 14) | (childVariants & 0x3FFF);
    return childVariants;
}

// GB (2008-05-07): Introduced this function to handle the general pattern
// of hashing stuff that has a mangled name. If the mangled name is empty --
// which it actually never is, but just to be sure -- use the plain name
// instead. There is a large number of unrelated classes that must be
// handled this way, so we use a template.
template <class C>
size_t NodeHash::hashMangledName(C *c)
{
    std::string name = c->get_mangled_name();
    if (name.empty())
    {
        name = c->get_name();
#if BE_PICKY_ABOUT_EMPTY_MANGLED_NAMES
        std::cerr
            << "empty mangled name in: "
            << c->class_name() << " " << (void *) c
            << " name is: " << name << std::endl;
     // exit(EXIT_FAILURE);
#endif
    }
    return hashString(name);
}

size_t NodeHash::hashInitializedNamePtrList(SgInitializedNamePtrList &args)
{
    size_t lowerOrderBits = 0;
    SgInitializedNamePtrList::iterator i;
    SgInitializedNamePtrList::iterator end = args.end();
    for (i = args.begin(); i != end; ++i)
    {
        lowerOrderBits += hashMangledName(*i);
        lowerOrderBits %= 0xFFFF;
    }
    return lowerOrderBits;
}

size_t NodeHash::hashValueExp(SgValueExp *value)
{
    size_t lowerOrderBits = 0;

    switch (value->variantT()) {
 // Include the generated code.
#include "ValueHashCases"

    default:
     // can't happen
        std::cerr
            << "*** internal error: "
            << __FILE__ << ":" << __LINE__ << ": "
            << "encountered default case in switch with node of type "
            << value->class_name()
            << std::endl;
        std::abort();
    }

    return lowerOrderBits;
}

// This function handles is responsible for hashing various AST nodes in
// various special ways.
size_t NodeHash::hashVarious(const NodeInfo& node)
{
    size_t lowerOrderBits = 0;

    // TODO: this can probably use a lot of tweaking still; use the debug
    // mechanism to see what to improve
    switch ((node.first)->variantT()) {
    case V_SgInitializedName:
        lowerOrderBits = hashMangledName(isSgInitializedName(node.first));
        break;

    case V_SgVarRefExp:
        {
            SgInitializedName *decl = isSgVarRefExp(node.first)
                                          ->get_symbol()->get_declaration();
            lowerOrderBits = hashMangledName(decl);
        }
        break;

    case V_SgFunctionRefExp:
        {
            SgFunctionDeclaration *d = isSgFunctionRefExp(node.first)
                                           ->get_symbol()->get_declaration();
            lowerOrderBits = hashMangledName(d);
        }
        break;

    case V_SgMemberFunctionRefExp:
        {
            SgMemberFunctionDeclaration *d
                = isSgMemberFunctionRefExp(node.first)
                      ->get_symbol()->get_declaration();
            lowerOrderBits = hashMangledName(d);
        }
        break;

    default:
        lowerOrderBits = hashChildren(node.first);
        break;
    }

    return lowerOrderBits & 0xFFFF;
}

// All declaration statements have mangled names, but not all have names. We
// want to use the names in certain important cases, however; also, some
// declarations have lists of names to hash. Therefore, some cases get
// special handling.
size_t NodeHash::hashDeclarationStatement(SgDeclarationStatement *decl)
{
    size_t hash = 0;

    if (SgFunctionDeclaration *fd = isSgFunctionDeclaration(decl))
    {
     // This case includes member function declarations and template
     // instantiations. The exact variant will be added to this hash by the
     // calling function.
        hash = hashMangledName(fd);
    }
    else if (SgFunctionParameterList *fp = isSgFunctionParameterList(decl))
    {
        hash = hashInitializedNamePtrList(fp->get_args());
    }
    else if (SgCtorInitializerList *ci = isSgCtorInitializerList(decl))
    {
        hash = hashInitializedNamePtrList(ci->get_ctors());
    }
    else if (SgTypedefDeclaration *td = isSgTypedefDeclaration(decl))
    {
        hash = hashMangledName(td);
    }
    else if (SgVariableDeclaration *vd = isSgVariableDeclaration(decl))
    {
        hash = hashInitializedNamePtrList(vd->get_variables());
    }
    else
        hash = hashString(decl->get_mangled_name().str());

    return hash;
}

size_t NodeHash::operator()(const NodeInfo& node) const {
  // is this a good idea? any better solution?
  size_t variant = (size_t)(node.first)->variantT(); 

  // GB (2008-05-06): Toying around with including child info in the hash.
  // There are only a few hundred variants, and size_t should be at least
  // four bytes on any useful system, so we can shift the variant by 16 and
  // use the 16 lower-order bits for other stuff such as the number of
  // children and a hash of the children's variants.
  size_t lowerOrderBits = 0;

  // On Cxx_Grammar.C, the AST statistics mechanism reported these node
  // types as the most frequent: SgInitializedName, SgFunctionParameterList,
  // SgCtorInitializerList, SgMemberFunctionDeclaration,
  // SgTemplateInstantiationMemberFunctionDecl, SgTypedefDeclaration,
  // SgVariableDeclaration, SgVarRefExp. Not sure whether that's
  // entirely correct, but it's a starting point for optimization.

  // Declarations are an important special case that need some special
  // handling.
  if (SgDeclarationStatement *decl = isSgDeclarationStatement(node.first))
      lowerOrderBits = hashDeclarationStatement(decl);
  // Value expressions are also important.
  else if (SgValueExp *value = isSgValueExp(node.first))
      lowerOrderBits = hashValueExp(value);
  // Handle all types uniformly.
  else if (SgType *type = isSgType(node.first))
      lowerOrderBits = hashString(type->get_mangled().str());
  // In the otherwise, try the switch on various types.
  else
      lowerOrderBits = hashVarious(node);
  
  size_t hash = (variant << 16) | (lowerOrderBits & 0xFFFF);

  return hash;
}

// GB (2008-05-07): In what follows, a number of operations have been
// factored out to ridiculously tiny functions. This is useful for profiling
// (in case you were wondering).
bool NodeEqual::variantsEqual(const NodeInfo& s1, const NodeInfo& s2) {
  if ((s1.first)->variantT() == (s2.first)->variantT()) return true;
  else return false;
}

bool NodeEqual::childrenEqual(const NodeInfo& s1, const NodeInfo& s2) {
  if (s1.second == s2.second) return true;
  else return false;
}

// Analogously to hashMangledName, this template function compares two
// things of the same type, where we don't care about the type. The only
// requirement is that the type have get_mangled_name and get_name methods.
// Again, the mangled name should always suffice, but we have the check for
// the names just to be sure.
template <class C>
bool NodeEqual::compareNamedThings(C *i1, C *i2)
{
    std::string n1 = i1->get_mangled_name();
    std::string n2 = i2->get_mangled_name();
    if (!n1.empty() && !n2.empty())
        return n1 == n2;
    n1 = i1->get_name();
    n2 = i2->get_name();
#if BE_PICKY_ABOUT_EMPTY_MANGLED_NAMES
    std::cerr
        << "empty mangled name in: "
        << i1->class_name() << " " << (void *) i1
        << " name is: " << n1
        << std::endl
        << "or:                    "
        << i2->class_name() << " " << (void *) i2
        << " name is: " << n2
        << std::endl;
 // exit(EXIT_FAILURE);
#endif
    return n1 == n2;
}

bool NodeEqual::compareInitializedNamePtrList(SgInitializedNamePtrList &a1,
                                              SgInitializedNamePtrList &a2)
{
    if (a1.size() != a2.size())
        return false;
    SgInitializedNamePtrList::iterator i = a1.begin(), j = a2.begin();
    SgInitializedNamePtrList::iterator end = a1.end();
    while (i != end)
    {
        if (compareNamedThings(*i, *j) == false)
            return false;
        i++;
        j++;
    }
    return true;
}

// Handle various special cases concerning equality of AST nodes.
bool NodeEqual::compareVarious(const NodeInfo& s1, const NodeInfo& s2) {
  switch ((s1.first)->variantT()) {
    // for the time being, we use the mangled names to define equality
    // mangled names are different for every scope, however they are
    // the same for parameters of a given function.
    // e.g. when comparing two files containing the declaration/definition of
    // foo(int a, int b) and foo(int x, int y) respectively, a and x get
    // the same mangled name, just like b and y. in the declarations of
    // foo(bool a) and foo(int a), the parameters' mangled names are different
   

    // TODO: more such cases? casts?
    // maybe most cases should be changed to use compareNamedThings()

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
    {
        SgFunctionDeclaration *d1
            = isSgFunctionRefExp(s1.first)->get_symbol()->get_declaration();
        SgFunctionDeclaration *d2
            = isSgFunctionRefExp(s2.first)->get_symbol()->get_declaration();
        return compareNamedThings(d1, d2);
    }
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
    {
        SgInitializedName *i1 = isSgInitializedName(s1.first);
        SgInitializedName *i2 = isSgInitializedName(s2.first);
        return compareNamedThings(i1, i2);
    }
    break;

    // TODO: function pointers

  case V_SgFunctionParameterList:
    {
        SgInitializedNamePtrList &a1 = isSgFunctionParameterList(s1.first)->get_args();
        SgInitializedNamePtrList &a2 = isSgFunctionParameterList(s2.first)->get_args();
        return compareInitializedNamePtrList(a1, a2);
    }
    break;
    
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
    {
        SgInitializedName *i1 = isSgVarRefExp(s1.first)->get_symbol()->get_declaration();
        SgInitializedName *i2 = isSgVarRefExp(s2.first)->get_symbol()->get_declaration();
        return compareNamedThings(i1, i2);
    }
    break;

  case V_SgCtorInitializerList:
    {
        SgCtorInitializerList *l1 = isSgCtorInitializerList(s1.first);
        SgCtorInitializerList *l2 = isSgCtorInitializerList(s2.first);
        SgInitializedNamePtrList &a1 = l1->get_ctors();
        SgInitializedNamePtrList &a2 = l2->get_ctors();
        return compareInitializedNamePtrList(a1, a2);
    }
    break;

  case V_SgVariableDeclaration:
    {
        SgVariableDeclaration *d1 = isSgVariableDeclaration(s1.first);
        SgVariableDeclaration *d2 = isSgVariableDeclaration(s2.first);
        SgInitializedNamePtrList &a1 = d1->get_variables();
        SgInitializedNamePtrList &a2 = d2->get_variables();
        return compareInitializedNamePtrList(a1, a2);
    }
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

  if (SgFunctionDeclaration *fd1 = isSgFunctionDeclaration(s1.first)) {
      SgFunctionDeclaration *fd2 = isSgFunctionDeclaration(s2.first);
      return compareNamedThings(fd1, fd2);
  }
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

#if DEBUG_EQUALITY_TRAVERSAL
unsigned long frequentVariants[V_SgNumVariants];
#endif

bool NodeEqual::operator()(const NodeInfo& s1, const NodeInfo& s2) const {

  if (s1.first == s2.first) return true;
  if (!variantsEqual(s1, s2)) return false;
  if (!childrenEqual(s1, s2)) return false;

#if DEBUG_EQUALITY_TRAVERSAL
  frequentVariants[s1.first->variantT()]++;
#endif

  return compareVarious(s1, s2);
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

EqualityId EqualityTraversal::idForNode(const NodeInfo& node)
{
  // GB (2008-05-19): There are two implementations for this function, the
  // "old safe" one (the first one) and the "new fast" one. The new
  // implementation takes advantage of the fact that the hash map's []
  // operator returns a reference to a newly-added object if it can't find
  // the key. Thus if the reference we get from [] is 0, we know we are
  // adding a new object, otherwise we got an existing one. We don't need to
  // do two lookups if the object is not in the hash map.
  // This works IFF the plain old data type unsigned long (EqualityID) is
  // initialized to 0 by the hash map. It looks like it does that, but I'm
  // reserving some skepticism.

#if 0
  // return the id if it already exists in the hashtable
  SimpleNodeMap::iterator id = node_map.find(node);
  EqualityId ret;
  if (id != node_map.end())
    ret = (*id).second;
  else {
    // or generate a new entry in the hashtable
    EqualityId i = node_map.size()+1; // start at 1 because 0 is the "invalid id"
    node_map[node] = i;

    id_child_map[i] = node.second;

    ret = i;
  }
  return ret;

#else

  // return the id if it already exists in the hashtable
  EqualityId& ref = node_map[node];

  if (!ref) {
    // or generate a new entry in the hashtable

    // start at 1 (because 0 is the "invalid id")
    EqualityId i = node_map.size(); 
    ref = i;

    id_child_map[i] = node.second;
  }
  return ref;
#endif
}

#if DEBUG_EQUALITY_TRAVERSAL
void printMostFrequentVariants()
{
    unsigned long max = 0;

    std::cout
        << "most frequent nontrivial cases in NodeEqual::operator()"
        << std::endl
        << "(prime targets for better hash functions or comparisons):"
        << std::endl;
    for (int v = 0; v < V_SgNumVariants; v++)
    {
        if (frequentVariants[v] > max)
            max = frequentVariants[v];
    }

    for (int v = 0; v < V_SgNumVariants; v++)
    {
        if (frequentVariants[v] > max / 50)
        {
            std::cout
                << std::setw(10)
                << frequentVariants[v]
                << ' '
                << roseGlobalVariantNameList[v]
                << std::endl;
        }
    }

    std::cout
        << std::endl
        << "most frequent hash values for strings:"
        << std::endl;
    max = 0;
    for (int i = 0; i < 0xFFFF; i++)
    {
        if (stringHashes[i] > max)
            max = stringHashes[i];
    }

    for (int i = 0; i < 0xFFFF; i++)
    {
        if (stringHashes[i] > max / 2)
        {
            std::cout
                << std::setw(10)
                << i << " occurred "
                << stringHashes[i] << " times"
                << std::endl;
        }
    }

    std::cout << "more frequent int hash values:" << std::endl;
    for (int i = 0; i < 0xFFFF; i++)
    {
        if (intHashes[i] > 25)
        {
            std::cout.setf(std::ios::showbase);
            std::cout
                << std::setw(10)
                << std::hex
                << i << " occurred "
                << std::dec
                << intHashes[i] << " times"
                << std::endl;
        }
    }
}

unsigned long nodenum = 0;
struct timespec startTime, endTime;
#endif

void EqualityTraversal::atTraversalEnd() {
#if DEBUG_EQUALITY_TRAVERSAL
    printMostFrequentVariants();
#endif
}

EqualityId EqualityTraversal::evaluateSynthesizedAttribute
(SgNode* astNode, SynthesizedAttributesList synList) {

#if DEBUG_EQUALITY_TRAVERSAL
    if (nodenum == 0)
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);
    }
    nodenum++;
    if (nodenum % 50000 == 0)
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);
        long startTimeSec     = startTime.tv_sec;
        long startTimeNanoSec = startTime.tv_nsec;
        long   endTimeSec     = endTime.tv_sec  - startTimeSec;
        long   endTimeNanoSec = endTime.tv_nsec - startTimeNanoSec;
        double performance  = ((double) endTimeSec) + ((double)endTimeNanoSec) / 1000000000.0;
        std::cout << "visiting node " << nodenum
         // << " at " << (void *) astNode
         // << " of type " << astNode->class_name()
            << "; " << performance << " sec since last timestamp"
            << std::endl;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);
    }
#if 0
    if (nodenum == 450000)
    {
        printMostFrequentVariants();
        std::exit(EXIT_FAILURE);
    }
#endif
#endif

  // GB (2008-05-20): We need to get the types as well. As those are not
  // traversed automatically, we need to sort of fake a traversal by calling
  // evaluateSynthesizedAttribute on a type node (but not recursively!).
  // Do this for every expression, and eventually for every declaration that
  // has a type as well.
  if (SgExpression *exp = isSgExpression(astNode)) {
    SynthesizedAttributesList emptySynthesizedAttributes;
    evaluateSynthesizedAttribute(exp->get_type(), emptySynthesizedAttributes);
  }

  // pass-through constant propagation nodes
  // since constant propagation nodes only have one
  // child, we can return the child's id directly

  if(SgValueExp* valExp=isSgValueExp(astNode)) {
    if(valExp->get_originalExpressionTree()) {
      assert(synList.size()==1);
    return synList[0];
    }
  }

  // create the nodeinfo (SgNode + children id vector)
  NodeInfo node = NodeInfo(astNode, std::vector<EqualityId>());

  // generate a vector that contains only the interesting
  // ids of the children
  for (SynthesizedAttributesList::iterator i = synList.begin();
       i != synList.end(); i++) {
    if (*i > 0)
      node.second.push_back(*i);
  }

  // GB: Factored out the lookup code for better profiling.
  EqualityId ret = idForNode(node);

  // add the node to the node list map
  // GB (2008-06-06): If the node is a type, only add it if it isn't already
  // there! We will see the same type pointer many times, but we only want
  // to add it once.
  if (isSgType(astNode))
  {
   // These lists will be very short, so this search shouldn't be too
   // costly, I think.
      std::vector<SgNode *> &nodes = node_list_map[ret];
      if (std::find(nodes.begin(), nodes.end(), astNode) == nodes.end())
      {
          node_list_map[ret].push_back(astNode);
          node_id_map[astNode] = ret;
      }
  }
  else
  {
      node_list_map[ret].push_back(astNode);
      node_id_map[astNode] = ret;
  }

  return ret;
}

EqualityId EqualityTraversal::defaultSynthesizedAttribute () {
  return 0;
}


// interface functions

void EqualityTraversal::output_tables() {
  // <id>(<children>) = <nodes>

  for (size_t id = 1; id <= node_list_map.size(); id++) {
    std::cout << id << "( ";
    std::vector<EqualityId> *children = &id_child_map[id];
    std::vector<SgNode*> *nodes = &node_list_map[id];
    
    for (std::vector<EqualityId>::iterator j = children->begin();
         j != children->end(); j++) {
      std::cout << *j << " ";
    }
    std::cout << ") = ";
    
    for (std::vector<SgNode*>::iterator j = nodes->begin();
         j != nodes->end(); j++) {
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
  //            << node->sage_class_name() << ")" << std::endl;

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
  std::vector<EqualityId>* children = &id_child_map[id];
  bool comma = false;

  if (r != IdRepresentation || children->size() > 0)
    out << "(";

  // add the extra criteria (variable name, value..)
  if (r != IdRepresentation)
    comma = add_extra_criteria(out, node);

  // output all children
  for (std::vector<EqualityId>::iterator i = children->begin(); 
       i != children->end(); i++) { 
    
    if (comma)
      out << ", ";
    comma = true;

    get_node_repr_recursive(out, *i, r);
    
  }

  if (r != IdRepresentation || children->size() > 0)
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
