#include "sage3basic.h"
#include <numeric>
#include <Rose/Diagnostics.h>
#include "nameQualificationSupport.h"

#include "sageInterfaceAda.h"
#include "sageGeneric.h"

using namespace std;

// DQ (3/24/2016): Adding Robb's message logging mechanism to control output debug message from the EDG/ROSE connection code.
using namespace Rose::Diagnostics;

namespace si = SageInterface;

// This value must be greater than 3 to cause most output to be generated.
#define DEBUG_NAME_QUALIFICATION_LEVEL 0

#ifndef WARNING_FOR_NONREAL_DEVEL
#  define WARNING_FOR_NONREAL_DEVEL 0
#endif

// DQ (9/2/2020): Moved to the top of the file from the SgInitializedName case in the evaluate inherited attribute function.
// DQ (4/27/2019): Set these to be the same for now.
#define DEBUG_INITIALIZED_NAME 0
// #define DEBUG_INITIALIZED_NAME DEBUG_NAME_QUALIFICATION_LEVEL

// DQ (10/17/2020): Worked with Tristan, but this was the only way that I could turn on the debug output for my tool in ROSE_GARDEN.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
#define mfprintf(a) printf
#endif

#define DEBUG_NONTERMINATION 0


// support for Ada
namespace
{
#if FOR_DEBUG_ONLY
  // auxiliary wrapper for printing Sg_File_Info objects
  struct SrcLoc
  {
    explicit
    SrcLoc(const SgLocatedNode& n)
    : info(n.get_file_info())
    {}

    const Sg_File_Info* info;
  };

  std::ostream& operator<<(std::ostream& os, SrcLoc el)
  {
    return os << el.info->get_filenameString()
              << "@" << el.info->get_line() << ":" << el.info->get_col();
  }
#endif /* FOR_DEBUG_ONLY */


#if FOR_DEBUG_ONLY
  inline
  const SgExprListExp* callArguments(const SgFunctionRefExp& n)
  {
    if (const SgCallExpression* callexp = isSgCallExpression(n.get_parent()))
      return callexp->get_args();

    return nullptr;
  }
#endif /* FOR_DEBUG_ONLY */


  /// Track visible renaming and using declarations
  struct AdaScopeStackEntry
  {
    explicit
    AdaScopeStackEntry(const SgScopeStatement* scope = nullptr)
    : currentScope(scope), addedRenamings(), addedUsedScopes()
    {}

    //~ SgUnparse_Info*                      unparseInfo = nullptr;
    const SgScopeStatement*              currentScope = nullptr;
    std::vector<const SgScopeStatement*> addedRenamings;
    std::vector<const SgScopeStatement*> addedUsedScopes;
    std::vector<const SgScopeStatement*> addedVisibleScopes;
  };

  struct ScopeDetails : std::tuple<std::string, bool>
  {
    using base = std::tuple<std::string, bool>;
    using base::base;

    std::string const& name()        const { return std::get<0>(*this); }
    bool               qualBarrier() const { return std::get<1>(*this); }
    // \todo consider renaming qualBarrier to compilerGenerated and set for all nodes
  };


  ScopeDetails
  scopeName(const SgStatement* n);

  struct ScopeName : sg::DispatchHandler<ScopeDetails>
  {
      // records the scope name, and if the scope is a qualification barrier.
      // \note a qualification barrier is an unnamed block; as it does not
      //       allow its elements being fully prefixed from global to inner scope.
      void withName(const std::string& name, bool actsAsBarrier = false);

      // a scope whose name does not appear in the scope qualification list.
      //   this does not introduce a qualification barrier.
      void withoutName() {}

      void checkParent(const SgScopeStatement& n);

      void handle(const SgNode& n)                 { SG_UNEXPECTED_NODE(n); }

      // default for all scopes and declarations
      void handle(const SgStatement&)              { withoutName(); }

      // void handle(const SgGlobal&)               { withName("Standard"); } // \todo

      // scopes that may have names
      // \todo do we also need named loops?
      void handle(const SgAdaTaskSpec& n)          { checkParent(n); }
      void handle(const SgAdaTaskBody& n)          { checkParent(n); }
      void handle(const SgAdaProtectedSpec& n)     { checkParent(n); }
      void handle(const SgAdaProtectedBody& n)     { checkParent(n); }
      void handle(const SgAdaPackageBody& n)       { checkParent(n); }
      void handle(const SgAdaPackageSpec& n)       { checkParent(n); }
      void handle(const SgFunctionDefinition& n)   { checkParent(n); }

      // generics and discriminated declarations do not have names per se.
      //   Instead they provide binding-context to some other declaration.
      //   e.g., package, subroutine (generics); record, task, protected object, .. (discriminated types)

      // parent handlers
      void handle(const SgDeclarationStatement&)   { withoutName(); }

      void handle(const SgAdaTaskSpecDecl& n)      { withName(n.get_name()); }
      void handle(const SgAdaTaskBodyDecl& n)      { withName(n.get_name()); }
      void handle(const SgAdaProtectedSpecDecl& n) { withName(n.get_name()); }
      void handle(const SgAdaProtectedBodyDecl& n) { withName(n.get_name()); }
      void handle(const SgAdaPackageSpecDecl& n)   { withName(n.get_name()); }
      void handle(const SgAdaPackageBodyDecl& n)   { withName(n.get_name()); }
      void handle(const SgAdaRenamingDecl& n)      { withName(n.get_name()); }
      void handle(const SgFunctionDeclaration& n)  { withName(n.get_name()); }

      void handle(const SgBasicBlock& n)
      {
        const std::string blockName     = n.get_string_label();
        const bool        qualBarrier   = blockName.empty() && si::Ada::blockExistsInSource(n);

        withName(blockName, qualBarrier);
      }
  };

  void ScopeName::withName(const std::string& s, bool actsAsBarrier)
  {
    res = {s, actsAsBarrier};
  }

  void ScopeName::checkParent(const SgScopeStatement& n)
  {
    // get the name
    res = sg::dispatch(ScopeName{}, n.get_parent());
  }

  ScopeDetails
  scopeName(const SgStatement* n)
  {
    if (n == nullptr) return {};

    return sg::dispatch(ScopeName{}, n);
  }


  struct NameQualificationInheritedAttributeAda : NameQualificationInheritedAttribute
  {
      using SkipNameQualificationSet = std::set<const SgNode*>;

      using base = NameQualificationInheritedAttribute;

      NameQualificationInheritedAttributeAda() = default;
      NameQualificationInheritedAttributeAda(const NameQualificationInheritedAttributeAda&) = default;
      NameQualificationInheritedAttributeAda(NameQualificationInheritedAttributeAda&&)      = default;

      NameQualificationInheritedAttributeAda&
      operator=(const NameQualificationInheritedAttributeAda& that)
      {
        static_cast<base&>(*this) = that;
        this->skipNameQualification = that.skipNameQualification;
        this->typeMode              = that.typeMode;

        return *this;
      }

      NameQualificationInheritedAttributeAda&
      operator=(NameQualificationInheritedAttributeAda&&) = default;


      /// returns a reference to the set of nodes that does not require name qualification
      SkipNameQualificationSet& get_nameQualIgnoreSet() { return skipNameQualification; }

      /// fixes the reference node for type sub trees
      void set_typeReferenceNode(const SgNode& referenceNode)
      {
        if (typeMode) return;

        typeMode = true;
        base::set_referenceNode(const_cast<SgNode*>(&referenceNode));
      }

      bool is_typeMode() const { return typeMode; }

    private:
      SkipNameQualificationSet skipNameQualification;
      bool                     typeMode      = false;
  };

  struct NameQualificationTraversalState
  {
      // new types to keep track of actual scopes
      using VisibleScopeContainer  = std::set<const SgScopeStatement*>;
      using UsePkgContainer        = std::set<const SgScopeStatement*>;
      using ScopeRenamingContainer = std::map<const SgScopeStatement*, const SgDeclarationStatement*>;

      VisibleScopeContainer  visibleScopes;         ///< fully-qualified names of visible scopes
      UsePkgContainer        useScopes;             ///< fully-qualified names of used scopes
                                                    ///  (i.e., use the.used.package;)

      ScopeRenamingContainer renamedScopes;         ///< renamed scopes
      std::vector<AdaScopeStackEntry> scopeState;   ///< stores info about scope state
  };


  // PP (04/05/24): just keep track of mappings that are actually used in Ada.
  // \note from the namequalification mappings in ROSE
  //       the Ada name-qualifier only uses two:
  //       * SgNode::get_globalQualifiedNameMapForNames()
  //       * SgNode::get_globalQualifiedNameMapForMapsOfTypes()
  struct NameQualificationTraversalAda :
            AstTopDownBottomUpProcessing< NameQualificationInheritedAttributeAda,
                                          NameQualificationSynthesizedAttribute
                                        >
  {
   // DQ (8/1/2025): Adding support for typedef to this map so that we can later change it to an unordered_map.
      typedef std::unordered_map<SgNode*,std::string> NameQualificationMapType;
      typedef std::map<SgNode*,NameQualificationMapType> NameQualificationMapOfMapsType;
#if 0
      typedef std::set<SgNode*> NameQualificationSetType;
#else
   // This must match the setting in the NameQualificationTraversal class.
   // typedef std::unordered_set<SgNode*> NameQualificationSetType;
      typedef NameQualificationTraversal::NameQualificationSetType NameQualificationSetType;
#endif

      using InheritedAttribute   = NameQualificationInheritedAttributeAda;
      using SynthesizedAttribute = NameQualificationSynthesizedAttribute;
      using base = AstTopDownBottomUpProcessing<InheritedAttribute, SynthesizedAttribute>;
      using SynthesizedAttributesList = base::SynthesizedAttributesList;

      using VisibleScopeContainer  = NameQualificationTraversalState::VisibleScopeContainer;
      using UsePkgContainer        = NameQualificationTraversalState::UsePkgContainer;
      using ScopeRenamingContainer = NameQualificationTraversalState::ScopeRenamingContainer;

      /// constructor for primary traversal
      // NameQualificationTraversalAda( std::map<SgNode*,std::string> & input_qualifiedNameMapForNames,
      //                                std::map<SgNode*,std::map<SgNode*,std::string> > & input_qualifiedNameMapForMapsOfTypes,
      //                                NameQualificationTraversalState& traversalState
      //                              )
         NameQualificationTraversalAda( NameQualificationMapType & input_qualifiedNameMapForNames,
                                     NameQualificationMapOfMapsType & input_qualifiedNameMapForMapsOfTypes,
                                     NameQualificationTraversalState& traversalState
                                   )
      : base(),
        qualifiedNameMapForNames(input_qualifiedNameMapForNames),
        qualifiedNameMapForMapsOfTypes(input_qualifiedNameMapForMapsOfTypes),
        state(traversalState)
      {}

      /// constructor for sub traversal
      /// \details
      ///   like a copy constructor except that it allows to pass in the name map
      // NameQualificationTraversalAda( const NameQualificationTraversalAda& orig,
      //                                std::map<SgNode*,std::string>& input_qualifiedNameMapForNames
      //                              )
      NameQualificationTraversalAda( const NameQualificationTraversalAda& orig,
                                     NameQualificationMapType& input_qualifiedNameMapForNames
                                   )
      : base(orig),
        qualifiedNameMapForNames(input_qualifiedNameMapForNames),
        qualifiedNameMapForMapsOfTypes(orig.qualifiedNameMapForMapsOfTypes),
        state(orig.state)
      {}


      InheritedAttribute
      evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute);

      SynthesizedAttribute
      evaluateSynthesizedAttribute( SgNode* n,
                                    InheritedAttribute ihAttr,
                                    SynthesizedAttributesList synAttrlst
                                  );

      /// keeps track of visible scopes, for which name qualification can be elided
      /// \{
      void addVisibleScope(const SgScopeStatement*);
      bool isVisibleScope(const SgScopeStatement*) const;
      /// \}

      /// keeps track of scopes injected through use declarations
      /// \{
      void addUsedScope(const SgScopeStatement*);
      bool isUsedScope(const SgScopeStatement*) const;
      /// \}

      /// keeps track of renamed scopes
      /// \{
      void addRenamedScope(ScopeRenamingContainer::key_type, ScopeRenamingContainer::mapped_type);
      ScopeRenamingContainer::mapped_type renamedScope(ScopeRenamingContainer::key_type) const;
      /// \}

      /// scope stack, used-scope, and renamed decl tracking
      /// \{
      void openScope(const SgScopeStatement& scope);
      void closeScope();
      /// \}

      /// records that a node \ref n needs to be qualified with \ref qual.
      void recordNameQual(const SgNode& n, std::string qual);
      void recordNameQual(NameQualificationMapType& m, const SgNode& n, std::string qual);

      /// Constructs a path from a scope statement to the top-level (global)
      /// scope.
      /// \param n innermost scope
      si::Ada::ScopePath pathToGlobal(const SgScopeStatement& n) const;

      /// Generates a string for the scope path [beg, lim) while considering visible and renamed scopes
      std::string nameQualString(si::Ada::ScopePath::const_reverse_iterator beg, si::Ada::ScopePath::const_reverse_iterator lim) const;

      /// computes the name qualification for a reference in scope \ref local
      ///   to a declaration \ref node in scope \ref remote.
      std::string computeNameQual(const SgNode& node, const SgScopeStatement& local, const SgScopeStatement& remote) const;

      /// creates a type mapping that is used for the type-subtree with reference node \ref n.
      /// Note, the subtree is NOT created iff already in type traversal mode. In this case,
      ///   the current mapping is returned.
      // std::map<SgNode*,std::string>& createQualMapForTypeSubtreeIfNeeded(const SgNode& n);
      NameQualificationMapType& createQualMapForTypeSubtreeIfNeeded(const SgNode& n);

      // PP (04/05/24): referencedNameSet currently not in use by Ada
      //~ /// returns referencedNameSet
      //~ std::set<SgNode*>& get_referencedNameSet() { return referencedNameSet; }

      /// returns current qualifiedNameMapForNames
      // std::map<SgNode*,std::string>& get_qualifiedNameMapForNames() { return qualifiedNameMapForNames; }
      NameQualificationMapType& get_qualifiedNameMapForNames() { return qualifiedNameMapForNames; }

      /// called after a unit change in global scope is detected.
      void resetNameQualificationContext();

      /// saves the namequalfication context at the end of \ref n's processing.
      void saveNameQualificationContext(const SgDeclarationStatement& n);

      /// loads the namequalfication context saved for \ref n's processing.
      /// \details
      ///    called, at the beginning of a "child" decl
      ///    (for example called for the spec decl at the beginning of a body decl).
      void loadNameQualificationContext(const SgDeclarationStatement& n);

    private:
      using NameQualContext        = std::tuple<VisibleScopeContainer, UsePkgContainer, ScopeRenamingContainer>;

      //
      // data members

      // references to ROSE wide maps
      // \{
      // PP (04/05/24): referencedNameSet currently not in use by Ada
      //~ std::set<SgNode*>&                                referencedNameSet;
      // std::map<SgNode*,std::string>&                    qualifiedNameMapForNames;
      NameQualificationMapType&                    qualifiedNameMapForNames;

      // std::map<SgNode*,std::string>&                    qualifiedNameMapForTypes;
      //~ std::map<SgNode*,std::string> & qualifiedNameMapForTemplateHeaders;
      //~ std::map<SgNode*,std::string>&                    typeNameMap;
      // std::map<SgNode*,std::map<SgNode*,std::string> >& qualifiedNameMapForMapsOfTypes;
      // std::map<SgNode*,NameQualificationMapType>& qualifiedNameMapForMapsOfTypes;
      NameQualificationMapOfMapsType& qualifiedNameMapForMapsOfTypes;
      // \}

      NameQualificationTraversalState& state; ///< traversal state

      std::map<const SgDeclarationStatement*, NameQualContext> nameQualContextMap = {}; ///< stores contexts of specs and bodies

      // deleted constructors and operators
      NameQualificationTraversalAda() = delete;
      NameQualificationTraversalAda(const NameQualificationTraversalAda&) = delete;
      NameQualificationTraversalAda(NameQualificationTraversalAda&&) = delete;
      NameQualificationTraversalAda& operator=(const NameQualificationTraversalAda&) = delete;
      NameQualificationTraversalAda& operator=(NameQualificationTraversalAda&&) = delete;
  };

  void
  NameQualificationTraversalAda::recordNameQual( NameQualificationMapType& m,
                                                 const SgNode& n,
                                                 std::string qual
                                               )
  {
    m.emplace(const_cast<SgNode*>(&n), std::move(qual));
  }


  void
  NameQualificationTraversalAda::recordNameQual(const SgNode& n, std::string qual)
  {
    //~ std::cerr << "record " << qual << "@" << &n << " %" << typeid(n).name()
              //~ << " / " << &qualifiedNameMapForNames
              //~ << std::endl;
    recordNameQual(qualifiedNameMapForNames, n, std::move(qual));
  }

  /// Constructs a path from a scope statement to the top-level (global)
  /// scope.
  /// \param n innermost scope
  si::Ada::ScopePath
  NameQualificationTraversalAda::pathToGlobal(const SgScopeStatement& n) const
  {
    si::Ada::ScopePath res = si::Ada::pathToGlobal(n);

    // Add the fictitious (?) root of all scopes ;)
    //   A procedure referring to itself needs to add the standard prefix "Standard."
    //   iff its name is overloaded within the function.
    //   If Standard is not required, the namequal will cut it out as part
    //   of the common path elimination (NameQualificationTraversalAda::computeNameQual 1b).
    // \code
    //   proc X (y:integer) is
    //     x : integer := y;
    //   begin
    //     Standard.X(x);
    //   end X;
    // \endcode
    res.push_back(si::Ada::pkgStandardScope());
    return res;
  }


  bool symbolMatchesDeclaration(const SgSymbol& sym, const SgNode& dcl)
  {
    const SgDeclarationStatement* symdcl = si::Ada::associatedDeclaration(sym);

    if (!symdcl) return false;
    if (symdcl == &dcl) return true;

    // for Ada language features check that spec and body refer to the same
    //   element.
    if (const SgAdaPackageBodyDecl* bdydcl = isSgAdaPackageBodyDecl(&dcl))
      return symdcl == si::Ada::getSpecificationDeclaration(bdydcl);

    if (const SgAdaPackageBodyDecl* bdydcl = isSgAdaPackageBodyDecl(symdcl))
      return &dcl == si::Ada::getSpecificationDeclaration(bdydcl);

    if (const SgAdaTaskBodyDecl* bdydcl = isSgAdaTaskBodyDecl(&dcl))
      return symdcl == si::Ada::getSpecificationDeclaration(bdydcl);

    if (const SgAdaTaskBodyDecl* bdydcl = isSgAdaTaskBodyDecl(symdcl))
      return &dcl == si::Ada::getSpecificationDeclaration(bdydcl);

    if (const SgAdaProtectedBodyDecl* bdydcl = isSgAdaProtectedBodyDecl(&dcl))
      return symdcl == si::Ada::getSpecificationDeclaration(bdydcl);

    if (const SgAdaProtectedBodyDecl* bdydcl = isSgAdaProtectedBodyDecl(symdcl))
      return &dcl == si::Ada::getSpecificationDeclaration(bdydcl);

    // for functions check that they have the same first nondefining declaration.
    if (const SgFunctionDeclaration* fndcl = isSgFunctionDeclaration(&dcl))
      if (const SgFunctionDeclaration* fnsydcl = isSgFunctionDeclaration(symdcl))
        return fndcl->get_firstNondefiningDeclaration() == fnsydcl->get_firstNondefiningDeclaration();

    return false;
  }

  struct NodeName : sg::DispatchHandler<std::string>
  {
    void handle(const SgNode& n)                 { SG_UNEXPECTED_NODE(n); }

    // declarations (base case)
    void handle(const SgDeclarationStatement& n) { res = si::get_name(&n); }
    //~ void handle(const SgInitializedName& n)      { res = si::get_name(&n); }

    // ref expressions
    void handle(const SgVarRefExp& n)            { res = si::Ada::nameOf(n); }
    void handle(const SgFunctionRefExp& n)       { res = si::Ada::nameOf(n); }
    void handle(const SgAdaUnitRefExp& n)        { res = si::Ada::nameOf(n); }
    void handle(const SgAdaProtectedRefExp& n)   { res = si::Ada::nameOf(n); }
    void handle(const SgAdaTaskRefExp& n)        { res = si::Ada::nameOf(n); }
    void handle(const SgAdaRenamingRefExp& n)    { res = si::Ada::nameOf(n); }
    void handle(const SgLabelRefExp& n)          { res = si::Ada::nameOf(n); }

    // scope statements
    void handle(const SgBasicBlock& n)           { res = n.get_string_label(); }
    void handle(const SgGotoStatement&)          { /* empty string -- can this still be reached? */ }

    // other
    void handle(const SgEnumVal& n)              { res = si::Ada::nameOf(n); }
    void handle(const SgBaseClass& n)            { res = SG_DEREF(n.get_base_class()).get_name(); }

    static const std::string AN_UNREAL_NAME;
  };

  const std::string NodeName::AN_UNREAL_NAME = "@@This#Is$An%Unreal^Name@@";

  struct DebugSeqPrinter
  {
    const si::Ada::ScopePath& el;
  };

  std::ostream& operator<<(std::ostream& os, const DebugSeqPrinter& s)
  {
    for (const SgScopeStatement* scope : s.el)
      os << ", " << typeid(*scope).name()
         << " (" << scope << ")";

    return os;
  }

  static bool DBG_PRINT_SCOPES = false;

  /// gets the name of the node
  std::string nodeName(const SgNode& n)
  {
    std::string res = sg::dispatch(NodeName{}, &n);

    // PP (10/24/22): To avoid false positives when unnamed variables
    //                are declared along the scope path (i.e., in a catch statement),
    //                a 'special' name is returned for unnamed scopes.
    // \todo fix the underlying cause of false positive reporting..
    if (res.empty()) res = NodeName::AN_UNREAL_NAME;
    return res;
  }

  bool isShadowedAlongPath( const SgNode& n,
                            si::Ada::ScopePath::const_reverse_iterator beg, si::Ada::ScopePath::const_reverse_iterator lim
                          )
  {
    std::string                   dclname = nodeName(n);
    const SgDeclarationStatement* dcl = isSgDeclarationStatement(&n);
    const SgScopeStatement*       dclscope = dcl ? dcl->get_scope() : nullptr;

    auto pred = [&dclname, &n, dclscope](const SgScopeStatement* scope)->bool
                {
                  const SgSymbol* sym = scope->lookup_symbol(dclname, nullptr, nullptr);
                  bool            shadowed = (sym != nullptr);

                  while (sym && shadowed)
                  {
                    shadowed = !symbolMatchesDeclaration(*sym, n);
                    sym = scope->next_any_symbol();
                  }

                  // for Ada bodies check their dual spec
                  if (shadowed)
                  {
                    if (const SgAdaPackageBody* bdy = isSgAdaPackageBody(scope))
                      shadowed = (dclscope != si::Ada::getSpecificationDefinition(bdy));
                    else if (const SgAdaProtectedBody* bdy = isSgAdaProtectedBody(scope))
                      shadowed = (dclscope != bdy->get_spec());
                    else if (const SgAdaTaskBody* bdy = isSgAdaTaskBody(scope))
                      shadowed = (dclscope != bdy->get_spec());
                  }


                  if (DBG_PRINT_SCOPES)
                  {
                    std::cerr << "+++    " << typeid(*scope).name() << " " << shadowed
                              << std::endl;

                    // const_cast<SgScopeStatement*>(scope)->print_symboltable(dclname, std::cerr);
                  }


                  // if a symbol with the same name exists in the scope
                  //   but the original declaration does not, then
                  //   the declaration is shadowed.

                  return shadowed;
                };

    bool const res = std::any_of(beg, lim, pred);

    if (DBG_PRINT_SCOPES)
      std::cerr << "+++++" << dclname << ": " << &n << " " << typeid(n).name()
                << ": " << std::distance(beg, lim)
                << ": " << res
                << std::endl;

    return res;
  }

  std::string
  NameQualificationTraversalAda::nameQualString( si::Ada::ScopePath::const_reverse_iterator beg,
                                                 si::Ada::ScopePath::const_reverse_iterator lim
                                               ) const
  {
    const NameQualificationTraversalAda* self = this;

    auto scopeFn = [self](std::string qual, const SgScopeStatement* scope)->std::string
                   {
                     //~ std::cerr << "nqs: " << qual << " + " << typeid(*scope).name() << std::flush;

                     const SgStatement* ref = nullptr;

                     // if the scope is visible, produce a fully qualified scope
                     if (self->isVisibleScope(scope))
                     {
                       ref = scope;
                     }
                     else if (const SgDeclarationStatement* alt = self->renamedScope(scope))
                     {
                       // use a renamed alternative for scope, and reset the path
                       //~ std::cerr << " *use renamed* ";
                       ref = alt;
                       qual.clear();
                     }
                     else
                     {
                       // use full path as fallback
                       ref = scope;
                     }

                     ScopeDetails scopedet = scopeName(ref);

                     if (scopedet.qualBarrier())
                     {
                       // names cannot be qualified across unnamed
                       //   blocks.
                       ROSE_ASSERT(scopedet.name().empty());
                       qual.clear();
                     }
                     else if (!scopedet.name().empty())
                     {
                       qual += scopedet.name();
                       //~ qual += "(";
                       //~ qual += typeid(*ref).name();
                       //~ qual += ")";
                       qual += '.';
                     }

                     //~ std::cerr << " -> " << qual << std::endl;
                     return qual;
                   };

    return std::accumulate(beg, lim, std::string{}, scopeFn);
  }

  // \todo can this be combined with scopeName/ScopeName ?
  struct NamedAstNode : sg::DispatchHandler<const SgNode*>
  {
    void setDecl(const SgNode* node)
    {
      ASSERT_not_null(isSgDeclarationStatement(node));

      res = node;
    }

    void handle(const SgNode& n)                   { SG_UNEXPECTED_NODE(n); }

    void handle(const SgAdaProtectedSpec& n)       { setDecl(n.get_parent()); }
    void handle(const SgAdaProtectedBody& n)       { setDecl(n.get_parent()); }
    void handle(const SgAdaTaskSpec& n)            { setDecl(n.get_parent()); }
    void handle(const SgAdaTaskBody& n)            { setDecl(n.get_parent()); }
    void handle(const SgAdaPackageSpec& n)         { setDecl(n.get_parent()); }
    void handle(const SgAdaPackageBody& n)         { setDecl(n.get_parent()); }
    void handle(const SgAdaGenericDefn& n)         { setDecl(n.get_parent()); }
    void handle(const SgFunctionDefinition& n)     { setDecl(n.get_parent()); }
    void handle(const SgDeclarationScope& n)       { setDecl(n.get_parent()); }
    void handle(const SgFunctionParameterScope& n) { setDecl(n.get_parent()); }
    void handle(const SgClassDefinition& n)        { setDecl(n.get_parent()); }

    void handle(const SgBasicBlock& n)             { res = &n; }

    // what to do with others?
    void handle(const SgScopeStatement&)           { /* res = nullptr; */ }
  };

  const SgNode*
  namedAstNode(const SgScopeStatement* scope)
  {
    return sg::dispatch(NamedAstNode{}, scope);
  }

  std::tuple<bool, const SgNode*>
  usableScope(const SgScopeStatement& scope)
  {
    const SgNode* namedNode = namedAstNode(&scope);
    std::string   scopeName = namedNode ? nodeName(SG_DEREF(namedNode)) : std::string{};

    if ((scopeName == "") || (scopeName == NodeName::AN_UNREAL_NAME))
      return { false, nullptr };

    return { true, namedNode };
  }

  template <class ScopePathIterator>
  std::tuple<ScopePathIterator, const SgNode*>
  namedAncestorScope(ScopePathIterator beg, ScopePathIterator pos, const SgNode* refNode)
  {
    if (beg == pos)
      return { pos, refNode };

    ScopePathIterator               prvpos = std::prev(pos);
    std::tuple<bool, const SgNode*> usable = usableScope(**prvpos);

    if (std::get<0>(usable))
      return { prvpos, std::get<1>(usable) };

    return namedAncestorScope(beg, prvpos, refNode);
  }

  template <class ScopePathIterator>
  ScopePathIterator
  extendNameQualUntilUnambiguous( ScopePathIterator remBeg,
                                  ScopePathIterator remMin,
                                  ScopePathIterator remLim,
                                  ScopePathIterator locMin,
                                  ScopePathIterator locLim,
                                  const SgNode& declOrRef
                                )
  {
    const SgNode* refNode = &declOrRef;

    // set the reference Node to the leading scope name (remMin != remLim)
    //   or start with the actual declOrRef node otherwise.
    if (remMin != remLim)
      if (const SgNode* namedNode = namedAstNode(*remMin))
        refNode = namedNode;
      // else ROSE_ABORT(); // \todo do we always have to have a node?

    // while the refnode is aliased along [locMin, locLim) and the scope is extensible |remBeg,remMin| > 0
    //   extend the scope by one.
    while ((std::distance(remBeg, remMin) > 0) && isShadowedAlongPath(*refNode, locMin, locLim))
    {
      std::tie(remMin, refNode) = namedAncestorScope(remBeg, remMin, refNode);
    }

    return remMin;
  }


  template <class ScopePathIterator>
  ScopePathIterator
  trimFrontForUseDirectives( ScopePathIterator /*remBeg*/,
                             ScopePathIterator remMin,
                             ScopePathIterator remLim,
                             const NameQualificationTraversalAda& /*trav*/
                           )
  {
    // right now only handle package Standard ..
    // \todo check if any scope in (remMin, remLim)
    //         is in trav.isUsedScope

    // scopepath cannot be shortened?
    if (remMin == remLim)
      return remMin;

    // shorten path if package Standard
    if (*remMin == si::Ada::pkgStandardScope())
      return std::next(remMin);

    return remMin;
  }

  template <class ScopePathIterator>
  ScopePathIterator
  trimBackForPackageExtensions( ScopePathIterator /*remBeg*/,
                                ScopePathIterator remMin,
                                ScopePathIterator remLim,
                                const NameQualificationTraversalAda& /*trav*/
                              )
  {
    static const std::string PKG_AUX_DEC{"aux_dec"};
    static const std::string PKG_SYSTEM{"system"};

    // right now handles only System.Aux_DEC
    // \todo consider tracking Extend_System pragma and shorten for
    //       all extensions.

    // scopepath cannot be shortened?
    if (remMin == remLim)
      return remLim;

    ScopePathIterator const limCand = std::prev(remLim);
    const bool              systemAuxDec = (  (std::distance(remMin, remLim) == 2)
                                           && (boost::iequals(scopeName(*limCand).name(), PKG_AUX_DEC))
                                           && (boost::iequals(scopeName(*std::prev(limCand)).name(), PKG_SYSTEM))
                                           );

    // shorten path is package System.Aux_Dec
    return systemAuxDec ? limCand : remLim;
  }

  auto
  areSpecAndBody(const SgScopeStatement* lhs, const SgScopeStatement* rhs) -> bool
  {
    if (const SgAdaProtectedSpec* spec = isSgAdaProtectedSpec(lhs))
      return spec->get_body() == rhs;

    if (const SgAdaTaskSpec* spec = isSgAdaTaskSpec(lhs))
      return spec->get_body() == rhs;

    if (const SgAdaPackageSpec* spec = isSgAdaPackageSpec(lhs))
      return si::Ada::getBodyDefinition(spec) == rhs;

    return false;
  }


  /// adaPathMismatch finds the first difference in two scope paths
  ///   [locPos, locLim) and [remPos, remLim)
  ///   and returns a pair of iterators indicating those positions.
  /// \note
  ///   similar to std::mismatch, but pathMismatch takes into account
  ///   that scope paths in Ada may have two consecutive entries for the
  ///   the same scope (e.g., PackageSpec, PackageBody).
  template <class ScopePathIterator>
  auto
  adaPathMismatch( ScopePathIterator lhsPos, ScopePathIterator lhsLim,
                   ScopePathIterator rhsPos, ScopePathIterator rhsLim
                 )
    -> std::pair<ScopePathIterator, ScopePathIterator>
  {
    while ((lhsPos != lhsLim) && (rhsPos != rhsLim) && (*lhsPos == *rhsPos))
    {
      if ((lhsPos != lhsLim) && areSpecAndBody(*lhsPos, *(lhsPos+1)))
        ++lhsPos;

      if ((rhsPos != rhsLim) && areSpecAndBody(*rhsPos, *(rhsPos+1)))
        ++rhsPos;

      ++lhsPos; ++rhsPos;
    }

    return {lhsPos, rhsPos};
  }

  std::string
  NameQualificationTraversalAda::computeNameQual( const SgNode& quasiDecl,
                                                  const SgScopeStatement& local,
                                                  const SgScopeStatement& remote
                                                ) const
  {
    using PathIterator = si::Ada::ScopePath::reverse_iterator;

    si::Ada::ScopePath remotePath = pathToGlobal(remote);
    si::Ada::ScopePath localPath  = pathToGlobal(local);
    ASSERT_require(remotePath.size() != 0);

    if (DBG_PRINT_SCOPES)
      std::cerr << "rp = " << DebugSeqPrinter{remotePath}
                << "   remote-scope = " << typeid(remote).name() << " " << &remote
                << std::endl;

    // compute the required scope qualification
    //   assume a decl declared in scope a.b.c
    //   and referenced in scope a.d.e:

    // 1a determine the first mismatch (mismPos) of the (reversed) scope paths "a.b.c" and "a.d.e"
    PathIterator const locBeg  = localPath.rbegin();
    PathIterator const locLim  = localPath.rend();
    PathIterator const remBeg  = remotePath.rbegin();
    PathIterator       remLim  = remotePath.rend();

    // 1b mismPos is  "a|b.c and a|d.e", thus the required scope qualification is b.c
    auto               mismPos = adaPathMismatch(locBeg, locLim, remBeg, remLim);

    // 1c trim b.c if any scope has a use-directive
    mismPos.second = trimFrontForUseDirectives(remBeg, mismPos.second, remLim, *this);

    // 1d shorten b.c if any scope has a use-directive
    remLim = trimBackForPackageExtensions(remBeg, mismPos.second, remLim, *this);

    // 2 extend the path if an overload for front(b.c) exists somewhere in d.e
    const PathIterator remPos  = extendNameQualUntilUnambiguous( remBeg, mismPos.second, remLim,
                                                                 mismPos.first, locLim,
                                                                 quasiDecl
                                                               );

    // 3 Since a body has its spec as the logical ancestor scope, adjacent spec/body combination
    //   are filtered from the path.
    const PathIterator remLmt = std::unique(remPos, remLim, areSpecAndBody);

    // turn the scope sequence into a name qualification string
    const std::string res = nameQualString(remPos, remLmt);

    if (DBG_PRINT_SCOPES)
    {
      std::cerr << "---  " << typeid(quasiDecl).name()
                << " len> " << std::distance(mismPos.first, locLim)
                << "/" << localPath.size() << DebugSeqPrinter{localPath}
                << "/" << nameQualString(locBeg, locLim) // unsquashed
                << " <> " << std::distance(mismPos.second, remLmt)
                << "/" << std::distance(remPos, remLmt)
                << " /" << nameQualString(remBeg, remLmt)
                << "  => " << res
                << std::endl;
    }

    return res;
  }


  void
  NameQualificationTraversalAda::addVisibleScope(const SgScopeStatement* scope)
  {
    ASSERT_not_null(scope);
    const bool added = state.visibleScopes.insert(scope).second;

    if (added)
    {
      // only record the scope if it was not already visible before.
      // (prevents the scope from being removed from the visible scopes too early.)
      state.scopeState.back().addedVisibleScopes.emplace_back(scope);
    }
  }

  bool
  NameQualificationTraversalAda::isVisibleScope(const SgScopeStatement* scope) const
  {
    return state.visibleScopes.find(scope) != state.visibleScopes.end();
  }

  void
  NameQualificationTraversalAda::addUsedScope(const SgScopeStatement* scope)
  {
    ASSERT_not_null(scope);
    const bool added = state.useScopes.insert(scope).second;

    if (added)
    {
      state.scopeState.back().addedUsedScopes.emplace_back(scope);
    }
  }

  // inline prevents GCC's function not used warning (which is correct currently)
  inline
  bool
  NameQualificationTraversalAda::isUsedScope(const SgScopeStatement* scope) const
  {
    return state.useScopes.find(scope) != state.useScopes.end();
  }

  void
  NameQualificationTraversalAda::addRenamedScope( ScopeRenamingContainer::key_type orig,
                                                  ScopeRenamingContainer::mapped_type renamed
                                                )
  {
    ASSERT_not_null(orig); ASSERT_not_null(renamed);
    ROSE_ASSERT(state.scopeState.size());

    const bool added = state.renamedScopes.emplace(orig, renamed).second;

    if (added)
    {
      state.scopeState.back().addedRenamings.emplace_back(orig);
    }
  }

  NameQualificationTraversalAda::ScopeRenamingContainer::mapped_type
  NameQualificationTraversalAda::renamedScope(ScopeRenamingContainer::key_type orig) const
  {
    ScopeRenamingContainer::const_iterator pos        = state.renamedScopes.find(orig);
    const bool                             wasRenamed = (pos != state.renamedScopes.end());

    return wasRenamed ? pos->second : nullptr;
  }


  void
  NameQualificationTraversalAda::openScope(const SgScopeStatement& scope)
  {
    state.scopeState.emplace_back(&scope);
  }

  void
  NameQualificationTraversalAda::closeScope()
  {
    ROSE_ASSERT(state.scopeState.size());

    AdaScopeStackEntry& top = state.scopeState.back();

    // remove active renaming declarations and use clauses that were added
    //   in the scope that is about to end.
    for (const SgScopeStatement* el : top.addedRenamings)
      state.renamedScopes.erase(el);

    for (const SgScopeStatement* el : top.addedUsedScopes)
      state.useScopes.erase(el);

    for (const SgScopeStatement* el : top.addedVisibleScopes)
      state.visibleScopes.erase(el);

    state.scopeState.pop_back();
  }

  void
  NameQualificationTraversalAda::resetNameQualificationContext()
  {
    //~ std::cerr << "** reset context" << std::endl;
    ASSERT_require(state.scopeState.size() == 1); // just the global scope

    state.visibleScopes.clear();
    state.useScopes.clear();
    state.renamedScopes.clear();
  }

  void
  NameQualificationTraversalAda::saveNameQualificationContext(const SgDeclarationStatement& n)
  {
    //~ std::cerr << "saving renamings: " << &n << ": " << state.renamedScopes.size()
              //~ << std::endl;

    bool added = nameQualContextMap.emplace( &n,
                                             std::make_tuple(state.visibleScopes, state.useScopes, state.renamedScopes)
                                           ).second;

    ASSERT_require(added);
  }

  void
  NameQualificationTraversalAda::loadNameQualificationContext(const SgDeclarationStatement& n)
  {
    auto pos = nameQualContextMap.find(&n);

    if (pos == nameQualContextMap.end())
      return;

    //~ std::cerr << "loading renamings: " << &n << ": " << std::get<2>(pos->second).size()
              //~ << std::endl;

    for (const SgScopeStatement* sc : std::get<0>(pos->second)) addVisibleScope(sc);
    for (const SgScopeStatement* sc : std::get<1>(pos->second)) addUsedScope(sc);

    for (const ScopeRenamingContainer::value_type& ren : std::get<2>(pos->second))
      addRenamedScope(ren.first, ren.second);
  }


  const SgScopeStatement*
  unitDefinition(const SgDeclarationStatement& n)
  {
    if (const SgFunctionDeclaration* fundcl = isSgFunctionDeclaration(&n))
    {
      const SgFunctionDeclaration* def = isSgFunctionDeclaration(fundcl->get_definingDeclaration());

      //~ std::cerr << "fundcl: " << fundcl->get_name() << " " << def << std::endl;
      return def ? def->get_definition() : nullptr;
    }

    if (const SgAdaPackageSpecDecl* pkgspc = isSgAdaPackageSpecDecl(&n))
    {
      //~ std::cerr << "pkgspc: " << pkgspc->get_name() << " " << pkgspc->get_definition()
                //~ << std::endl;
      return pkgspc->get_definition();
    }

    if (const SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(&n))
      return unitDefinition(SG_DEREF(gendcl->get_declaration()));

    if (/*const SgAdaGenericInstanceDecl* gendcl =*/ isSgAdaGenericInstanceDecl(&n))
    {
      // \todo should be the body of the instantiation specification
      return nullptr;
    }

    // should not happen with a full Ada implemention
    //   SgImportStatement is used when the package is not available
    //   => do nothing for now
    if (/*const SgImportStatement* impstm =*/ isSgImportStatement(&n))
    {
      //~ std::cerr << "import: nullptr" << std::endl;
      return nullptr;
    }

    SG_UNEXPECTED_NODE(n);
  }

#if OBSOLETE_CODE
  const SgDeclarationStatement&
  importedDecl(const SgImportStatement& impstm)
  {
    return si::Ada::importedUnit(impstm).decl();
  }
#endif /* OBSOLETE_CODE */

  struct AdaPreNameQualifier : sg::DispatchHandler<NameQualificationTraversalAda::InheritedAttribute>
  {
      using InheritedAttribute = NameQualificationTraversalAda::InheritedAttribute;
      using base = sg::DispatchHandler<InheritedAttribute>;
      using NameQualificationMapType = NameQualificationTraversalAda::NameQualificationMapType;

      AdaPreNameQualifier(NameQualificationTraversalAda& trav, InheritedAttribute inh)
      : base(std::move(inh)), traversal(trav), skipNameQualification()
      {
        // Obtain the set of nodes where name qualification is not required
        // and reset the result set where name qualification is not required
        // by swapping the two sets.
        res.get_nameQualIgnoreSet().swap(skipNameQualification);
      }

      /// tests if the name qualification is turned off for \ref n
      bool elideNameQualification(const SgNode&) const;

      /// records that name qualification should be suppressed for \ref child
      /// \details \ref child must be a direct child of the current node
      void suppressNameQualification(const SgNode* child);

      /// computes the name qualification for \ref n when used in the current scope.
      /// if the name qualification is not the empty string it gets recorded
      /// \param n     a declaration of a node referenced in the active scope
      /// \param scope the scope where n was declared
      void recordNameQualIfNeeded(const SgNode& n, const SgScopeStatement* scope);
      void recordNameQualIfNeeded(NameQualificationMapType& m, const SgNode& n, const SgScopeStatement* scope);

      /// computes the name qualification for \ref n with reference node \ref ref.
      /// \details
      ///    Introduces a local reference map based on the anchor point \ref ref.
      ///    Suitable for name qualifying types and back-references (e.g., declarations).
      void computeNameQualForShared(const SgNode& ref, const SgNode* n);

      /// if n is an AdaGenericDecl, actualDecl returns the "generified" declaration,
      ///   otherwise returns n;
      const SgDeclarationStatement*
      actualDecl(const SgDeclarationStatement* n);

      /// computes the name qualification for a non-shared node \ref n.
      /// \details
      ///    suitable for declarations, expressions, and other non-shared nodes.
      ///    note: some expression subtrees (e.g., those in types, ..)
      ///          are not reached by the type traversal mechanism. Those need to be
      ///          visited separately.
      ///          e.g., SgDeclType::get_base_expression
      void computeNameQualForNonshared(const SgNode* n, bool inTypeSubtree = true);

      /// computes the name qualification for declaration \ref n in scope \ref scope referenced from anchor node \ref ref
      /// \param ref   the visited node that refers to \ref b
      /// \param n     the referenced declaration
      /// \param scope the scope used for name qualification (usually n.get_scope()) but special nodes
      ///              such as SgAdaGenericInstanceDecl compute may receive an externally supplied scope.
      /// \todo
      ///    not sure if the special handling is really needed, or if the unparsing of SgAdaGenericInstanceDecl
      ///    could just use the name from the instantiated declaration...
      void computeNameQualForDeclLink(const SgNode& ref, const SgDeclarationStatement& n, const SgScopeStatement* scope);

      /// computes the name qualification for declaration \ref n referenced from anchor node \ref ref
      void computeNameQualForDeclLink(const SgNode& ref, const SgDeclarationStatement& n);

      /// records a use declaration of \ref scope.
      void addVisibleScope(const SgScopeStatement* scope);

      /// records renaming iff the renamed entity is associated with a scope.
      /// \param e reference to renamed unit, exception, object, etc.
      /// \param n the current renaming declaration
      /// \details
      ///   - does not record variable renamings
      ///   - in most cases n.get_renamed() == e, except when the renamed declaration
      ///     was itself a renaming (recursive call).
      void addRenamedScopeIfNeeded(const SgExpression* e, const SgAdaRenamingDecl& n);

      /// records a use declaration of \ref scope iff \ref n is associated with a scope.
      void addUsedScopeIfNeeded(const SgDeclarationStatement* n);

      /// produce name qualification required for \ref remote with respect
      /// to the current scope.
      /// \{
      std::string nameQual(const SgNode& n, const SgScopeStatement& remote);
      std::string nameQual(const SgNode& n, const SgScopeStatement* remote);
      /// \}


      void handle(const SgNode&) { /* do nothing by default */ }

      //
      // statements and declarations

      void handle(const SgStatement& n)
      {
        handle(sg::asBaseType(n));

        res.set_currentStatement(const_cast<SgStatement*>(&n));
      }

      void handle(const SgGotoStatement& n)
      {
        handle(sg::asBaseType(n));

        // \todo LABEL_NAMEQUAL_1
        //~ recordNameQualIfNeeded(n, n.get_label()->get_scope());
        //~ recordNameQualIfNeeded(n, sg::ancestor<SgScopeStatement>(n.get_label()));

        //~ recordNameQualIfNeeded(n, sg::ancestor<SgScopeStatement>(n.get_label()));
        // avoid enless recursion for
        // <<lbl>> goto lbl;
        //   \todo consider switching over to fortran label representation
        handle(SG_DEREF(n.get_label_expression()));
      }

      void handle(const SgScopeStatement& n)
      {
        handle(sg::asBaseType(n));

        traversal.openScope(n);
        res.set_currentScope(const_cast<SgScopeStatement*>(&n));

        if (scopeName(&n).name().size())
          traversal.addVisibleScope(&n);
      }

      void handle(const SgAdaRenamingDecl& n)
      {
        handle(sg::asBaseType(n));

        computeNameQualForShared(n, n.get_type());
        addRenamedScopeIfNeeded(n.get_renamed(), n);
      }

      void handle(const SgTypedefDeclaration& n)
      {
        handle(sg::asBaseType(n));

        computeNameQualForShared(n, n.get_base_type());
      }

      void handle(const SgAdaFormalTypeDecl& n)
      {
        handle(sg::asBaseType(n));

        SgAdaFormalType* ty = n.get_type();
        ASSERT_not_null(ty);

        if (SgType* formalType = ty->get_formal_type())
          computeNameQualForShared(n, formalType);
      }

      void handle(const SgEnumDeclaration& n)
      {
        handle(sg::asBaseType(n));

        if (SgType* parentType = n.get_adaParentType())
          computeNameQualForShared(n, parentType);
      }


      /// loads scope and renaming visibility from parent
      /// \details
      ///    should be used only from nodes, if a node (child) is not declared
      ///    within its logical parent.
      ///    e.g., package Ada.Text_IO: Text_IO is logically declared within Ada
      ///            but since it is a separate package/file its parent
      ///            is the global scope.
      void inheritScopeVisibility(const SgScopeStatement* n);

      void handle(const SgAdaPackageSpecDecl& n)
      {
        handle(sg::asBaseType(n));

        recordNameQualIfNeeded(n, n.get_scope());
        inheritScopeVisibility(n.get_definition());
      }

      void handle(const SgAdaPackageBodyDecl& n)
      {
        handle(sg::asBaseType(n));

        recordNameQualIfNeeded(n, n.get_scope());
        inheritScopeVisibility(n.get_definition());
      }

      void handle(const SgImportStatement& n)
      {
        handle(sg::asBaseType(n));

        for (si::Ada::ImportedUnitResult impUnit : si::Ada::importedUnits(n))
        {
          const SgExpression&           importExpr = impUnit.unitref();
          const SgDeclarationStatement& importDecl = impUnit.decl();

          recordNameQualIfNeeded(importExpr, importDecl.get_scope());

          // check whether a renamed unit was imported
          if (const SgAdaRenamingRefExp* renexp = isSgAdaRenamingRefExp(&importExpr))
          {
            const SgAdaRenamingDecl& rendcl = SG_DEREF(renexp->get_decl());

            addRenamedScopeIfNeeded(rendcl.get_renamed(), rendcl);
          }
          else if (const SgScopeStatement* unitDef = unitDefinition(importDecl))
            addVisibleScope(unitDef);
        }
      }

      void handle(const SgAdaRepresentationClause& n)
      {
        handle(sg::asBaseType(n));

        computeNameQualForShared(n, n.get_recordType());
      }

      void handle(const SgAdaEnumRepresentationClause& n)
      {
        handle(sg::asBaseType(n));

        computeNameQualForShared(n, n.get_enumType());
      }

      void handle(const SgFunctionDeclaration& n)
      {
        //~ std::cerr << "entering " << n.get_name() << " " << &n
                  //~ << std::endl;
        handle(sg::asBaseType(n));

        SgScopeStatement* fnscope = n.get_definition();

        recordNameQualIfNeeded(n, n.get_scope());

        // set the scope to the logical parent before type qualification of
        //   parameter and return types are computed.
        //~ res.set_currentScope(n.get_scope());

        if (fnscope == nullptr) fnscope = n.get_functionParameterScope();

        inheritScopeVisibility(fnscope);

        res.set_currentScope(fnscope);

        // parameters are handled by the traversal, so just qualify
        //   the return type, if this is a function.
        const SgFunctionType* ty = n.get_type();

        if (SageInterface::Ada::isFunction(ty))
        {
          computeNameQualForShared(n, ty->get_return_type());

          //~ std::cerr << "<- " << ty->get_return_type() << std::endl;
        }
      }

      void handle(const SgAdaDiscriminatedTypeDecl& n)
      {
        handle(sg::asBaseType(n));

        res.set_currentScope(n.get_discriminantScope());
      }

      void handle(const SgAdaTaskBodyDecl& n)
      {
        handle(sg::asBaseType(n));

        recordNameQualIfNeeded(n, n.get_scope());
      }

      void handle(const SgAdaProtectedBodyDecl& n)
      {
        handle(sg::asBaseType(n));

        recordNameQualIfNeeded(n, n.get_scope());
      }


      void handle(const SgAdaGenericInstanceDecl& n)
      {
        handle(sg::asBaseType(n));

        recordNameQualIfNeeded(n, n.get_scope());

        const SgDeclarationStatement& basedecl    = SG_DEREF(actualDecl(n.get_genericDeclaration()));
        const SgScopeStatement*       parentscope = SG_DEREF(n.get_instantiatedScope()).get_scope();

        /// if this is a logically nested instantiation, namequal requires the parent instantiation.
        ///   (this is necessary to capture the actual parameters of the parent instantiation)
        /// otherwise, just reset parentscope and unparse the name of the generic declaration.
        // \todo consider introducing a flag in SgAdaGenericInstanceDecl indicating whether
        //       this is a nested instantiation or not (e.g., instantiation level).
        if (sg::ancestor<SgAdaGenericInstanceDecl>(parentscope) == nullptr)
          parentscope = basedecl.get_scope();

        computeNameQualForDeclLink(n, basedecl, parentscope);
      }

      void handle(const SgAdaFormalPackageDecl& n)
      {
        handle(sg::asBaseType(n));

        // PP not needed: recordNameQualIfNeeded(n, n.get_scope());
        const SgDeclarationStatement* basedecl = actualDecl(n.get_declaration());

        computeNameQualForDeclLink(n, SG_DEREF(basedecl));
      }


      void handle(const SgUsingDeclarationStatement& n)
      {
        handle(sg::asBaseType(n));

        const SgDeclarationStatement* usedDcl = n.get_declaration();

        ASSERT_require(isSgImportStatement(usedDcl) == nullptr);

/*
        if (const SgImportStatement* impstm = isSgImportStatement(usedDcl))
          usedDcl = &importedDecl(*impstm);
*/
        recordNameQualIfNeeded(n, SG_DEREF(usedDcl).get_scope());
        addUsedScopeIfNeeded(n.get_declaration());
      }

      // needed because SgBaseClass objects cannot be traversed
      //   see Cxx_GrammarTreeTraversalSuccessorContainer for details!
      void handleBaseClass(const SgBaseClass* b)
      {
        if (b == nullptr)
          return;

        if (const SgExpBaseClass* eb = isSgExpBaseClass(b))
          handle(*eb);
        else
          handle(*b);
      }

      void handle(const SgClassDefinition& n)
      {
        handle(sg::asBaseType(n));

        // bases seem not to be traversed, so let's traverse them
        for (const SgBaseClass* base : n.get_inheritances())
          handleBaseClass(base);
      }

      void handle(const SgClassDeclaration& n)
      {
        handle(sg::asBaseType(n));
        handleBaseClass(n.get_adaParentType());
      }


      //
      // declaration-like nodes

      void handle(const SgInitializedName& n)
      {
        //~ std::cerr << "ini: " << n.get_name()
                  //~ << "@" << &n
                  //~ << " t: " << typeid(*n.get_type()).name()
                  //~ << std::endl;
        computeNameQualForShared(n, n.get_type());
      }

      void handle(const SgBaseClass& n)
      {
        const SgClassDeclaration& decl = SG_DEREF(n.get_base_class());

        recordNameQualIfNeeded(n, decl.get_scope());
      }

      void handle(const SgExpBaseClass& n)
      {
        // get_base_class_exp would be traversed by default, yet, here it is NOT traversed.
        // the reason is that we cannot traverse any SgBaseClass objects.. ???
        //   see Cxx_GrammarTreeTraversalSuccessorContainer for details!
        computeNameQualForNonshared(n.get_base_class_exp(), false /* not a type subtree */);
      }

      void handle(const SgPragma& n)
      {
        handle(sg::asBaseType(n));

        if (SgExprListExp* arglst = n.get_args())
          computeNameQualForNonshared(arglst, false /* not a type subtree */);
      }


      //
      // expressions

      void handle(const SgExpression&) { /* do nothing */ }

      void handle(const SgExprListExp& n)
      {
        if (!elideNameQualification(n)) return;

        // forward name qualifcation suppression to children
        for (const SgExpression* child : n.get_expressions())
          suppressNameQualification(child);
      }

      void handle(const SgVarRefExp& n)
      {
        //~ if (!isSgDotExp(n.get_parent()) && !SageInterface::Ada::isFieldReference(n))
        if (!elideNameQualification(n))
          recordNameQualIfNeeded(n, si::Ada::declOf(n).get_scope());
      }

      void handle(const SgLabelRefExp& n)
      {
        recordNameQualIfNeeded(n, si::Ada::declOf(n).get_scope());
      }

      void handle(const SgEnumVal& n)
      {
        recordNameQualIfNeeded(n, SG_DEREF(n.get_declaration()).get_scope());
      }

      void handle(const SgAdaUnitRefExp& n)
      {
        if (!elideNameQualification(n))
        {
          const SgDeclarationStatement* dcl = &si::Ada::declOf(n);

          if (const SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(dcl))
            dcl = gendcl->get_declaration();

          //~ std::cerr << "uref " << &si::Ada::declOf(n) << std::endl;
          recordNameQualIfNeeded(n, SG_DEREF(dcl).get_scope());
        }
      }

      void handle(const SgAdaRenamingRefExp& n)
      {
        //~ if (!elideNameQualification(n))
        recordNameQualIfNeeded(n, si::Ada::declOf(n).get_scope());
      }

      // \returns the scope of a function symbol
      const SgScopeStatement&
      declarativeScope(const SgFunctionRefExp& n)
      {
        const SgFunctionSymbol& sym = SG_DEREF(n.get_symbol());
        const SgScopeStatement* res = sym.get_scope();

        return SG_DEREF(res);
      }

      void handle(const SgFunctionRefExp& n)
      {
        if (!elideNameQualification(n))
          recordNameQualIfNeeded(n, &declarativeScope(n));
      }

      void handle(const SgAdaTaskRefExp& n)
      {
        if (!elideNameQualification(n))
          recordNameQualIfNeeded(n, si::Ada::declOf(n).get_scope());
      }

      void handle(const SgAdaProtectedRefExp& n)
      {
        if (!elideNameQualification(n))
          recordNameQualIfNeeded(n, si::Ada::declOf(n).get_scope());
      }

      void handle(const SgDotExp& n)
      {
        suppressNameQualification(n.get_rhs_operand());
      }

      void handle(const SgFunctionCallExp& n)
      {
        const bool suppressNameQual = (  n.get_uses_operator_syntax()
                                      || n.get_usesObjectCallSyntax()
                                      // || si::Ada::isDispatching(n)
                                      );

        if (suppressNameQual)
          suppressNameQualification(n.get_function());
      }

      void handle(const SgDesignatedInitializer& n)
      {
        // SgDesignatedInitializer -> SgExprListExp -> SgAggregateInitializer
        const SgAggregateInitializer* parentinit = sg::ancestorPath<SgExprListExp, SgAggregateInitializer>(n);
        const bool                    suppressNameQual = (  !parentinit
                                                         || si::Ada::getArrayTypeInfo(parentinit->get_type()).type() == nullptr
                                                         );

        // suppress on fields but not enumval or variables
        if (suppressNameQual)
          suppressNameQualification(n.get_designatorList());
      }

      void handle(const SgNewExp& n)
      {
        computeNameQualForShared(n, n.get_specified_type());
      }

      void handle(const SgCastExp& n)
      {
        computeNameQualForShared(n, n.get_type());
      }

      void handle(const SgTypeExpression& n)
      {
        computeNameQualForShared(n, n.get_type());
      }

      //
      // types

      void handle(const SgType&) { /* do nothing */ }

      void handle(const SgNamedType& n)
      {
        const SgDeclarationStatement& dcl = SG_DEREF(n.get_declaration());

        //~ std::cerr << "nmt: " << n.get_name() << " "
                  //~ << &dcl << " "
                  //~ << typeid(dcl).name()
                  //~ << std::endl;
        recordNameQualIfNeeded(dcl, dcl.get_scope());
      }

      void handle(const SgRangeType& n)
      {
        computeNameQualForShared(SG_DEREF(res.get_referenceNode()), n.get_base_type());
      }

      //
      // define subtree traversal for type elements that are not defined by default.

      void handle(const SgDeclType& n)
      {
        computeNameQualForNonshared(n.get_base_expression());
      }

      void handle(const SgArrayType& n)
      {
        computeNameQualForNonshared(n.get_dim_info());
        computeNameQualForShared(SG_DEREF(res.get_referenceNode()), n.get_base_type());
      }

      void handle(const SgAdaSubtype& n)
      {
        computeNameQualForShared(SG_DEREF(res.get_referenceNode()), n.get_base_type());
      }

      void handle(const SgAdaDerivedType& n)
      {
        computeNameQualForShared(SG_DEREF(res.get_referenceNode()), n.get_base_type());
      }

      void handle(const SgAdaAccessType& n)
      {
        computeNameQualForShared(SG_DEREF(res.get_referenceNode()), n.get_base_type());
      }

      void handle(const SgAdaModularType& n)
      {
        computeNameQualForNonshared(n.get_modexpr());
      }

      void handle(const SgTypeTuple& n)
      {
        const SgNode& refnode = SG_DEREF(res.get_referenceNode());

        for (const SgType* elem : n.get_types())
          computeNameQualForShared(refnode, elem);
      }

    private:
      NameQualificationTraversalAda&               traversal;
      InheritedAttribute::SkipNameQualificationSet skipNameQualification;
  };

  struct AdaPostNameQualifier : sg::DispatchHandler<NameQualificationTraversalAda::SynthesizedAttribute>
  {
      using InheritedAttribute = NameQualificationTraversalAda::InheritedAttribute;
      using SynthesizedAttribute = NameQualificationTraversalAda::SynthesizedAttribute;
      using SynthesizedAttributesList = NameQualificationTraversalAda::SynthesizedAttributesList;
      using base = sg::DispatchHandler<SynthesizedAttribute>;

      AdaPostNameQualifier( NameQualificationTraversalAda& trav,
                            InheritedAttribute inh,
                            SynthesizedAttributesList synlst
                          )
      : base(),
        traversal(trav),
        inheritedAttribute(std::move(inh)),
        synthesizedAttributes(std::move(synlst))
      {}

      /// resets the name qualification context after the last global declaration
      /// \ref n of an input file.
      /// \details
      ///    note, a compilation exists of several input files, the main file
      ///    specified as command line arguments, and imported packages and functions.
      ///    At the end of each processed file, the name qualification context
      ///    comprised of using declarations, renamed entities, and visible scopes (*)
      ///    need to be reset.
      ///    This is because using declarations and renamed entities can appear at
      ///    the global scope so each file is not insulated from declarations
      ///    of directly or indirectly imported files.
      ///  (*) should be handled by \ref closeScope, but added for consistency.
      void resetNameQualificationContextIfGlobal(const SgDeclarationStatement& n);

      /// called when a scope \ref n ends
      /// \details
      ///   - saves away name qualification context (if applicable)
      ///   - resets name qualification context
      void endOfScope(const SgScopeStatement& n);

      void handle(const SgNode&)                  { /* do nothing by default */ }

      void handle(const SgScopeStatement& n)      { endOfScope(n); }
      void handle(const SgAdaPackageSpecDecl& n)  { resetNameQualificationContextIfGlobal(n); }
      void handle(const SgAdaPackageBodyDecl& n)  { resetNameQualificationContextIfGlobal(n); }
      void handle(const SgFunctionDeclaration& n) { resetNameQualificationContextIfGlobal(n); }

      void handle(const SgAdaRenamingDecl& n)
      {
        if (SageInterface::Ada::renamedPackage(n))
          resetNameQualificationContextIfGlobal(n);
      }

    private:
      NameQualificationTraversalAda& traversal;
      InheritedAttribute             inheritedAttribute;
      SynthesizedAttributesList      synthesizedAttributes;
  };

  void AdaPostNameQualifier::endOfScope(const SgScopeStatement& n)
  {
    const SgDeclarationStatement* parent = isSgDeclarationStatement(n.get_parent());

    if (  isSgAdaPackageSpecDecl(parent)
       || isSgAdaPackageBodyDecl(parent)
       || isSgFunctionDeclaration(parent)
       )
    {
      traversal.saveNameQualificationContext(*parent);
    }

    traversal.closeScope();
  }

  void AdaPostNameQualifier::resetNameQualificationContextIfGlobal(const SgDeclarationStatement&)
  {
    // \note we may need to also check whether the next statement
    //       is in the same or a different file.
    if (isSgGlobal(inheritedAttribute.get_currentScope()))
      traversal.resetNameQualificationContext();
  }

  void AdaPreNameQualifier::inheritScopeVisibility(const SgScopeStatement* n)
  {
    if (n == nullptr) return;

    if (const SgScopeStatement* parent_scope = si::Ada::logicalParentScope(*n))
    {
      if (const SgDeclarationStatement* parent_dcl = isSgDeclarationStatement(parent_scope->get_parent()))
        traversal.loadNameQualificationContext(*parent_dcl);
      else if (isSgGlobal(parent_scope))
      {
        traversal.addVisibleScope(si::Ada::pkgStandardScope());
        traversal.addUsedScope(si::Ada::pkgStandardScope());
      }

      traversal.addVisibleScope(parent_scope);
    }

/*
    n = si::Ada::logicalParentScope(*n);
    while (requiresNameQual(n))
    {
      traversal.addVisibleScope(n);
      n = n->get_scope(); // use logicalParentScope?
      ASSERT_not_null(n);
    }
*/
  }


  std::string
  AdaPreNameQualifier::nameQual(const SgNode& n, const SgScopeStatement& remote)
  {
    const SgScopeStatement* current = res.get_currentScope();

    return current ? traversal.computeNameQual(n, *current, remote)
                   : "<missing-scope>"; // <-- this may be used iff invoked
                                        //     from unparseToString..
  }

  std::string
  AdaPreNameQualifier::nameQual(const SgNode& node, const SgScopeStatement* remote)
  {
    //~ if (remote == nullptr)
      //~ mlog[ERROR] << "@" << node.unparseToString() << std::endl;
    return nameQual(node, SG_DEREF(remote));
  }

  void
  AdaPreNameQualifier::addRenamedScopeIfNeeded(const SgExpression* e, const SgAdaRenamingDecl& n)
  {
    if (const SgAdaRenamingRefExp* renref = isSgAdaRenamingRefExp(e))
    {
      addRenamedScopeIfNeeded(SG_DEREF(renref->get_decl()).get_renamed(), n);
      return ;
    }

    if (const SgAdaUnitRefExp* unitref = isSgAdaUnitRefExp(e))
    {
      const SgDeclarationStatement* dcl = unitref->get_decl();

      if (const SgAdaPackageSpecDecl* pkgspc = isSgAdaPackageSpecDecl(dcl))
        traversal.addRenamedScope(pkgspc->get_definition(), &n);
      else if (const SgAdaPackageBodyDecl* pkgbdy = isSgAdaPackageBodyDecl(dcl))
        traversal.addRenamedScope(SG_DEREF(pkgbdy->get_spec()).get_definition(), &n);

      // other unit refs?
      return;
    }

    if (/*const SgFunctionRefExp* funref =*/ isSgFunctionRefExp(e))
    {
/*
      SgFunctionSymbol& funsym = SG_DEREF(funref->get_symbol());

      if (isSgAdaFunctionRenamingDecl(funsym.get_declaration())
        traversal.addRenamedScope(funsym.get_scope(), &n);
*/
      // \todo what needs to be done here?
      return;
    }

    // \todo for now the expected types are listed separately to understand
    //       what kind of types can possibly occur here.
    //       in the long run, just returning should be sufficient.

    if (/*const SgAdaTaskRefExp* tskref =*/ isSgAdaTaskRefExp(e))
      return;

    if (/*const SgAdaProtectedRefExp* tskref =*/ isSgAdaProtectedRefExp(e))
      return;

    if (/*const SgTypeExpression* tskref =*/ isSgTypeExpression(e))
      return;

    if (/*const SgVariableSymbol* varsym =*/ isSgVarRefExp(e))
      return;

    if (/*const SgPntrArrRefExp* arref =*/ isSgPntrArrRefExp(e))
      return;

    if (/*const SgDotExp* fldref =*/ isSgDotExp(e))
      return;

    if (/*const SgPointerDerefExp* ptrref =*/ isSgPointerDerefExp(e))
      return;

    if (/*const SgCastExp* castexp =*/ isSgCastExp(e))
      return;

    if (/*const SgEnumVal* castexp =*/ isSgEnumVal(e))
      return;

    if (/*const SgEnumVal* castexp =*/ isSgVoidVal(e))
      return;

    if (/*const SgEnumVal* castexp =*/ isSgAdaAttributeExp(e))
      return;

    if (/*const SgEnumVal* castexp =*/ isSgNullExpression(e))
      return;

    if (/*const SgEnumVal* castexp =*/ isSgCharVal(e))
      return;

    if (e == nullptr)
    {
      mlog[WARN] << "nullptr in AdaPreNameQualifier::addRenamedScopeIfNeeded"
                 << std::endl;
    }
    else
    {
      mlog[WARN] << "unexpected type in AdaPreNameQualifier::addRenamedScopeIfNeeded " << typeid(*e).name()
                 << std::endl;
    }
  }

  void
  AdaPreNameQualifier::addUsedScopeIfNeeded(const SgDeclarationStatement* n)
  {
    if (const SgAdaPackageSpecDecl* pkgspc = isSgAdaPackageSpecDecl(n))
    {
      traversal.addUsedScope(pkgspc->get_definition());
      return;
    }

    if (const SgAdaPackageBodyDecl* pkgbdy = isSgAdaPackageBodyDecl(n))
    {
      traversal.addUsedScope(pkgbdy->get_definition());
      return;
    }

    if (/*const SgAdaRenamingDecl* rendcl =*/ isSgAdaRenamingDecl(n))
    {
      // \todo
      //~ traversal.addUsedScope(rendcl->get_definition());
      return;
    }

    if (const SgAdaGenericInstanceDecl* genins = isSgAdaGenericInstanceDecl(n))
    {
      addUsedScopeIfNeeded(genins->get_genericDeclaration());
      return;
    }

    if (const SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(n))
    {
      addUsedScopeIfNeeded(gendcl->get_declaration());
      return;
    }

    if (/*const SgAdaFormalPackageDecl* frmpkg =*/ isSgAdaFormalPackageDecl(n))
    {
      // \todo addUsedScopeIfNeeded(...);
      return;
    }

#if 0
    if (/*const SgImportStatement* impstm =*/ isSgImportStatement(n))
      return; // traversal.addUsedScope();

    // types needed?
    if (/*const SgTypedefDeclaration* typdef =*/ isSgTypedefDeclaration(n))
      return; // traversal.addUsedScope();

    if (/*const SgAdaTaskTypeDecl* tsktyp =*/ isSgAdaTaskTypeDecl(n))
      return; // traversal.addUsedScope();

    if (/*const SgAdaProtectedTypeDecl* potyp =*/ isSgAdaProtectedTypeDecl(n))
      return; // traversal.addUsedScope();

    if (/*const SgClassDeclaration* clsdcl =*/ isSgClassDeclaration(n))
      return; // traversal.addUsedScope();

    if (/*const SgEnumDeclaration* enudcl =*/ isSgEnumDeclaration(n))
      return; // traversal.addUsedScope();

    // SG_UNEXPECTED_NODE(SG_DEREF(n));
#endif
  }

  void
  AdaPreNameQualifier::addVisibleScope(const SgScopeStatement* scope)
  {
    ASSERT_not_null(scope);

    traversal.addVisibleScope(scope);
  }

  void
  AdaPreNameQualifier::recordNameQualIfNeeded(const SgNode& n, const SgScopeStatement* scope)
  {
    ROSE_ASSERT((isSgType(&n) == nullptr) || res.is_typeMode());

    std::string qual = nameQual(n, scope);

    if (qual.size() > 0)
    {
      //~ std::cerr << "rec " << qual.c_str() << " for " << &n << " " << typeid(n).name()
                //~ << std::endl;
      traversal.recordNameQual(n, std::move(qual));
    }
  }

  void
  AdaPreNameQualifier::recordNameQualIfNeeded( NameQualificationMapType& m,
                                               const SgNode& n,
                                               const SgScopeStatement* scope
                                             )
  {
    ROSE_ASSERT((isSgType(&n) == nullptr) || res.is_typeMode());

    std::string qual = nameQual(n, scope);

    if (qual.size() > 0)
      traversal.recordNameQual(m, n, std::move(qual));
  }

  void
  AdaPreNameQualifier::computeNameQualForDeclLink( const SgNode& ref,
                                                   const SgDeclarationStatement& n,
                                                   const SgScopeStatement* scope
                                                 )
  {
    NameQualificationMapType& localQualMapForTypes = traversal.createQualMapForTypeSubtreeIfNeeded(ref);

    recordNameQualIfNeeded(localQualMapForTypes, n, scope);
  }

  void
  AdaPreNameQualifier::computeNameQualForDeclLink(const SgNode& ref, const SgDeclarationStatement& n)
  {
    computeNameQualForDeclLink(ref, n, n.get_scope());
  }

  const SgDeclarationStatement*
  AdaPreNameQualifier::actualDecl(const SgDeclarationStatement* n)
  {
    if (const SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(n))
      return gendcl->get_declaration();

    return n;
  }


  void
  AdaPreNameQualifier::computeNameQualForShared(const SgNode& ref, const SgNode* n)
  {
    using NameQualMap = std::map<SgNode*, std::string>;

    NameQualificationMapType&     localQualMapForTypes = traversal.createQualMapForTypeSubtreeIfNeeded(ref);
    NameQualificationTraversalAda sub{traversal, localQualMapForTypes};
    InheritedAttribute            attr{res};

    // fix the reference node for the type subtree traversal
    attr.set_typeReferenceNode(ref);
    // explore the sub-tree
    sub.traverse(const_cast<SgNode*>(n), attr);
  }


  // SgNode::get_globalQualifiedNameMapForNames

  void
  AdaPreNameQualifier::computeNameQualForNonshared(const SgNode* n, bool inTypeSubtree)
  {
    // since it is a non-shared node, we do not need to switch the local reference map.
    ROSE_ASSERT(  (!inTypeSubtree)
               || (&traversal.get_qualifiedNameMapForNames() != &SgNode::get_globalQualifiedNameMapForNames())
               );

    NameQualificationTraversalAda sub{traversal, traversal.get_qualifiedNameMapForNames()};
    InheritedAttribute            attr{res};

    // the reference node for the type subtree traversal should be set already..
    //~ attr.set_typeReferenceNode(n);

    // explore the sub-tree
    sub.traverse(const_cast<SgNode*>(n), attr);
  }

  bool
  AdaPreNameQualifier::elideNameQualification(const SgNode& n) const
  {
    return skipNameQualification.find(&n) != skipNameQualification.end();
  }

  void
  AdaPreNameQualifier::suppressNameQualification(const SgNode* child)
  {
    res.get_nameQualIgnoreSet().insert(child);
  }

// std::map<SgNode*,std::string>&
  NameQualificationTraversalAda::NameQualificationMapType&
  NameQualificationTraversalAda::createQualMapForTypeSubtreeIfNeeded(const SgNode& n)
  {
    // are we already in type traversal mode?
    if (&SgNode::get_globalQualifiedNameMapForNames() != &qualifiedNameMapForNames)
      return qualifiedNameMapForNames;

    // not yet in type traversal => create a new qualified name map for types
    SgNode*                        key = const_cast<SgNode*>(&n);
    // std::map<SgNode*,std::string>& res = qualifiedNameMapForMapsOfTypes[key];
    NameQualificationMapType& res = qualifiedNameMapForMapsOfTypes[key];

    // do we see this reference node for the first time?
    /*
    if (!res.empty())
    {
      const SgTypedefDeclaration* tydcl = isSgTypedefDeclaration(&n);

      mlog [ WARN ] << "map already exists for " << typeid(n).name()
                    << ( tydcl ? std::string{tydcl->get_name()} : std::string{} )
                    << std::endl;
    }
    */

    //~ ROSE_ASSERT(res.empty());
    return res;
  }

  NameQualificationTraversalAda::InheritedAttribute
  NameQualificationTraversalAda::evaluateInheritedAttribute( SgNode* n,
                                                             InheritedAttribute inh
                                                           )
  {
    return sg::dispatch(AdaPreNameQualifier{*this, std::move(inh)}, n);
  }


  NameQualificationTraversalAda::SynthesizedAttribute
  NameQualificationTraversalAda::evaluateSynthesizedAttribute( SgNode* n,
                                                               InheritedAttribute inh,
                                                               SynthesizedAttributesList synlst
                                                             )
  {
    return sg::dispatch(AdaPostNameQualifier{*this, std::move(inh), std::move(synlst)}, n);
  }


// void generateNameQualificationSupportAda(SgNode* node, std::set<SgNode*>&)
   void
   generateNameQualificationSupportAda(SgNode* node, NameQualificationTraversalAda::NameQualificationSetType&)
   {
     NameQualificationTraversalState nqState;
     NameQualificationTraversalAda   nqual{ SgNode::get_globalQualifiedNameMapForNames(),
                                            SgNode::get_globalQualifiedNameMapForMapsOfTypes(),
                                            nqState
                                          };

     nqual.traverse(node, NameQualificationInheritedAttributeAda{});
   }
} // end anonymous namespace for Ada

// ***********************************************************
// Main calling function to support name qualification support
// ***********************************************************


// void generateNameQualificationSupport( SgNode* node, std::set<SgNode*>& referencedNameSet )
void
generateNameQualificationSupport( SgNode* node, NameQualificationTraversal::NameQualificationSetType& referencedNameSet )
   {
  // This function is the top level API for Name Qualification support.
  // This is the only function that need be seen by ROSE.  This function
  // is called in the function:
  //      Unparser::unparseFile(SgSourceFile* file, SgUnparse_Info& info )
  // in the unparser.C file.  Thus the name qualification is computed
  // as we start to process a file and the computed values saved into the
  // SgNode static data member maps. Two maps are used:
  //    one to support qualification of IR nodes that are named, and
  //    one to support name qualification of types.
  // These are passed by reference and references are stored to them in
  // the NameQualificationTraversal class.

     TimingPerformance timer ("Name qualification support:");

     if (SageInterface::is_Ada_language())
     {
       generateNameQualificationSupportAda(node, referencedNameSet);
       return;
     }

  // DQ (5/28/2011): Initialize the local maps to the static maps in SgNode.  This is requires so the
  // types used in template arguments can call the unparser to support there generation of name qualified
  // nested types.

#if 0
  // DQ (5/3/2024): Debugging non-terminating name qualification case in unit testing.
     printf ("NQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQ \n");
     printf ("NQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQ \n");
     printf ("In generateNameQualificationSupport(): node = %p = %s \n",node,node->class_name().c_str());
     SgSourceFile* sourceFile = isSgSourceFile(node);
     if (sourceFile != NULL)
        {
          printf (" --- sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
        }
     printf ("NQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQ \n");
     printf ("NQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQNQ \n");
#endif

#if 0
     mfprintf(mlog [ WARN ] ) ("In generateNameQualificationSupport(): MangledNameSupport::visitedTemplateDefinitions.size() = %zu \n",MangledNameSupport::visitedTemplateDefinitions.size());
#endif

  // DQ (9/7/2014): Modified to handle template header map (for template declarations).
  // NameQualificationTraversal t(SgNode::get_globalQualifiedNameMapForNames(),SgNode::get_globalQualifiedNameMapForTypes(),SgNode::get_globalTypeNameMap(),referencedNameSet);
  // NameQualificationTraversal t(SgNode::get_globalQualifiedNameMapForNames(),SgNode::get_globalQualifiedNameMapForTypes(),
  //                              SgNode::get_globalQualifiedNameMapForTemplateHeaders(),SgNode::get_globalTypeNameMap(),referencedNameSet);
     NameQualificationTraversal t(SgNode::get_globalQualifiedNameMapForNames(),SgNode::get_globalQualifiedNameMapForTypes(),
                                  SgNode::get_globalQualifiedNameMapForTemplateHeaders(),SgNode::get_globalTypeNameMap(),
                                  SgNode::get_globalQualifiedNameMapForMapsOfTypes(),referencedNameSet);

     NameQualificationInheritedAttribute ih;

#if 0
     mfprintf(mlog [ WARN ] ) ("Calling SageInterface::buildDeclarationSets(node = %p = %s) \n",node,node->class_name().c_str());
#endif

        {
       // DQ (8/14/2025): Adding performance timer for call to buildDeclarationSets().
          TimingPerformance timer ("Name qualification support: buildDeclarationSets");

       // DQ (4/3/2014): Added assertion.
          t.declarationSet = SageInterface::buildDeclarationSets(node);
          ASSERT_not_null(t.declarationSet);
        }
#if 0
     mfprintf(mlog [ WARN ] ) ("DONE: Calling SageInterface::buildDeclarationSets(node = %p = %s) t.declarationSet = %p \n",node,node->class_name().c_str(),t.declarationSet);
#endif

  // DQ (8/14/2025): Adding a performance optimization to only do name qualification on the part of the AST that will unparsed.
  // This should be a significant subset of the number of lines of code (O(1000) or so, since the source files is typically
  // such a small part of the whole translation unit.  Note that the default is still to process the whole translation unit,
  // but this optimization will support the better handling of large files ($1M line translation units).
  // Call the traversal.
  // t.traverse(node,ih);

  // Pei-Hung (8/19/2025): revert to use the traversal over parents
  // SageInterface::getProject() would find multiple SgProject in copyAST_tests and break the assertion

  // Get the project from a traversal over the parents back to the root of the AST.
     SgProject* project = SageInterface::getProject(node);
  // Or we can use this function which does not require a traversal.
  //   SgProject* project = SageInterface::getProject();
     ROSE_ASSERT(project != NULL);

#if 0
     printf ("project->get_suppressNameQualificationAcrossWholeTranslationUnit() = %s \n",project->get_suppressNameQualificationAcrossWholeTranslationUnit() ? "true" : "false");
#endif

#if 0
  // DQ (8/18/2025): We do not call this function recursively, but for the case of multiple files
  // on the command line we do call it multiple times, which is not a problem for the
  // suppressNameQualificationAcrossWholeTranslationUnit mode.
  // DQ (8/14/2025): Since we are using the traverseInputFiles() based traversal, we have to input
  // the project, and so we need to make sure this function is not called recursively.
     static int invocation = 0;
     invocation++;
     if (invocation > 1)
        {
          printf("Error: We can't be calling this function recursively: invocation = %d \n",invocation);
          ROSE_ASSERT(false);
        }
#endif

     if (project->get_suppressNameQualificationAcrossWholeTranslationUnit() == true)
        {
          t.set_suppressNameQualificationAcrossWholeTranslationUnit(true);

       // DQ (8/14/2025): Adding performance timer for call to traverseInputFiles().
          TimingPerformance timer ("Name qualification support: traverseInputFiles:");

       // t.traverseInputFiles(node,ih);
          t.traverseInputFiles(project,ih);
        }
       else
        {
       // DQ (8/14/2025): Adding performance timer for call to traverse().
          TimingPerformance timer ("Name qualification support: traverse:");

          t.traverse(node,ih);
        }

#if 0
  // DQ (5/3/2024): Debugging non-terminating name qualification case in unit testing.
  // mfprintf(mlog [ WARN ] ) ("DONE: Calling name qualification traversal (node = %p = %s) \n",node,node->class_name().c_str());
     printf("\nNQNQNQ DONE: Calling name qualification traversal (node = %p = %s) \n",node,node->class_name().c_str());
#endif

#if 0
     printf("Exiting as a test! \n");
     ROSE_ABORT();
#endif
   }

void NameQualificationTraversal::initDiagnostics()
   {
     static bool initialized = false;
     if (!initialized)
        {
          initialized = true;
        }
   }

// DQ (8/14/2025): Adding optimization (default is false) to support name qualification
// retricted to just the input source file (instead of the whole translation unit).
void
NameQualificationTraversal::set_suppressNameQualificationAcrossWholeTranslationUnit(bool value)
   {
     suppressNameQualificationAcrossWholeTranslationUnit = value;
   }

// DQ (8/14/2025): Adding optimization (default is false) to support name qualification
// retricted to just the input source file (instead of the whole translation unit).
bool
NameQualificationTraversal::get_suppressNameQualificationAcrossWholeTranslationUnit()
   {
     return suppressNameQualificationAcrossWholeTranslationUnit;
   }

void
NameQualificationTraversal::generateNestedTraversalWithExplicitScope( SgNode* node, SgScopeStatement* input_currentScope, SgStatement* input_currentStatement, SgNode* input_referenceNode )
   {
     ASSERT_not_null(input_currentScope);

  // DQ (9/7/2014): Modified to handle template header map (for template declarations).
  // NameQualificationTraversal t(this->qualifiedNameMapForNames,this->qualifiedNameMapForTypes,this->typeNameMap,this->referencedNameSet);
  // NameQualificationTraversal t(this->qualifiedNameMapForNames,this->qualifiedNameMapForTypes,this->qualifiedNameMapForTemplateHeaders,this->typeNameMap,this->referencedNameSet);
     NameQualificationTraversal t(this->qualifiedNameMapForNames,this->qualifiedNameMapForTypes,this->qualifiedNameMapForTemplateHeaders,
                                  this->typeNameMap,this->qualifiedNameMapForMapsOfTypes,this->referencedNameSet);

     t.explictlySpecifiedCurrentScope     = input_currentScope;

  // DQ (4/19/2019): This might not be required (passin it via the inherited attribute might be all thisis required.
  // DQ (4/19/2019): Added support to include current statement (required for nested traversals
  // of types to support name qualification for SgPointerMemberType).
     t.explictlySpecifiedCurrentStatement = input_currentStatement;

  // DQ (4/7/2014): Set this explicitly using the one already built.
     ASSERT_not_null(declarationSet);
     t.declarationSet = declarationSet;
     ASSERT_not_null(t.declarationSet);

     NameQualificationInheritedAttribute ih;

  // DQ (4/3/2014): Added assertion.
     ASSERT_not_null(declarationSet);

  // This fails for test2001_02.C.
  // ROSE_ASSERT(declarationSet == NULL);

  // DQ (5/24/2013): Added scope to inherited attribute.
     ih.set_currentScope(input_currentScope);

  // DQ (4/19/2019): Added support to include current statement (required for nested traversals
  // of types to support name qualification for SgPointerMemberType).
     ih.set_currentStatement(input_currentStatement);
     ih.set_referenceNode(input_referenceNode);

#if 0
     mfprintf(mlog [ WARN ] ) ("**************************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("**************************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("********************* RECURSIVE TRAVERSAL ********************** \n");
     mfprintf(mlog [ WARN ] ) ("**************************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("**************************************************************** \n");
#endif

  // Call the traversal.
     t.traverse(node,ih);

#if 0
     mfprintf(mlog [ WARN ] ) ("**************************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("**************************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("****************** DONE: RECURSIVE TRAVERSAL ******************* \n");
     mfprintf(mlog [ WARN ] ) ("**************************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("**************************************************************** \n");
#endif
   }


// *******************
// Inherited Attribute
// *******************

NameQualificationInheritedAttribute::NameQualificationInheritedAttribute()
   {
  // Default constructor

  // DQ (5/24/2013): Allow the current scope to be tracked from the traversal of the AST
  // instead of being computed at each IR node which is a problem for template arguments.
  // See test2013_187.C for an example of this.
     currentScope     = NULL;
     currentStatement = NULL;
     referenceNode    = NULL;

#if 0
  // DQ (2/8/2019): And then I woke up in the morning and had a better idea.

  // DQ (2/7/2019): Namen qualification can under rare circumstances depend on the type.
     usingPointerToMemberType = NULL;

  // DQ (2/7/2019): Name qualification can under rare circumstances depends on the type.
     containsFunctionArgumentsOfPointerMemberType = false;
#endif
   }

NameQualificationInheritedAttribute::NameQualificationInheritedAttribute ( const NameQualificationInheritedAttribute & X )
   {
  // Copy constructor.

  // DQ (5/24/2013): Allow the current scope to be tracked from the traversal of the AST
  // instead of being computed at each IR node which is a problem for template arguments.
  // See test2013_187.C for an example of this.
     currentScope     = X.currentScope;
     currentStatement = X.currentStatement;
     referenceNode    = X.referenceNode;

#if 0
  // DQ (8/1/2020): Need to copy the STL map.
     namespaceAliasDeclarationMap = X.namespaceAliasDeclarationMap;
#endif
#if 0
  // DQ (2/8/2019): And then I woke up in the morning and had a better idea.

  // DQ (2/7/2019): Name qualification can under rare circumstances depends on the type.
     usingPointerToMemberType = X.usingPointerToMemberType;

  // DQ (2/7/2019): Namen qualification can under rare circumstances depend on the type.
     containsFunctionArgumentsOfPointerMemberType = X.containsFunctionArgumentsOfPointerMemberType;
#endif
#if 0
     mfprintf(mlog [ WARN ] ) ("In NameQualificationInheritedAttribute(): copy constructor: currentScope     = %p = %s \n",currentScope,currentScope != NULL ? currentScope->class_name().c_str() : "NULL");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationInheritedAttribute(): copy constructor: currentStatement = %p = %s \n",currentStatement,currentStatement != NULL ? currentStatement->class_name().c_str() : "NULL");
#endif
   }

SgScopeStatement* NameQualificationInheritedAttribute::get_currentScope()
   {
     return currentScope;
   }

void NameQualificationInheritedAttribute::set_currentScope(SgScopeStatement* scope)
   {
     currentScope = scope;
   }

SgStatement* NameQualificationInheritedAttribute::get_currentStatement()
   {
     return currentStatement;
   }

void NameQualificationInheritedAttribute::set_currentStatement(SgStatement* statement)
   {
     currentStatement = statement;
   }

SgNode* NameQualificationInheritedAttribute::get_referenceNode()
   {
     return referenceNode;
   }

void NameQualificationInheritedAttribute::set_referenceNode(SgNode* node)
   {
     referenceNode = node;
   }

#if 0
NameQualificationInheritedAttribute::namespaceAliasMapType &
NameQualificationInheritedAttribute::get_namespaceAliasDeclarationMap()
   {
     return namespaceAliasDeclarationMap;
   }
#endif

#if 0
  // DQ (2/8/2019): And then I woke up in the morning and had a better idea.

SgPointerMemberType* NameQualificationInheritedAttribute::get_usingPointerToMemberType()
   {
     return usingPointerToMemberType;
   }

void NameQualificationInheritedAttribute::set_usingPointerToMemberType(SgPointerMemberType* type)
   {
     usingPointerToMemberType = type;
   }

bool NameQualificationInheritedAttribute::get_containsFunctionArgumentsOfPointerMemberType()
   {
     return containsFunctionArgumentsOfPointerMemberType;
   }

void NameQualificationInheritedAttribute::set_containsFunctionArgumentsOfPointerMemberType( bool x )
   {
     containsFunctionArgumentsOfPointerMemberType = x;
   }
#endif

// *********************
// Synthesized Attribute
// *********************

NameQualificationSynthesizedAttribute::NameQualificationSynthesizedAttribute()
   {
  // Default constructor
     node = NULL;
   }

NameQualificationSynthesizedAttribute::NameQualificationSynthesizedAttribute( SgNode* astNode )
   {
  // DQ (8/2/2020): Added support for debugging.
     node = astNode;
   }


NameQualificationSynthesizedAttribute::NameQualificationSynthesizedAttribute ( const NameQualificationSynthesizedAttribute & X )
   {
  // Copy constructor.

  // DQ (8/2/2020): Added support for debugging.
     node = X.node;
   }


// *******************
// NameQualificationTraversal
// *******************

// NameQualificationTraversal::NameQualificationTraversal(
//      std::map<SgNode*,std::string> & input_qualifiedNameMapForNames,
//      std::map<SgNode*,std::string> & input_qualifiedNameMapForTypes,
//      std::map<SgNode*,std::string> & input_qualifiedNameMapForTemplateHeaders,
//      std::map<SgNode*,std::string> & input_typeNameMap,
//      std::map<SgNode*,std::map<SgNode*,std::string> > & input_qualifiedNameMapForMapsOfTypes,
//      std::set<SgNode*> & input_referencedNameSet)
NameQualificationTraversal::NameQualificationTraversal(
     NameQualificationMapType & input_qualifiedNameMapForNames,
     NameQualificationMapType & input_qualifiedNameMapForTypes,
     NameQualificationMapType & input_qualifiedNameMapForTemplateHeaders,
     NameQualificationMapType & input_typeNameMap,
     NameQualificationMapOfMapsType & input_qualifiedNameMapForMapsOfTypes,
     NameQualificationSetType & input_referencedNameSet)
   : referencedNameSet(input_referencedNameSet),
     qualifiedNameMapForNames(input_qualifiedNameMapForNames),
     qualifiedNameMapForTypes(input_qualifiedNameMapForTypes),
     qualifiedNameMapForTemplateHeaders(input_qualifiedNameMapForTemplateHeaders),
     typeNameMap(input_typeNameMap),
     qualifiedNameMapForMapsOfTypes(input_qualifiedNameMapForMapsOfTypes)
   {
  // Nothing to do here.

     explictlySpecifiedCurrentScope     = NULL;
     explictlySpecifiedCurrentStatement = NULL;

#if 0
  // DQ (8/3/2019): Output a message so that I can verify this is called one per file.
     mfprintf(mlog [ WARN ] ) ("Inside NameQualificationTraversal() constructor \n");
#endif

  // DQ (8/3/2019): Reset the static data member that holds the aliasSymbolCausalNodeSet.
     SgSymbolTable::get_aliasSymbolCausalNodeSet().clear();
     ROSE_ASSERT(SgSymbolTable::get_aliasSymbolCausalNodeSet().empty() == true);

  // DQ (7/19/2025): This is how we are turning on and off a special name qualification mode
  // required in the symbol table support.  In general, is is too expensive to be use everywhere,
  // and has a dramatic imact on the support for SgAliasSymbols within the AST_PostProcessing()
  // (specifically the support for using directives such as "using namespace std;", which can
  // take 80% of the compilation time).
  // ROSE_ASSERT(SgSymbolTable::get_name_qualification_mode() == false);

#if 0
     printf ("In NameQualificationTraversal() constructor: SgSymbolTable::get_name_qualification_mode() = %s \n",
          (SgSymbolTable::get_name_qualification_mode() == true) ? "true" : "false");
#endif

     SgSymbolTable::set_name_qualification_mode(true);

     ROSE_ASSERT(SgSymbolTable::get_name_qualification_mode() == true);

     declarationSet = NULL;

  // DQ (5/22/2024): Building a mechanism to turn off name qualification after a specific
  // template instantiation function has been processed.  This is debug code to trace down
  // a problem with name qualification growing too large and consuming all memory.
     disableNameQualification = false;

  // DQ (8/14/2025): Adding optimization (default is false) to support name qualification
  // retricted to just the input source file (instead of the whole translation unit).
     suppressNameQualificationAcrossWholeTranslationUnit = false;
   }


// DQ (5/28/2011): Added support to set the static global qualified name map in SgNode.
// const std::map<SgNode*,std::string> &
const NameQualificationTraversal::NameQualificationMapType &
NameQualificationTraversal::get_qualifiedNameMapForNames() const
   {
     return qualifiedNameMapForNames;
   }

// DQ (5/28/2011): Added support to set the static global qualified name map in SgNode.
// const std::map<SgNode*,std::string> &
const NameQualificationTraversal::NameQualificationMapType &
NameQualificationTraversal::get_qualifiedNameMapForTypes() const
   {
     return qualifiedNameMapForTypes;
   }

// DQ (3/13/2019): Added support to set the static global qualified name map in SgNode.
// const std::map<SgNode*,std::map<SgNode*,std::string> > &
// const std::map<SgNode*,NameQualificationTraversal::NameQualificationMapType> &
const NameQualificationTraversal::NameQualificationMapOfMapsType &
NameQualificationTraversal::get_qualifiedNameMapForMapsOfTypes() const
   {
     return qualifiedNameMapForMapsOfTypes;
   }

// DQ (9/7/2014): Added support to set the template headers in template declarations.
// const std::map<SgNode*,std::string> &
const NameQualificationTraversal::NameQualificationMapType &
NameQualificationTraversal::get_qualifiedNameMapForTemplateHeaders() const
   {
     return qualifiedNameMapForTemplateHeaders;
   }


int
numberOfSymbolsWithName( const SgName & name, SgScopeStatement* scope )
   {
  // DQ (6/20/2011): This function counts the number of symbols associated with the same name.
  // This function should be put into the SgScopeStatement for more general use.

  // We might have to have separate functions specific to functions, variables, etc.
  // This function addresses a requirement associated with a bug demonstrated by test2011_84.C.

     ASSERT_not_null(scope);
     SgSymbol* symbol = scope->lookup_function_symbol(name);

     int count = 0;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In numberOfSymbolsWithName(): symbol = %p scope = %p = %s \n",symbol,scope,scope->class_name().c_str());
#endif

     while (symbol != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("     In loop: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
          count++;
          symbol = scope->next_any_symbol();
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In numberOfSymbolsWithName(): count = %d \n",count);
#endif

     return count;
   }

SgDeclarationStatement*
NameQualificationTraversal::associatedDeclaration(SgScopeStatement* scope)
   {
     SgDeclarationStatement* return_declaration = NULL;
     switch (scope->variantT())
        {
          case V_SgClassDefinition:
             {
               SgClassDefinition* definition = isSgClassDefinition(scope);
               ASSERT_not_null(definition);

               SgClassDeclaration* declaration = definition->get_declaration();
               ASSERT_not_null(declaration);

               return_declaration = declaration;
               break;
             }

       // DQ (11/20/2011): Added support for template declarations (template class declarations)
          case V_SgTemplateClassDefinition:
             {
               SgTemplateClassDefinition* definition = isSgTemplateClassDefinition(scope);
               ASSERT_not_null(definition);

               SgTemplateClassDeclaration* declaration = definition->get_declaration();
               ASSERT_not_null(declaration);

               return_declaration = declaration;
               break;
             }

          case V_SgNamespaceDefinitionStatement:
             {
               SgNamespaceDefinitionStatement* definition = isSgNamespaceDefinitionStatement(scope);
               ASSERT_not_null(definition);

            // Let the first definition be used to get the associated first declaration so that we
            // are always refering to a consistant declaration for any chain of namespaces.  If not
            // the first then perhaps the last?
               while (definition->get_previousNamespaceDefinition() != NULL)
                  {
                 // mfprintf(mlog [ WARN ] ) ("Iterating through the namespace chain... \n");
                    definition = definition->get_previousNamespaceDefinition();
                  }

               SgNamespaceDeclarationStatement* declaration = definition->get_namespaceDeclaration();
               ASSERT_not_null(declaration);

               return_declaration = declaration;
               break;
             }

          case V_SgTemplateInstantiationDefn:
             {
               SgTemplateInstantiationDefn* definition = isSgTemplateInstantiationDefn(scope);
               ASSERT_not_null(definition);

               SgTemplateInstantiationDecl* declaration = isSgTemplateInstantiationDecl(definition->get_declaration());
               ASSERT_not_null(declaration);

               return_declaration = declaration;
               break;
             }

       // DQ (7/11/2014): Added this case to support test2014_84.C.
          case V_SgFunctionDefinition:
             {
               SgFunctionDefinition* definition = isSgFunctionDefinition(scope);
               ASSERT_not_null(definition);

               SgFunctionDeclaration* declaration = isSgFunctionDeclaration(definition->get_declaration());
               ASSERT_not_null(declaration);

               return_declaration = declaration;
               break;
             }

       // DQ (7/19/2017): Adding support for new SgDeclarationScope, though it might be that we want the parent defining or non-defining declaration.
          case V_SgDeclarationScope:

       // DQ (6/26/2019): Added rage-based for loop (see test2019_483.C).
          case V_SgRangeBasedForStatement:

       // Some scopes don't have an associated declaration (return NULL in these cases).
       // Also missing some of the Fortran specific scopes.
          case V_SgGlobal:
          case V_SgIfStmt:
          case V_SgWhileStmt:
          case V_SgDoWhileStmt:
          case V_SgForStatement:
          case V_SgForAllStatement:
          case V_SgBasicBlock:
          case V_SgSwitchStatement:
          case V_SgCatchOptionStmt:
          case V_SgUpcForAllStatement:
          case V_SgJovialForThenStatement:
          case V_SgAdaPackageSpec:
             {
               return_declaration = NULL;
               break;
             }

       // Catch anything that migh have been missed (and exit so it can be identified and fixed).
          default:
             {
               mfprintf(mlog [ WARN ] ) ("Default reached in NameQualificationTraversal::associatedDeclaration() scope = %s \n",scope->class_name().c_str());
               ROSE_ABORT();
             }
        }

     return return_declaration;
   }

SgDeclarationStatement*
NameQualificationTraversal::associatedDeclaration(SgType* type)
   {
     SgDeclarationStatement* return_declaration = NULL;

  // DQ (1/26/2013): Added assertion.
     ASSERT_not_null(type);

  // DQ (4/15/2019): This is a new default that appears to work well for all of our ROSE regression tests.
  // mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::associatedDeclaration(): Calling stripType() with SgType::STRIP_POINTER_MEMBER_TYPE explicitly \n");

  // DQ (Don't skip over SgPointerMemberType.
  // DQ (4/15/2019): Adding SgType::STRIP_POINTER_MEMBER_TYPE to the stripType() call.
  // We want to strip away all by typedef types.
  // SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
  // SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_POINTER_MEMBER_TYPE);
     SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
     ASSERT_not_null(strippedType);

  // switch (type->variantT())
     switch (strippedType->variantT())
        {
          case V_SgClassType:
             {
               SgClassType* classType = isSgClassType(strippedType);
               ASSERT_not_null(classType);

               SgClassDeclaration* declaration = isSgClassDeclaration(classType->get_declaration());
               ASSERT_not_null(declaration);

               return_declaration = declaration;
               break;
             }

          case V_SgTypedefType:
             {
               SgTypedefType* typedefType = isSgTypedefType(strippedType);
               ASSERT_not_null(typedefType);

               SgTypedefDeclaration* declaration = isSgTypedefDeclaration(typedefType->get_declaration());
               ASSERT_not_null(declaration);

               return_declaration = declaration;
               break;
             }

          case V_SgEnumType:
             {
               SgEnumType* enumType = isSgEnumType(strippedType);
               ASSERT_not_null(enumType);

               SgEnumDeclaration* declaration = isSgEnumDeclaration(enumType->get_declaration());
               ASSERT_not_null(declaration);

               return_declaration = declaration;
               break;
             }

       // DQ (11/20/2011): Adding support for template declarations in the AST.
          case V_SgTemplateType:
             {
               SgTemplateType* templateType = isSgTemplateType(strippedType);
               ASSERT_not_null(templateType);
#if 0
               SgTemplateDeclaration* declaration = isSgTemplateDeclaration(templateType->get_declaration());
               ASSERT_not_null(declaration);

               return_declaration = declaration;
#else
               return_declaration = NULL;
#endif
               break;
             }

          case V_SgFunctionType:
          case V_SgMemberFunctionType:
             {
            // Not clear if I have to resolve declarations associated with function types.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 0)
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::associatedDeclaration(): Case of SgFunctionType not implemented strippedType = %s \n",strippedType->class_name().c_str());
#endif
               return_declaration = NULL;
               break;
             }

       // DQ (6/25/2011): Demonstrated by calling unparseToString on all possible type.
          case V_SgTypeDefault:

       // Some scopes don't have an associated declaration (return NULL in these cases).
       // Also missing some of the Fortran specific scopes.
          case V_SgTypeInt:
          case V_SgTypeUnsignedLong:
          case V_SgTypeUnsignedLongLong:
          case V_SgTypeUnsignedChar:
          case V_SgTypeUnsignedShort:
          case V_SgTypeUnsignedInt:
          case V_SgTypeSignedChar:
          case V_SgTypeSignedShort:
          case V_SgTypeSignedInt:
          case V_SgTypeSignedLong:
          case V_SgTypeSignedLongLong:

       // DQ (11/6/2014): Added support for C++11 rvalue references.
          case V_SgRvalueReferenceType:

       // DQ (3/24/2014): Added support for 128-bit integers.
          case V_SgTypeSigned128bitInteger:
          case V_SgTypeUnsigned128bitInteger:

       // DQ (7/30/2014): Adding C++11 support.
          case V_SgTypeNullptr:

       // DQ (8/12/2014): Adding C++11 support.
          case V_SgDeclType:

       // DQ (3/28/2015): Adding GNU C language extension.
          case V_SgTypeOfType:

       // DQ (4/29/2016): Added support for complex types.
          case V_SgTypeComplex:
          //  pp (7/16/2016) Matrix and tuple  (Matlab)
          case V_SgTypeMatrix:
          case V_SgTypeTuple:
          case V_SgTypeShort:
          case V_SgTypeLong:
          case V_SgTypeLongLong:
          case V_SgTypeVoid:
          case V_SgTypeChar:

       // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
          case V_SgTypeChar16:
          case V_SgTypeChar32:

          case V_SgTypeFloat:
          case V_SgTypeDouble:
          case V_SgTypeLongDouble:
          case V_SgTypeBool:
          case V_SgTypeWchar:

          case V_SgTypeFloat80:
          case V_SgTypeFloat128:

          case V_SgTypeFixed:
          case V_SgJovialTableType:
       // TV (09/06/2018): Type of an unresolved auto keyword
          case V_SgAutoType:
             {
               return_declaration = NULL;
               break;
             }

          case V_SgNonrealType:
             {
               SgNonrealType * nrtype = isSgNonrealType(strippedType);
               ASSERT_not_null(nrtype);
               return_declaration = nrtype->get_declaration();
               ASSERT_not_null(return_declaration);
               break;
             }

       // DQ (4/10/2019): Needing to support this case after recompiling ROSE (debugging SgPointerMemberType
       // and cleaning up handling of SgInitializedName name qualification support).
          case V_SgTypeEllipse:
             {
               return_declaration = NULL;
               break;
             }

       // DQ (4/11/2019): This case appears in the testRoseHeaders_03.C test code (ROSE compiling ROSE).
          case V_SgTypeUnknown:
             {
               return_declaration = NULL;
               break;
             }

       // DQ (4/12/2019): This case appears in the roseTests/astInterfaceTests/inputbuildIfStmt.C code.
          case V_SgTypeString:
             {
               return_declaration = NULL;
               break;
             }

       // DQ (4/18/2019): This case is required because we need to process chains of SgPointerMemberType IR nodes (see test2019_373.C).
          case V_SgPointerMemberType:
             {
               SgPointerMemberType* pointerMemberType = isSgPointerMemberType(strippedType);
               return_declaration = pointerMemberType->get_class_declaration_of();
               ASSERT_not_null(return_declaration);
               break;
             }
          // Liao, Oct 4, 2021. We skip translation of Ada AST from some system packages. We also ignore them in the unparser.
          case V_SgAdaSubtype:
          case V_SgAdaModularType:
          case V_SgAdaDerivedType:
          case V_SgAdaDiscreteType:
          case V_SgAdaAccessType:
             {
               return_declaration = NULL;
               break;
             }

       // Catch anything that migh have been missed (and exit so it can be identified and fixed).
          default:
             {
               mfprintf(mlog [ WARN ] ) ("Default reached in NameQualificationTraversal::associatedDeclaration() type = %s strippedType = %s \n",type->class_name().c_str(),strippedType->class_name().c_str());
               ROSE_ABORT();
             }
        }

     return return_declaration;
   }


void
NameQualificationTraversal::evaluateTemplateInstantiationDeclaration ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // DQ (9/23/2012): Added assertions.
     ASSERT_not_null(declaration);
     ASSERT_not_null(currentScope);
     ASSERT_not_null(positionStatement);

#define DEBUG_TEMPINSTDECL 0

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NONTERMINATION
     mfprintf(mlog [ WARN ] ) ("11111111111111111111111111111111111111111111111111111111111111111111 \n");
     mfprintf(mlog [ WARN ] ) ("In evaluateTemplateInstantiationDeclaration(): declaration = %p = %s currentScope = %p = %s positionStatement = %p = %s \n",
          declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());
#endif
#if DEBUG_TEMPINSTDECL
     printf ("11111111111111111111111111111111111111111111111111111111111111111111 \n");
     printf ("In evaluateTemplateInstantiationDeclaration(): declaration = %p = %s currentScope = %p = %s positionStatement = %p = %s \n",
          declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());
#endif
  // DQ (10/31/2015): This code is designed to eliminate the infinite recursion possible in some rare cases of
  // template instantiation (see test2015_105.C extracted from ROSE compiling ROSE header files and the boost
  // usage present there).  Note that this could be restricted to the handling of SgTemplateInstantiationDecl
  // instead (I think).  But it might be that I have just not yet seen a recursive case using template functions
  // instantiations, template member function instantiations and template variable instantiations.
     SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
     SgClassDefinition* nonconst_def = templateInstantiationDeclaration != NULL ? isSgClassDefinition(templateInstantiationDeclaration->get_definition()) : NULL;

#if DEBUG_TEMPINSTDECL
  // Note that nonconst_def can be NULL when the template instantiation has no defining declaration and is used in a template instantiation directive (see test2017_56.C).
  // mfprintf(mlog [ WARN ] ) ("In evaluateTemplateInstantiationDeclaration(): nonconst_def = %p \n",nonconst_def);
     printf ("In evaluateTemplateInstantiationDeclaration(): nonconst_def = %p \n",nonconst_def);
#endif

  // DQ (5/22/2024): Count the number of function invocations so that we can turn on forceSkip selectively.
     static size_t functionCallCounter = 0;

     functionCallCounter++;

#if DEBUG_TEMPINSTDECL
     printf ("In evaluateTemplateInstantiationDeclaration(): functionCallCounter = %zu \n",functionCallCounter);
#endif

     bool forceSkip = false;

#if 0
  // DQ (5/12/2024): Skipping the handling of template instantiations in the name qualification as a test!
  // forceSkip = true;
  // if (functionCallCounter >= 50000) // working
  // if (functionCallCounter >= 70000) // working
  // if (functionCallCounter >= 90000) // working
  // if (functionCallCounter >= 91000) // working
  // if (functionCallCounter >= 93000) // max is 91224
  // if (functionCallCounter >= 120000) // max is 91224? it changes as we increase the bound.
     if (functionCallCounter >= 150000) // max is 149764 before failing.
        {
       // DQ (5/22/2024): Use the disableNameQualification flag and set it to true when the qualified name sizes reach 5000 or so in length, then just disable further name qualification.
          forceSkip = true;
        }
#endif
#if 1
  // DQ (5/22/2024): Building a mechanism to turn off name qualification after a specific
  // template instantiation function has been processed.  This is debug code to trace down
  // a problem with name qualification growing too large and consuming all memory.
     if (disableNameQualification == true)
        {
#if DEBUG_NONTERMINATION || DEBUG_TEMPINSTDECL || 0
          printf ("In evaluateTemplateInstantiationDeclaration(): Setting forceSkip = true \n");
#endif
          forceSkip = true;
        }
#endif

  // DQ (5/12/2024): Make sure that we don't have any NULL entries.
     ROSE_ASSERT(MangledNameSupport::visitedTemplateDefinitions.find(NULL) == MangledNameSupport::visitedTemplateDefinitions.end());

  // if (MangledNameSupport::visitedTemplateDefinitions.find(nonconst_def) != MangledNameSupport::visitedTemplateDefinitions.end())
     if (forceSkip == true || MangledNameSupport::visitedTemplateDefinitions.find(nonconst_def) != MangledNameSupport::visitedTemplateDefinitions.end())
        {
       // Skip the call that would result in infinte recursion.
#if DEBUG_NONTERMINATION
          mfprintf(mlog [ WARN ] ) ("In nameQualificationSupport.C: evaluateTemplateInstantiationDeclaration(): skipping the call to process the template class instantiation definition: def = %p = %s \n",declaration,declaration->class_name().c_str());
          mfprintf(mlog [ WARN ] ) (" --- declaration name = %s \n",SageInterface::get_name(declaration).c_str());
#endif
        }
       else
        {
       // Only handle the case of a SgTemplateInstantiationDecl.
          SgClassDefinition* templateInstantiationDefinition = isSgTemplateInstantiationDefn(nonconst_def);
          if (templateInstantiationDefinition != NULL)
             {
#if 0
               mfprintf(mlog [ WARN ] ) ("Insert templateInstantiationDefinition = %p into visitedTemplateDeclarations (stl set)\n",templateInstantiationDefinition);
#endif
            // Not clear why we need to use an iterator to simply insert a pointer into the set.
            // SgTemplateInstantiationDefn* nonconst_templateInstantiationDefinition = const_cast<SgTemplateInstantiationDefn*>(templateInstantiationDefinition);
#if 0
            // DQ (5/21/2024): Old code.
               MangledNameSupport::setType::iterator it = MangledNameSupport::visitedTemplateDefinitions.begin();
            // MangledNameSupport::visitedTemplateDeclarations.insert(it,nonconst_templateInstantiationDefinition);
               MangledNameSupport::visitedTemplateDefinitions.insert(it,nonconst_def);
#else
            // DQ (5/21/2024): New code using single argument for inserting to a set (this is the way it is supposed to work).
               MangledNameSupport::visitedTemplateDefinitions.insert(nonconst_def);
#endif
             }

       // DQ (11/1/2015): Indented this code (switch statement) to conform to new block layout.
       // DQ (6/1/2011): Added support for template arguments.
          switch (declaration->variantT())
             {
               case V_SgTemplateInstantiationDecl:
                  {
                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
                    ASSERT_not_null(templateInstantiationDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPINSTDECL
                 // mfprintf(mlog [ WARN ] ) ("$$$$$$$$$ --- templateInstantiationDeclaration = %p \n",templateInstantiationDeclaration);
                    mfprintf(mlog [ WARN ] ) ("$$$$$$$$$ --- templateInstantiationDeclaration = %p templateInstantiationDeclaration->get_templateArguments().size() = %" PRIuPTR " \n",templateInstantiationDeclaration,templateInstantiationDeclaration->get_templateArguments().size());
#endif

                 // Evaluate all template arguments.
                    evaluateNameQualificationForTemplateArgumentList (templateInstantiationDeclaration->get_templateArguments(),currentScope,positionStatement);
                    break;
                  }

               case V_SgTemplateInstantiationFunctionDecl:
                  {
                    SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(declaration);
                    ASSERT_not_null(templateInstantiationFunctionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPINSTDECL
                    mfprintf(mlog [ WARN ] ) ("$$$$$$$$$ --- templateInstantiationFunctionDeclaration = %p \n",templateInstantiationFunctionDeclaration);
#endif
                 // Evaluate all template arguments.
                    evaluateNameQualificationForTemplateArgumentList (templateInstantiationFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
                    break;
                  }

               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(declaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPINSTDECL
                    mfprintf(mlog [ WARN ] ) ("$$$$$$$$$ --- templateInstantiationMemberFunctionDeclaration = %p \n",templateInstantiationMemberFunctionDeclaration);
#endif
                    ASSERT_not_null(templateInstantiationMemberFunctionDeclaration);

                 // Evaluate all template arguments.
                 // evaluateNameQualificationForTemplateArgumentList (templateInstantiationMemberFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
                    SgTemplateArgumentPtrList & templateArgumentList = templateInstantiationMemberFunctionDeclaration->get_templateArguments();
                    evaluateNameQualificationForTemplateArgumentList(templateArgumentList,currentScope,positionStatement);
                    break;
                  }

            // DQ (3/31/2018): Added code to help debug strange case (see Cxx11_tests/test2018_68.C).
               case V_SgTemplateFunctionDeclaration:
                  {
                 // Actually there is nothing to do here.
#if DEBUG_NONTERMINATION
                    mfprintf(mlog [ WARN ] ) ("Note: Template arguments being evaluated against template declaration (which has only template parameters) \n");
#endif
#if 0
                    mfprintf(mlog [ WARN ] ) ("Exiting as a test! \n");
                    ROSE_ABORT();
#endif
                    break;
                  }

            // DQ (4/14/2018): Added case for template typedef instantiations (see test2018_83.C for an example where name qualification of the template arguments is required).
               case V_SgTemplateInstantiationTypedefDeclaration:
                  {
                    SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(declaration);
                    ASSERT_not_null(templateInstantiationTypedefDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPINSTDECL
                    mfprintf(mlog [ WARN ] ) ("$$$$$$$$$ --- templateInstantiationTypedefDeclaration = %p \n",templateInstantiationTypedefDeclaration);
#endif
                 // Evaluate all template arguments.
                    evaluateNameQualificationForTemplateArgumentList (templateInstantiationTypedefDeclaration->get_templateArguments(),currentScope,positionStatement);
                    break;
                  }

               case V_SgNonrealDecl:
                  {
                    SgNonrealDecl* nrdecl = isSgNonrealDecl(declaration);
                    ASSERT_not_null(nrdecl);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPINSTDECL
                    mfprintf(mlog [ WARN ] ) ("$$$$$$$$$ --- nrdecl = %p \n",nrdecl);
#endif
                    evaluateNameQualificationForTemplateArgumentList (nrdecl->get_tpl_args(),currentScope,positionStatement);

                    break;
                  }

               default:
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPINSTDECL
                    mfprintf(mlog [ WARN ] ) ("$$$$$$$$$ --- This IR node does not contain template arguments to process: declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
                  }
             }

       // DQ (10/31/2015): The rule here is that after processing as a mangled name we remove the
       // template instantiation from the list so that other non-nested uses of the template
       // instantiation will force the manged name to be generated.
          if (templateInstantiationDefinition != NULL)
             {
#if 0
               mfprintf(mlog [ WARN ] ) ("Erase templateInstantiationDefinition = %p from visitedTemplateDeclarations (stl set)\n",templateInstantiationDefinition);
#endif
               MangledNameSupport::visitedTemplateDefinitions.erase(nonconst_def);
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPINSTDECL
     mfprintf(mlog [ WARN ] ) ("Leaving evaluateTemplateInstantiationDeclaration(): declaration = %p = %s currentScope = %p = %s positionStatement = %p = %s \n",
          declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());
#endif
   }


int
NameQualificationTraversal::nameQualificationDepthOfParent(SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // Now resolve how much name qualification is required.
     int qualificationDepth = 0;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("***** Inside of NameQualificationTraversal::nameQualificationDepthOfParent() ***** \n");
     mfprintf(mlog [ WARN ] ) ("   declaration  = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
     mfprintf(mlog [ WARN ] ) ("   currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
#endif

  // qualificationDepth++;
  // SgDeclaration* classOrNamespaceDefinition = classDefinition->get_declaration()->get_scope();
     SgScopeStatement* parentScope = declaration->get_scope();
  // SgName parentName = associatedName(parentScope);

  // DQ (6/24/2018): Added assertion.
     ASSERT_not_null(parentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("***** Inside of NameQualificationTraversal::nameQualificationDepthOfParent() ***** \n");
     mfprintf(mlog [ WARN ] ) ("   parentScope = %p = %s = %s \n",parentScope,parentScope->class_name().c_str(),SageInterface::get_name(parentScope).c_str());
#endif

  // qualificationDepth = nameQualificationDepth(parentName,parentScope,positionStatement) + 1;
     SgGlobal* globalScope = isSgGlobal(parentScope);
     if (globalScope != NULL)
        {
       // There is no declaration associated with global scope so we have to process the case of a null pointer...
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 1)
          mfprintf(mlog [ WARN ] ) ("parentDeclaration == NULL: parentScope = %p = %s \n",parentScope,parentScope->class_name().c_str());
#endif
#if 0
          mfprintf(mlog [ WARN ] ) ("This case is not handled yet! \n");
          ROSE_ABORT();
#endif
        }
       else
        {
       // Now ask the same question recursively using the parent declaration and the same currentScope (is it visible from the same point in the code).
          SgDeclarationStatement* parentDeclaration = associatedDeclaration(parentScope);

       // In some cases the declaration can be in a scope with is not associated with a declaration (e.g. SgBasicBlock or SgForStatement).
          if (parentDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepthOfParent(): calling nameQualificationDepth(): parentDeclaration = %p = %s = %s \n",
                    parentDeclaration,parentDeclaration->class_name().c_str(),SageInterface::get_name(parentDeclaration).c_str());
               mfprintf(mlog [ WARN ] ) ("   --- currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("   --- parentScope = %p = %s \n",parentScope,parentScope->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
            // qualificationDepth = nameQualificationDepth(parentDeclaration,parentScope,positionStatement);
               qualificationDepth = nameQualificationDepth(parentDeclaration,currentScope,positionStatement);
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
// #if 0
     mfprintf(mlog [ WARN ] ) ("Leaving nameQualificationDepthOfParent(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif

     return qualificationDepth;
   }

bool
NameQualificationTraversal::requiresTypeElaboration(SgSymbol* symbol)
   {
  // DQ (5/14/2011): type elaboration only works between non-types and types.  Different types must be distinquished using name qualification.
  // If this is a type then since all types are given equal weight we need more name qualification to distinquish them.
  // However, if this is a non-type then type elaboration is sufficent to distinquish the type (e.g. from a variable name).
     bool typeElaborationRequired = false;

     ASSERT_not_null(symbol);
     switch (symbol->variantT())
        {
          case V_SgClassSymbol:
          case V_SgTemplateClassSymbol:
          case V_SgTemplateMemberFunctionSymbol:
          case V_SgTemplateFunctionSymbol:
          case V_SgTemplateTypedefSymbol:
          case V_SgFunctionSymbol:
          case V_SgMemberFunctionSymbol:
          case V_SgTemplateVariableSymbol:
          case V_SgVariableSymbol:
             {
               typeElaborationRequired = true;
               break;
             }

          case V_SgEnumFieldSymbol:
          case V_SgEnumSymbol:
          case V_SgNamespaceSymbol: // Note sure about this!!!
          case V_SgTemplateSymbol: // Note sure about this!!!
          case V_SgNonrealSymbol: // Note sure about this!!!
          case V_SgAdaGenericSymbol: // Note sure about this!!!
          case V_SgTypedefSymbol:
             {
               typeElaborationRequired = false;
               break;
             }

          case V_SgAliasSymbol:
             {
            // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
               ROSE_ABORT();
             }

          default:
             {
               mfprintf(mlog [ WARN ] ) ("Default reached in NameQualificationTraversal::requiresTypeElaboration(): symbol = %p = %s \n",symbol,symbol->class_name().c_str());
               ROSE_ABORT();
             }
        }

     return typeElaborationRequired;
   }


void
NameQualificationTraversal::processNameQualificationArrayType(SgArrayType* arrayType, SgScopeStatement* currentScope)
   {
  // Note that we may have to traverse base types in case they include other SgArrayType IR nodes where their index requires name qualification.
  // SgType* strippedArrayType = arrayType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE);
  // ASSERT_not_null(strippedArrayType);

     SgExpression* index = arrayType->get_index();
     if (index != NULL)
        {
#if 0
          mfprintf(mlog [ WARN ] ) ("~~~~~ Handling case of SgArrayType: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
          mfprintf(mlog [ WARN ] ) ("~~~~~ Handling case of SgArrayType: index        = %p = %s \n",index,index->class_name().c_str());
#endif
       // DQ (7/23/2011): This will not work since the current scope is not know and can't be determined from the type (which is shared).
          ASSERT_not_null(currentScope);
          generateNestedTraversalWithExplicitScope(index,currentScope);
#if 0
          mfprintf(mlog [ WARN ] ) ("DONE: Support name qualification on the array index expression if required (recursive call) \n");
#endif
        }

   }

void
NameQualificationTraversal::processNameQualificationForPossibleArrayType(SgType* possibleArrayType, SgScopeStatement* currentScope)
   {
  // DQ (7/23/2011): Refactored support for name qualification of the index expressions used in array types.

#if 0
     mfprintf(mlog [ WARN ] ) ("In processNameQualificationForPossibleArrayType(): possibleArrayType = %p = %s \n",possibleArrayType,possibleArrayType->class_name().c_str());
#endif

     SgType* strippedPossibleArrayType = possibleArrayType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE);
     ASSERT_not_null(strippedPossibleArrayType);
     SgArrayType* arrayType = isSgArrayType(strippedPossibleArrayType);
     if (arrayType != NULL)
        {
          processNameQualificationArrayType(arrayType,currentScope);

       // Now process the base type, since it might be part of a multi-dimentional array type (in C/C++ these are a chain of array types).
          processNameQualificationForPossibleArrayType(arrayType->get_base_type(),currentScope);
        }
   }



void
NameQualificationTraversal::functionReport(SgFunctionDeclaration* functionDeclaration)
   {
  // Report on the details of functions as part of debugging.  This function supports
  // an analysis of the use of the getline function in iostream header file.  It is used
  // as a template, it is overloaded, and it is instantiationed in several ways.  As a
  // result it is a subject for a case study in the name qualification to eveluate the
  // influence of different instances (kinds) of the same function function and how
  // the logic for name qualification and detection of functions hiding other functions
  // works to drive the name qualification.

  // SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
     if (functionDeclaration != NULL)
        {
          string name = functionDeclaration->get_name();
          std::size_t pos = name.find("getline");
          if (pos != string::npos)
             {
               mfprintf(mlog [ WARN ] ) ("found getline function: pos = %zu \n",pos);
               mfprintf(mlog [ WARN ] ) ("   --- function name = %s \n",name.c_str());
               mfprintf(mlog [ WARN ] ) ("   --- functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("   --- function mangled name = %s \n",functionDeclaration->get_mangled_name().str());
               SgTemplateInstantiationFunctionDecl*       templateInstantiationFunctionDecl       = isSgTemplateInstantiationFunctionDecl(functionDeclaration);
               SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(functionDeclaration);

               if (templateInstantiationFunctionDecl != NULL)
                  {
                    SgTemplateFunctionDeclaration* templateFunctionDeclaration = isSgTemplateFunctionDeclaration(templateInstantiationFunctionDecl->get_templateDeclaration());
                    ASSERT_not_null(templateFunctionDeclaration);
                    mfprintf(mlog [ WARN ] ) ("   --- templateFunctionDeclaration = %p = %s \n",templateFunctionDeclaration,templateFunctionDeclaration->class_name().c_str());
                    mfprintf(mlog [ WARN ] ) ("   --- templateFunctionDeclaration->get_file_info()->get_filename() = %s \n",templateFunctionDeclaration->get_file_info()->get_filename());
                    mfprintf(mlog [ WARN ] ) ("   --- templateFunctionDeclaration->get_file_info()->get_line()     = %d \n",templateFunctionDeclaration->get_file_info()->get_line());
                  }
                 else
                  {
                    if (templateInstantiationMemberFunctionDecl != NULL)
                       {
                         SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = isSgTemplateMemberFunctionDeclaration(templateInstantiationMemberFunctionDecl->get_templateDeclaration());
                         ASSERT_not_null(templateMemberFunctionDeclaration);
                         mfprintf(mlog [ WARN ] ) ("   --- templateMemberFunctionDeclaration = %p = %s \n",templateMemberFunctionDeclaration,templateMemberFunctionDeclaration->class_name().c_str());
                         mfprintf(mlog [ WARN ] ) ("   --- templateMemberFunctionDeclaration->get_file_info()->get_filename() = %s \n",templateMemberFunctionDeclaration->get_file_info()->get_filename());
                         mfprintf(mlog [ WARN ] ) ("   --- templateMemberFunctionDeclaration->get_file_info()->get_line()     = %d \n",templateMemberFunctionDeclaration->get_file_info()->get_line());
                       }
                      else
                       {
                         mfprintf(mlog [ WARN ] ) ("   --- functionDeclaration->get_file_info()->get_filename() = %s \n",functionDeclaration->get_file_info()->get_filename());
                         mfprintf(mlog [ WARN ] ) ("   --- functionDeclaration->get_file_info()->get_line()     = %d \n",functionDeclaration->get_file_info()->get_line());
                       }
                  }

               mfprintf(mlog [ WARN ] ) ("   --- functionDeclaration->isCompilerGenerated() = %s \n",functionDeclaration->isCompilerGenerated() ? "true" : "false");
               mfprintf(mlog [ WARN ] ) ("   --- functionDeclaration->isTransformation()    = %s \n",functionDeclaration->isTransformation()    ? "true" : "false");
            // mfprintf(mlog [ WARN ] ) ("   --- function: %s \n",functionDeclaration->unparseToString().c_str());
               if (functionDeclaration->isCompilerGenerated() == true)
                  {
                    ROSE_ASSERT(functionDeclaration->isTransformation() == false);

                    functionDeclaration->setTransformation();
                    functionDeclaration->setOutputInCodeGeneration();

                    mfprintf(mlog [ WARN ] ) ("   --- (before unparse) functionDeclaration->isTransformation()    = %s \n",functionDeclaration->isTransformation()    ? "true" : "false");

                    mfprintf(mlog [ WARN ] ) ("   --- function: %s \n",functionDeclaration->unparseToString().c_str());

                    functionDeclaration->unsetOutputInCodeGeneration();
                    functionDeclaration->unsetTransformation();
#if 0
                    mfprintf(mlog [ WARN ] ) ("Exiting as a test! \n");
                    ROSE_ABORT();
#endif
                  }
                 else
                  {
                 // mfprintf(mlog [ WARN ] ) ("   --- (non-compiler-generated) function: %s \n",functionDeclaration->unparseToString().c_str());
                    mfprintf(mlog [ WARN ] ) ("   --- function: %s \n",functionDeclaration->unparseToString().c_str());
                  }
             }
        }
   }

#define DEBUG__nameQualificationDepth 0

int
NameQualificationTraversal::nameQualificationDepth ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement, bool forceMoreNameQualification )
   {
  // Note that the input must be a declaration because it can include enums (SgDeclarationStatement IR nodes)
  // that don't have a corresponding definition (SgScopeStatement IR nodes).

  // This function computes the number of qualified names required to uniquely qualify and input reference.
  // It evaluates how much name qualification is required (typically 0 (no qualification), but sometimes
  // the depth of the nesting of scopes plus 1 (full qualification with global scoping operator)).

  // The positionStatement is the position of the associated reference to the declaration.
  // It is used when "using declarations" are not at the top of the scope.  Initially
  // we will assume that the such "using declarations" are at the top of the scope.

  // How this works:
  // The function inputs are:
  //    1) the declaration whose reference we are evaluating for name qualification.
  //    2) the current scope of the reference to the declaration (the computed name qualification will be
  //       prepended to the name of the declaration which is a reference to the declaration passed to this function).
  //    3) The positonStatement is required to allow name qualification decisions to be based on the reference
  //       declarations position in scope relative to the input declaration.  Some function declarations require
  //       name qualification depending on if they appear before or after a prototype declaration for the function
  //       that would define it's scope (seperate from its visability.

  // Note: we are evaluating the name qualification for references to declarations (e.g. the defining member
  // function outside of the class which contains the non-defining (prototype) member function declaration).

  // At this point, the symbol for the input function has been looked up in the parent scope of the declaration we are
  // evaluating for name qualification.  The lookup is for any symbol matching the name, not the name plus the kind of
  // declaration (which will come in a next step). If it is not found, then it is not visible, and thus at least one level
  // of name qualification will be required.  If it is found then we need an additional step to decide if the existence of
  // some declaration with that name will force name elaboration (type elaboration) or name qualification.

  // A test is done on the kind of symbol to determine if its associated declaration will force qualification or type
  // elaboration.  If the symbol kind (of the kind of declaration associated with the symbol) matches the input declaration
  // then name qualification will be required, if it is a different kind of declaration the type elaboration might be all
  // that is required.  For example, a variable name may be the same as a class name, but where this happens, only type
  // elaboration is required to distinquish the two.  However, if a declaration name is classing with another declaration
  // name of the same kind, then only name qualification will distinguish the two.

  // If name qualification is required, then we repeat the lookup in the symbol table, using a more refined search to only
  // get symbols of the type that would force name qualification.

  // Note that this function is recursive, the current scope will remain fixed, but the declaration associated with the
  // target decaration will be walked up in the AST toward the global scope, each time computing the associated declaration
  // for each new scope where we evaluated.  The count of the number of scopes (scope depth) required for name qualification
  // (until the scope containing the input declaration is found) is all that this function returns.

  // Note: this function is overloaded to take other kinds of IR nodes that require name qualification, but are not
  // SgDeclarationStatements (e.g. SgInitializedName).

     ASSERT_not_null(declaration);
     ASSERT_not_null(currentScope);
     ROSE_ASSERT(forceMoreNameQualification == false);
     ASSERT_not_null(positionStatement);

     int  qualificationDepth        = 0;
     bool typeElaborationIsRequired = false;

#define DEBUG_FUNCTION_RESOLUTION 0

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("\n\n########################################################################## \n");
     mfprintf(mlog [ WARN ] ) ("########################################################################## \n");
     mfprintf(mlog [ WARN ] ) ("########################################################################## \n");
     mfprintf(mlog [ WARN ] ) ("########################################################################## \n");
     mfprintf(mlog [ WARN ] ) ("##### Inside of NameQualificationTraversal::nameQualificationDepth() ##### \n");
     mfprintf(mlog [ WARN ] ) ("########################################################################## \n");
     mfprintf(mlog [ WARN ] ) ("########################################################################## \n");
     mfprintf(mlog [ WARN ] ) ("########################################################################## \n");

  // The use of SageInterface::generateUniqueName() can cause the unparser to be called and triggers the name
  // qualification recursively but only for template declaration (SgTemplateInstantiationDecl, I think).
  // mfprintf(mlog [ WARN ] ) ("declaration  = %p = %s = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str(),SageInterface::generateUniqueName(declaration,true).c_str());
     mfprintf(mlog [ WARN ] ) ("   --- declaration  = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
     declaration->get_startOfConstruct()->display("declaration");
     mfprintf(mlog [ WARN ] ) ("   --- currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
     currentScope->get_startOfConstruct()->display("currentScope");
     mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
     positionStatement->get_startOfConstruct()->display("positionStatement");
#endif

#if DEBUG__nameQualificationDepth
     printf("Enter NameQualificationTraversal::nameQualificationDepth()\n");
     printf("  declaration       = %p = %s\n", declaration, declaration->class_name().c_str());
     printf("  currentScope      = %p = %s\n", currentScope, currentScope->class_name().c_str());
     printf("  positionStatement = %p = %s\n", positionStatement, positionStatement->class_name().c_str());
     printf("  forceMoreNameQualification = %s\n", forceMoreNameQualification ? "true" : "false");
#endif

     SgNonrealDecl*                        nonrealDecl               = isSgNonrealDecl(declaration);
     SgClassDeclaration*                   classDeclaration          = isSgClassDeclaration(declaration);
     SgVariableDeclaration*                variableDeclaration       = isSgVariableDeclaration(declaration);
     SgFunctionDeclaration*                functionDeclaration       = isSgFunctionDeclaration(declaration);
     SgTypedefDeclaration*                 typedefDeclaration        = isSgTypedefDeclaration(declaration);
     SgTemplateDeclaration*                templateDeclaration       = isSgTemplateDeclaration(declaration);
     SgEnumDeclaration*                    enumDeclaration           = isSgEnumDeclaration(declaration);
     SgNamespaceDeclarationStatement*      namespaceDeclaration      = isSgNamespaceDeclarationStatement(declaration);
     SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(declaration);
     ROSE_ASSERT(
         classDeclaration != NULL ||
         namespaceDeclaration != NULL ||
         namespaceAliasDeclaration != NULL ||
         variableDeclaration != NULL ||
         functionDeclaration != NULL ||
         typedefDeclaration != NULL ||
         templateDeclaration != NULL ||
         enumDeclaration != NULL ||
         nonrealDecl != NULL
     );
     SgName name = (nonrealDecl               != NULL) ? nonrealDecl->get_name()          :
                   (classDeclaration          != NULL) ? classDeclaration->get_name()     :
                   (namespaceDeclaration      != NULL) ? namespaceDeclaration->get_name() :
                   (namespaceAliasDeclaration != NULL) ? namespaceAliasDeclaration->get_name() :
                   (variableDeclaration       != NULL) ? SageInterface::getFirstInitializedName(variableDeclaration)->get_name() :
                   (functionDeclaration       != NULL) ? functionDeclaration->get_name()  :
                   (typedefDeclaration        != NULL) ? typedefDeclaration->get_name()   :
                   (templateDeclaration       != NULL) ? templateDeclaration->get_name()  :
                   (enumDeclaration           != NULL) ? enumDeclaration->get_name()      : "unknown_name";

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In nameQualificationDepth(SgDeclarationStatement*,...): declaration = %p = %s name = %s \n",declaration,declaration->class_name().c_str(),name.str());
#endif

  // DQ (8/16/2013): Build the template parameters and template arguments as appropriate (will be NULL pointers for some types of declarations).
     SgTemplateParameterPtrList* templateParameterList = SageBuilder::getTemplateParameterList(declaration);
     SgTemplateArgumentPtrList*  templateArgumentList  = SageBuilder::getTemplateArgumentList(declaration);

        {
       // Note that there can be more than one symbol if the name is hidden in a base class scope (and thus there are SgAliasSymbols using the same name).
          ASSERT_not_null(currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Initial lookup: name = %s currentScope = %p = %s \n",name.str(),currentScope,currentScope->class_name().c_str());
#endif

       // DQ (8/16/2013): Added support for more precise symbol lookup (which requires the template parameters and template arguments).
       // DQ 8/21/2012): this is looking in the parent scopes of the currentScope and thus not including the currentScope.
       // This is a bug for test2011_31.C where there is a variable who's name hides the name in the parent scopes (and it not detected).
          SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope,templateParameterList,templateArgumentList);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
          mfprintf(mlog [ WARN ] ) ("Initial lookup: symbol = %p = %s \n",symbol,(symbol != NULL) ? symbol->class_name().c_str() : "NULL");
#endif

       // DQ (4/12/2014): we need to record that there was another function identified in the parent scopes that we will want to have force name qualification.
          bool foundAnOverloadedFunctionWithSameName = false;
          bool foundAnOverloadedFunctionInSameScope  = false;

       // DQ (2/14/2019): Save a copy of the symbol looked up by name so that we can resolve if a
       // variable hides a type (which is where name qualification is not appropriate).
          SgSymbol* original_symbol_lookedup_by_name = symbol;

          if (symbol != NULL)
             {
               SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);
               ROSE_ASSERT(aliasSymbol == NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
            // We have to check the kind of declaration against the kind of symbol found. A local variable (for example)
            // could hide the same name used for the declaration.  This if we find symbol inconsistant with the declaration
            // then we need some form of qualification (sometimes just type elaboration).
               mfprintf(mlog [ WARN ] ) ("### Targeting a declaration = %p = %s \n",declaration,declaration->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("   --- declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
               mfprintf(mlog [ WARN ] ) ("   --- declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());
#endif
               switch (declaration->variantT())
                  {
                 // DQ (12/26/2011): Added support for template class declarations (part of new design for template declarations).
                    case V_SgTemplateClassDeclaration:
                    case V_SgClassDeclaration:
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                         ASSERT_not_null(classDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("classDeclaration name = %s \n",classDeclaration->get_name().str());
#endif
                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                      // ASSERT_not_null(classSymbol);
                         if (classSymbol == NULL)
                            {
                           // This is only type elaboration if it is a variable that is the conflict, if it is a typedef then more qualification is required. (see test2011_37.C).
                           // mfprintf(mlog [ WARN ] ) ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                           // typeElaborationIsRequired = true;
                              if (requiresTypeElaboration(symbol) == true)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                                   typeElaborationIsRequired = true;

                                // DQ (2/13/2019): Adding more name qualification (debugging test2011_33.C).
                                // forceMoreNameQualification = true;
                                 }
                                else
                                 {
                                // DQ (2/13/2019): I think we need to check if a qualified nondefining declaration
                                // has been made for this class, else no qualification should be output.
                                   SgDeclarationStatement* declarationToSearchForInReferencedNameSet =
                                        declaration->get_firstNondefiningDeclaration() != NULL ? declaration->get_firstNondefiningDeclaration() : declaration;
                                   ASSERT_not_null(declarationToSearchForInReferencedNameSet);
                                   bool skipNameQualification = false;
                                   if (referencedNameSet.find(declarationToSearchForInReferencedNameSet) == referencedNameSet.end())
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("   --- $$$$$$$$$$ NOT Found: declaration %p = %s in referencedNameSet referencedNameSet.size() = %" PRIuPTR " \n",
                                             declaration,declaration->class_name().c_str(),referencedNameSet.size());
#endif
                                        skipNameQualification = true;
                                      }
                                     else
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("   --- $$$$$$$$$$ FOUND: declaration %p = %s in referencedNameSet \n",declaration,declaration->class_name().c_str());
#endif
                                      }

                                // Check if a nondefining declaration has been seen already, if so then this may be a non-defining
                                // or defining declaration in another scope and they name qualification would be required.
                                // forceMoreNameQualification = true;
                                   if (skipNameQualification == false)
                                      {
                                        forceMoreNameQualification = true;
                                      }
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("Forcing an extra level of name qualification forceMoreNameQualification = %s \n",forceMoreNameQualification ? "true" : "false");
#endif
                                 }

                           // DQ (8/16/2013): Modified API for symbol lookup.
                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope,NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): classSymbol == NULL \n");
                                 }
#endif
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Symbol matches the class declaration (classDeclaration->get_firstNondefiningDeclaration()) classSymbol->get_declaration() = %p \n",classSymbol->get_declaration());
#endif
                           // DQ (6/9/2011): I would prefer to have this be true and it might work if it is not, but I would like to have this be a warning for now!
                           // ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration() == classSymbol->get_declaration());
                              if (classDeclaration->get_firstNondefiningDeclaration() != classSymbol->get_declaration())
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("WARNING: classDeclaration->get_firstNondefiningDeclaration() != classSymbol->get_declaration() \n");
#endif
                                 }

                           // DQ (1/24/2019): Find any associated (outer) class definition scope and check if we need global qualification.
#if 0
                              mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                              SgScopeStatement* temp_scope = currentScope;
                              while (isSgGlobal(temp_scope) == NULL && isSgClassDefinition(temp_scope) == NULL)
                                 {
#if 0
                                   mfprintf(mlog [ WARN ] ) ("  --- temp_scope = %p = %s \n",temp_scope,temp_scope->class_name().c_str());
#endif
                                   temp_scope = temp_scope->get_scope();
                                 }
#if 0
                              mfprintf(mlog [ WARN ] ) ("After loop over parent scopes: temp_scope = %p = %s \n",temp_scope,temp_scope->class_name().c_str());
#endif
                              SgClassDefinition* classDefinition = isSgClassDefinition(temp_scope);
                              if (classDefinition != NULL)
                                 {
                                   SgClassDeclaration* definingClassDeclaration = classDefinition->get_declaration();
                                   ASSERT_not_null(definingClassDeclaration);
                                   SgClassDeclaration* nondefiningClassDeclaration = isSgClassDeclaration(definingClassDeclaration->get_firstNondefiningDeclaration());
                                   ASSERT_not_null(nondefiningClassDeclaration);

                                   if (inaccessibleClassSets.find(nondefiningClassDeclaration) != inaccessibleClassSets.end())
                                      {
                                     // If any of the class declarations in the list of inaccessible class declaration match, then we have to add global qualification.
                                        std::set<SgClassDeclaration*> & inaccessible_classes = inaccessibleClassSets[nondefiningClassDeclaration];
#if 0
                                        mfprintf(mlog [ WARN ] ) ("classDeclaration = %p = %s name = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
#endif
                                        SgClassDeclaration* nondefiningClassDeclaration = isSgClassDeclaration(classDeclaration->get_firstNondefiningDeclaration());
                                     // ROSE_ASSERT(nondefiningClassDeclaration == classDeclaration->get_firstNondefiningDeclaration());

                                        if (inaccessible_classes.find(nondefiningClassDeclaration) != inaccessible_classes.end())
                                           {
                                             qualificationDepth++;
#if 0
                                             mfprintf(mlog [ WARN ] ) ("Found a reference to an inaccessible class --- add global name qualification: qualificationDepth = %d \n",qualificationDepth);
#endif
                                           }
                                      }
                                 }
                            }

                         break;
                       }

                    case V_SgNamespaceDeclarationStatement:
                       {
                      // There is no type elaboration for a reference to a namespace, so I am not sure what to do here.
                         SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(declaration);
                         ASSERT_not_null(namespaceDeclaration);

                         SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);

                      // DQ (6/5/2011): Added support for case where namespaceSymbol == NULL.
                      // ASSERT_not_null(namespaceSymbol);
                         if (namespaceSymbol == NULL)
                            {
                           // This is the case of test2011_72.C (where there is a function with a name matching the name of the namespace).
                           // There is no such think a namespace elaboration, but if there was it might be required at this point.

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupNamespaceSymbolInParentScopes(name,currentScope);

                           // ASSERT_not_null(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): functionSymbol == NULL \n");
                                 }
#endif
                            }

                         break;
                       }

                    case V_SgNamespaceAliasDeclarationStatement:
                       {
                      // There is no type elaboration for a reference to a namespace, so I am not sure what to do here.
                         SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(declaration);
                         ASSERT_not_null(namespaceAliasDeclaration);

                         SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);

                      // DQ (8/1/2020): Record the associated NamespaceAliasDeclarationStatement so it can be used instead in namequalification.
                      // SgNamespaceDeclarationStatement* namespaceDeclaration = namespaceAliasDeclaration->get_namespaceDeclaration();
                      // namespaceAliasDeclarationMap.insert(pair<SgNamespaceDeclarationStatement,SgNamespaceAliasDeclarationStatement>(namespaceDeclaration,namespaceAliasDeclaration));
                      // printf ("case V_SgNamespaceAliasDeclarationStatement: namespaceAliasDeclarationMap.size() = %zu \n",namespaceAliasDeclarationMap.size());

                      // DQ (6/5/2011): Added support for case where namespaceSymbol == NULL.
                      // ASSERT_not_null(namespaceSymbol);
                         if (namespaceSymbol == NULL)
                            {
                           // This is the case of test2011_72.C (where there is a function with a name matching the name of the namespace).
                           // There is no such think a namespace elaboration, but if there was it might be required at this point.

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupNamespaceSymbolInParentScopes(name,currentScope);

                           // ASSERT_not_null(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): functionSymbol == NULL \n");
                                 }
#endif
                            }
                         break;
                       }

                    case V_SgTemplateFunctionDeclaration:
                    case V_SgTemplateMemberFunctionDeclaration:
                    case V_SgTemplateInstantiationMemberFunctionDecl:
                    case V_SgMemberFunctionDeclaration:
                    case V_SgFunctionDeclaration:
                       {
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
                         ASSERT_not_null(functionDeclaration);
#if DEBUG_FUNCTION_RESOLUTION
                         printf ("functionDeclaration = %p = %s name = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
#endif
                         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                         mfprintf(mlog [ WARN ] ) ("case function declaration: functionSymbol = %p \n",functionSymbol);
#endif
                      // DQ (7/22/2017): Added test for SgTemplateInstantiationDirectiveStatement, so that we can process the
                      // template arguments correctly (using the scope of the SgTemplateInstantiationDirectiveStatement instead
                      // of the scope of the SgTemplateInstantiationMemberFunctionDecl (which can be different)).
                      // DQ (6/3/2017): Add test to check if this is part of a template instantiation directive.
                      // However, I think that out use of name qualification is independent of this result.
                         SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement = isSgTemplateInstantiationDirectiveStatement(functionDeclaration->get_parent());
                         if (templateInstantiationDirectiveStatement != NULL)
                            {
                              mfprintf(mlog [ WARN ] ) ("******** Found a member function template instantiation that is a part of a SgTemplateInstantiationDirectiveStatement \n");
                              ROSE_ABORT();
                            }

                         SgFunctionType* functionType = functionDeclaration->get_type();
                         ASSERT_not_null(functionType);
#if DEBUG_FUNCTION_RESOLUTION
                         printf ("functionSymbol = %p \n",functionSymbol);
#endif
                      // ASSERT_not_null(classSymbol);
                         if (functionSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                              mfprintf(mlog [ WARN ] ) ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                           // DQ (7/25/2018): Type elaboration does not make sense for functions.  This is a case where name qualification
                           // is required because the function is hidden by some non-function. The symbol was non-null and it was not a
                           // function.  Question: could it be a function that hides the function name from another function (I think so)?
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                              mfprintf(mlog [ WARN ] ) ("########### NOTE: NEED TO FORCE NAME QUALIFICATION SINCE TYPE ELABLORATION IS NOT A SOLUTION FOR declarations hiding a function \n");
                              mfprintf(mlog [ WARN ] ) ("functionSymbol == NULL: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
                              forceMoreNameQualification = true;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                              mfprintf(mlog [ WARN ] ) ("WARNING: Present implementation of symbol table will not find alias symbols of SgFunctionSymbol \n");
#endif
                           // Reset the symbol to one that will match the declaration.
                           // DQ (4/12/2014): I think we need to use the version of the function that matches the function type.
                           // See test2014_42.C for an example of this.
                           // symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,currentScope);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                              mfprintf(mlog [ WARN ] ) ("NOTE: we are now using the function type in the initial function symbol lookup? \n");
#endif

                           // DQ (4/6/2018): Note that since we use the function type, we are getting the subset of
                           // matching function that would force additional name qualification.
                              symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,functionType,currentScope);

                           // ASSERT_not_null(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                              if (symbol != NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): functionSymbol == NULL \n");
                                 }
#endif
                            }
                           else
                            {
                           // DQ (4/12/2014): But is this the correct symbol for a function of the same type.
                           // See test2014_42.C for an example where this is an overloaded function declaration and the WRONG one.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                              mfprintf(mlog [ WARN ] ) ("case function declaration: functionSymbol = %p: but is it associated with the correct type \n",functionSymbol);
#endif
                              SgFunctionType* functionTypeAssociatedWithSymbol = isSgFunctionType(functionSymbol->get_type());
                              ASSERT_not_null(functionTypeAssociatedWithSymbol);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                              mfprintf(mlog [ WARN ] ) ("case function declaration: functionType = %p \n",functionType);
                              mfprintf(mlog [ WARN ] ) ("case function declaration: functionTypeAssociatedWithSymbol = %p \n",functionTypeAssociatedWithSymbol);
#endif
                              if (functionType != functionTypeAssociatedWithSymbol)
                                 {
#if ((DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 0) || DEBUG_FUNCTION_RESOLUTION
                                   printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                                   mfprintf(mlog [ WARN ] ) ("NOTE: we are now using the function type in the initial function symbol lookup? \n");
                                   printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
                                // DQ (4/12/2014): we need to record that there was another function identified in the parent scopes that we will want to have force name qualification.
                                   foundAnOverloadedFunctionWithSameName = true;

                                // DQ (4/12/2014): Check if the scopes are the same.  If the same then we don't need name qualification.
                                   SgScopeStatement* scopeAssociatedWithSymbol = functionSymbol->get_declaration()->get_scope();
                                   ASSERT_not_null(scopeAssociatedWithSymbol);
                                   SgScopeStatement* scopeOfDeclaration = declaration->get_scope();
                                   ASSERT_not_null(scopeOfDeclaration);

                                   if (scopeAssociatedWithSymbol == scopeOfDeclaration)
                                      {
                                        foundAnOverloadedFunctionInSameScope = true;
                                      }

                                // DQ (4/6/2018): Note that since we use the function type, we are getting the subset of
                                // matching function that would force additional name qualification.
                                   symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,functionType,currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                                   mfprintf(mlog [ WARN ] ) ("After using the function type: symbol = %p \n",symbol);
#endif
                                 }
                                else
                                 {

                                // DQ (9/21/2020): Cxx11_tests/test2020_95.C and test2020_100.C demonstrate that this is not enough.
                                // DQ (8/30/2020): Adding support for a more sophisticated level of function ambiguity resolution.
                                // Here we add the lookup of same named functions in the scopes defined by the types associated
                                // with function parameters.

#define DEBUG_FUNCTION_AMBIGUITY (0 || DEBUG_FUNCTION_RESOLUTION)

#if DEBUG_FUNCTION_AMBIGUITY
                                   printf ("\n\nWe found the correct function, but now we need to check for any other possible matches that would drive more name qualification \n");

                                   printf ("Before loop over function parameter types: foundAnOverloadedFunctionWithSameName = %s \n",foundAnOverloadedFunctionWithSameName ? "true" : "false");
                                   printf ("Before loop over function parameter types: foundAnOverloadedFunctionInSameScope = %s \n",foundAnOverloadedFunctionInSameScope   ? "true" : "false");
                                   printf (" --- currentScope   = %p = %s name = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
                                   printf (" --- functionSymbol = %p = %s name = %s \n",functionSymbol,functionSymbol->class_name().c_str(),SageInterface::get_name(functionSymbol).c_str());
#endif
                                   SgDeclarationStatement* declaration = functionSymbol->get_declaration();
                                   ROSE_ASSERT(declaration != NULL);
                                   bool isFriendFunction = (declaration->get_declarationModifier().isFriend() == true);

                                // Compute a scope outside of the scope where the function is recognized.
                                   SgScopeStatement* alternate_scope = declaration->get_scope();
                                   if (isSgGlobal(alternate_scope) == NULL)
                                      {
                                        alternate_scope = alternate_scope->get_scope();
                                      }

#if ((DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 0) || DEBUG_FUNCTION_RESOLUTION
                                   printf ("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n");
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): check if we need function parameter resolution: forceMoreNameQualification = %s \n",
                                        forceMoreNameQualification ? "true" : "false");
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): check if we need function parameter resolution: name = %s \n",name.str());
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): check if we need function parameter resolution: declaration = %p = %s name = %s \n",
                                        declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): check if we need function parameter resolution: currentScope = %p = %s name = %s \n",currentScope,
                                        currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): check if we need function parameter resolution: alternate_scope = %p = %s name = %s \n",alternate_scope,
                                        alternate_scope->class_name().c_str(),SageInterface::get_name(alternate_scope).c_str());
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): check if we need function parameter resolution: positionStatement = %p = %s name = %s \n",positionStatement,
                                        positionStatement->class_name().c_str(),SageInterface::get_name(positionStatement).c_str());
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): check if we need function parameter resolution: functionType = %p = %s name = %s \n",functionType,
                                        functionType->class_name().c_str(),SageInterface::get_name(functionType).c_str());
#endif
                                   SgSymbol* alternate_symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,functionType,alternate_scope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_FUNCTION_RESOLUTION
                                   printf ("alternate_symbol = %p \n",alternate_symbol);
                                   if (alternate_symbol != NULL)
                                      {
                                        printf ("alternate_symbol = %p = %s name = %s \n",alternate_symbol,alternate_symbol->class_name().c_str(),alternate_symbol->get_name().str());
                                      }
#endif


#if DEBUG_FUNCTION_AMBIGUITY
                                   printf (" --- declaration = %p = %s name = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
                                   printf (" --- isFriendFunction = %s \n",isFriendFunction ? "true" : "false");
#endif
                                   bool symbols_match = ((alternate_symbol != NULL) && (functionSymbol == alternate_symbol));
#if DEBUG_FUNCTION_AMBIGUITY
                                   printf (" --- symbols_match = %s \n",symbols_match ? "true" : "false");
#endif
                                // DQ (9/22/2020): Cxx11_tests/test2020_95.C demonstrated that we needed more than just
                                // this code below to handle Cxx11_tests/test2020_101.C.
                                // DQ (8/31/2020): friend functions are not processed using this parameter based lookup.
                                // Specifically, less name qualification is allowed for GNU versions after 7.x and in
                                // particular version 10.2. Also an error for clang version 10.x.
                                // if (isFriendFunction == false)
                                   if (isFriendFunction == false && symbols_match == false)
                                      {
                                     // Use the scopes of the function parameters to look for where there could be an ambiguity.
                                        SgFunctionParameterTypeList* functionParameterTypeList = functionType->get_argument_list();
                                        ROSE_ASSERT(functionParameterTypeList != NULL);

                                        SgTypePtrList & typeList = functionParameterTypeList->get_arguments();
                                        for (SgTypePtrList::iterator i = typeList.begin(); i != typeList.end(); i++)
                                           {
                                          // for each type in the parameter type list.
                                             SgType* parameter_type = *i;
#if DEBUG_FUNCTION_AMBIGUITY
                                             printf ("parameter_type = %p = %s \n",parameter_type,parameter_type->class_name().c_str());
#endif
                                             unsigned char strip_bit_array = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE |
                                                  SgType::STRIP_RVALUE_REFERENCE_TYPE | SgType::STRIP_POINTER_TYPE | SgType::STRIP_ARRAY_TYPE |
                                                  SgType::STRIP_TYPEDEF_TYPE | SgType::STRIP_POINTER_MEMBER_TYPE;

                                             SgType* stripped_parameter_type = parameter_type->stripType(strip_bit_array);
#if DEBUG_FUNCTION_AMBIGUITY
                                             printf ("stripped_parameter_type = %p = %s \n",stripped_parameter_type,stripped_parameter_type->class_name().c_str());
#endif
                                             SgNamedType* parameter_namedType = isSgNamedType(stripped_parameter_type);
                                             if (parameter_namedType != NULL)
                                                {
                                                  SgDeclarationStatement* parameter_declaration = parameter_namedType->get_declaration();
                                                  ROSE_ASSERT(parameter_declaration != NULL);
#if DEBUG_FUNCTION_AMBIGUITY
                                                  printf ("parameter_declaration = %p = %s \n",parameter_declaration,parameter_declaration->class_name().c_str());
#endif
                                                  SgScopeStatement* parameter_scope = declaration->get_scope();
                                                  ROSE_ASSERT(parameter_scope != NULL);
#if DEBUG_FUNCTION_AMBIGUITY
                                               // printf ("parameter_scope = %p = %s \n",parameter_scope,parameter_scope->class_name().c_str());
                                                  printf ("parameter_scope = %p = %s name = %s \n",parameter_scope,parameter_scope->class_name().c_str(),SageInterface::get_name(parameter_scope).c_str());
#endif

                                               // Check if this is in the parent scopes.
                                                  bool detectedInParentScope = false;
                                                  SgScopeStatement* tmp_scope = currentScope;
                                               // while (tmp_scope != NULL && tmp_scope != parameter_scope)
                                                  while (tmp_scope != NULL && isSgGlobal(tmp_scope) == NULL && tmp_scope != parameter_scope)
                                                     {
#if DEBUG_FUNCTION_AMBIGUITY
                                                       printf ("tmp_scope = %p = %s name = %s \n",tmp_scope,tmp_scope->class_name().c_str(),SageInterface::get_name(tmp_scope).c_str());
#endif
                                                       tmp_scope = tmp_scope->get_scope();
                                                       if (tmp_scope != NULL && tmp_scope == parameter_scope)
                                                          {
#if DEBUG_FUNCTION_AMBIGUITY
                                                            printf ("Found parameter_scope in parent scopes \n");
#endif
                                                            detectedInParentScope = true;
                                                          }
                                                     }

                                                  ROSE_ASSERT(tmp_scope != NULL);
#if DEBUG_FUNCTION_AMBIGUITY
                                                  printf ("detectedInParentScope = %s \n",detectedInParentScope ? "true" : "false");
                                                  printf ("After loop: tmp_scope = %p = %s name = %s \n",tmp_scope,tmp_scope->class_name().c_str(),SageInterface::get_name(tmp_scope).c_str());
#endif
                                                  SgGlobal* globalScope = isSgGlobal(tmp_scope);
                                               // if (detectedInParentScope == true)
                                                  if (detectedInParentScope == true && globalScope == NULL)
                                                     {
                                                       SgSymbol* parameter_symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,functionType,parameter_scope);
                                                       if (parameter_symbol != NULL)
                                                          {
#if DEBUG_FUNCTION_AMBIGUITY
                                                            printf ("Found an ambiguity: parameter_symbol = %p = %s \n",parameter_symbol,parameter_symbol->class_name().c_str());
#endif
                                                            foundAnOverloadedFunctionWithSameName = true;
                                                            foundAnOverloadedFunctionInSameScope  = false;
                                                          }
                                                     }
                                                }
                                           }
                                      }
#if DEBUG_FUNCTION_AMBIGUITY
                                   printf ("After loop over function parameter types: foundAnOverloadedFunctionWithSameName = %s \n",foundAnOverloadedFunctionWithSameName ? "true" : "false");
                                   printf ("After loop over function parameter types: foundAnOverloadedFunctionInSameScope = %s \n",foundAnOverloadedFunctionInSameScope   ? "true" : "false");
                                   printf ("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n");
#endif
                                 }
                            }
                      // numberOfSymbols = currentScope->count_symbol(name);
                         break;
                       }

                 // DQ (11/10/2014): Added support for templated typedefs (and their instantiations).
                    case V_SgTemplateTypedefDeclaration:
                    case V_SgTemplateInstantiationTypedefDeclaration:

                    case V_SgTypedefDeclaration:
                       {
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(declaration);
                         ASSERT_not_null(typedefDeclaration);

                      // DQ (7/22/2017): Added test for SgTemplateInstantiationDirectiveStatement, so that we can process the
                      // template arguments correctly (using the scope of the SgTemplateInstantiationDirectiveStatement instead
                      // of the scope of the SgTemplateInstantiationTypedefDeclaration (which can be different)).
                      // DQ (6/3/2017): Add test to check if this is part of a template instantiation directive.
                      // However, I think that out use of name qualification is independent of this result.
                         SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement = isSgTemplateInstantiationDirectiveStatement(typedefDeclaration->get_parent());
                         if (templateInstantiationDirectiveStatement != NULL)
                            {
                              mfprintf(mlog [ WARN ] ) ("******** Found a typedef template instantiation that is a part of a SgTemplateInstantiationDirectiveStatement \n");
                              ROSE_ABORT();
                            }

                         SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                         if (typedefSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupTypedefSymbolInParentScopes(name,currentScope);

                           // DQ (5/15/2011): Added this to support where symbol after moving name qualification
                           // support to the astPostProcessing phase instead of calling it in the unparser.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): typedefSymbol == NULL \n");
                                 }
#endif
                            }
                         break;
                       }


                 // DQ (8/13/2013): I think that this case should not appear, since SgTemplateDeclaration is a part of pre-EDG4x work.
                    case V_SgTemplateDeclaration:
                       {
                         SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(declaration);
                         ASSERT_not_null(templateDeclaration);
                      // DQ (7/24/2018): This is output spew for Cxx11_tests/test2016_90.C and Cxx_tests/test2013_63.C (and others).
                      // It is not new, but it is also not clear that it is too much of an issue that we have some used of SgTemplateDeclaration
                      // in place since within templates we can at times not have enough information to build anything more specific.
                      // All of these issues appear to be related to input codes using boost: e.g. boost/graph/topological_sort.hpp.
#if 0
                         mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): case V_SgTemplateDeclaration: still emitted for template template parameter (seen in template template argument of `this`)\n");
#endif
                         symbol = NULL;

                         break;
                       }

                 // DQ (11/16/2013): I think we do need this case and test2013_273.C demonstrates this.
                 // DQ (6/1/2011): Not clear if we need a special case for the case of SgTemplateInstantiationMemberFunctionDecl.
                 // I think we need to call: evaluateNameQualificationForTemplateArgumentList()
                 // to evaluate template arguments for both SgTemplateInstantiationFunctionDecl and SgTemplateInstantiationMemberFunctionDecl.
                    case V_SgTemplateInstantiationFunctionDecl:
                       {
                      // DQ (6/4/2017): Added notes on the additional complexity of name qualification for template instantiations.
                      // Note that there are several things that can cause name qualification for a template instantiation:
                      //    1) A different template instantiation that is visible from the same scope (current scope),
                      //       this would be the typical case most similar to other constructs.
                      //    2) A template declaration that is visible from the same scope (current scope) using the same
                      //       name as the template declaration associated with the template instantiation.
                      //    3) A non-template function declaration (member of non-member) that is visible from the same
                      //       scope (current scope) using the same name. See test2017_40.C.
                      // Note also that when the current scope is a namespace, that lookups have to be normalized in terms
                      // of the global namespace (unique namespace definition used to union all declaration across all
                      // namespaces that are equivalent (becuase namespaces are reentrent).

                         SgTemplateInstantiationFunctionDecl* templateInstantiationFunction = isSgTemplateInstantiationFunctionDecl(declaration);
                         ASSERT_not_null(templateInstantiationFunction);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): case V_SgTemplateInstantiationFunctionDecl: templateInstantiationFunction = %p = %s \n",
                              templateInstantiationFunction,templateInstantiationFunction->class_name().c_str());
                         mfprintf(mlog [ WARN ] ) ("   --- templateInstantiationFunction->get_name()         = %s \n",templateInstantiationFunction->get_name().str());
                         mfprintf(mlog [ WARN ] ) ("   --- templateInstantiationFunction->get_templateName() = %s \n",templateInstantiationFunction->get_templateName().str());
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                      // If this is a SgNamespaceDefinition, then we need to reset it to the uniquly represented namespace definition.
                         mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                      // DQ (6/3/2017): Adding code to normalize the scope where it is a namespace definition, since that can be more
                      // than one that are the same namespace and we have a mechanism to resolve equivalents for this special case.
                      // BTW, I worry that this should be done more generally and uniformally within the name qualification support.
                         SgNamespaceDefinitionStatement* namespaceDefinitionStatement = isSgNamespaceDefinitionStatement(currentScope);
                         if (namespaceDefinitionStatement != NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Reset the currentScope to the namespace's global definition (namespace normalization) \n");
#endif
                              currentScope = namespaceDefinitionStatement->get_global_definition();
                              ASSERT_not_null(currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("calling lookupFunctionSymbolInParentScopes(): name = %s currentScope = %p = %s \n",name.str(),currentScope,currentScope->class_name().c_str());
#endif
                           // Reset the symbol to be consistant with the unique scope (in case the currentScope was reset above.
                              symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,currentScope);

                           // DQ (7/21/2024): Added debugging output for testing.
                              if (symbol == NULL)
                                 {
                                   printf ("SageInterface::lookupFunctionSymbolInParentScopes() returned NULL: name = %s currentScope = %p = %s = %s \n",
                                        name.str(),currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
                                 }

                           // DQ (7/21/2024): This can be NULL for the processing of nlohmann/json.hpp with ROSE
                           // ASSERT_not_null(symbol);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("@@@@@@@@ name = %s declaration = %p = %s symbol = %s \n",name.str(),declaration,declaration->class_name().c_str(),symbol->class_name().c_str());
                                 }
#endif
                            }

                         SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);

                      // DQ (5/21/2017): I think this is never a SgTemplateSymbol (checking).
                      // DQ (6/4/2017): Maybe if it was a template specialization that was forcing the name qualification
                      // (then the names might match, we would need to have a test code example of this).
                         ROSE_ASSERT(templateSymbol == NULL);

                         if (templateSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                           // The existance of any symbol identified to hide the current statement is cause for at least type elaboration.
                              typeElaborationIsRequired = true;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("calling lookupFunctionSymbolInParentScopes(): name = %s currentScope = %p = %s \n",name.str(),currentScope,currentScope->class_name().c_str());
#endif
                           // Reset the symbol to one that will match the declaration.
                           // DQ (4/12/2014): I think we need to use the function type here!
#if (DEBUG_NAME_QUALIFICATION_LEVEL >= 1) || 0
                              mfprintf(mlog [ WARN ] ) ("Should we be using the function type in the initial SgTemplateInstantiationFunctionDecl symbol lookup? \n");
#endif
                           // Reset the symbol to be consistatn with the unique scope (in case the currentScope was reset above.
                           // symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,currentScope);

                           // DQ (6/23/2013): Fixing test2013_223.C (function hiding template function instantiation).
                              SgName templateInstantiationFunctionNameWithoutTemplateArguments = templateInstantiationFunction->get_templateName();

                           // DQ (5/21/2017): Get the name with template arguments (I think we need it instead).
                              SgName templateInstantiationFunctionName = templateInstantiationFunction->get_name();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("   --- templateInstantiationFunctionNameWithoutTemplateArguments = %s \n",templateInstantiationFunctionNameWithoutTemplateArguments.str());
                              mfprintf(mlog [ WARN ] ) ("   --- templateInstantiationFunctionName                         = %s \n",templateInstantiationFunctionName.str());
#endif

                           // DQ (4/6/2018): I think we should be using the function type in the lookupFunctionSymbolInParentScopes(),
                           // else we could be confusing overloaded function which would not require name qualification.
                           // mfprintf(mlog [ WARN ] ) ("Shouldn't we be using the function type to refine the symbol table lookup? \n");

                           // DQ (5/22/2017): Modified to use the name with the template arguments.
                           // DQ (4/4/2014): Modified this to use the SgFunctionSymbol type.
                           // SgSymbol* symbolHiddingTemplateSymbol = SageInterface::lookupFunctionSymbolInParentScopes(templateFunctionNameWithoutTemplateArguments,currentScope);
                           // SgFunctionSymbol* symbolHiddingTemplateInstantiationSymbol = SageInterface::lookupFunctionSymbolInParentScopes(templateInstantiationFunctionNameWithoutTemplateArguments,currentScope);

                           // DQ (4/7/2018): Adding the function type to make this more precise (and avoid different overloaded functions).
                           // SgFunctionSymbol* symbolHiddingTemplateInstantiationSymbol = SageInterface::lookupFunctionSymbolInParentScopes(templateInstantiationFunctionName,currentScope);
                              SgFunctionType* functionType = templateInstantiationFunction->get_type();
                              ASSERT_not_null(functionType);
                              SgFunctionSymbol* symbolHiddingTemplateInstantiationSymbol = SageInterface::lookupFunctionSymbolInParentScopes(templateInstantiationFunctionName,functionType,currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("symbolHiddingTemplateInstantiationSymbol = %p \n",symbolHiddingTemplateInstantiationSymbol);
                              mfprintf(mlog [ WARN ] ) ("symbol                                   = %p \n",symbol);
#endif
                           // Handle the case of a template instantiation hidding the template instantiation for which we want to determine
                           // name qualification.  See test2017_39.C
                              if (symbolHiddingTemplateInstantiationSymbol != NULL && symbolHiddingTemplateInstantiationSymbol != symbol)
                                 {
                                // There is reason to think this template instantiation should have some name qualification.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                                   mfprintf(mlog [ WARN ] ) ("WARNING: There is reason to think this template instantiation should have some name qualification because it may be hidden by another template instantiation \n");
                                   mfprintf(mlog [ WARN ] ) ("   --- templateInstantiationFunctionName = %s \n",templateInstantiationFunctionName.str());
                                   mfprintf(mlog [ WARN ] ) ("   --- templateInstantiationFunction mangled name = %s \n",templateInstantiationFunction->get_mangled_name().str());
                                   SgFunctionDeclaration* functionHiddingInputFunction =  symbolHiddingTemplateInstantiationSymbol->get_declaration();
                                   ASSERT_not_null(functionHiddingInputFunction);
                                   mfprintf(mlog [ WARN ] ) ("   --- functionHiddingInputFunction = %s \n",functionHiddingInputFunction->get_name().str());
                                   mfprintf(mlog [ WARN ] ) ("   --- functionHiddingInputFunction mangled name = %s \n",functionHiddingInputFunction->get_mangled_name().str());
#endif
                                 }
                                else
                                 {
                                // There is no template instantiation hidding the template instantiation for which we are computing the name qualification.
                                 }

                           // Now we need to check for a non-template instantiation function hidding the template instantiation.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Look for symbols from name without template arguments: name = %s template declaration = %s symbol = %s \n",
                                   templateInstantiationFunctionNameWithoutTemplateArguments.str(),declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                           // DQ (4/7/2018): I think we should MAYBE be using the function type in the lookupFunctionSymbolInParentScopes(),
                           // else we could be confusing overloaded function which would not require name qualification.
                           // mfprintf(mlog [ WARN ] ) ("Shouldn't we be using the function type to refine the symbol table lookup? \n");

                           // Note name change to variable (for clarification).
                           // DQ (5/23/2017): Note that for template instatiations the template must be visible from the template instatiation (or name qualified to to be visible).
                              SgFunctionSymbol* symbolHiddingTemplateSymbol = SageInterface::lookupFunctionSymbolInParentScopes(templateInstantiationFunctionNameWithoutTemplateArguments,currentScope);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("symbolHiddingTemplateSymbol              = %p \n",symbolHiddingTemplateSymbol);
#endif
                           // Handle the case of a non-template instnatiation hidding the template declaration for the template instantiation
                           // for which we want to determine name qualification. See test2017_40.C
                              if (symbolHiddingTemplateSymbol != NULL && symbolHiddingTemplateSymbol != symbol)
                                 {
                                // This looks up the scope via the symbol table's parent (not sure that is a great approach).
                                // SgScopeStatement* hiddingSymbolScope = isSgScopeStatement(symbolHiddingTemplateInstantiationSymbol->get_parent()->get_parent());

                                // Get the scope where this symbol is in the symbol table.
                                // SgScopeStatement* hiddingSymbolScope = isSgScopeStatement(symbolHiddingTemplateInstantiationSymbol->get_parent()->get_parent());
                                   SgScopeStatement* hiddingSymbolScope = isSgScopeStatement(symbolHiddingTemplateSymbol->get_parent()->get_parent());
                                   ASSERT_not_null(hiddingSymbolScope);

                                // Get the scope of the template instantiation.
                                   SgScopeStatement* functionScope = templateInstantiationFunction->get_scope();
                                   ASSERT_not_null(functionScope);
#if 0
                                   mfprintf(mlog [ WARN ] ) ("functionScope = %p = %s \n",functionScope,functionScope->class_name().c_str());
#endif
                                // DQ (4/4/2014): Look at the declarations associated with these symbols.
                                // SgDeclarationStatement* declarationHidingCurrentDeclaration = symbolHiddingTemplateInstantiationSymbol->get_declaration();
                                   SgDeclarationStatement* declarationHidingCurrentDeclaration = symbolHiddingTemplateSymbol->get_declaration();
                                   ASSERT_not_null(declarationHidingCurrentDeclaration);

                                   SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                                   ASSERT_not_null(functionSymbol);

                                   SgDeclarationStatement* declarationFromSymbol = functionSymbol->get_declaration();
                                   ASSERT_not_null(declarationFromSymbol);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("hiddingSymbolScope                  = %p = %s \n",hiddingSymbolScope,hiddingSymbolScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("declaration from symbol             = %p = %s \n",declarationFromSymbol,declarationFromSymbol->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("declaration hidding template symbol = %p = %s \n",declarationHidingCurrentDeclaration,declarationHidingCurrentDeclaration->class_name().c_str());
#endif
                                   bool currentScopeIsNestedWithinScopeOfHiddingDeclaration = false;

                                // SgScopeStatement* temp_symbolScope = symbolScope;
                                   SgScopeStatement* temp_scope = currentScope;
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("case SgTemplateInstantiationFunctionDecl (template declaration hidden): temp_scope = %p = %s \n",temp_scope,temp_scope->class_name().c_str());
#endif
#if 1
                                   while (isSgGlobal(temp_scope) == NULL && temp_scope != hiddingSymbolScope)
                                      {
                                        temp_scope = temp_scope->get_scope();
#if 0
                                        mfprintf(mlog [ WARN ] ) ("reset the temp_scope = %p = %s \n",temp_scope,temp_scope->class_name().c_str());
#endif
                                      }
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   if (temp_scope != NULL)
                                      {
                                        mfprintf(mlog [ WARN ] ) ("final value of temp_scope = %p = %s \n",temp_scope,temp_scope->class_name().c_str());
                                      }
                                     else
                                      {
                                        mfprintf(mlog [ WARN ] ) ("final value of temp_scope = NULL \n");
                                      }

                                   mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                                // if (temp_scope == hiddingSymbolScope)
                                   if (hiddingSymbolScope != currentScope && temp_scope == hiddingSymbolScope)
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("Note: hiddingSymbolScope != currentScope && temp_scope == hiddingSymbolScope \n");
#endif
                                        currentScopeIsNestedWithinScopeOfHiddingDeclaration = true;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("   --- Resetting the recorded symbol to the hidding symbol  (triggering the name qualification evaluation) \n");
#endif
                                        symbol = symbolHiddingTemplateSymbol;
                                      }
                                     else
                                      {
                                     // if (hiddingSymbolScope == currentScope)
                                        if (hiddingSymbolScope == currentScope && declarationFromSymbol != declarationHidingCurrentDeclaration)
                                           {
                                          // DQ (6/3/2017): In this case we need to detect when the template instantiation is hidden
                                          // by a non-template instantiation with the same name (sans template arguments).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                             mfprintf(mlog [ WARN ] ) ("Note: hiddingSymbolScope == currentScope \n");
#endif
                                             currentScopeIsNestedWithinScopeOfHiddingDeclaration = true;

                                          // DQ (6/3/2017): Test resetting the symbol to NULL (triggering the name qualification evaluation).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                             mfprintf(mlog [ WARN ] ) ("   --- Resetting the recorded symbol to the hidding symbol  (triggering the name qualification evaluation) \n");
#endif
                                          // symbol = NULL;

                                             symbol = symbolHiddingTemplateSymbol;
                                           }
                                      }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("currentScopeIsNestedWithinScopeOfHiddingDeclaration = %s \n",currentScopeIsNestedWithinScopeOfHiddingDeclaration ? "true" : "false");
                                   mfprintf(mlog [ WARN ] ) ("currentScope       = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("hiddingSymbolScope = %p = %s \n",hiddingSymbolScope,hiddingSymbolScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("temp_scope         = %p = %s \n",temp_scope,temp_scope->class_name().c_str());
#endif
                                // DQ (4/4/2014): We don't want to treat the template instantiation as be hidden by the template declaration
                                // because that does not make sense. So check the declarations.  Also, if the template instatiation is
                                // not output in the generated code, then it can't be name qualified.
                                   bool willBeOutput = (declaration->get_file_info()->isCompilerGenerated() == false ||
                                                       (declaration->get_file_info()->isCompilerGenerated() &&
                                                        declaration->get_file_info()->isOutputInCodeGeneration()) );
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("$$$$$ --- willBeOutput = %s \n",willBeOutput ? "true" : "false");
#endif
#if 1
                                // DQ (6/3/2017): Comment out the resetting of the symbol to NULL (as a test).

                                // If we visit the scope with the function hidding our template function then we will need some name qualification.
                                // if (temp_scope == symbolScope)
                                // if (willBeOutput && isSameKindOfFunction && temp_scope == symbolScope)
                                // if (willBeOutput == true || (willBeOutput == false && temp_scope == hiddingSymbolScope))
                                   if (willBeOutput == true || (willBeOutput == false && currentScopeIsNestedWithinScopeOfHiddingDeclaration == true))
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("WARNING: Some qualification is required to get past the non-template function hidding the template function = %p = %s \n",templateInstantiationFunction,templateInstantiationFunction->get_name().str());
#endif
                                     // mfprintf(mlog [ WARN ] ) ("   --- Resetting the recorded symbol to NULL \n");
                                     // symbol = NULL;
                                      }
#endif
                                 }
                                else
                                 {
                                // DQ (5/23/2017): This case is not a problem because the template is visible
                                // from the template instantiation is does not require name qualification.
                                 }

                           // DQ (7/24/2011): The symbol is NULL for test2011_121.C
                           // ASSERT_not_null(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                // DQ (6/22/2011): This is demonstrated by test2011_121.C
                                   mfprintf(mlog [ WARN ] ) ("Detected no template function instantiation symbol in a parent scope (ignoring this case for now) \n");
                                 }
#endif
                            }
                         break;
                       }

                    case V_SgEnumDeclaration:
                       {
                         SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declaration);
                         ASSERT_not_null(enumDeclaration);

                         SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                         if (enumSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupEnumSymbolInParentScopes(name,currentScope);

                           // ASSERT_not_null(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                // DQ (6/22/2011): This is demonstrated by test2011_95.C
                                   mfprintf(mlog [ WARN ] ) ("Detected no enum symbol in a parent scope (ignoring this case for now) \n");
                                 }
#endif
                            }
                           else
                            {
                           // DQ (5/30/2019): If this is a SgEnumSymbol then are they others such that we require name qualification
                           // to select the correct enum declaration (type).

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope,templateParameterList,templateArgumentList);
                              mfprintf(mlog [ WARN ] ) ("Previous lookup: symbol = %p = %s \n",symbol,(symbol != NULL) ? symbol->class_name().c_str() : "NULL");
#endif
                              size_t symbol_count       = currentScope->count_symbol(name);
                              bool isUnNamed            = enumDeclaration->get_isUnNamed();

                              if (symbol_count > 1 && isUnNamed == false)
                                 {
#if 0
                                   mfprintf(mlog [ WARN ] ) ("Detected multiple symbols with the same name, so force name qualification: symbol_count = %zu \n",symbol_count);
#endif
                                   forceMoreNameQualification = true;
                                 }
                            }

                         break;
                       }

                    case V_SgTemplateInstantiationDecl:
                       {
                         SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
                         ASSERT_not_null(templateInstantiationDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("Found a case of declaration == SgTemplateInstantiationDecl \n");
#endif

                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                         if (classSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

                           // mfprintf(mlog [ WARN ] ) ("We might need the template arguments to look up this template class instantiation. \n");

                           // DQ (8/15/2013): This needs to be used in lookupClassSymbolInParentScopes(), but the function does not accept a SgTemplateArgumentPtrList pointer yet.
                              SgTemplateArgumentPtrList* templateArgumentsList = &(templateInstantiationDeclaration->get_templateArguments());

                           // DQ (8/16/2013): Modified API for symbol lookup.
                           // Reset the symbol to one that will match the declaration.
                           // symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope);
                              symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope,templateArgumentsList);

                           // DQ (5/15/2011): Added this to support where symbol after moving name qualification
                           // support to the astPostProcessing phase instead of calling it in the unparser.
                              if (symbol != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): classSymbol == NULL \n");
#endif
                                   SgTemplateClassDeclaration* templateClassDeclaration = templateInstantiationDeclaration->get_templateDeclaration();
                                   ASSERT_not_null(templateClassDeclaration);

                                   SgTemplateParameterPtrList & templateParameterList = templateClassDeclaration->get_templateParameters();
                                   SgTemplateArgumentPtrList  & templateArgumentList  = templateClassDeclaration->get_templateSpecializationArguments();

                                // DQ (8/13/2013): This needs to be looked up as a SgTemplateClassSymbol.
                                   symbol = SageInterface::lookupTemplateClassSymbolInParentScopes(name,&templateParameterList,&templateArgumentList,currentScope);
                                 }
                            }
                           else
                            {
                              SgDeclarationStatement* nestedDeclaration = classSymbol->get_declaration();
                              ASSERT_not_null(nestedDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Need to dig deeper into this symbol! \n");
                              mfprintf(mlog [ WARN ] ) ("nestedDeclaration = %p = %s \n",nestedDeclaration,nestedDeclaration->class_name().c_str());
#endif
                              SgTemplateInstantiationDecl* nestedTemplateDeclaration = isSgTemplateInstantiationDecl(nestedDeclaration);
                              if (nestedTemplateDeclaration != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("nestedTemplateDeclaration = %p = %s \n",nestedTemplateDeclaration,nestedTemplateDeclaration->get_name().str());
#endif
                                 }
                            }

                         break;
                       }

                    case V_SgTemplateVariableDeclaration:
                    case V_SgTemplateVariableInstantiation:
                    case V_SgVariableDeclaration:
                       {
                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declaration);
                         ASSERT_not_null(variableDeclaration);

                         SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("variableSymbol = %p = %s \n",variableSymbol,symbol->class_name().c_str());
#endif
#if 0
                         mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): variableSymbol = %p = %s \n",variableSymbol,(symbol != NULL) ? symbol->class_name().c_str() : "null");
#endif
                      // DQ (7/22/2017): Added test for SgTemplateInstantiationDirectiveStatement, so that we can process the
                      // template arguments correctly (using the scope of the SgTemplateInstantiationDirectiveStatement instead
                      // of the scope of the SgTemplateInstantiationVariableDeclaration (which can be different)).
                      // DQ (6/3/2017): Add test to check if this is part of a template instantiation directive.
                      // However, I think that out use of name qualification is independent of this result.
                         SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement = isSgTemplateInstantiationDirectiveStatement(variableDeclaration->get_parent());
                         if (templateInstantiationDirectiveStatement != NULL)
                            {
                              mfprintf(mlog [ WARN ] ) ("******** Found a variable template instantiation that is a part of a SgTemplateInstantiationDirectiveStatement \n");
                              ROSE_ABORT();
                            }

                         if (variableSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                           // symbol = SageInterface::lookupVariableSymbolInParentScopes(name,currentScope);
                              variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(name,currentScope);
                              if (variableSymbol != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: reset symbol = %p = %s \n",variableSymbol,variableSymbol->class_name().c_str());
#endif
                                   symbol = variableSymbol;
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): variableSymbol == NULL \n");
#endif
                                // DQ (8/14/2013): Use an alternative mechanism to get the correct symbol more directly
                                // (might be more expensive, or perhaps this mechanism should be used more generally in
                                // this name qualification support).
                                   SgInitializedName* currentVariableDeclarationInitializedName = SageInterface::getFirstInitializedName(variableDeclaration);
                                   ASSERT_not_null(currentVariableDeclarationInitializedName);

                                // DQ (4/7/2014): Reset the symbol to NULL (snce we didn't find an associated SgVariableSymbol).
                                // In the switch statement (below) we will use the declaration to obtain the correct
                                // symbol and then compare if we have found the correct one using the name lookup through
                                // parent scopes.  Then we will beable to know if name qualification is required. If we
                                // use the declaration to find the symbol here, then we will detect that no name
                                // qualification is required (where it might be).  Testcode test2014_39.C demonstrates
                                // this issue.
                                   symbol = NULL;
                                 }
                            }

                         break;
                       }

                    case V_SgNonrealDecl:
                       {
#if 0
                         mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): Found a case of declaration == SgNonrealDecl => return 0\n");
#endif
                         ASSERT_not_null(symbol);
                         if (!isSgNonrealSymbol(symbol)) {
                           symbol = SageInterface::lookupNonrealSymbolInParentScopes(name,currentScope,templateParameterList,templateArgumentList);
                         }

                         break;
                       }

                    case V_SgProcedureHeaderStatement:
                    case V_SgJovialTableStatement:
                       {
                         return 0;
                         break;
                       }

                    default:
                       {
                         mfprintf(mlog [ WARN ] ) ("default reached declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                         ROSE_ABORT();
                       }
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("2222222222222222222222222222222222222222222222222222222222222222222 \n");
          mfprintf(mlog [ WARN ] ) ("Calling evaluateTemplateInstantiationDeclaration() from nameQualificationDepth() declaration = %p = %s currentScope = %p = %s \n",
               declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str());
#endif

       // Refactored this code to another member function so that it could also support evaluation of declarations found in types (more generally).
          evaluateTemplateInstantiationDeclaration(declaration,currentScope,positionStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("DONE: Calling evaluateTemplateInstantiationDeclaration() from nameQualificationDepth() declaration = %p = %s = %s currentScope = %p = %s \n",
               declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str(),currentScope,currentScope->class_name().c_str());
       // DQ (5/12/2024): This fails for test_06.C used to debug the infinite recursion in name qualification for Group G code.
       // mfprintf(mlog [ WARN ] ) ("===== declaration->unparseToString() = %s \n",declaration->unparseToString().c_str());
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): symbol = %p \n",symbol);
          if (symbol != NULL)
             {
               mfprintf(mlog [ WARN ] ) ("   --- symbol = %s \n",symbol->class_name().c_str());
             }
#endif

       // At this point if there was any ambiguity in the first matching symbol that was found, then
       // we have resolved this to the correct type of symbol (SgClassSymbol, SgFunctionSymbol, etc.).
       // Now we want to resolve it to the exact symbol that matches the declaration.
          if (symbol != NULL)
             {
               SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);
               ROSE_ASSERT(aliasSymbol == NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("forceMoreNameQualification = %s \n",forceMoreNameQualification ? "true" : "false");
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 0
               printf ("In NameQualificationTraversal::nameQualificationDepth(): forceMoreNameQualification = %s \n",forceMoreNameQualification ? "true" : "false");
               printf ("In NameQualificationTraversal::nameQualificationDepth(): declaration = %p = %s name = %s \n",
                    declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
               printf ("In NameQualificationTraversal::nameQualificationDepth(): currentScope = %p = %s name = %s \n",currentScope,
                    currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
               printf ("In NameQualificationTraversal::nameQualificationDepth(): positionStatement = %p = %s name = %s \n",positionStatement,
                    positionStatement->class_name().c_str(),SageInterface::get_name(positionStatement).c_str());
#endif

               if (forceMoreNameQualification == true)
                  {
                 // If there is more than one symbol with the same name then name qualification is required to distinguish between them.
                 // The exception to this is overloaded member functions.  But might also be where type evaluation is required.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Found a case of ambiguity (forceMoreNameQualification == true) of declaration = %s in the currentScope = %p = %s = %s (trigger additional name qualifier). \n",
                         declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
#endif
                    switch(declaration->variantT())
                       {
                         case V_SgFunctionDeclaration:
                            {
                           // DQ (7/25/2018): If in the original matching (for name collission) there was a match, then we need to force
                           // at least one more level of name qualification for functions since type elaboration can not be used to resolve
                           // an ambiguity on function (only makes sense for types).
                              qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                              break;
                            }

                         case V_SgMemberFunctionDeclaration:
                            {
                           // Don't qualify member function defined in their associated class.
                              SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declaration);
                              SgScopeStatement* structurallyAssociatedScope = isSgScopeStatement(memberFunctionDeclaration->get_parent());
                              ASSERT_not_null(structurallyAssociatedScope);

                           // Note that structurallyAssociatedDeclaration could be NULL if the function declaration is in global scope.
                              SgDeclarationStatement* structurallyAssociatedDeclaration = associatedDeclaration(structurallyAssociatedScope);
                              SgDeclarationStatement* semanticallyAssociatedDeclaration = memberFunctionDeclaration->get_associatedClassDeclaration();

                              ASSERT_not_null(semanticallyAssociatedDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("structurallyAssociatedDeclaration = %p \n",structurallyAssociatedDeclaration);
                              mfprintf(mlog [ WARN ] ) ("semanticallyAssociatedDeclaration = %p \n",semanticallyAssociatedDeclaration);
#endif
                              if (structurallyAssociatedDeclaration != semanticallyAssociatedDeclaration)
                                 {
                                // The associated class for the member function does not match its structural location so we require name qualification.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("The associated class for the member function does not match its structural location so we require name qualification \n");
#endif
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("structurallyAssociatedDeclaration == semanticallyAssociatedDeclaration: qualificationDepth = %d \n",qualificationDepth);
#endif
                                // DQ (4/27/2019): We need to force a level of qualification (I think).
                                // DQ (4/28/2019): This does fix the ctor preinitialization list name qualification bug (represented by test2019_415.C).
                                   qualificationDepth += 1;
                                 }

                              break;
                            }

                         case V_SgEnumDeclaration:
                            {
                           // An Enum can have a tag and it will be the it scope and trigger unwanted name qualification.
                           // SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declaration);

                           // I think what we want to do is recognize when there enum declaration is declared directly in the typedef.
                           // We now make sure that name qualification is not called in this case, so we should not reach this point!
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Error: Skipping forced name qualification for enum types (sorry, not implemented) \n");
#endif
                           // DQ (5/30/2019): If we are forcing name qualification then I think we need to increment this variable.
                           // See test2019_448.C for an example of where this is needed.
                              qualificationDepth += 1;
#if 0
                              mfprintf(mlog [ WARN ] ) ("case V_SgEnumDeclaration: after incrementing qualificationDepth: qualificationDepth = %d \n",qualificationDepth);
#endif
                           // We do reach this point in test2004_105.C
                           // ROSE_ASSERT(false);

                              break;
                            }

                         default:
                            {
                           // But we have to keep moving up the chain of scopes to see if the parent might also require qualification.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("We are forcing the name qualification so continue to resolve the name qualification depth... \n");
#endif
                              qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                            }
                       }
                  }
                 else
                  {
                 // The numberOfSymbols can be zero or one, because the symbol might not be the the current scope.
                 // If it is zero then it just means that the name is visible
                 // from the current scope by is not located in the current scope.  If it is one, then there is a
                 // symbol matching the name and we need to check if it is associated with the same declaration or not.

                 // However, since symbol != NULL, the numberOfSymbols should be non-zero.
                 // ROSE_ASSERT(numberOfSymbols > 0);

                 // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
                    ROSE_ASSERT(aliasSymbol == NULL);

                 // Not clear if we want to resolve this to another scope since the alias symbols scope
                 // is want might have to be qualified (not the scope of the aliased declaration).
                    if (aliasSymbol != NULL)
                       {
                      // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
                         mfprintf(mlog [ WARN ] ) ("ERROR: The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level \n");
                         ROSE_ABORT();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("Resetting the symbol to that stored in the SgAliasSymbol \n");
#endif
                         symbol = aliasSymbol->get_alias();

                      // DQ (7/23/2011): If we can't assert this, then we need to loop through the chain of alias
                      // symbols to get to the non-alias (original) symbol.
                         ROSE_ASSERT(isSgAliasSymbol(symbol) == NULL);
                       }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                    mfprintf(mlog [ WARN ] ) ("AT SWITCH: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
                    switch (symbol->variantT())
                       {
                      // DQ (12/27/2011): Added support for template class symbols.
                         case V_SgTemplateClassSymbol:
                         case V_SgClassSymbol:
                            {
                              SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                              ASSERT_not_null(classSymbol);

                           // This is a class symbol, check if the declaration is the same.
                           // SgClassDeclaration* associatedClassDeclaration = baseClass->get_base_class();
                              SgClassDeclaration* associatedClassDeclaration = classSymbol->get_declaration();

#if DEBUG_NONTERMINATION || 0
                           // DQ (7/21/2024): Added debugging support.
                              printf("In NameQualificationTraversal::nameQualificationDepth(): case V_SgClassSymbol: declaration  = %p = %s = %s \n",
                                   declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
#endif
                           // DQ (7/21/2024): This can be a namespaceDeclaration (for the case of processing the nlohmann/json header file).
                           // ASSERT_not_null(classDeclaration);
                              ASSERT_not_null(associatedClassDeclaration);

                           // DQ (7/21/2024): This can be a namespaceDeclaration (for the case of processing the nlohmann/json header file).
                           // if (associatedClassDeclaration->get_firstNondefiningDeclaration() == classDeclaration->get_firstNondefiningDeclaration())
                              if ( (classDeclaration != NULL) && (associatedClassDeclaration->get_firstNondefiningDeclaration() == classDeclaration->get_firstNondefiningDeclaration()) )
                                 {
                                // DQ (1/4/2020): This is the better implementation and it should be isolated into a seperate
                                // function so that we can call it from the case V_SgTypedefSymbol and case V_SgEnumSymbol
                                // (and maybe some other locations as well (generating more test codes to drive this would be helpful).

                                // DQ (1/4/2020): Need to check if there is an opportunity for an ambigous reference.
                                // size_t numberOfAliasSymbols = currentScope->count_alias_symbol(name);
                                // symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);
                                // SgScopeStatement* scopeOfAssociatedTypedefDeclaration = associatedTypedefDeclaration->get_scope();
                                // ASSERT_not_null(scopeOfAssociatedTypedefDeclaration);
                                // size_t numberOfAliasSymbols = scopeOfAssociatedTypedefDeclaration->count_alias_symbol(name);
                                   bool includeCurrentScope = true;
                                   SgClassDefinition* current_classDefinition = SageInterface::getEnclosingNode<SgClassDefinition>(currentScope,includeCurrentScope);
                                   if (current_classDefinition != NULL)
                                      {
                                     // DQ (2/4/2020): Check if there is an existing class in the current_classDefinition
                                     // (which is not an alias), and if so then we don't need to worry about any ambiguity.
                                     // See Cxx11_test/test2020_11.C for an example.
                                     // SgClassSymbol* lookupClassSymbol = current_classDefinition->lookup_class_symbol(name);
                                        size_t symbolCount               = current_classDefinition->count_symbol(name);
                                        size_t numberOfAliasSymbols      = current_classDefinition->count_alias_symbol(name);
                                        ROSE_ASSERT(symbolCount >= numberOfAliasSymbols);
                                        size_t declarationsInThisScope = symbolCount - numberOfAliasSymbols;
                                     // When all of the declarations are in base classes then there is an ambiguity to resolve.
                                     // if (lookupClassSymbol == NULL)
                                        if (declarationsInThisScope == 0)
                                           {
                                          // size_t numberOfAliasSymbols = current_classDefinition->count_alias_symbol(name);
#if 0
                                             mfprintf(mlog [ WARN ] ) ("case V_SgClassSymbol: numberOfAliasSymbols = %zu \n",numberOfAliasSymbols);
#endif
                                             if (numberOfAliasSymbols >= 2)
                                                {
#if 0
                                                  mfprintf(mlog [ WARN ] ) ("Detected numberOfAliasSymbols >= 2: numberOfAliasSymbols = %zu \n",numberOfAliasSymbols);
#endif
                                                  qualificationDepth += 1;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                                  mfprintf(mlog [ WARN ] ) ("   --- qualificationDepth = %d \n",qualificationDepth);
#endif
                                                }
                                           }
                                      }
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
                                   mfprintf(mlog [ WARN ] ) ("This class is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   mfprintf(mlog [ WARN ] ) ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                 }

                              break;
                            }

                         case V_SgNamespaceSymbol:
                            {
                              SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);
                              ASSERT_not_null(namespaceSymbol);
#if 0
                              mfprintf(mlog [ WARN ] ) ("namespaceSymbol = %p \n",namespaceSymbol);
                              mfprintf(mlog [ WARN ] ) ("namespaceSymbol->get_declaration() = %p \n",namespaceSymbol->get_declaration());
#endif
                              SgNamespaceDeclarationStatement*      associatedNamespaceDeclaration      = namespaceSymbol->get_declaration();
                              SgNamespaceAliasDeclarationStatement* associatedNamespaceAliasDeclaration = namespaceSymbol->get_aliasDeclaration();

                           // DQ (4/9/2018): Adding support for namespace alias.
                           // ASSERT_not_null(namespaceDeclaration);
                              ROSE_ASSERT(namespaceDeclaration != NULL || namespaceAliasDeclaration != NULL);

                           // DQ (4/9/2018): Adding support for namespace alias.
                           // ASSERT_not_null(associatedNamespaceDeclaration);
                              ROSE_ASSERT(associatedNamespaceDeclaration != NULL || associatedNamespaceAliasDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                              mfprintf(mlog [ WARN ] ) ("namespaceDeclaration                = %p \n",namespaceDeclaration);
                              mfprintf(mlog [ WARN ] ) ("namespaceAliasDeclaration           = %p \n",namespaceAliasDeclaration);
                              mfprintf(mlog [ WARN ] ) ("associatedNamespaceDeclaration      = %p \n",associatedNamespaceDeclaration);
                              mfprintf(mlog [ WARN ] ) ("associatedNamespaceAliasDeclaration = %p \n",associatedNamespaceAliasDeclaration);
#endif
                           // if (associatedNamespaceDeclaration->get_firstNondefiningDeclaration() == namespaceDeclaration->get_firstNondefiningDeclaration())
                              if (associatedNamespaceDeclaration != NULL && namespaceDeclaration != NULL &&
                                  associatedNamespaceDeclaration->get_firstNondefiningDeclaration() == namespaceDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This class is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This namespace IS visible from where it is referenced \n");
#endif
                                 }
                                else
                                 {
                                // DQ (4/9/2018): Added support for namespace alias.
                                   if (associatedNamespaceAliasDeclaration != NULL && namespaceAliasDeclaration != NULL &&
                                       associatedNamespaceAliasDeclaration->get_firstNondefiningDeclaration() == namespaceAliasDeclaration->get_firstNondefiningDeclaration())
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("This namespace alias IS visible from where it is referenced \n");
#endif
                                      }
                                     else
                                      {
                                     // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                                        mfprintf(mlog [ WARN ] ) ("This namespace is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                        mfprintf(mlog [ WARN ] ) ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                        mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
                                     // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                        qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
                                        mfprintf(mlog [ WARN ] ) ("case (namespace): qualificationDepth = %d \n",qualificationDepth);
#endif
                                      }
                                 }
                              break;
                            }

                         case V_SgTemplateVariableSymbol:
                         case V_SgVariableSymbol:
                            {
                              SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);
                              ASSERT_not_null(variableSymbol);

                           // This is a variable symbol, check if the declaration is the same.
                           // SgVariableDeclaration* associatedVariableDeclaration = variableSymbol->get_declaration();
                              SgInitializedName* associatedInitializedName = variableSymbol->get_declaration();

                              ASSERT_not_null(variableDeclaration);
                              ASSERT_not_null(associatedInitializedName);

                           // if (associatedInitializedName->get_firstNondefiningDeclaration() == variableDeclaration->get_firstNondefiningDeclaration())
                              if (associatedInitializedName == SageInterface::getFirstInitializedName(variableDeclaration))
                                 {
                                // This variable is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This variable IS visible from where it is referenced \n");
#endif
                                // DQ (12/23/2015): Need to check if there is an opportunity for an ambigous reference.
                                   size_t numberOfAliasSymbols = currentScope->count_alias_symbol(name);
                                   if (numberOfAliasSymbols >= 2)
                                      {
                                        qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("   --- qualificationDepth = %d \n",qualificationDepth);
#endif
                                      }
                                 }
                                else
                                 {
                                // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This variable is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   mfprintf(mlog [ WARN ] ) ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                 }
                              break;
                            }

                      // DQ (12/28/2011): Added support for new template handling in the AST.
                         case V_SgTemplateMemberFunctionSymbol:
                         case V_SgTemplateFunctionSymbol:

                         case V_SgMemberFunctionSymbol:
                         case V_SgFunctionSymbol:
                            {
                              SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                              ASSERT_not_null(functionSymbol);

                              ASSERT_not_null(functionDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Case of SgFunctionSymbol or SgMemberFunctionSymbol: functionSymbol = %p = %s \n",functionSymbol,functionSymbol->class_name().c_str());
                              mfprintf(mlog [ WARN ] ) ("   --- functionDeclaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
#endif
                           // This is a function symbol, check if the declaration is the same.
                           // SgFunctionDeclaration* associatedFunctionDeclaration = functionSymbol->get_declaration();
                              SgFunctionDeclaration* associatedFunctionDeclarationFromSymbol = functionSymbol->get_declaration();
                              ASSERT_not_null(associatedFunctionDeclarationFromSymbol);

                              ASSERT_not_null(functionDeclaration);

                           // DQ (11/19/2013): This is added to support testing cases where we would clearly fail the AST consistancy tests (e.g. LoopProcessing.C).
                           // I hate this work around, but I am hoping it will help identify a root cause of the problem.
                           // ASSERT_not_null(functionDeclaration->get_firstNondefiningDeclaration());
                              if (functionDeclaration->get_firstNondefiningDeclaration() == NULL)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("***** ERROR: In NameQualificationTraversal::nameQualificationDepth(): we are supporting this case though it is a violation of the AST consistancy tests! ***** \n");
                                   return 0;
                                 }

                           // DQ (11/18/2013): This is an assertion inside of get_declaration_associated_with_symbol() which we are now failing.
                              ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() == functionDeclaration->get_firstNondefiningDeclaration()->get_firstNondefiningDeclaration());

                              SgDeclarationStatement* declarationFromSymbol = functionDeclaration->get_declaration_associated_with_symbol();
                           // DQ (11/18/2013): Try to reset this...
                              if (declarationFromSymbol == NULL)
                                 {
#if 0
                                   mfprintf(mlog [ WARN ] ) ("In name qualification support: declarationFromSymbol == NULL: retry using functionDeclaration->get_firstNondefiningDeclaration() \n");
#endif
                                   declarationFromSymbol = functionDeclaration->get_firstNondefiningDeclaration()->get_declaration_associated_with_symbol();
                                 }
                              ASSERT_not_null(declarationFromSymbol);

                              SgFunctionDeclaration* functionDeclarationFromSymbol = isSgFunctionDeclaration(declarationFromSymbol);
                              ASSERT_not_null(functionDeclarationFromSymbol);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("associatedFunctionDeclarationFromSymbol                 = %p = %s \n",associatedFunctionDeclarationFromSymbol,associatedFunctionDeclarationFromSymbol->class_name().c_str());
                              mfprintf(mlog [ WARN ] ) ("--- associatedFunctionDeclarationFromSymbol->get_name() = %s \n",associatedFunctionDeclarationFromSymbol->get_name().str());
                              mfprintf(mlog [ WARN ] ) ("functionDeclarationFromSymbol                           = %p = %s \n",functionDeclarationFromSymbol,functionDeclarationFromSymbol->class_name().c_str());
                              mfprintf(mlog [ WARN ] ) ("--- functionDeclarationFromSymbol->get_name()           = %s \n",functionDeclarationFromSymbol->get_name().str());

                              mfprintf(mlog [ WARN ] ) ("associatedFunctionDeclarationFromSymbol->get_firstNondefiningDeclaration() = %p \n",associatedFunctionDeclarationFromSymbol->get_firstNondefiningDeclaration());
                              mfprintf(mlog [ WARN ] ) ("functionDeclarationFromSymbol->get_firstNondefiningDeclaration()           = %p \n",functionDeclarationFromSymbol->get_firstNondefiningDeclaration());
                              if (associatedFunctionDeclarationFromSymbol->get_firstNondefiningDeclaration() == NULL)
                                 {
                                // DQ (6/22/2011): This is the case when a function has only a defining declaration and in this case no nondefining declaration is built for a function.
                                // This is true for both SgFunctionDeclaration and SgMemberFunctionDeclaration handling (but may change to be more uniform with other declarations in the future).
                                   mfprintf(mlog [ WARN ] ) ("Found a valid function with get_firstNondefiningDeclaration() == NULL (not a problem, just a special case)\n");
                                 }
#endif

                           // if (associatedFunctionDeclaration->get_firstNondefiningDeclaration() == functionDeclaration->get_firstNondefiningDeclaration())
                              if (associatedFunctionDeclarationFromSymbol == functionDeclarationFromSymbol)
                                 {
                                // DQ (4/12/2014): Now we know that it can be found, but we still need to check if there would
                                // be another function that could be used and for which we need name qualification to avoid.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 0
                                   printf ("Using foundAnOverloadedFunctionWithSameName = %s \n",foundAnOverloadedFunctionWithSameName ? "true" : "false");
                                   printf ("Using foundAnOverloadedFunctionInSameScope  = %s \n",foundAnOverloadedFunctionInSameScope  ? "true" : "false");
#endif
                                // DQ (4/12/2014): We need to use the recorded value foundAnOverloadedFunctionWithSameName because we may want to have force name qualification.
                                   if (foundAnOverloadedFunctionWithSameName == true)
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("There was another function identified in the process of resolving that this function could be found. thus we will require some name qualification \n");
#endif
                                        if (foundAnOverloadedFunctionInSameScope == false)
                                           {
#if (DEBUG_NAME_QUALIFICATION_LEVEL >= 1)
                                             mfprintf(mlog [ WARN ] ) ("In name qualification support: case V_SgFunctionSymbol: We need to compute the CORRECT name qualification depth: using 1 for now! \n");
#endif
                                             qualificationDepth = 1;
                                           }
                                      }
                                // DQ (6/20/2011): But we don't check for if there was another declaration that might be a problem (overloaded functions don't count!)...
                                // This function is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This function or member function IS visible from where it is referenced (but there could still be ambiguity if this was just the first of several symbols found in the current scope) \n");
                                   mfprintf(mlog [ WARN ] ) ("   --- currentScope        = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("   --- functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());

                                // But we need to check if there is another such symbol in the same scope that would trigger qualification.
                                // SgScopeStatement* associatedScope = associatedFunctionDeclaration->get_scope();
                                // ASSERT_not_null(associatedScope);
                                // mfprintf(mlog [ WARN ] ) ("Searching associatedScope = %p = %s \n",associatedScope,associatedScope->class_name().c_str());
                                   SgClassDefinition* classDefinition = isSgClassDefinition(functionDeclaration->get_parent());
                                   if (classDefinition != NULL)
                                      {
                                        mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                        mfprintf(mlog [ WARN ] ) ("Searching classDefinition = %p \n",classDefinition);

                                     // int numberOfSymbolsWithMatchingName = numberOfSymbolsWithName(name,associatedScope);
                                        int numberOfSymbolsWithMatchingName = numberOfSymbolsWithName(name,classDefinition);
                                        mfprintf(mlog [ WARN ] ) ("numberOfSymbolsWithMatchingName = %d \n",numberOfSymbolsWithMatchingName);

                                     // ROSE_ASSERT(numberOfSymbolsWithMatchingName == 1);
                                      }
#endif
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the function declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This function or member function is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   mfprintf(mlog [ WARN ] ) ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif

                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                                   mfprintf(mlog [ WARN ] ) ("   --- qualificationDepth = %d \n",qualificationDepth);
#endif
                                 }
                              break;
                            }

                      // DQ (11/10/2014): Adding support for templated typedef declarations.
                         case V_SgTemplateTypedefSymbol:

                         case V_SgTypedefSymbol:
                            {
                              SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                              ASSERT_not_null(typedefSymbol);

                           // This is a typdef symbol, check if the declaration is the same.
                              SgTypedefDeclaration* associatedTypedefDeclaration = typedefSymbol->get_declaration();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("associatedTypedefDeclaration = %p = %s \n",associatedTypedefDeclaration,associatedTypedefDeclaration->get_name().str());
#endif
                              ASSERT_not_null(typedefDeclaration);
                              ASSERT_not_null(associatedTypedefDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("associatedTypedefDeclaration->get_firstNondefiningDeclaration() = %p \n",associatedTypedefDeclaration->get_firstNondefiningDeclaration());
                              mfprintf(mlog [ WARN ] ) ("typedefDeclaration->get_firstNondefiningDeclaration()           = %p \n",typedefDeclaration->get_firstNondefiningDeclaration());
                              mfprintf(mlog [ WARN ] ) ("currentScope                                                    = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                              if (associatedTypedefDeclaration->get_firstNondefiningDeclaration() == typedefDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This typedef is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This typedef IS visible from where it is referenced \n");
#endif
                                // DQ (2/8/2019): If type elaboration was required, and the symbol is from a base class.
                                // Then we need name qualification because the type elaboration will not protect the type
                                // reference from being hidden.
                                   if (typeElaborationIsRequired == true)
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("   --- Since type elaboration was required because it was hidden, add name qualification to support it being unambiguous \n");
#endif
                                        qualificationDepth = 1;
                                      }
#if 0
                                   mfprintf(mlog [ WARN ] ) ("case V_SgTypedefSymbol: name         = %s \n",name.str());
                                   mfprintf(mlog [ WARN ] ) ("case V_SgTypedefSymbol: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif

                                   bool includeCurrentScope = true;
                                   SgClassDefinition* current_classDefinition = SageInterface::getEnclosingNode<SgClassDefinition>(currentScope,includeCurrentScope);
                                   if (current_classDefinition != NULL)
                                      {
                                        size_t numberOfAliasSymbols = current_classDefinition->count_alias_symbol(name);
#if 0
                                        mfprintf(mlog [ WARN ] ) ("case V_SgTypedefSymbol: numberOfAliasSymbols = %zu \n",numberOfAliasSymbols);
#endif
                                        if (numberOfAliasSymbols >= 2)
                                           {
#if 0
                                             mfprintf(mlog [ WARN ] ) ("Detected numberOfAliasSymbols >= 2: numberOfAliasSymbols = %zu \n",numberOfAliasSymbols);
#endif
                                             qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                          // qualificationDepth += 1;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                             mfprintf(mlog [ WARN ] ) ("   --- qualificationDepth = %d \n",qualificationDepth);
#endif
                                           }
                                      }
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This typedef is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   mfprintf(mlog [ WARN ] ) ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                 }

                              break;
                            }

                         case V_SgTemplateSymbol:
                            {
                              // FIXME Don't think it should occur as this node has been deprecated for a long time
                              break;
                            }

                         case V_SgEnumSymbol:
                            {
                              SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                              ASSERT_not_null(enumSymbol);

                           // This is a typdef symbol, check if the declaration is the same.
                              SgEnumDeclaration* associatedEnumDeclaration = enumSymbol->get_declaration();

                              ASSERT_not_null(enumDeclaration);
                              ASSERT_not_null(associatedEnumDeclaration);

                              if (associatedEnumDeclaration->get_firstNondefiningDeclaration() == enumDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This class is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This enum IS visible from where it is referenced \n");
#endif
                                // DQ (2/8/2019): If type elaboration was required, and the symbol is from a base class.
                                // Then we need name qualification because the type elaboration will not protect the type
                                // reference from being hidden.
                                   if (typeElaborationIsRequired == true)
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        mfprintf(mlog [ WARN ] ) ("   --- Since type elaboration was required because it was hidden, add name qualification to support it being unambiguous \n");
#endif
                                     // DQ (2/14/2019): If this is a typedef that is hidden by a variable then we don't require extra name qualification.
                                        if (isSgVariableSymbol(original_symbol_lookedup_by_name) != NULL)
                                           {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                             mfprintf(mlog [ WARN ] ) ("This enum IS visible and variables can't hide types, so no extra name qualification is required \n");
#endif
                                           }
                                          else
                                           {
                                             qualificationDepth = 1;
                                           }
                                      }

                                   bool includeCurrentScope = true;
                                   SgClassDefinition* current_classDefinition = SageInterface::getEnclosingNode<SgClassDefinition>(currentScope,includeCurrentScope);
                                   if (current_classDefinition != NULL)
                                      {
                                        size_t numberOfAliasSymbols = current_classDefinition->count_alias_symbol(name);
#if 0
                                        mfprintf(mlog [ WARN ] ) ("case V_SgEnumSymbol: numberOfAliasSymbols = %zu \n",numberOfAliasSymbols);
#endif
                                        if (numberOfAliasSymbols >= 2)
                                           {
#if 0
                                             mfprintf(mlog [ WARN ] ) ("Detected numberOfAliasSymbols >= 2: numberOfAliasSymbols = %zu \n",numberOfAliasSymbols);
#endif
                                             qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                             mfprintf(mlog [ WARN ] ) ("   --- qualificationDepth = %d \n",qualificationDepth);
#endif
                                           }
                                      }
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("This enum is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   mfprintf(mlog [ WARN ] ) ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                 }

                              break;
                            }

                         case V_SgNonrealSymbol:
                            {
#if WARNING_FOR_NONREAL_DEVEL
                              mfprintf(mlog [ WARN ] ) ("WARNING: Support for name qualification depth for SgNonrealSymbol is not implemented yet \n");
#endif
                              break;
                            }

                         default:
                            {
                           // Handle cases are we work through specific example codes.
                              mfprintf(mlog [ WARN ] ) ("default reached symbol = %s \n",symbol->class_name().c_str());
                              ROSE_ABORT();
                            }
                       }
                  }
             }
            else
             {
            // DQ (4/2/2018): This is the predicate for this false branch.
               ROSE_ASSERT(symbol == NULL);

            // This class is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               mfprintf(mlog [ WARN ] ) ("This declaration = %p = %s is NOT visible from where it is referenced (no declaration with same name, calling nameQualificationDepthOfParent()) \n",
                    declaration,declaration->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("   --- currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());

               mfprintf(mlog [ WARN ] ) ("Calling nameQualificationDepthOfParent() \n");
#endif
               qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
               mfprintf(mlog [ WARN ] ) ("NOTE: If the class is visible from where it is referenced then why are are adding one? \n");
#endif
             }
        }

  // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
  // DQ (12/10/2016): Debugging information that makes sure that typeElaborationIsRequired is used and so will not generate a warning.
  // This is a variable that is essential for internal debugging so we certainly don't want to eliminate it.
     if (typeElaborationIsRequired == true)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Note that typeElaborationIsRequired == true \n");
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): qualificationDepth = %d Report type elaboration: typeElaborationIsRequired = %s \n",
          qualificationDepth,(typeElaborationIsRequired == true) ? "true" : "false");
#endif

#if DEBUG_NONTERMINATION
     printf("Leaving NameQualificationTraversal::nameQualificationDepth(): declaration  = %p = %s = %s \n",
          declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
#endif

     return qualificationDepth;
   }


SgDeclarationStatement*
NameQualificationTraversal::getDeclarationAssociatedWithType( SgType* type )
   {
  // Note that this function could be eliminated since it only wraps another function.

     ASSERT_not_null(type);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In getDeclarationAssociatedWithType(): type = %s \n",type->class_name().c_str());
#endif

  // DQ (4/15/2019): Strip away any wrapped types (e.g. pointers and references).
  // Note: SgPointerMemberType is processed explicitly.
     SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
     ASSERT_not_null(strippedType);

  // Use the stripped type to evaluate the associated declaration.
     type = strippedType;

  // DQ (4/28/2019): Note that this function calls stripType(), so it's use above is redundant.
     SgDeclarationStatement* declaration = type->getAssociatedDeclaration();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In getDeclarationAssociatedWithType(): declaration = %p \n",declaration);
#endif

  // Primative types will not have an asociated declaration...
  // ASSERT_not_null(declaration);
     if (declaration == NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In getDeclarationAssociatedWithType(): declaration == NULL type = %s \n",type->class_name().c_str());
#endif
        }
       else
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In getDeclarationAssociatedWithType(): declaration                                    = %p = %s \n",declaration,SageInterface::get_name(declaration).c_str());
          mfprintf(mlog [ WARN ] ) ("In getDeclarationAssociatedWithType(): declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
          mfprintf(mlog [ WARN ] ) ("In getDeclarationAssociatedWithType(): declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          if (declaration != declaration->get_firstNondefiningDeclaration())
             {
            // Output some debug information to learn more about this error
               mfprintf(mlog [ WARN ] ) ("In getDeclarationAssociatedWithType(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
               ASSERT_not_null(declaration->get_file_info());
               declaration->get_file_info()->display("declaration");

               ASSERT_not_null(declaration->get_firstNondefiningDeclaration());
               mfprintf(mlog [ WARN ] ) ("In getDeclarationAssociatedWithType(): declaration->get_firstNondefiningDeclaration() = %p = %s \n",declaration->get_firstNondefiningDeclaration(),declaration->get_firstNondefiningDeclaration()->class_name().c_str());
               ASSERT_not_null(declaration->get_firstNondefiningDeclaration()->get_file_info());
               declaration->get_firstNondefiningDeclaration()->get_file_info()->display("declaration->get_firstNondefiningDeclaration()");
             }
#endif

          ROSE_ASSERT(declaration == declaration->get_firstNondefiningDeclaration() || isSgEnumDeclaration(declaration) != NULL);
        }

     return declaration;
   }


#define DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS 0

// void evaluateNameQualificationForTemplateArgumentList (SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement);
void
NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList (SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement)
   {
  // DQ (6/4/2011): Note that test2005_73.C demonstrate where the Template arguments are shared between template instantiations.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
  // DQ (9/24/2012): Track the recursive depth in computing name qualification for template arguments of template instantiations used as template arguments.
     static int recursiveDepth = 0;
     int counter = 0;
     mfprintf(mlog [ WARN ] ) ("\n\n*********************************************************************************************************************\n");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): templateArgumentList.size() = %" PRIuPTR " recursiveDepth = %d \n",templateArgumentList.size(),recursiveDepth);
     mfprintf(mlog [ WARN ] ) ("*********************************************************************************************************************\n");

     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): currentScope = %p = %s positionStatement = %p = %s \n",
          currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());

     ASSERT_not_null(positionStatement);
     positionStatement->get_file_info()->display("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList()");
#endif

     SgTemplateArgumentPtrList::iterator i = templateArgumentList.begin();
     while (i != templateArgumentList.end())
        {
          SgTemplateArgument* templateArgument = *i;
          ASSERT_not_null(templateArgument);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
          mfprintf(mlog [ WARN ] ) ("*** Processing template argument #%d templateArgument = %p \n",counter,templateArgument);
       // SgName testNameInMap = templateArgument->get_qualified_name_prefix();
#endif
#if 0
          mfprintf(mlog [ WARN ] ) ("===== Before being finished with evaluation of templateArgument name qualification: testNameInMap = %s \n",templateArgument->unparseToString().c_str());
#endif

       // DQ (5/29/2019): Newer version of code (still refactoring this section).
          switch (templateArgument->get_argumentType())
             {
               case SgTemplateArgument::type_argument:
                  {
                    ASSERT_not_null(templateArgument->get_type());
                    SgType* type = templateArgument->get_type();

                    ASSERT_not_null(type);

                    break;
                  }

               case SgTemplateArgument::nontype_argument:
                  {
                 // DQ (8/12/2013): This can be either an SgExpression or SgInitializedName.
                 // ASSERT_not_null(templateArgument->get_expression());
                    ROSE_ASSERT (templateArgument->get_expression() != NULL || templateArgument->get_initializedName() != NULL);
                    ROSE_ASSERT (templateArgument->get_expression() == NULL || templateArgument->get_initializedName() == NULL);
                    if (templateArgument->get_expression() != NULL)
                       {
                         SgExpression* expression = templateArgument->get_expression();

                         ASSERT_not_null(expression);
                       }
                      else
                       {
                         SgType* type = templateArgument->get_initializedName()->get_type();
                         ASSERT_not_null(type);
                         SgInitializedName * iname = templateArgument->get_initializedName();

                         ASSERT_not_null(iname);
                       }

                    break;
                  }

               case SgTemplateArgument::template_template_argument:
                  {
                 // SgDeclarationStatement * tpldecl = templateArgument->get_templateDeclaration();
                    SgDeclarationStatement * decl = templateArgument->get_templateDeclaration();
                    ASSERT_not_null(decl);

                    SgTemplateDeclaration * tpl_decl = isSgTemplateDeclaration(decl);
                    ROSE_ASSERT(tpl_decl == NULL);

                    break;
                  }

                case SgTemplateArgument::start_of_pack_expansion_argument:
                  {
#if 0
                    mfprintf(mlog [ WARN ] ) ("WARNING: start_of_pack_expansion_argument in evaluateNameQualificationForTemplateArgumentList (can happen from some debug output)\n");
#endif
                    break;
                  }

               case SgTemplateArgument::argument_undefined:
                  {
                    mfprintf(mlog [ WARN ] ) ("Error argument_undefined in evaluateNameQualificationForTemplateArgumentList \n");
                    ROSE_ABORT();
                    break;
                  }

               default:
                  {
                    mfprintf(mlog [ WARN ] ) ("Error default reached in evaluateNameQualificationForTemplateArgumentList \n");
                    ROSE_ABORT();
                  }
             }

       // DQ (5/29/2019): Older version of code.
          SgType* type                     = templateArgument->get_type();
          SgExpression* expression         = templateArgument->get_expression();
          SgDeclarationStatement * tpldecl = templateArgument->get_templateDeclaration();
          SgInitializedName * iname        = templateArgument->get_initializedName();
          if (type != NULL)
             {
            // Reduce the type to the base type stripping off wrappers that would hide the fundamental type inside.
               SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
               ASSERT_not_null(strippedType);

            // SgNamedType* namedType = isSgNamedType(type);
               SgNamedType* namedType = isSgNamedType(strippedType);
               if (namedType != NULL)
                  {
                 // This could be a type that requires name qualification (reference to a declaration).

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                    mfprintf(mlog [ WARN ] ) ("templateArgument = %p contains type which is namedType = %p = %s \n",templateArgument,namedType,namedType->class_name().c_str());
#endif
                    SgDeclarationStatement* templateArgumentTypeDeclaration = getDeclarationAssociatedWithType(type);
                    if (templateArgumentTypeDeclaration != NULL)
                       {
                      // Check the visability and unambiguity of this declaration.
                      // Note that since the recursion happens before we set the names, all qualified name are set first
                      // at the nested types and then used in the setting of qualified names at the higher level types
                      // (less nested types).

                      // DQ (5/15/2011): Added recursive handling of template arguments which can require name qualification.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                         mfprintf(mlog [ WARN ] ) ("xxxxxx --- Making a RECURSIVE call to nameQualificationDepth() on the template argument recursiveDepth = %d \n",recursiveDepth);
                      // DQ (9/24/2012): I think this is the way to make the recursive call to handle name qualification of template arguments in nexted template instantiations.
                         mfprintf(mlog [ WARN ] ) ("Need to call evaluateNameQualificationForTemplateArgumentList() on any possible template argument list for type in templateArgument = %p (namely namedType = %p = %s) \n",
                              templateArgument,namedType,namedType->class_name().c_str());
#endif

                         SgClassType* classType = isSgClassType(namedType);
                         SgNonrealType* nrType = isSgNonrealType(namedType);
                         if (classType != NULL)
                            {
                           // If this is a class then it should be relative to it's declaration.
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                              ASSERT_not_null(classDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                              mfprintf(mlog [ WARN ] ) ("namedType is a SgClassType: classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
#endif
                              SgTemplateInstantiationDecl* templateClassInstantiationDeclaration = isSgTemplateInstantiationDecl(classDeclaration);
                              if (templateClassInstantiationDeclaration != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   recursiveDepth++;
#endif
                                   evaluateNameQualificationForTemplateArgumentList(templateClassInstantiationDeclaration->get_templateArguments(),currentScope,positionStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   recursiveDepth--;
#endif
                                 }
                            }
                           else if (nrType == NULL)
                            {
                           // If not a class then (e.g. typedef) then it is relative to the typedef declaration, but we don't have to recursively evaluate the type.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                              mfprintf(mlog [ WARN ] ) ("This is not a SgClassType nor a SgNonrealType, so we don't have to recursively evaluate for template arguments. \n");
#endif
                            }

                         if (nrType != NULL) {
                           SgNonrealDecl * nrdecl = isSgNonrealDecl(nrType->get_declaration());
                           ASSERT_not_null(nrdecl);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                           mfprintf(mlog [ WARN ] ) ("namedType is a SgNonrealType: nrdecl = %p = %s \n",nrdecl,nrdecl->class_name().c_str());
#endif
                           do {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                             recursiveDepth++;
#endif
                             evaluateNameQualificationForTemplateArgumentList(nrdecl->get_tpl_args(), currentScope, positionStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                             recursiveDepth--;
#endif

                             if (nrdecl->get_templateDeclaration() != NULL) {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                               mfprintf(mlog [ WARN ] ) (" - nrdecl->get_templateDeclaration() = %p = %s \n", nrdecl->get_templateDeclaration(), nrdecl->get_templateDeclaration() ? nrdecl->get_templateDeclaration()->class_name().c_str() : "");
#endif
                               int amountOfNameQualificationRequired = nameQualificationDepth(nrdecl->get_templateDeclaration(),currentScope,positionStatement);
                               setNameQualification(templateArgument,nrdecl->get_templateDeclaration(),amountOfNameQualificationRequired);
                             }

                             SgNode * nrdecl_parent = nrdecl->get_parent();
                             ASSERT_not_null(nrdecl_parent);
                             nrdecl_parent = nrdecl_parent->get_parent();
                             ASSERT_not_null(nrdecl_parent);

                             ROSE_ASSERT(nrdecl_parent != nrdecl); // That would be a loop...

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                             mfprintf(mlog [ WARN ] ) (" - nrdecl_parent = %p = %s \n", nrdecl_parent, nrdecl_parent->class_name().c_str());
#endif

                             nrdecl = isSgNonrealDecl(nrdecl_parent);
                           } while (nrdecl != NULL);

                         } else {
                           int amountOfNameQualificationRequiredForTemplateArgument = nameQualificationDepth(namedType,currentScope,positionStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                           mfprintf(mlog [ WARN ] ) ("xxxxxx --- amountOfNameQualificationRequiredForTemplateArgument = %d (for type = %p (%s) = %s) (counter = %d recursiveDepth = %d) \n",
                                amountOfNameQualificationRequiredForTemplateArgument,namedType,namedType->class_name().c_str(), namedType->get_name().str(),counter,recursiveDepth);
                           mfprintf(mlog [ WARN ] ) ("xxxxxx --- Must call a function to set the name qualification data in the SgTemplateArgument = %p \n",templateArgument);
#endif

                        // TV (10/09/2018): FIXME ROSE-1511
                           SgNamespaceDefinitionStatement * nsp_defn = isSgNamespaceDefinitionStatement(currentScope);
                           if (nsp_defn != NULL) {
                             SgNamespaceDeclarationStatement * nsp_decl = nsp_defn->get_namespaceDeclaration();
                             ASSERT_not_null(nsp_decl);
                             if (nsp_decl->get_name() == "std" && (
                                    namedType->get_name().getString().find("allocator") == 0 ||
                                    namedType->get_name().getString().find("less") == 0
                                  )) {
                               amountOfNameQualificationRequiredForTemplateArgument = 1;
                             }
                           }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                           mfprintf(mlog [ WARN ] ) ("templateArgumentTypeDeclaration = %p = %s \n",templateArgumentTypeDeclaration,templateArgumentTypeDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION || 0
                        // DQ (5/5/2024): Debugging non-termination behavior.
                           printf("calling setNameQualification(templateArgument): templateArgumentTypeDeclaration = %p = %s \n",templateArgumentTypeDeclaration,templateArgumentTypeDeclaration->class_name().c_str());
                           flush(cout);
#endif
                           setNameQualification(templateArgument,templateArgumentTypeDeclaration,amountOfNameQualificationRequiredForTemplateArgument);
                         }
                       }
                  }

            // If this was not a SgNamedType (and even if it is, see test2011_117.C), it still might be a type
            // where name qualification is required (might be a SgArrayType with an index requiring qualification).
               processNameQualificationForPossibleArrayType(type,currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): DONE: processing type = %p = %s \n",type,type->class_name().c_str());
#endif
             }
            else if (expression != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
               mfprintf(mlog [ WARN ] ) ("Template argument was an expression = %p \n",expression);
#endif
            // Check if this is a variable in which case it might require name qualification.  If we we have to traverse this expression recursively.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
            // We need to traverse this expression and evaluate if any name qualification is required on its pieces (e.g. referenced variables)
               mfprintf(mlog [ WARN ] ) ("Call to generateNestedTraversalWithExplicitScope() with expression = %p = %s \n",expression,expression->class_name().c_str());
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): Calling generateNestedTraversalWithExplicitScope(): with expression = %p = %s and currentScope = %p = %s \n",
                    expression,expression->class_name().c_str(),currentScope,currentScope->class_name().c_str());
#endif
            // DQ (3/15/2019): Added Comment: This is required because the expression can be a subtree that would have to be seperately traversed.
               generateNestedTraversalWithExplicitScope(expression,currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
                    mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): DONE: Call to generateNestedTraversalWithExplicitScope() with expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
             }
            else if (iname != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): Empty case: template argument is an initialized name = %p \n",iname);
#endif
             }
            else if (tpldecl != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): template argument is a template = %p (%s)\n", tpldecl, tpldecl->class_name().c_str());
#endif
              int amountOfNameQualificationRequiredForTemplateArgument = nameQualificationDepth( tpldecl, currentScope, positionStatement );
              setNameQualification(templateArgument,tpldecl,amountOfNameQualificationRequiredForTemplateArgument);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): DONE: template argument is a template = %p (%s)\n", tpldecl, tpldecl->class_name().c_str());
#endif
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
          mfprintf(mlog [ WARN ] ) ("===== After finishing with evaluation of templateArgument name qualification: templateArgument = %p testNameInMap = %s \n",templateArgument,templateArgument->unparseToString().c_str());
          mfprintf(mlog [ WARN ] ) ("===== templateArgument->unparseToString() = %s \n",templateArgument->unparseToString().c_str());
#endif

          i++;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
       // Used for debugging...
          counter++;
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_TEMPLATE_ARGUMENTS
     mfprintf(mlog [ WARN ] ) ("*****************************************************************************************************************************\n");
     mfprintf(mlog [ WARN ] ) ("Leaving NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): templateArgumentList.size() = %" PRIuPTR " recursiveDepth = %d \n",templateArgumentList.size(),recursiveDepth);
     mfprintf(mlog [ WARN ] ) ("*****************************************************************************************************************************\n\n");
#endif
   }

#define DEBUG_NAME_QUALIFICATION_LEVEL_FOR_NAME_QUALIFICATION_DEPTH 0

int
NameQualificationTraversal::nameQualificationDepth ( SgType* type, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
     int amountOfNameQualificationRequired = 0;

     ASSERT_not_null(type);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_NAME_QUALIFICATION_DEPTH || 0
     mfprintf(mlog [ WARN ] ) ("In nameQualificationDepth(SgType*): type = %p = %s \n",type,type->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
     static int counter = 0;
     printf("In nameQualificationDepth(SgType*): counter = %d type = %p = %s \n",counter,type,type->class_name().c_str());
     counter++;
#endif

  // DQ (7/23/2011): If this is an array type, then we need special processing for any name qualification of its index expressions.
     processNameQualificationForPossibleArrayType(type,currentScope);

     SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(type);
     if (declaration != NULL)
        {
       // Check the visability and unambiguity of this declaration.
#if DEBUG_NONTERMINATION
          printf("In nameQualificationDepth(SgType*): declaration != NULL: declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
          amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_NAME_QUALIFICATION_DEPTH
          mfprintf(mlog [ WARN ] ) ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
#if DEBUG_NONTERMINATION
          printf("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
        }
       else
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_NAME_QUALIFICATION_DEPTH
          mfprintf(mlog [ WARN ] ) ("ERROR: In nameQualificationDepth(SgType*): declaration NOT found for type = %p = %s\n",type,type->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("ERROR: In nameQualificationDepth(SgType*): declaration NOT found for type = %p = %s\n",type,type->class_name().c_str());
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NAME_QUALIFICATION_LEVEL_FOR_NAME_QUALIFICATION_DEPTH
     mfprintf(mlog [ WARN ] ) ("Leaving nameQualificationDepth(SgType*): type = %p = %s amountOfNameQualificationRequired = %d \n",type,type->class_name().c_str(),amountOfNameQualificationRequired);
#endif
#if DEBUG_NONTERMINATION
     counter--;
     printf("Leaving nameQualificationDepth(SgType*): counter = %d type = %p = %s amountOfNameQualificationRequired = %d \n",
          counter,type,type->class_name().c_str(),amountOfNameQualificationRequired);
     flush(cout);
#endif

     return amountOfNameQualificationRequired;
   }


int
NameQualificationTraversal::nameQualificationDepthForType ( SgInitializedName* initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
     ASSERT_not_null(initializedName);
     ASSERT_not_null(positionStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In nameQualificationDepthForType(): initializedName = %s type = %p = %s currentScope = %p = %s \n",initializedName->get_name().str(),initializedName->get_type(),initializedName->get_type()->class_name().c_str(),currentScope,currentScope->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
     printf("In nameQualificationDepthForType(): initializedName = %s type = %p = %s currentScope = %p = %s \n",
          initializedName->get_name().str(),initializedName->get_type(),initializedName->get_type()->class_name().c_str(),
          currentScope,currentScope->class_name().c_str());
#endif

     SgType* initializedNameType = initializedName->get_type();

     SgPointerMemberType* pointerMemberType = isSgPointerMemberType(initializedNameType);
     if (pointerMemberType != nullptr)
        {
          SgType* baseType = pointerMemberType->get_base_type();
          ASSERT_not_null(baseType);

       // Handle member functions as a special case.
          SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(baseType);
          if (memberFunctionType != NULL)
             {
               SgType* returnType = memberFunctionType->get_return_type();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("nameQualificationDepthForType(): case SgPointerMemberType: Reset associated initializedNameType: returnType = %p = %s \n",returnType,returnType->class_name().c_str());
#endif
               initializedNameType = returnType;
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("nameQualificationDepthForType(): case SgPointerMemberType: Reset associated initializedNameType: baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
               initializedNameType = baseType;
             }
        }

#if DEBUG_NONTERMINATION
     printf ("Calling nameQualificationDepth(): initializedNameType = %p = %s \n",initializedNameType,initializedNameType->class_name().c_str());
#endif

  // return nameQualificationDepth(initializedName->get_type(),initializedName->get_scope(),positionStatement);
  // return nameQualificationDepth(initializedName->get_type(),currentScope,positionStatement);
     return nameQualificationDepth(initializedNameType,currentScope,positionStatement);
   }

int
NameQualificationTraversal::nameQualificationDepth ( SgInitializedName* initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
     int amountOfNameQualificationRequired = 0;

     ASSERT_not_null(initializedName);
     ASSERT_not_null(currentScope);

     SgName name = initializedName->get_name();

  // DQ (6/5/2011): Test if this has a valid name (if not then it need not be qualified).
  // Examples of tests codes: test2005_114.C and test2011_73.C.
     if (name.is_null() == true)
        {
       // An empty name implies that no name qualification would make sense.
          return 0;
        }

     ASSERT_not_null(initializedName->get_scope());
     SgDeclarationStatement* declaration = associatedDeclaration(initializedName->get_scope());
     SgVariableSymbol* variableSymbol = NULL;

  // DQ (8/16/2013): Modified to support new API.
     SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope,NULL,NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(SgInitializedName* = %p): symbol = %p \n",initializedName,symbol);
#endif

     if (symbol != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Lookup symbol based on name only (via parents starting at currentScope = %p = %s: name = %s symbol = %p = %s) \n",currentScope,currentScope->class_name().c_str(),name.str(),symbol,symbol->class_name().c_str());
#endif

       // Loop over possible chain of alias symbols to find the original sysmbol.
          SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);

       // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
          ROSE_ASSERT(aliasSymbol == NULL);

          while (aliasSymbol != NULL)
             {
            // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
               ROSE_ABORT();
#if 1
            // DQ (7/12/2014): debugging use of SgAliasSymbol.
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(): resolving alias symbol in loop: alias = %p baseSymbol = %p = %s \n",aliasSymbol,aliasSymbol->get_alias(),aliasSymbol->get_alias()->class_name().c_str());
#endif
               symbol = aliasSymbol->get_alias();
               aliasSymbol = isSgAliasSymbol(symbol);
             }
          ROSE_ASSERT(isSgAliasSymbol(symbol) == NULL);
          variableSymbol = isSgVariableSymbol(symbol);

          if (variableSymbol == NULL)
             {
               variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(name,currentScope);

            // ASSERT_not_null(variableSymbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               if (variableSymbol != NULL)
                  {
                    mfprintf(mlog [ WARN ] ) ("Lookup symbol based symbol type: variableSymbol = %p = %s \n",variableSymbol,variableSymbol->class_name().c_str());
                  }
                 else
                  {
                    mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(SgInitializedName*,SgScopeStatement*,SgStatement*): variableSymbol == NULL \n");
                  }
#endif

               if (declaration != NULL)
                  {
                    amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
                  }
             }
            else
             {

#define DEBUG_SKIP_VARIABLE_SYMBOL 0

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_SKIP_VARIABLE_SYMBOL
               mfprintf(mlog [ WARN ] ) ("initializedName->get_prev_decl_item() = %p \n",initializedName->get_prev_decl_item());
               mfprintf(mlog [ WARN ] ) ("initializedName->get_parent() = %p = %s \n",initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
#endif
            // DQ (6/1/2019): If this is associated with an extern declaration then don't use this symbol (see test2019_470.C).
            // This should not apply to enum values which we would want to detect the correct symbol for to support the name qualification.
               bool skipThisSymbol = true;

            // Check if the initializedName is appearing in different scopes, which would trigger name qualification.
               if (initializedName->get_prev_decl_item() != NULL)
                  {
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedName->get_prev_decl_item()->get_parent());
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_SKIP_VARIABLE_SYMBOL
                    mfprintf(mlog [ WARN ] ) ("variableDeclaration = %p \n",variableDeclaration);
#endif
                 // if (variableDeclaration != NULL && variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() && variableDeclaration->get_linkage().empty() == true)
                    if (variableDeclaration != NULL)
                       {
                         SgVariableDeclaration* possible_extern_variableDeclaration = isSgVariableDeclaration(initializedName->get_prev_decl_item()->get_parent());
                         SgVariableDeclaration* original_variableDeclaration        = isSgVariableDeclaration(initializedName->get_parent());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_SKIP_VARIABLE_SYMBOL
                         mfprintf(mlog [ WARN ] ) ("possible_extern_variableDeclaration = %p \n",possible_extern_variableDeclaration);
                         mfprintf(mlog [ WARN ] ) ("original_variableDeclaration        = %p \n",original_variableDeclaration);
#endif
                         if (possible_extern_variableDeclaration != NULL && original_variableDeclaration != NULL)
                            {
                           // Need to check if either of these declarations was marked as extern.
                              bool possible_extern_foundExternModifier = (possible_extern_variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() &&
                                                                          possible_extern_variableDeclaration->get_linkage().empty() == true);
                              bool original_foundExternModifier        = (original_variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() &&
                                                                          original_variableDeclaration->get_linkage().empty() == true);
                              bool foundExternModifier = (possible_extern_foundExternModifier || original_foundExternModifier);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_SKIP_VARIABLE_SYMBOL
                              mfprintf(mlog [ WARN ] ) ("foundExternModifier = %s \n",foundExternModifier ? "true" : "false");
#endif
                              if (foundExternModifier == true)
                                 {
                                   SgScopeStatement* possible_extern_variable_scope = possible_extern_variableDeclaration->get_scope();
                                   SgScopeStatement* original_variable_scope        = original_variableDeclaration->get_scope();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_SKIP_VARIABLE_SYMBOL
                                   mfprintf(mlog [ WARN ] ) ("possible_extern_variable_scope = %p = %s \n",possible_extern_variable_scope,possible_extern_variable_scope->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("original_variable_scope        = %p = %s \n",original_variable_scope,original_variable_scope->class_name().c_str());
#endif
                                   SgNamespaceDefinitionStatement* possible_extern_variable_namespace_definition = isSgNamespaceDefinitionStatement(possible_extern_variable_scope);
                                   SgNamespaceDefinitionStatement* original_variable_namespace_definition        = isSgNamespaceDefinitionStatement(original_variable_scope);
                                   if (possible_extern_variable_namespace_definition != NULL && original_variable_namespace_definition != NULL)
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_SKIP_VARIABLE_SYMBOL
                                        mfprintf(mlog [ WARN ] ) ("possible_extern_variable_namespace_definition = %p \n",possible_extern_variable_namespace_definition);
                                        mfprintf(mlog [ WARN ] ) ("original_variable_namespace_definition        = %p \n",original_variable_namespace_definition);
                                        mfprintf(mlog [ WARN ] ) ("possible_extern_variable_namespace_definition->get_global_definition() = %p \n",possible_extern_variable_namespace_definition->get_global_definition());
                                        mfprintf(mlog [ WARN ] ) ("original_variable_namespace_definition->get_global_definition()        = %p \n",original_variable_namespace_definition->get_global_definition());
#endif
                                        if (possible_extern_variable_namespace_definition->get_global_definition() == original_variable_namespace_definition->get_global_definition())
                                           {
                                             skipThisSymbol = false;
                                           }
                                      }
                                     else
                                      {
                                        ROSE_ASSERT(possible_extern_variable_scope != NULL && original_variable_scope != NULL);
                                        if (possible_extern_variable_scope == original_variable_scope)
                                           {
                                             skipThisSymbol = false;
                                           }
                                      }
                                 }
                                else
                                 {
                                // If neither is marked extern, then they should not be name qualified.
                                   skipThisSymbol = false;
                                 }
                            }
                       }
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_SKIP_VARIABLE_SYMBOL
               mfprintf(mlog [ WARN ] ) ("skipThisSymbol = %s \n",skipThisSymbol ? "true" : "false");
#endif

#if 0
            // DQ (6/1/2019): Better to seatch if the scopes are the same then is the previous variable declaration was marked as extern.
                  {
                    if (variableDeclaration != NULL && variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() && variableDeclaration->get_linkage().empty() == true)
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                         mfprintf(mlog [ WARN ] ) ("FOUND extern modifier \n");
#endif
                         if ( (variableDeclaration->get_declarationModifier().isFriend() == false) && (isSgTemplateVariableDeclaration(variableDeclaration) == NULL) )
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                              mfprintf(mlog [ WARN ] ) ("SETTING skipThisSymbol = true \n");
#endif
                              skipThisSymbol = true;
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                              mfprintf(mlog [ WARN ] ) ("NOT setting skipThisSymbol = true \n");
#endif
                            }
                       }
                      else
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                         mfprintf(mlog [ WARN ] ) ("NOT found extern modifier \n");
#endif
                       }
                  }
#endif

            // DQ (6/4/2011): Get the associated symbol so that we can avoid matching on name only; and not the actual SgVariableSymbol symbols.
               SgVariableSymbol* targetInitializedNameSymbol = isSgVariableSymbol(initializedName->search_for_symbol_from_symbol_table());
               ASSERT_not_null(targetInitializedNameSymbol);

            // DQ (6/1/2019): If this is associated with an extern declaration then don't use this symbol (see test2019_470.C).
            // DQ (6/4/2011): Make sure we have the correct symbol, else we have detected a collision which will require name qualification to resolve.
            // if (variableSymbol == targetInitializedNameSymbol)
            // if (variableSymbol == targetInitializedNameSymbol && skipThisSymbol == false)
               if (variableSymbol == targetInitializedNameSymbol && skipThisSymbol == false)
                  {
                 // Found the correct symbol.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Found the correct SgVariableSymbol \n");
#endif
                  }
                 else
                  {
                 // This is not the correct symbol, even though the unqualified names match.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("These symbols only match based on name and is not the targetInitializedNameSymbol. \n");
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                 // DQ (12/15/2014): Liao's move tool will cause this message to be output, but it is not a problem (I think).
                    if (declaration == NULL)
                       {
                         mfprintf(mlog [ WARN ] ) ("variableSymbol = %p \n",variableSymbol);
                         mfprintf(mlog [ WARN ] ) ("targetInitializedNameSymbol = %p = %s \n",targetInitializedNameSymbol,targetInitializedNameSymbol->get_name().str());
                         mfprintf(mlog [ WARN ] ) ("initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
                         initializedName->get_file_info()->display("NameQualificationTraversal::nameQualificationDepth(): initializedName");
                       }
#endif

                 // DQ (12/28/2011): I think it may be OK to have this be NULL, in which case there is not name qualification (scope has no associated declaration, so it is NULL as is should be).
                 // ASSERT_not_null(declaration);
                    if (declaration != NULL)
                       {
                         amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
                       }
                      else
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                      // DQ (12/15/2014): Liao's move tool will cause this message to be output, but it is not a problem (I think).
                         mfprintf(mlog [ WARN ] ) ("Warning: In NameQualificationTraversal::nameQualificationDepth() --- It might be that this is an incorrect fix for where declaration == NULL in test2004_97.C \n");
#endif
                       }
                  }
             }
        }
       else
        {
       // Symbol for the SgInitializedName is not in the current scope or those of parent scopes.  So some name qualification is required.
       // amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration(initializedName->get_scope()),currentScope,positionStatement) + 1;
       // SgDeclarationStatement* declaration = associatedDeclaration(initializedName->get_scope());
       // amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
       // ASSERT_not_null(declaration);

       // See test2004_34.C for an example of where declaration == NULL
          if (declaration != NULL)
             {
               amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
             }
            else
             {
            // This can be the case of ??? "catch (Overflow)" (see test2004_43.C) instead of "catch (Overflow xxx)" (see test2011_71.C).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::nameQualificationDepth(SgInitializedName*): declaration == NULL, why is this? initializedName->get_scope() = %p = %s \n",initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());
#endif
            // ROSE_ASSERT(false);
             }
        }

  // amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

     return amountOfNameQualificationRequired;
   }


// DQ (3/14/2019): Adding debugging support to output the map of names.
// void NameQualificationTraversal::outputNameQualificationMap( const std::map<SgNode*,std::string> & qualifiedNameMap )
void
NameQualificationTraversal::outputNameQualificationMap( const NameQualificationMapType & qualifiedNameMap )
   {
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::outputNameQualificationMap(): qualifiedNameMap.size() = %zu \n",qualifiedNameMap.size());

     int counter = 0;
  // std::map<SgNode*,std::string>::const_iterator i = qualifiedNameMap.begin();
     NameQualificationMapType::const_iterator i = qualifiedNameMap.begin();
     while (i != qualifiedNameMap.end())
       {
         ASSERT_not_null(i->first);

         mfprintf(mlog [ WARN ] ) (" --- counter = %d *i = i->first = %p = %s i->second = %s \n",counter,i->first,i->first->class_name().c_str(),i->second.c_str());

         counter++;
         i++;
       }
   }



void
NameQualificationTraversal::addToNameMap ( SgNode* nodeReference, string typeNameString )
   {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In addToNameMap(): nodeReference = %p = %s typeNameString = %s \n",nodeReference,nodeReference->class_name().c_str(),typeNameString.c_str());
#endif

  // DQ (6/21/2011): This is refactored code used in traverseType() and traverseTemplatedFunction().
  // bool isTemplateName = (typeNameString.find('<') != string::npos) && (typeNameString.find("::") != string::npos);
     bool isTemplateName = (typeNameString.find('<') != string::npos); // && (typeNameString.find("::") != string::npos);

     bool isPointerMemberType = (isSgPointerMemberType(nodeReference) != NULL);

  // DQ (4/21/2019): Unclear if we should store the intermediately generated strings for each part of a type.
  // I think that test2019_385.C makes it clear that we need to allow the SgPointerMemberType intermediate
  // strings to be reset.
  // isPointerMemberType = false;

     bool isInitializedName    = (isSgInitializedName(nodeReference) != NULL);

  // DQ (4/28/2019): Adding support for the base type if a SgTypedefDeclaration.
     bool isTypedefDeclaration = (isSgTypedefDeclaration(nodeReference) != NULL);

  // DQ (4/28/2019): We only want to save the type as a string if it has a SgPointerMemberType (or a template instantiation).
  // I think the same thing is also true for the type associated with a SgInitializedName as well.
     SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(nodeReference);
     if (typedefDeclaration != NULL)
        {
          SgType* baseType = typedefDeclaration->get_base_type();

       // We realy need to strip off any modifiers before we check if it is a SgPointerMembertype.
          unsigned char bit_array = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_RVALUE_REFERENCE_TYPE |
                                    SgType::STRIP_POINTER_TYPE  | SgType::STRIP_ARRAY_TYPE;
          baseType = baseType->stripType(bit_array);
          ASSERT_not_null(baseType);

       // Check if this is something that could be name qualified.
          SgPointerMemberType* pointerMemberType = isSgPointerMemberType(baseType);
          SgNamedType*         namedType         = isSgNamedType(baseType);
       // if (pointerMemberType == NULL)
          if (pointerMemberType == NULL && namedType == NULL)
             {
               isTypedefDeclaration = false;
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
       // Find out what type this is, we need to detect template instantiations as well, so that they CAN be used to generate strings.
          mfprintf(mlog [ WARN ] ) ("In addToNameMap(): case SgTypedefDeclaration: baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
        }

  // DQ (4/28/2019): Handle the SgInitialzedName the same as the typedef.
     SgInitializedName* initializedName = isSgInitializedName(nodeReference);
     if (initializedName != NULL)
        {
          SgType* type = initializedName->get_type();

       // We realy need to strip off any modifiers before we check if it is a SgPointerMembertype.
          unsigned char bit_array = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_RVALUE_REFERENCE_TYPE |
                                    SgType::STRIP_POINTER_TYPE  | SgType::STRIP_ARRAY_TYPE;
          type = type->stripType(bit_array);
          ASSERT_not_null(type);

       // Check if this is something that could be name qualified.
          SgPointerMemberType* pointerMemberType = isSgPointerMemberType(type);
          SgNamedType*         namedType         = isSgNamedType(type);
       // if (pointerMemberType == NULL)
          if (pointerMemberType == NULL && namedType == NULL)
             {
               isInitializedName = false;
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
       // Find out what type this is, we need to detect template instantiations as well, so that they CAN be used to generate strings.
          mfprintf(mlog [ WARN ] ) ("In addToNameMap(): case SgInitializedName: type = %p = %s \n",type,type->class_name().c_str());
          mfprintf(mlog [ WARN ] ) ("In addToNameMap(): case SgInitializedName: isInitializedName = %s \n",isInitializedName ? "true" : "false");
#endif
        }

  // DQ (4/21/2019): The list of nodes which we will have to allow to store types as generated strings will
  // have to include expressions (SgNewExp, SgSizeOf, SgCastExp, SgTypeIdOp)
  // and functions (SgFunctionDeclaration)
  // and maybe that is about it.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In addToNameMap(): isTemplateName = %s isPointerMemberType = %s nodeReference = %p = %s typeNameString = %s \n",
          isTemplateName ? "true" : "false",isPointerMemberType ? "true" : "false",nodeReference,nodeReference->class_name().c_str(),typeNameString.c_str());
#endif

  // DQ (4/21/2019): Adding case to support SgInitializedName.
  // DQ (4/18/2019): We need to support SgPointerMemberType as well.
     if (isTemplateName == true || isPointerMemberType == true || isInitializedName == true || isTypedefDeclaration == true)
        {
          if (typeNameMap.find(nodeReference) == typeNameMap.end())
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("============== Inserting qualifier for name = %s into typeNameMap list at IR node = %p = %s \n",
                    typeNameString.c_str(),nodeReference,nodeReference->class_name().c_str());
#endif
               typeNameMap.insert(std::pair<SgNode*,std::string>(nodeReference,typeNameString));
             }
            else
             {
            // If it already exists, then overwrite the existing information.
            // std::map<SgNode*,std::string>::iterator i = typeNameMap.find(nodeReference);
               NameQualificationMapType::iterator i = typeNameMap.find(nodeReference);
               ROSE_ASSERT (i != typeNameMap.end());

               string previousTypeName = i->second.c_str();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("WARNING: replacing previousTypeName = %s with new typeNameString = %s for nodeReference = %p = %s \n",
                    previousTypeName.c_str(),typeNameString.c_str(),nodeReference,nodeReference->class_name().c_str());
#endif
               if (i->second != typeNameString)
                  {
                    i->second = typeNameString;
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                    mfprintf(mlog [ WARN ] ) ("In addToNameMap(): Allow this for replacement now! \n");
#endif
                 }
             }
        }
       else
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
       // DQ (8/19/2013): comment added for debugging.
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::addToNameMap(): isTemplateName == false, typeNameString = %s NOT added to typeNameMap for key = %p = %s \n",typeNameString.c_str(),nodeReference,nodeReference->class_name().c_str());
#endif
        }

   }


void
NameQualificationTraversal::traverseType ( SgType* type, SgNode* nodeReferenceToType, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // The type can contain subtypes (e.g. template arguments) and when the subtypes need to be name qualified the name of the encompassing type
  // has a name that depends upon its location in the source code (and could vary depending on the positon in a single basic block, I think).

#define DEBUG_TRAVERSE_TYPE 0

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_TYPE
     mfprintf(mlog [ WARN ] ) ("<<<<< Starting traversal of type: type = %p = %s \n",type,type->class_name().c_str());
     mfprintf(mlog [ WARN ] ) ("In traverseType(): nodeReferenceToType = %p = %s \n",nodeReferenceToType,nodeReferenceToType->class_name().c_str());
#endif

  // DQ (4/27/2019): If this is an un-named type then it can't be name qualified, so we can exit directly.
  // We don't want to generate strings to represent the types from unNamed declarations that could not be name qualified anyway.
     bool isUnNamed = false;
  // SgNamedType* namedType = isSgNamedType(type);
     unsigned char bit_array = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_RVALUE_REFERENCE_TYPE |
                               SgType::STRIP_POINTER_TYPE  | SgType::STRIP_ARRAY_TYPE;
     SgType* strippedType = type->stripType(bit_array);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_TYPE
     mfprintf(mlog [ WARN ] ) ("In traverseType(): strippedType = %p = %s \n",strippedType,strippedType->class_name().c_str());
#endif

     SgNamedType* namedType = isSgNamedType(strippedType);
     if (namedType != NULL)
        {
          SgEnumType*  enumType  = isSgEnumType(namedType);
          SgClassType* classType = isSgClassType(namedType);

          if (enumType != NULL)
             {
               SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(enumType->get_declaration());
               ASSERT_not_null(enumDeclaration);
#if DEBUG_TRAVERSE_TYPE
               mfprintf(mlog [ WARN ] ) ("enumDeclaration->get_isUnNamed()        = %s \n",enumDeclaration->get_isUnNamed() ? "true" : "false");
               mfprintf(mlog [ WARN ] ) ("namedType->get_autonomous_declaration() = %s \n",namedType->get_autonomous_declaration() ? "true" : "false");
#endif
               isUnNamed = enumDeclaration->get_isUnNamed();
             }
            else
             {
               if (classType != NULL)
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                    ASSERT_not_null(classDeclaration);
#if DEBUG_TRAVERSE_TYPE
                    mfprintf(mlog [ WARN ] ) ("classDeclaration->get_isUnNamed()       = %s \n",classDeclaration->get_isUnNamed() ? "true" : "false");
                    mfprintf(mlog [ WARN ] ) ("namedType->get_autonomous_declaration() = %s \n",namedType->get_autonomous_declaration() ? "true" : "false");
#endif
                    isUnNamed = classDeclaration->get_isUnNamed();
                  }
                 else
                  {
                 // This is most commonly a SgTypedefType, but they can't be un-named.
#if DEBUG_TRAVERSE_TYPE
                    mfprintf(mlog [ WARN ] ) ("In traverseType(): Alternative SgNamedType: namedType = %p = %s \n",namedType,namedType->class_name().c_str());
                    mfprintf(mlog [ WARN ] ) ("namedType->get_autonomous_declaration()               = %s \n",namedType->get_autonomous_declaration() ? "true" : "false");
#endif
                  }
             }
       // Return from function if this is associated wqith an unNamed declaration (since it could not be name qualified).
          if (isUnNamed == true)
             {
#if DEBUG_TRAVERSE_TYPE
               mfprintf(mlog [ WARN ] ) ("In traverseType(): isUnNamed == true: returning \n");
#endif
               return;
             }
        }

  // DQ (4/22/2019): Need to detect when this is part of a type from a paremter in a function parameter list.
     SgInitializedName* initializedName = isSgInitializedName(nodeReferenceToType);
     bool inArgList = false;
     if (initializedName != NULL)
        {
#if DEBUG_TRAVERSE_TYPE
          mfprintf(mlog [ WARN ] ) ("Found a SgInitializedName: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
       // If this is a part of a function parameter list, then we need to set info.inArgList() in the Unparse_Info object that we pass to the unparse the type.
          SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(initializedName->get_parent());
          inArgList = (functionParameterList != NULL);
        }

  // DQ (4/28/2019): Need to detect when this is part of a base type from a typedef declaration.
     SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(nodeReferenceToType);
     bool inTypedefDecl = false;
     if (typedefDeclaration != NULL)
        {
#if DEBUG_TRAVERSE_TYPE
          mfprintf(mlog [ WARN ] ) ("Found a SgTypedefDeclaration: typedefDeclaration = %p name = %s \n",typedefDeclaration,typedefDeclaration->get_name().str());
#endif
          inTypedefDecl = true;
        }


  // DQ (4/27/2019): Could this be how we trigger name qualification for constructor preinitialization lists?
     SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(nodeReferenceToType);
     if (constructorInitializer != NULL)
        {
#if DEBUG_TRAVERSE_TYPE
          mfprintf(mlog [ WARN ] ) ("Found a SgConstructorInitializer: %p \n",constructorInitializer);
#endif
        }

     ASSERT_not_null(nodeReferenceToType);

  // DQ (3/29/2019): I think we are ready to address this now.
  // Some type IR nodes are difficult to save as a string and reuse. So for now we will skip supporting
  // some type IR nodes with generated name qualification specific to where they are used.
     bool skipThisType = false;
     if (isSgPointerMemberType(type) != NULL)
        {
       // DQ (3/29/2019): We would like to no longer skip this type.
       // skipThisType = true;
        }

  // DQ (4/20/2019): We should have a SgType member function for this (which would be more complete).
  // bool isPrimativeType = (isSgTypeInt(type) != NULL) || (isSgTypeLong(type) != NULL) || (isSgTypeVoid(type) != NULL);
     bool isPrimativeType = type->isPrimativeType();
     bool isFunctionType = (isSgFunctionType(type) != NULL);

     if (isPrimativeType == true || isFunctionType == true)
        {
       // DQ (3/29/2019): We should skip this type because they are never name qualified.
          skipThisType = true;
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] )("In NameQualificationTraversal::traverseType:\n");
     mfprintf(mlog [ WARN ] )(" -- type = %p (%s) : %s\n", type, type->class_name().c_str(), type->unparseToString().c_str());
#endif

  // DQ (5/17/2019): Adding support to only handle class types that are from template instantiations (since they can contains types that require name qualificaiton).
  // bool isTemplateInstantiationType = false;
     SgClassType* classType = isSgClassType(type);
     if (classType != NULL)
        {
          SgDeclarationStatement* classDeclaration = classType->get_declaration();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::traverseType: classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
#endif
          SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(classDeclaration);
          if (templateInstantiationDecl == NULL)
             {
            // isTemplateInstantiationType = true;
               skipThisType = true;
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::traverseType: skipThisType = %s \n",skipThisType ? "true" : "false");
#endif

     if (skipThisType == false)
        {
          SgDeclarationStatement* declaration = associatedDeclaration(type);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] )(" -- declaration = %p (%s)\n", declaration, declaration ? declaration->class_name().c_str() : "");
#endif
          if (declaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::traverseType(): Calling evaluateTemplateInstantiationDeclaration(): declaration = %p = %s currentScope = %p = %s positionStatement = %p = %s \n",
                    declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());
#endif
               evaluateTemplateInstantiationDeclaration(declaration,currentScope,positionStatement);
             }

          SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
          ASSERT_not_null(unparseInfoPointer);
          unparseInfoPointer->set_outputCompilerGeneratedStatements();

       // Avoid unpasing the class definition when unparseing the type.
          unparseInfoPointer->set_SkipClassDefinition();

       // DQ (5/8/2013): Added specification to skip enum definitions also (see test2012_202.C).
          unparseInfoPointer->set_SkipEnumDefinition();

       // Associate the unparsing of this type with the statement or scope where it occures.
       // This is the key to use in the lookup of the qualified name. But this is the correct key....
       // unparseInfoPointer->set_reference_node_for_qualification(positionStatement);
       // unparseInfoPointer->set_reference_node_for_qualification(currentScope);
          unparseInfoPointer->set_reference_node_for_qualification(nodeReferenceToType);

       // DQ (5/7/2013): A problem with this is that it combines the first and second parts of the
       // type into a single string (e.g. the array type will include two parts "base_type" <array name> "[index]".
       // When this is combined for types that have two parts (most types don't) the result is an error
       // when the type is unparsed.  It is not clear, but a solution might be for this to be built here
       // as just the 1st part, and let the second part be generated when the array type is unparsed.
       // BTW, the reason why it is computed here is that there may be many nested types that require
       // name qualifications and so it is required that we save the whole string.  However, name
       // qualification might only apply to the first part of types.  So we need to investigate this.
       // This is a problem demonstrated in test2013_156.C and test2013_158.C.

       // DQ (5/8/2013): Set the SgUnparse_Info so that only the first part will be unparsed.
          unparseInfoPointer->set_isTypeFirstPart();
#if 0
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::traverseType(): Comment out out the setting of SkipClassSpecifier \n");
#endif
       // DQ (4/27/2019): Comment out out the setting of SkipClassSpecifier.  If this is commented out then
       // we pass a lot more (about 19 more) of the test codes in the Cxx_test directory, else we only fail
       // 7 test codes.  However, we fail a number of the tests in the Cxx11_tests directory.
       // Not clear how to proceed here.

       // DQ (5/14/2019): We can sometimes need this class specifier to be output (e.g. test2019_430.C) (testing).
       // DQ (8/19/2013): Added specification to skip class specifier (fixes problem with test2013_306.C).
       // unparseInfoPointer->set_SkipClassSpecifier();
#if 0
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::traverseType(): Comment out the setting of SkipClassSpecifier() \n");
#endif
       // DQ (4/22/2019): Make this as being from a parameter list (types are unparse with extra parenthesis).
          if (inArgList == true)
             {
               unparseInfoPointer->set_inArgList();
             }

       // DQ (4/28/2019): Need to detect when this is part of a base type from a typedef declaration.
          if (inTypedefDecl == true)
             {
               unparseInfoPointer->set_inTypedefDecl();
             }

       // DQ (5/18/2019): Makr this as being in a SgAggregateInitializer.
          bool exit_in_processing_aggregateInitializer = false;
          SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(nodeReferenceToType);
          if (aggregateInitializer != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_TYPE
               mfprintf(mlog [ WARN ] ) ("++++++++++++++++ explicitly setting unparseInfoPointer->set_inAggregateInitializer() \n");
#endif
               exit_in_processing_aggregateInitializer = true;
               unparseInfoPointer->set_inAggregateInitializer();

            // DQ (5/18/2019): when in the SgAggregateInitializer, don't output the class specifier.
               unparseInfoPointer->set_SkipClassSpecifier();
             }

          SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(nodeReferenceToType);
          if (constructorInitializer != NULL)
             {
            // DQ (5/19/2019): when in the function argument list, don't output the class specifier.
               unparseInfoPointer->set_SkipClassSpecifier();
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_TYPE
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ Calling globalUnparseToString(): type = %p = %s \n",type,type->class_name().c_str());
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ unparseInfoPointer->inAggregateInitializer() = %s \n",unparseInfoPointer->inAggregateInitializer() ? "true" : "false");
#endif

          bool isContainedInTemplateInstantiationDefn = false;
          SgTemplateInstantiationDefn* templateInstantiationDefn = NULL;
          SgScopeStatement* parentScope = NULL;
          SgStatement*       statement       = isSgStatement(nodeReferenceToType);
          SgInitializedName* initializedName = isSgInitializedName(nodeReferenceToType);
          if (statement != NULL)
             {
               parentScope = statement->get_scope();
             }
            else
             {
               if (initializedName != NULL)
                  {
                    parentScope = initializedName->get_scope();
                  }
                 else
                  {
                    parentScope = NULL;
                  }
             }

       // SgGlobal* globalScope = isSgGlobal(parentScope);
          while (isSgGlobal(parentScope) == NULL && parentScope != NULL)
             {
#if 0
               printf ("parentScope = %p = %s \n",parentScope,parentScope->class_name().c_str());
#endif
               templateInstantiationDefn = isSgTemplateInstantiationDefn(parentScope);
               if (templateInstantiationDefn != NULL)
                  {
                    isContainedInTemplateInstantiationDefn = true;
                 }

              parentScope = parentScope->get_scope();
            }

       // DQ (7/12/2022): If this is inside of a SgTemplateInstantiationDefn then see if we can
       // suppress the generation since this is where the type names that are too long come from.
       // string typeNameString = globalUnparseToString(type,unparseInfoPointer);
          string typeNameString;
       // DQ (7/13/2022): Modified code to avoid name qualification in template class instantiations.
          if (isContainedInTemplateInstantiationDefn == false)
             {
               typeNameString = globalUnparseToString(type,unparseInfoPointer);
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_TYPE || 0
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ typeNameString (globalUnparseToString()) = %s \n",typeNameString.c_str());
#endif

       // DQ (5/18/2019): Testing...
       // ROSE_ASSERT(exit_in_processing_aggregateInitializer == false);
          if (exit_in_processing_aggregateInitializer != false)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_TYPE
            // DQ (5/18/2019): Adding debugging info.
               unparseInfoPointer->display("In NameQualificationTraversal::traverseType(): exit_in_processing_aggregateInitializer == true: unparseInfoPointer \n");
#endif
             }

       // DQ (7/13/2011): OSX can have types that are about 2487 characters long (see test2004_35.C).
       // This is symptematic of an error which causes the whole class to be included with the class
       // definition.  This was fixed by calling unparseInfoPointer->set_SkipClassDefinition() above.
          if (typeNameString.length() > 6000)
             {
               if (SgProject::get_verbose() > 0)
                  {
                    mfprintf(mlog [ WARN ] ) ("Warning: type names should not be this long...(unless this is boost) typeNameString.length() = %" PRIuPTR " \n",typeNameString.length());
                  }

            // DQ (10/11/2015): Increased max size of typename handled in ROSE (a 42K charater long
            // typename was generated by test2015_87.C), so we will allow this.
            // DQ (1/30/2013): Increased already too long limit for string lengths for typenames.  This
            // test fails for ROSE compiling ROSE with a type name that is 17807 characters long.  I have
            // increased the max allowable typename lengtt to over twice that for good measure.
            // DQ (7/22/2011): The a992-thrifty-mips-compiler Hudson test fails because it generates a
            // typename that is even longer 5149, so we need an even larger upper bound.  This should be
            // looked into later to see why some of these different platforms are generating such large
            // typenames. See testcode: tests/nonsmoke/functional/CompileTests/PythonExample_tests/test2004_92.C (on thrifty).
               if (typeNameString.length() > 400000)
                  {
                 // DQ (7/11/2022): Output the type info:
                    mfprintf(mlog [ WARN ] ) ("Type name from unparseToString is too long: type = %p = %s \n",type,type->class_name().c_str());

                    printf ("Output debugging info: calling recursivePrintCurrentAndParent() \n");
                    SageInterface::recursivePrintCurrentAndParent(nodeReferenceToType);

                 // DQ (2/7/2017): Output offending type name string to a file for inspection.
                    ASSERT_not_null(positionStatement);
                    positionStatement->get_file_info()->display("Output offending type name string to a file for inspection: debug");

                    SgFile* problemFile = TransformationSupport::getFile(positionStatement);
                    string filename = problemFile->getFileName();
                    filename += ".typename";

                    mfprintf(mlog [ WARN ] ) ("Generating a file (%s) to hold the typename \n",filename.c_str());
                    mfprintf(mlog [ WARN ] ) ("Error: type names should not be this long... (even in boost, I think) typeNameString.length() = %" PRIuPTR " \n",typeNameString.length());
                    mfprintf(mlog [ WARN ] ) ("nodeReferenceToType = %p = %s \n",nodeReferenceToType,nodeReferenceToType->class_name().c_str());
                    if (nodeReferenceToType->get_file_info())
                       {
                         nodeReferenceToType->get_file_info()->display("Error: type names should not be this long...: debug");
                       }
                    ROSE_ABORT();
                  }
             }

#if 0
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::traverseType(): addToNameMap(): nodeReferenceToType = %p = %s typeNameString = %s \n",
               nodeReferenceToType,nodeReferenceToType->class_name().c_str(),typeNameString.c_str());
#endif

       // DQ (6/21/2011): Refactored this code for use in traverseTemplatedFunction()
          addToNameMap(nodeReferenceToType,typeNameString);

       // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
          delete unparseInfoPointer;
        }
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("<<<<< Ending traversal of type: type = %p = %s \n",type,type->class_name().c_str());
#endif
   }


void
NameQualificationTraversal::traverseTemplatedFunction(SgFunctionRefExp* functionRefExp, SgNode* nodeReference, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // Called using traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement)

     ASSERT_not_null(functionRefExp);
     ASSERT_not_null(nodeReference);
     ASSERT_not_null(currentScope);
     ASSERT_not_null(positionStatement);

  // mfprintf(mlog [ WARN ] ) ("Inside of traverseTemplatedFunction functionRefExp = %p currentScope = %p = %s \n",functionRefExp,currentScope,currentScope->class_name().c_str());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("<<<<< Starting traversal of traverseTemplatedFunction functionRefExp = %p currentScope = %p = %s \n",functionRefExp,currentScope,currentScope->class_name().c_str());
#endif

     bool skipThisFunction = false;
     if (skipThisFunction == false)
        {
          SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(functionRefExp->getAssociatedFunctionDeclaration());
          if (templateInstantiationFunctionDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Found a SgTemplateInstantiationFunctionDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationFunctionDeclaration->get_name().str());
#endif
               evaluateTemplateInstantiationDeclaration(templateInstantiationFunctionDeclaration,currentScope,positionStatement);
             }

          SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
          ASSERT_not_null(unparseInfoPointer);
          unparseInfoPointer->set_outputCompilerGeneratedStatements();

       // Avoid unpasing the class definition when unparseing the type.
          unparseInfoPointer->set_SkipClassDefinition();

       // DQ (1/13/2014): Set the output of the enum defintion to match that of the class definition (consistancy is now inforced).
          unparseInfoPointer->set_SkipEnumDefinition();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("nodeReference = %p = %s \n",nodeReference,nodeReference->class_name().c_str());
#endif
       // Associate the unparsing of this type with the statement or scope where it occures.
       // This is the key to use in the lookup of the qualified name. But this is the correct key....
          unparseInfoPointer->set_reference_node_for_qualification(nodeReference);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Calling globalUnparseToString() \n");
#endif
          string functionNameString = globalUnparseToString(functionRefExp,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ functionNameString (globalUnparseToString()) = %s \n",functionNameString.c_str());
#endif
#if 0
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ functionNameString (globalUnparseToString()) = %s \n",functionNameString.c_str());
#endif

       // DQ (10/31/2015): Increased the maximum allowable size of function names (because test2015_98.C
       // demonstrates a longer name (length == 5062)).
       // DQ (6/24/2013): Increased upper bound to support ROSE compiling ROSE.
       // This is symptematic of an error which causes the whole class to be included with the class
       // definition.  This was fixed by calling unparseInfoPointer->set_SkipClassDefinition() above.
          if (functionNameString.length() > 10000)
             {
               mfprintf(mlog [ WARN ] ) ("Error: function names should not be this long... functionNameString.length() = %" PRIuPTR " \n",functionNameString.length());
#if 1
               mfprintf(mlog [ WARN ] ) ("Error: function names should not be this long... functionNameString          = \n%s \n",functionNameString.c_str());
#endif
             }

       // DQ (6/21/2011): Refactored this code for use in traverseTemplatedFunction()
          addToNameMap(nodeReference,functionNameString);

       // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
          delete unparseInfoPointer;
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("<<<<< Ending traversal of traverseTemplatedFunction functionRefExp = %p currentScope = %p = %s \n",functionRefExp,currentScope,currentScope->class_name().c_str());
#endif
   }


void
NameQualificationTraversal::traverseTemplatedMemberFunction(SgMemberFunctionRefExp* memberFunctionRefExp, SgNode* nodeReference, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // Called using traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement)

     ASSERT_not_null(memberFunctionRefExp);
     ASSERT_not_null(nodeReference);
     ASSERT_not_null(currentScope);
     ASSERT_not_null(positionStatement);

  // mfprintf(mlog [ WARN ] ) ("Inside of traverseTemplatedFunction functionRefExp = %p currentScope = %p = %s \n",functionRefExp,currentScope,currentScope->class_name().c_str());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("<<<<< Starting traversal of traverseTemplatedFunction memberFunctionRefExp = %p currentScope = %p = %s \n",memberFunctionRefExp,currentScope,currentScope->class_name().c_str());
#endif

     bool skipThisFunction = false;
     if (skipThisFunction == false)
        {
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionRefExp->getAssociatedMemberFunctionDeclaration());
          if (templateInstantiationMemberFunctionDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Found a SgTemplateInstantiationMemberFunctionDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationMemberFunctionDeclaration->get_name().str());
#endif
               evaluateTemplateInstantiationDeclaration(templateInstantiationMemberFunctionDeclaration,currentScope,positionStatement);
             }

          SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
          ASSERT_not_null(unparseInfoPointer);
          unparseInfoPointer->set_outputCompilerGeneratedStatements();

       // Avoid unpasing the class definition when unparsing the type.
          unparseInfoPointer->set_SkipClassDefinition();

       // DQ (1/13/2014): Set the output of the enum defintion to match that of the class definition (consistancy is now inforced).
          unparseInfoPointer->set_SkipEnumDefinition();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("nodeReference = %p = %s \n",nodeReference,nodeReference->class_name().c_str());
#endif
       // Associate the unparsing of this type with the statement or scope where it occures.
       // This is the key to use in the lookup of the qualified name. But this is the correct key....
       // unparseInfoPointer->set_reference_node_for_qualification(positionStatement);
       // unparseInfoPointer->set_reference_node_for_qualification(currentScope);
          unparseInfoPointer->set_reference_node_for_qualification(nodeReference);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Calling globalUnparseToString() \n");
#endif
          string memberFunctionNameString = globalUnparseToString(memberFunctionRefExp,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ memberFunctionNameString (globalUnparseToString()) = %s \n",memberFunctionNameString.c_str());
#endif
#if 0
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ memberFunctionNameString (globalUnparseToString()) = %s \n",memberFunctionNameString.c_str());
#endif
       // DQ (3/30/2018): Incremented this for ROSE compiling ROSE using Boost (after bugfix for private types to be replaced with non-private unparsable types).
       // DQ (12/3/2014): Incremented this for ARES application files.
       // DQ (6/9/2013): I have incremented this value to support mangled names in the protobuf-2.5.0 application.
       // This is symptematic of an error which causes the whole class to be included with the class
       // definition.  This was fixed by calling unparseInfoPointer->set_SkipClassDefinition() above.
       // [Robb Matzke 2018-06-19]: Incremented from 8000 to 9000 because <rose.h> has a name that's 8960 characters, namely "__gnu_cxx::new_allocator< _Rb_tree_node< map< ... >
       //    ::value_type > > ::deallocate".
          if (memberFunctionNameString.length() > 9000)
             {
               mfprintf(mlog [ WARN ] ) ("Error: function names should not be this long... memberFunctionNameString.length() = %" PRIuPTR " \n",memberFunctionNameString.length());
               ROSE_ABORT();
             }

       // DQ (6/21/2011): Refactored this code for use in traverseTemplatedFunction()
          addToNameMap(nodeReference,memberFunctionNameString);

       // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
          delete unparseInfoPointer;
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("<<<<< Ending traversal of traverseTemplatedMemberFunction memberFunctionRefExp = %p currentScope = %p = %s \n",memberFunctionRefExp,currentScope,currentScope->class_name().c_str());
#endif
   }


void
NameQualificationTraversal::traverseTemplatedClass(SgBaseClass* baseClass, SgNode* nodeReference, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // DQ (4/12/2019): Notes on Cxx11_tests/test342.C:
  // It may be that we need a function like "traverseTemplatedFunction" for classes to support the reference to a template class instantiation in a SgBaseClass.
  // The issue is that the template arguments can have arbitrary complexity of name qualification requirements such that we need to save the associtate string
  // generated once after all of the required name qualification (of the template arguments) has been figured out.

  // The point is to save the generated name of the class declaration, correctly name qualified (with all template parameters name qualified) so that it can
  // be used directly (as a string) in the unparsing. The call to "addToNameMap()" is the key part that saves the name of the template instantiation.

  // Currently the base class references the shared template class instantiation (so this might be the root of the problem as well).
  // If sharing is the issue, then generateding a strring to hold the name of the class with ccntext depenent template argument name
  // qualification would be the solution (just as it is for shared types).


  // Called similar to traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement)

     ASSERT_not_null(baseClass);
     ASSERT_not_null(nodeReference);
     ASSERT_not_null(currentScope);
     ASSERT_not_null(positionStatement);

#if 0
     mfprintf(mlog [ WARN ] ) ("Inside of traverseTemplatedClass baseClass = %p currentScope = %p = %s \n",baseClass,currentScope,currentScope->class_name().c_str());
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("<<<<< Starting traversal of traverseTemplatedFunction baseClass = %p currentScope = %p = %s \n",baseClass,currentScope,currentScope->class_name().c_str());
#endif

     bool skipThisClass = false;
     if (skipThisClass == false)
        {
          SgTemplateInstantiationDecl* templateInstantiationClassDeclaration = isSgTemplateInstantiationDecl(baseClass->get_base_class());
          if (templateInstantiationClassDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("Found a SgTemplateInstantiationDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationClassDeclaration->get_name().str());
#endif
               evaluateTemplateInstantiationDeclaration(templateInstantiationClassDeclaration,currentScope,positionStatement);
             }

          SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
          ASSERT_not_null(unparseInfoPointer);
          unparseInfoPointer->set_outputCompilerGeneratedStatements();

       // Avoid unpasing the class definition when unparseing the type.
          unparseInfoPointer->set_SkipClassDefinition();

       // DQ (1/13/2014): Set the output of the enum defintion to match that of the class definition (consistancy is now inforced).
          unparseInfoPointer->set_SkipEnumDefinition();

       // DQ (4/12/2019): This is how we skip the generation of the name with the "template<> struct" specifiers and trailing ";".
          unparseInfoPointer->set_inEmbeddedDecl();
          unparseInfoPointer->set_SkipSemiColon();
          unparseInfoPointer->set_SkipClassSpecifier();

#if 0
       // DQ (4/12/2019): Test2019_342.C and test2019_350.C demonstrate that this needs to be more carefully supported.
          mfprintf(mlog [ WARN ] ) ("############################################################################################################################# \n");
          mfprintf(mlog [ WARN ] ) ("In traverseTemplatedClass(): Need to supress name qualification in the generated string to avoid redundant name qualification \n");
          mfprintf(mlog [ WARN ] ) ("############################################################################################################################# \n");
#endif
          unparseInfoPointer->set_SkipNameQualification();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("In traverseTemplatedClass(): nodeReference = %p = %s \n",nodeReference,nodeReference->class_name().c_str());
#endif

       // DQ (3/13/2019): Not setting the reference_node_for_qualification, may be the best way to make sure
       // we don't get a name qualification prefix.
       // Associate the unparsing of this type with the statement or scope where it occures.
       // This is the key to use in the lookup of the qualified name. But this is the correct key....
          unparseInfoPointer->set_reference_node_for_qualification(nodeReference);

       // DQ (3/16/2021): Need to set the assocated file, else it is caught in unparseStatement().
       // After review, I think we may not need this.
       // SgSourceFile* sourceFile = xxx;
       // unparseInfoPointer->set_current_source_file(xxx);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("Calling globalUnparseToString() \n");
#endif
          string classNameString = globalUnparseToString(templateInstantiationClassDeclaration,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ classNameString (globalUnparseToString()) = %s \n",classNameString.c_str());
#endif
#if 0
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ classNameString (globalUnparseToString()) = %s \n",classNameString.c_str());
#endif

       // DQ (10/31/2015): Increased the maximum allowable size of function names (because test2015_98.C
       // demonstrates a longer name (length == 5062)).
       // DQ (6/24/2013): Increased upper bound to support ROSE compiling ROSE.
       // This is symptematic of an error which causes the whole class to be included with the class
       // definition.  This was fixed by calling unparseInfoPointer->set_SkipClassDefinition() above.
          if (classNameString.length() > 10000)
             {
               mfprintf(mlog [ WARN ] ) ("Error: class names should not be this long... classNameString.length() = %" PRIuPTR " \n",classNameString.length());
             }

       // DQ (6/21/2011): Refactored this code for use in traverseTemplatedFunction()
          addToNameMap(nodeReference,classNameString);

       // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
          delete unparseInfoPointer;
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("<<<<< Ending traversal of traverseTemplatedClass baseClass = %p currentScope = %p = %s \n",baseClass,currentScope,currentScope->class_name().c_str());
#endif
   }






bool
NameQualificationTraversal::skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(SgDeclarationStatement* declaration)
   {
  // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in
  // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
  // definition could not live in the SgBasicBlock.

  // DQ (4/27/2019): Added assertion.
     ASSERT_not_null(declaration);

     bool skipNameQualification = false;
     SgDeclarationStatement* declarationToSearchForInReferencedNameSet = declaration->get_firstNondefiningDeclaration() != NULL ? declaration->get_firstNondefiningDeclaration() : declaration;
     ASSERT_not_null(declarationToSearchForInReferencedNameSet);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): declaration->get_firstNondefiningDeclaration() = %p \n",
          declaration->get_firstNondefiningDeclaration());
     mfprintf(mlog [ WARN ] ) ("   --- declarationToSearchForInReferencedNameSet->get_parent() = %p = %s \n",
          declarationToSearchForInReferencedNameSet->get_parent(),declarationToSearchForInReferencedNameSet->get_parent()->class_name().c_str());

     mfprintf(mlog [ WARN ] ) ("   --- declaration                                             = %p = %s \n",declaration,declaration->class_name().c_str());
     mfprintf(mlog [ WARN ] ) ("   --- declaration->get_parent()                               = %p = %s \n",declaration->get_parent(),declaration->get_parent()->class_name().c_str());
     if (declaration->get_firstNondefiningDeclaration() != NULL)
        {
          mfprintf(mlog [ WARN ] ) ("   --- declaration ->get_firstNondefiningDeclaration()              = %p = %s \n",
               declaration->get_firstNondefiningDeclaration(),declaration->get_firstNondefiningDeclaration()->class_name().c_str());
          mfprintf(mlog [ WARN ] ) ("   --- declaration->get_firstNondefiningDeclaration()->get_parent() = %p = %s \n",
               declaration->get_firstNondefiningDeclaration()->get_parent(),declaration->get_firstNondefiningDeclaration()->get_parent()->class_name().c_str());
        }
     if (declaration->get_definingDeclaration() != NULL)
        {
          mfprintf(mlog [ WARN ] ) ("   --- declaration ->get_definingDeclaration()                      = %p = %s \n",
               declaration->get_definingDeclaration(),declaration->get_definingDeclaration()->class_name().c_str());
       // DQ (5/12/2024): This fails for test_06.C used to debug infinite recursion in the Group G code.
       // mfprintf(mlog [ WARN ] ) ("   --- declaration->get_definingDeclaration()->get_parent()         = %p = %s \n",
       //      declaration->get_definingDeclaration()->get_parent(),declaration->get_definingDeclaration()->get_parent()->class_name().c_str());
        }
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 0
     SgFunctionDeclaration* inputFunctionDeclaration = isSgFunctionDeclaration(declaration);
     if (inputFunctionDeclaration != NULL)
        {
          printf ("inputFunctionDeclaration name = %s \n",inputFunctionDeclaration->get_name().str());
          SgScopeStatement* scope = inputFunctionDeclaration->get_scope();
          printf ("scope = %p = %s \n",scope,scope->class_name().c_str());

          SgName mangledName = inputFunctionDeclaration->get_mangled_name();
          printf ("mangledName = %s \n",mangledName.str());

          SgSourceFile* sourceFile = SageInterface::getEnclosingNode<SgSourceFile>(inputFunctionDeclaration);
          ROSE_ASSERT(sourceFile != NULL);
          printf ("sourceFile = %p name = %s \n",sourceFile,sourceFile->getFileName().c_str());
          inputFunctionDeclaration->get_file_info()->display("inputFunctionDeclaration: debug");
        }

     SgFunctionDeclaration* inputFunctionDeclarationToSearchForInReferencedNameSet = isSgFunctionDeclaration(declarationToSearchForInReferencedNameSet);
     if (inputFunctionDeclarationToSearchForInReferencedNameSet != NULL)
        {
          printf ("inputFunctionDeclarationToSearchForInReferencedNameSet name = %s \n",inputFunctionDeclarationToSearchForInReferencedNameSet->get_name().str());
          SgScopeStatement* scope = inputFunctionDeclarationToSearchForInReferencedNameSet->get_scope();
          printf ("scope = %p = %s \n",scope,scope->class_name().c_str());

          SgName mangledName = inputFunctionDeclarationToSearchForInReferencedNameSet->get_mangled_name();
          printf ("mangledName = %s \n",mangledName.str());

          SgSourceFile* sourceFile = SageInterface::getEnclosingNode<SgSourceFile>(inputFunctionDeclarationToSearchForInReferencedNameSet);
          ROSE_ASSERT(sourceFile != NULL);
          printf ("sourceFile = %p name = %s \n",sourceFile,sourceFile->getFileName().c_str());

          inputFunctionDeclarationToSearchForInReferencedNameSet->get_file_info()->display("inputFunctionDeclarationToSearchForInReferencedNameSet: debug");
        }

     printf ("Output referencedNameSet: \n");
  // for (std::set<SgNode*>::iterator i = referencedNameSet.begin(); i != referencedNameSet.end(); i++)
     for (NameQualificationSetType::iterator i = referencedNameSet.begin(); i != referencedNameSet.end(); i++)
        {
       // printf (" --- referencedNameSet: element: \n");

       // DQ (10/17/2020): There is a NULL entry in the referencedNameSet, this should not exist.
       // ROSE_ASSERT(*i != NULL);

          if (*i != NULL)
             {
               mfprintf(mlog [ WARN ] ) ("   --- *** referencedNameSet member *i = %p = %s \n",*i,(*i)->class_name().c_str());
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
               if (functionDeclaration != NULL)
                  {
                 // DQ (10/17/2020): This is a declaration from likely a different file, but with the same name as what we are searching for.
                 // So although I would not want to compare function names, that could be one solution.
                 // Either that or identify if these are from a different file.
                    printf ("   --- *** functionDeclaration name = %s \n",functionDeclaration->get_name().str());
                    SgScopeStatement* scope = functionDeclaration->get_scope();
                    printf ("   --- *** scope = %p = %s \n",scope,scope->class_name().c_str());

                    SgName mangledName = functionDeclaration->get_mangled_name();
                    printf ("   --- *** mangledName = %s \n",mangledName.str());

                    SgSourceFile* sourceFile = SageInterface::getEnclosingNode<SgSourceFile>(functionDeclaration);
                    ROSE_ASSERT(sourceFile != NULL);
                    printf ("   --- *** sourceFile = %p name = %s \n",sourceFile,sourceFile->getFileName().c_str());

                    functionDeclaration->get_file_info()->display("functionDeclaration: debug");
                  }
             }
            else
             {
            // printf (" --- element = %p \n",*i);
               mfprintf(mlog [ WARN ] ) ("   --- *** referencedNameSet member *i = %p \n",*i);
             }
        }
#endif

  // DQ (8/18/2012): If this is a template instantiation, then we need to look at where the template declaration is and if IT is defined.
  // See test2009_30.C for an example of this.
     SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(declaration);
     if (templateInstantiationFunctionDecl != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): templateInstantiationFunctionDecl->get_name() = %p = %s \n",
               templateInstantiationFunctionDecl,templateInstantiationFunctionDecl->get_name().str());
#endif

       // DQ (8/18/2012): Note that test2012_57.C and test2012_59.C have template specalizations that don't appear
       // to have there associated template declaration set properly, issue a warning for now.
       // declarationToSearchForInReferencedNameSet = templateInstantiationFunctionDecl->get_templateDeclaration();
          if (templateInstantiationFunctionDecl->get_templateDeclaration() == NULL)
             {
               mfprintf(mlog [ WARN ] ) ("WARNING: templateInstantiationFunctionDecl->get_templateDeclaration() == NULL for templateInstantiationFunctionDecl = %p = %s \n",
                    templateInstantiationFunctionDecl,templateInstantiationFunctionDecl->get_name().str());
             }
            else
             {
               declarationToSearchForInReferencedNameSet = templateInstantiationFunctionDecl->get_templateDeclaration();
             }
          ASSERT_not_null(declarationToSearchForInReferencedNameSet);
        }
       else
        {
       // Also test for member function.
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(declaration);
          if (templateInstantiationMemberFunctionDecl != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("In skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): templateInstantiationMemberFunctionDecl->get_name() = %p = %s \n",
                    templateInstantiationMemberFunctionDecl,templateInstantiationMemberFunctionDecl->get_name().str());
#endif
               declarationToSearchForInReferencedNameSet = templateInstantiationMemberFunctionDecl->get_templateDeclaration();
               ASSERT_not_null(declarationToSearchForInReferencedNameSet);
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("In skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): This is not a template function instantation (member nor non-member function) \n");
#endif
             }
        }

  // DQ (6/22/2011): This fixes test2011_97.C which only has a defining declaration so that the declaration->get_firstNondefiningDeclaration() was NULL.
  // if (referencedNameSet.find(declaration->get_firstNondefiningDeclaration()) == referencedNameSet.end())
     if (referencedNameSet.find(declarationToSearchForInReferencedNameSet) == referencedNameSet.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("   --- $$$$$$$$$$ NOT Found: declaration %p = %s in referencedNameSet referencedNameSet.size() = %" PRIuPTR " \n",
               declaration,declaration->class_name().c_str(),referencedNameSet.size());
#endif
          skipNameQualification = true;
        }
       else
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("   --- $$$$$$$$$$ FOUND: declaration %p = %s in referencedNameSet \n",declaration,declaration->class_name().c_str());
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("Leaving skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): skipNameQualification = %s \n",skipNameQualification ? "true" : "false");
#endif

     return skipNameQualification;
   }


// void NameQualificationTraversal::nameQualificationTypeSupport  ( SgType* type, SgScopeStatement* currentScope, SgInitializedName* initializedName, SgStatement* currentStatement, SgStatement* positionStatement )
void
NameQualificationTraversal::nameQualificationTypeSupport  ( SgType* type, SgScopeStatement* currentScope, SgInitializedName* initializedName )
   {
  // DQ (8/8/2020): this is code refactored from the evaluateInheritedAttribute() function within the SgInitializeName handling.
  // This code support the name qualification of the type associated with a SgInitializedName (it might be useful else where as well).

       // else for type handling.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: initializedName->get_type(): before stripType(): type = %p = %s \n",type,type->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf ("NQTSNQTSNQTSNQTSNQTSNQTSNQTSNQTSNQTS \n");
          printf("Case SgInitializedName: initializedName->get_type(): before stripType(): type = %p = %s \n",type,type->class_name().c_str());
          printf ("NQTSNQTSNQTSNQTSNQTSNQTSNQTSNQTSNQTS \n");
#endif
       // DQ (4/15/2019): Reset the type so that we don't miss the SgPointerMemberType.
          unsigned char bit_array = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_RVALUE_REFERENCE_TYPE |
                                    SgType::STRIP_POINTER_TYPE  | SgType::STRIP_ARRAY_TYPE;
          type = type->stripType(bit_array);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
          mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: initializedName->get_type(): after stripType(): type = %p = %s \n",type,type->class_name().c_str());
#endif
          SgStatement* currentStatement = TransformationSupport::getStatement(initializedName);
          ASSERT_not_null(currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("case of SgInitializedName: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif

#if DEBUG_INITIALIZED_NAME
          mfprintf(mlog [ WARN ] ) ("################################################################## \n");
          mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: Processing the SgInitializedName IR's type \n");
          mfprintf(mlog [ WARN ] ) ("################################################################## \n");
#endif

       // DQ (4/19/2019): It might be that we should call this after the traveral over each type instead of before we traverse the type.
       // traverseType(initializedName->get_type(),initializedName,currentScope,currentStatement);

       // DQ (4/19/2019): Added current statement to paremter list for recursive call.
       // DQ (4/18/2019): I think we need to traverse the type doing a proper type travesal, since it can consist of
       // long chains of types that each must be name qualified. The example of a chain of SgPointerToMemberTypes is
       // the best example of this.
          ASSERT_not_null(currentScope);

       // DQ (4/27/2019): Refactored this code to be outside of the false block below, so it can be used both there
       // and outside the false branch afterward.
          SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
#if DEBUG_NONTERMINATION
       // DQ (5/4/2024): Added to support debugging non-termination of unit test tool.
          printf ("debugging non-termination: declaration = %p \n",declaration);
          if (declaration != NULL)
             {
               printf ("debugging non-termination: declaration = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
               Sg_File_Info* fileInfo = declaration->get_file_info();
               if (fileInfo != NULL)
                  {
                    printf("debugging non-termination: declaration: --- line %d col = %d file = %s \n",fileInfo->get_line(),fileInfo->get_col(),fileInfo->get_filenameString().c_str());
                  }
             }
#endif

       // DQ (4/22/2019): If we have resolved the type (after stripType() function) to a SgPointerMemberType,
       // then we need to traverse the type using a type traversal.  Else we can handle it normally.
       // generateNestedTraversalWithExplicitScope(type,currentScope,currentStatement,initializedName);
       // traverseType(initializedName->get_type(),initializedName,currentScope,currentStatement);
          SgPointerMemberType* pointerMemberType = isSgPointerMemberType(type);
          if (pointerMemberType != NULL)
             {
#if DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("########################################################################## \n");
               mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: Calling generateNestedTraversalWithExplicitScope() \n");
               mfprintf(mlog [ WARN ] ) ("########################################################################## \n");
#endif
               generateNestedTraversalWithExplicitScope(type,currentScope,currentStatement,initializedName);

            // DQ (4/19/2019): It might be that we should call this after the traveral over each type instead of before we traverse the type.
            // This way we save the correctly computed string for each type after the different parts of name qualificaiton are in place.
               traverseType(initializedName->get_type(),initializedName,currentScope,currentStatement);

#if DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("#################################################################################################### \n");
               mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: DONE: Processing the recursive evaluation of the SgInitializedName IR's type \n");
               mfprintf(mlog [ WARN ] ) ("#################################################################################################### \n");
#endif
             }
            else
             {
#if DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("Normal processing of type (no recursive call to evaluate the type) \n");
#endif
#if DEBUG_NONTERMINATION
               printf("Normal processing of type (no recursive call to evaluate the type) \n");
#endif
            // The code for the normal processing of the type is below.

#if DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("######################################################################### \n");
               mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: Normal Processing the SgInitializedName IR's type \n");
               mfprintf(mlog [ WARN ] ) ("######################################################################### \n");
#endif


            // DQ (4/27/2019): refactoring: As a result of the block processing of the type being extended, and this variable
            // being used outside of that block, we need to move this variable declaration to be outside of this block at the
            // top of this block.
            // We want to handle types from every where a SgInitializedName might be used.
            // SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());

            // DQ (4/22/2019): If there is a SgPointerMemberType, then don't processs as a normal type.
               if (pointerMemberType != NULL)
                  {
                    if (declaration != NULL)
                       {
                         mfprintf(mlog [ WARN ] ) ("None null declaration where detected valid SgPointerMemberType: declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                       }
                    ROSE_ASSERT(declaration == NULL);
                  }

            // DQ (4/14/2019): An alternative might be to support this in the getDeclarationAssociatedWithType() function.

               SgDeclType* declType = isSgDeclType(type);
               if (declType != NULL)
                  {
                 // Not clear if we need to worry about when the base type of the SgPointerMemberType is a SgDeclType.
#if 0
                    mfprintf(mlog [ WARN ] ) ("################################################################################## \n");
                    mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: Processing the SgInitializedName decltype IR node directly \n");
                    mfprintf(mlog [ WARN ] ) ("################################################################################## \n");
#endif

                 // We need to handle any possible name qualification of a type or SgVarRefExp used as decltype argument.
                    SgExpression* baseExpression = declType->get_base_expression();
                    SgType*       baseType       = declType->get_base_type();
                    if (baseExpression != NULL)
                       {
                      // Need name qualification for expression used in decltype().
#if 0
                         mfprintf(mlog [ WARN ] ) ("Make a recursive call from this context (processing decltype taking SgExpression) \n");
#endif
                      // DQ (8/8/2020): Removed reference to "n".
                      // DQ (6/30/2013): Added to support using generateNestedTraversalWithExplicitScope() instead of generateNameQualificationSupport().
                      // SgStatement* currentStatement = TransformationSupport::getStatement(n);
                         SgStatement* currentStatement = TransformationSupport::getStatement(initializedName);
#if 0
                         mfprintf(mlog [ WARN ] ) ("SgInitializedName: decltype: currentStatement = %p = %s \n",currentStatement,currentStatement != NULL ? currentStatement->class_name().c_str() : "null");
#endif
#if 0
                         mfprintf(mlog [ WARN ] ) ("Exiting as a test! (before recursive call) \n");
                         ROSE_ABORT();
#endif
                      // DQ (9/14/2015): Added debugging code.
                      // DQ (9/14/2015): This can be an expression in a type, in which case we don't have an associated scope.
                         if (currentStatement == NULL)
                            {
                           // This can be an expression in a type, in which case we don't have an associated scope.
                              mfprintf(mlog [ WARN ] ) ("Note: This can be an expression in a type, in which case we don't have an associated scope: baseExpression = %p = %s \n",
                                      baseExpression,baseExpression->class_name().c_str());
                            }
                           else
                            {
                              ASSERT_not_null(currentStatement);
                              SgScopeStatement* currentScope = currentStatement->get_scope();
                              ASSERT_not_null(currentScope);
#if 0
                              mfprintf(mlog [ WARN ] ) ("SgInitializedName: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                           // DQ (6/30/2013): For the recursive call use generateNestedTraversalWithExplicitScope() instead of generateNameQualificationSupport().
                           // generateNameQualificationSupport(originalExpressionTree,referencedNameSet);
                              generateNestedTraversalWithExplicitScope(baseExpression,currentScope);
                            }
#if 0
                         mfprintf(mlog [ WARN ] ) ("Exiting as a test! (after recursive call) \n");
                         ROSE_ABORT();
#endif
                       }
                      else
                       {
                      // Need name qualification for type used in decltype().  Not clear what I good example is of this!
                         ASSERT_not_null(baseType);
#if 0
                         mfprintf(mlog [ WARN ] ) ("REPORT ME: Unknow case: SgInitializedName: decltype: calling getDeclarationAssociatedWithType(): baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
                         declaration = getDeclarationAssociatedWithType(baseType);
#if 0
                         mfprintf(mlog [ WARN ] ) ("Exiting as a test! \n");
                         ROSE_ABORT();
#endif
                       }
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Case of SgInitializedName: getDeclarationAssociatedWithType(): type = %p = %s declaration = %p \n",initializedName->get_type(),initializedName->get_type()->class_name().c_str(),declaration);
#endif

            // DQ (4/22/2019): The detect of a SgPointerMemberType will force a type traversal, which means
            // we don't process the type as a normal type.  Even if it has a valid declaration.
            // Note: "Normal Type" in the name below means not having pointer to member types and being
            // a type derived from a declaration.
               bool processAsNormalTypeThatMightRequireNameQualification = ((pointerMemberType == NULL) && (declaration != NULL));
#if DEBUG_INITIALIZED_NAME || 0
               mfprintf(mlog [ WARN ] ) ("processAsNormalTypeThatMightRequireNameQualification = %s \n",processAsNormalTypeThatMightRequireNameQualification ? "true" : "false");
#endif
#if DEBUG_NONTERMINATION
               printf("processAsNormalTypeThatMightRequireNameQualification = %s \n",processAsNormalTypeThatMightRequireNameQualification ? "true" : "false");
#endif
            // if (declaration != NULL)
               if (processAsNormalTypeThatMightRequireNameQualification == true)
                  {
                    SgStatement* currentStatement = TransformationSupport::getStatement(initializedName);
                    ASSERT_not_null(currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
                    mfprintf(mlog [ WARN ] ) ("case of SgInitializedName: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif
                    SgScopeStatement* currentScope = currentStatement->get_scope();
                    ASSERT_not_null(currentScope);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
                    mfprintf(mlog [ WARN ] ) ("case of SgInitializedName: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                    SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(initializedName->get_parent());
                    if (ctorInitializerList != NULL)
                       {
                         SgClassDefinition* classDefinition = isSgClassDefinition(initializedName->get_scope());
                         if (classDefinition != NULL)
                            {
                              SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                              ASSERT_not_null(classDeclaration);

                              currentScope = classDeclaration->get_scope();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Found case of SgInitializedName in constructor preinitialization list: currentScope = %p = %s \n",
                                   currentScope,currentScope->class_name().c_str());
#endif
                            }
                       }
                    int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,currentStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
                    mfprintf(mlog [ WARN ] ) ("SgInitializedName's (%s) type: amountOfNameQualificationRequiredForType = %d \n",
                         initializedName->get_name().str(),amountOfNameQualificationRequiredForType);
#endif
#if 1
                 // DQ (8/4/2012): This is redundant code with where the SgInitializedName appears in the SgVariableDeclaration.
                 // **************************************************
                 // DQ (8/4/2012): The type being used might not have to be qualified if it is associated with a SgClassDeclaration
                 // that has not been defined yet.  This fixes test2012_165.C.
                 // **************************************************
                    bool skipGlobalNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(declaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("case of SgInitializedName: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                 // DQ (8/4/2012): However, this quasi-pathological case does not apply to template instantiations
                 // (only non-template classes or maybe named types more generally?).  Handle template declarations similarly.
                 // OR enum declarations (since they can have a forward declaration (except that this is a common languae extension...).
                    if (isSgTemplateInstantiationDecl(declaration) != NULL || isSgEnumDeclaration(declaration) != NULL || isSgNonrealDecl(declaration) != NULL)
                       {
                      // Do the regularly schedule name qualification for these cases.
                         skipGlobalNameQualification = false;
                       }
                      else
                       {
                      // Look back through the scopes and see if we are in a template instantiation or template scope,
                      // if so then do the regularly scheduled name qualification.

                         SgScopeStatement* scope = declaration->get_scope();
                      // mfprintf(mlog [ WARN ] ) ("case of SgInitializedName: scope = %p = %s \n",scope,scope->class_name().c_str());
                         int distanceBackThroughScopes = amountOfNameQualificationRequiredForType;
                      // mfprintf(mlog [ WARN ] ) ("case of SgInitializedName: distanceBackThroughScopes = %d \n",distanceBackThroughScopes);
                         while (distanceBackThroughScopes > 0 && scope != NULL)
                            {
                           // Traverse backwards through the scopes checking for a SgTemplateClassDefinition scope
                           // If we traverse off the end of SgGlobal then the amountOfNameQualificationRequiredForType
                           // value was trying to trigger global qualification, so this is not a problem.
                           // We at least need isSgTemplateInstantiationDefn, not clear about isSgTemplateClassDefinition.

                              if (isSgTemplateInstantiationDefn(scope) != NULL || isSgTemplateClassDefinition(scope) != NULL)
                                 {
                                   skipGlobalNameQualification = false;
                                 }

                              ASSERT_not_null(scope);
                              scope = scope->get_scope();

                              distanceBackThroughScopes--;
                            }
                       }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
                    mfprintf(mlog [ WARN ] ) ("Test of Type used in SgInitializedName: declaration = %p = %s skipGlobalNameQualification = %s \n",declaration,declaration->class_name().c_str(),skipGlobalNameQualification ? "true" : "false");
#endif
                 // DQ (4/26/2019): Need to call setNameQualificationForType so that we can save the name qualification string using the SgInitializedName as the key.
                 // DQ (8/4/2012): Added support to permit global qualification be be skipped explicitly (see test2012_164.C and test2012_165.C for examples where this is important).
                    setNameQualificationOnType(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);

#if DEBUG_INITIALIZED_NAME
                    mfprintf(mlog [ WARN ] ) ("@@@@@@@@@ Calling traverseType() to save type as string if it is contained types that would be shared AND name qualified \n");
#endif
                 // DQ (4/27/2019): I think we need to call this function to handle the name qualification on template
                 // arguments where the type is a template instantiation.
                    SgStatement* associatedStatement = currentScope;

                    SgNode* initializedNameParent = initializedName->get_parent();
                    ASSERT_not_null(initializedNameParent);

#if DEBUG_INITIALIZED_NAME || 0
                    mfprintf(mlog [ WARN ] ) ("initializedNameParent = %p = %s \n",initializedNameParent,initializedNameParent->class_name().c_str());
#endif
                    bool skipTraverseType = false;
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedNameParent);
                    if (variableDeclaration != NULL)
                       {
#if DEBUG_INITIALIZED_NAME
                         mfprintf(mlog [ WARN ] ) ("variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() = %s \n",
                              variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() ? "true" : "false");
#endif
                         // DQ (5/26/2019): I think this should be the reversed.
                         if (variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true)
                            {
                              skipTraverseType = true;
                            }
                       }

#if DEBUG_INITIALIZED_NAME || 0
                    mfprintf(mlog [ WARN ] ) ("skipTraverseType = %s \n",skipTraverseType ? "true" : "false");
                    mfprintf(mlog [ WARN ] ) ("initializedName->get_type() = %p = %s \n",initializedName->get_type(),initializedName->get_type()->class_name().c_str());
#endif
                 // mfprintf(mlog [ WARN ] ) ("Calling traverseType ALWAYS! \n");
                    if (skipTraverseType == false)
                       {
                         traverseType(initializedName->get_type(),initializedName,currentScope,associatedStatement);
                       }
                  }
                 else
                  {
                 // DQ (8/23/2014): This case is demonstrated by test2014_145.C. where a SgInitializedName is used in a SgArrayType.
                 // However, it would provide greater symetry to handle the SgInitializedName objects in the processing of the
                 // SgFunctionParameterList similar to how they are handling in the SgVariableDeclaration.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Case of SgInitializedName: getDeclarationAssociatedWithType() == NULL (this not associated with a type)  \n");
#endif
                    ASSERT_not_null(currentScope);

                    SgStatement* associatedStatement = currentScope;
                    int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,associatedStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgInitializedName's type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForType);
#endif
                 // DQ (4/27/2019): This will avoid over use of the generated string mechanism to represent types, but
                 // we need to allow template instantiations to be processed.
                 // DQ (4/10/2019): I think we need to call this function to handle the name qualification on template
                 // arguments where the type is a template instantiation.
                    if (amountOfNameQualificationRequiredForType > 0)
                       {
                         mfprintf(mlog [ WARN ] ) ("Calling traverseType() on initializedName->get_type(): amountOfNameQualificationRequiredForType > 0: amountOfNameQualificationRequiredForType = %d Calling traverseType() \n",
                              amountOfNameQualificationRequiredForType);
                         traverseType(initializedName->get_type(),initializedName,currentScope,associatedStatement);
                       }
                  }
#endif

#if DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("######################################################################## \n");
               mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: DONE: Processing the SgInitializedName IR's type \n");
               mfprintf(mlog [ WARN ] ) ("######################################################################## \n");
#endif

            // DQ (4/27/2019): This is the new location of the end of the block to process the SgInitializedName's type.
             }

       // endif for type handling.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NONTERMINATION
          printf ("NQTSNQTSNQTSNQTSNQTSNQTSNQTSNQTSNQTS \n");
          printf ("Leaving nameQualificationTypeSupport \n");
          printf ("NQTSNQTSNQTSNQTSNQTSNQTSNQTSNQTSNQTS \n");
#endif
   }




NameQualificationInheritedAttribute
NameQualificationTraversal::evaluateInheritedAttribute(SgNode* n, NameQualificationInheritedAttribute inheritedAttribute)
   {
     ASSERT_not_null(n);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("\n\n****************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("****************************************************** \n");
  // mfprintf(mlog [ WARN ] ) ("Inside of NameQualificationTraversal::evaluateInheritedAttribute(): node = %p = %s = %s \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str());
     mfprintf(mlog [ WARN ] ) ("Inside of NameQualificationTraversal::evaluateInheritedAttribute(): node = %s = %s = %p \n",n->class_name().c_str(),SageInterface::get_name(n).c_str(),n);
     mfprintf(mlog [ WARN ] ) ("****************************************************** \n");
#endif

  // DQ (8/14/2025): This is an optimization to skip the traversal of the AST outside of what is in the source tree.
     if (suppressNameQualificationAcrossWholeTranslationUnit == true)
        {
       // SgStatement* statement = isSgStatement(n);
          SgLocatedNode* locatedNode = isSgLocatedNode(n);
       // if (statement != NULL)
          if (locatedNode != NULL)
             {
            // DQ (8/14/2025): Adding support to count the number of statements traversed in the name qualification when using traverseInputFile().
            // It should be only the statements in the source file, but it appears to include statements marked as compilerGenerated.
               AstPerformance::numberOfStatementsProcessedInNameQualificationUsingTraverseInputFile++;

            // if (statement->get_file_info()->get_filenameString() != "compilerGenerated")
            // if (statement->isCompilerGenerated() == true)
               if (locatedNode->isCompilerGenerated() == false)
                  {
                 // We could just check is the nearest parent statement is compiler generated.
                 // Or we could see if this is from a header file...(let's not do that).
                    SgStatement* statement = SageInterface::getEnclosingStatement(locatedNode);
                    if (statement->isCompilerGenerated() == false)
                       {
#if 0
                         printf("Inside of NameQualificationTraversal::evaluateInheritedAttribute(): counter = %d node = %s = %s = %p \n",
                              AstPerformance::numberOfStatementsProcessedInNameQualificationUsingTraverseInputFile,n->class_name().c_str(),SageInterface::get_name(n).c_str(),n);
                         printf(" --- statement->get_file_info()->get_filenameString() = %s \n",statement->get_file_info()->get_filenameString().c_str());
                      // printf(" --- locatedNode->get_file_info()->get_filenameString() = %s \n",locatedNode->get_file_info()->get_filenameString().c_str());
#endif
                       }
                      else
                       {
                         return NameQualificationInheritedAttribute(inheritedAttribute);
                       }
                  }
                 else
                  {
                    return NameQualificationInheritedAttribute(inheritedAttribute);
                  }
             }
        }

#if DEBUG_NONTERMINATION
  // DQ (5/3/2024): Debugging non-terminating name qualification case in unit testing.
     printf("In evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
     Sg_File_Info* tmp_fileInfo = n->get_file_info();
     if (tmp_fileInfo != NULL)
        {
          printf("NameQualificationTraversal: --- n = %p = %s line %d col = %d file = %s \n",n,n->class_name().c_str(),tmp_fileInfo->get_line(),tmp_fileInfo->get_col(),tmp_fileInfo->get_filenameString().c_str());
        }
#endif

#if 0
     SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(n);
     if (templateInstantiationMemberFunctionDecl != NULL)
        {
          if (templateInstantiationMemberFunctionDecl->get_name() == "allocator")
             {
               printf ("Setting disableNameQualification == true \n");
               disableNameQualification = true;
             }
        }
#endif

#if 0
  // DQ (8/1/2020): Setup a reference to the namespace alias map (so that the map is available in member functions of NameQualificationTraversal).
  // Specifically the function setNameQualificationSupport() requires this information so that any reference to a namespace declaration that
  // is in the map will use the alias as a priority.
     namespaceAliasDeclarationMapFromInheritedAttribute = &(inheritedAttribute.get_namespaceAliasDeclarationMap());
#endif

#if 0
  // DQ (2/13/2017): This will report progress through the global scope.
     if (SgProject::get_verbose() >= 0)
       {
         SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(n);
         SgDeclarationStatement* definingDeclarationStatement = NULL;
         if (declarationStatement != NULL)
            {
#if 0
              mfprintf(mlog [ WARN ] ) ("Name qualification progress: test: declarationStatement = %p = %s \n",declarationStatement,declarationStatement->class_name().c_str());
#endif
              definingDeclarationStatement = declarationStatement->get_definingDeclaration();
            }
         if (declarationStatement != NULL && declarationStatement == definingDeclarationStatement)
            {
           // This is the defining declaration
              mfprintf(mlog [ WARN ] ) ("Name qualification progress (inheritedAttribute): declarationStatement = %p = %s \n",declarationStatement,declarationStatement->class_name().c_str());
              string filename = declarationStatement->get_file_info()->get_filename();
              int linenumber  = declarationStatement->get_file_info()->get_line();
              mfprintf(mlog [ WARN ] ) ("--- filename = %s line = %d \n",filename.c_str(),linenumber);
            }
         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(n);
         if (typedefDeclaration != NULL)
            {
              mfprintf(mlog [ WARN ] ) ("Name qualification progress: typedefDeclaration = %p = %s name = %s \n",typedefDeclaration,typedefDeclaration->class_name().c_str(),typedefDeclaration->get_name().str());
            }
       }
#endif

#if 0
     mfprintf(mlog [ WARN ] ) ("In evaluateInheritedAttribute(): inheritedAttribute.get_currentScope() = %p \n",inheritedAttribute.get_currentScope());
     if (inheritedAttribute.get_currentScope() != NULL)
        {
          mfprintf(mlog [ WARN ] ) ("In evaluateInheritedAttribute(): inheritedAttribute.get_currentScope() = %p = %s \n",inheritedAttribute.get_currentScope(),inheritedAttribute.get_currentScope()->class_name().c_str());
        }
       else
        {
          mfprintf(mlog [ WARN ] ) ("inheritedAttribute.get_currentScope() NOT set \n");
        }
#endif

     SgSourceFile* sourceFile = isSgSourceFile(n);
     if (sourceFile != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In evaluateInheritedAttribute(): sourceFile = %p = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif
        }

  // DQ (5/24/2013): Allow the current scope to be tracked from the traversal of the AST
  // instead of being computed at each IR node which is a problem for template arguments.
  // See test2013_187.C for an example of this.
     SgScopeStatement* evaluateInheritedAttribute_currentScope = isSgScopeStatement(n);
     if (evaluateInheritedAttribute_currentScope != NULL)
        {
#if 0
          mfprintf(mlog [ WARN ] ) ("In evaluateInheritedAttribute(): resetting inheritedAttribute.get_currentScope() = %p = %s \n",n,n->class_name().c_str());
#endif
          inheritedAttribute.set_currentScope(evaluateInheritedAttribute_currentScope);
        }

  // DQ (5/24/2013): We can't set the current scope until we at first get past the SgProject and SgSourceFile IR nodes in the AST traversal.
     if (isSgSourceFile(n) == NULL && isSgProject(n) == NULL)
        {
       // DQ (5/25/2013): This only appears to fail for test2013_63.C.
          if (inheritedAttribute.get_currentScope() == NULL)
             {
               mfprintf(mlog [ WARN ] ) ("WARNING: In NameQualificationTraversal::evaluateInheritedAttribute(): inheritedAttribute.get_currentScope() == NULL: node = %p = %s = %s \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str());
             }
       // ASSERT_not_null(inheritedAttribute.get_currentScope());
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
  // Extra information about the location of the current node.
     Sg_File_Info* fileInfo = n->get_file_info();
     if (fileInfo != NULL)
        {
          mfprintf(mlog [ WARN ] ) ("NameQualificationTraversal: --- n = %p = %s line %d col = %d file = %s \n",n,n->class_name().c_str(),fileInfo->get_line(),fileInfo->get_col(),fileInfo->get_filenameString().c_str());
        }
#endif

  // Locations where name qualified references can exist:
  //   1) Base class names
  //   2) Variable names in declarations (see test2011_30.C)
  //   3) Types referenced by variables
  //   4) Types referenced in function parameter lists
  //   5) Return types referenced by functions (including covariant types for member functions)
  //   6) References to functions thrown by functions
  //   7) Namespaces referenced by SgUsingDirectiveStatement IR nodes
  //   8) Variables and declarations reference from SgUsingDeclarationStatement IR nodes
  //   9) Functions reference by SgFunctionRefExp IR nodes
  //  10) Functions reference by SgMemberFunctionRefExp IR nodes
  //  11) Variable reference by SgVarRefExp IR nodes
  //  12) Template arguments (and default template parameter specifications)
  //  13) Template parameters?
  //  14) Function declarations
  //  15) Member function declarations
  //  16) Typedef declarations
  //  17) Throw exception lists
  //  18) A number of expressions (listed below)
  //         SgVarRefExp
  //         SgFunctionRefExp
  //         SgMemberFunctionRefExp
  //         SgConstructorInitializer
  //         SgNewExp
  //         SgCastExp
  //         SgSizeOfOp
  //         SgTypeIdOp
  //  19) SgVarRefExp's hidden in array types (SgArrayType) (requires explicitly specified current scope).
  //  20)

  // The use of name qualification in types is a complicated because types are shared and the same type can
  // have it's template arguments qualified differently depending on where it is referenced.  This is an
  // issue for all references to types containing template arguments and not just where SgInitializedName
  // are used. Since name qualification of the same type can only vary at most from statement to statement
  // in some cases likely only from scope to scope) we need only associate names to statements (see note 1).
  // I would like to for now use scopes as the finest level of resolution.
  // The solution:
  //    1) Support a test for which types are effected.  a member function of SgType will evaluate if
  //       a type uses template arguments or subtypes using template arguments and if these could require
  //       name qualification.
  //    2) A map will be created in each scope (or maybe statement) for types used in that scope (or statement)
  //       which will store the computed name of the type (evaluated as part of the name qualification support;
  //       called immediately at the start of the unparsing of each SgFile).  The SgType pointer will be used
  //       as the key into the map of SgType to names (stored as strings).
  //    3) The unparser will check for entries in the associated map and use the stringified type names if they
  //       are available.  This can be done at the top level of the unparseType() function.

  // Note 1: A forward declaraion of a function (and maybe a class) can happen in a scope that does not allow a defining
  // declaration and when this happens the name qualification of that function is undefined.  However after a
  // forward declaration in a scope permitting a defining declaration, the function name must be qualified as per
  // usual name qualification rules.

  // DQ (8/4/2012): It is too complex to add this declaration support here (use the previous code and just
  // handle the specific cases where we need to add a declaration to the reference set separately.
  // DQ (8/4/2012): Let any procesing define a declaration to be used for the reference set.
  // Ititially it is NULL, but specific cases can set this so that the associated declaration
  // we be recorded as referenced.  This is important for test2012_164.C, where a variable declaration
  // generates a reference to a type that at first must not use a qualified name (since there is no
  // explicit forward declaration for the type (a class/struct).  This is a facinating case since
  // the scope of the declaration is the outer namespace from where it is first implicitly referenced
  // via a variable declaration.  This is part of debugging test2005_133.C (of which test2012_16[3-5].C
  // are simpler cases.
  // SgDeclarationStatement* declarationForReferencedNameSet = NULL;

  // DQ (6/11/2011): This is a new IR nodes, but the use of it causes a few problems (test2004_109.C)
  // because the source position is not computed correctly (I think).
     SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(n);
     if (templateClassDefinition != NULL)
        {
#if 0
       // DQ (3/5/2012): I think this is OK, since the SgTemplateClassDefinition is derived from the SgClassDefinition,
       // that case below will work well. This example is demonstrated in test2012_10.C.
       // DQ (11/20/2011): Debugging new use of SgTemplateClassDefinition (only used in new version of EDG 4.x support).
          mfprintf(mlog [ WARN ] ) ("Name qualification of SgTemplateClassDefinition = %p not implemented, OK (no special handling is required, we be processed as a SgClassDefinition) (need and example to debug this case) \n",templateClassDefinition);
#endif

       // DQ (11/20/2011): Commented out this assertion.
       // ROSE_ASSERT(false);
        }

     SgClassDefinition* classDefinition = isSgClassDefinition(n);
  // if (classDefinition != NULL && templateClassDefinition == NULL)
     if (classDefinition != NULL)
        {
       // Add all of the named types from this class into the set that have already been seen.
       // Note that this should not include nested classes (I think).

          SgBaseClassPtrList & baseClassList = classDefinition->get_inheritances();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("!!!!! Evaluate the derived classes: are they visible --- baseClassList.size() = %" PRIuPTR " \n",baseClassList.size());
#endif
#if 0
          string declaration_name = classDefinition->get_declaration()->unparseToString();
          mfprintf(mlog [ WARN ] ) ("test: declaration_name (unparseToString()) = %s \n",declaration_name.c_str());
#endif

          SgBaseClassPtrList::iterator i = baseClassList.begin();
          while (i != baseClassList.end())
             {

#define DEBUG_BASE_CLASS_SUPPORT 0

            // Check each base class.
               SgBaseClass* baseClass = *i;
               ASSERT_not_null(baseClass);

               if (isSgNonrealBaseClass(baseClass)) {
                 // FIXME nothing to do?
               } else if (isSgExpBaseClass(baseClass)) {
                  ROSE_ABORT(); // TODO traverse the expression ???
                  }
                 else
                  {
                    SgClassDeclaration * classDeclaration = baseClass->get_base_class();
                    ASSERT_not_null(classDeclaration);
                    SgScopeStatement * currentScope = classDefinition->get_scope();
                    ASSERT_not_null(currentScope);

                 // Name these better to be more clear.
                    SgClassDeclaration* derivedClassDeclaration = classDefinition->get_declaration();
                    if (derivedClassDeclaration != derivedClassDeclaration->get_firstNondefiningDeclaration())
                       {
                         derivedClassDeclaration = isSgClassDeclaration(derivedClassDeclaration->get_firstNondefiningDeclaration());
#if DEBUG_BASE_CLASS_SUPPORT
                         mfprintf(mlog [ WARN ] ) ("RESET derivedClassDeclaration to firstNondefiningDeclaration: %p \n",derivedClassDeclaration);
#endif
                       }
                 // SgClassDeclaration* baseClassDeclaration    = classDeclaration;
                    SgClassDeclaration* baseClassDeclaration    = baseClass->get_base_class();
                    if (baseClassDeclaration != baseClassDeclaration->get_firstNondefiningDeclaration())
                       {
                         baseClassDeclaration = isSgClassDeclaration(baseClassDeclaration->get_firstNondefiningDeclaration());
#if DEBUG_BASE_CLASS_SUPPORT
                         mfprintf(mlog [ WARN ] ) ("RESET baseClassDeclaration to firstNondefiningDeclaration: %p \n",baseClassDeclaration);
#endif
                       }

                    ASSERT_not_null(derivedClassDeclaration);
                    ASSERT_not_null(baseClassDeclaration);

                 // DQ (1/24/2019): Build a list of private base classes and accumulate them from any base classes.
                 // This is important to support additional name qualification required when derived classes
                 // reference a nested base class that may be private.
                    SgBaseClassModifier* baseClassDeclarationBaseClassModifier = baseClass->get_baseClassModifier();
                    ASSERT_not_null(baseClassDeclarationBaseClassModifier);
                    SgAccessModifier & baseClassDeclarationAccessModifier = baseClassDeclarationBaseClassModifier->get_accessModifier();
#if DEBUG_BASE_CLASS_SUPPORT
                    mfprintf(mlog [ WARN ] ) ("  --- derivedClassDeclaration = %p name = %s \n",derivedClassDeclaration,SageInterface::get_name(derivedClassDeclaration).c_str());
                    mfprintf(mlog [ WARN ] ) ("  --- baseClassDeclaration    = %p name = %s \n",baseClassDeclaration,SageInterface::get_name(baseClassDeclaration).c_str());
                    mfprintf(mlog [ WARN ] ) ("  --- classDeclarationAccessModifier = %s \n",baseClassDeclarationAccessModifier.displayString().c_str());

                    mfprintf(mlog [ WARN ] ) ("  --- privateBaseClassSets.size()  = %zu \n",privateBaseClassSets.size());
                    mfprintf(mlog [ WARN ] ) ("  --- inaccessibleClassSets.size() = %zu \n",inaccessibleClassSets.size());
#endif
                    if (baseClassDeclarationAccessModifier.isPrivate() == true)
                       {
#if DEBUG_BASE_CLASS_SUPPORT
                         mfprintf(mlog [ WARN ] ) ("Found private derivation of baseClassDeclaration = %s from derivedClassDeclaration = %s \n",baseClassDeclaration->get_name().str(),derivedClassDeclaration->get_name().str());
#endif
                         if (privateBaseClassSets.find(derivedClassDeclaration) != privateBaseClassSets.end())
                            {
#if DEBUG_BASE_CLASS_SUPPORT
                              mfprintf(mlog [ WARN ] ) ("privateBaseClassSets has an entry for this class: derivedClassDeclaration = %p = %s name = %s \n",
                                   derivedClassDeclaration,derivedClassDeclaration->class_name().c_str(),derivedClassDeclaration->get_name().str());
#endif
                            }
                           else
                            {
                              std::set<SgClassDeclaration*> privateClasses;
                              privateClasses.insert(baseClassDeclaration);
                              privateBaseClassSets.insert(std::pair<SgClassDeclaration*,std::set<SgClassDeclaration*> >(derivedClassDeclaration,privateClasses));
                            }

                       }

                 // Build up the inaccessibleClassSets map.
                    if (privateBaseClassSets.find(baseClassDeclaration) != privateBaseClassSets.end())
                       {
                         std::set<SgClassDeclaration*> & privateBaseClasses = privateBaseClassSets[baseClassDeclaration];
#if DEBUG_BASE_CLASS_SUPPORT
                         mfprintf(mlog [ WARN ] ) ("In base class: find the set of private base classes: privateBaseClasses.size() = %zu \n",privateBaseClasses.size());
#endif
                         std::set<SgClassDeclaration*>::iterator j = privateBaseClasses.begin();
                         while (j != privateBaseClasses.end())
                            {
                              SgClassDeclaration* privateBaseClassDeclaration = *j;
                              ASSERT_not_null(privateBaseClassDeclaration);
#if DEBUG_BASE_CLASS_SUPPORT
                              mfprintf(mlog [ WARN ] ) ("  --- privateBaseClassDeclaration = %p = %s name = %s \n",privateBaseClassDeclaration,privateBaseClassDeclaration->class_name().c_str(),privateBaseClassDeclaration->get_name().str());
#endif
                              std::set<SgClassDeclaration*> privateClasses;
                              privateClasses.insert(privateBaseClassDeclaration);
                              inaccessibleClassSets.insert(std::pair<SgClassDeclaration*,std::set<SgClassDeclaration*> >(derivedClassDeclaration,privateClasses));

                              j++;
                            }
                       }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Calling nameQualificationDepth() for base class baseClassDeclaration name = %s \n",baseClassDeclaration->get_name().str());
#endif
                    int amountOfNameQualificationRequired = nameQualificationDepth(baseClassDeclaration,currentScope,classDefinition);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("amountOfNameQualificationRequired (base class) = %d \n",amountOfNameQualificationRequired);
#endif
                    setNameQualification(baseClass,baseClassDeclaration,amountOfNameQualificationRequired);
                  }

            // DQ (12/23/2015): Also need to add this to the aliasSymbolCausalNodeSet.
               SgSymbolTable::get_aliasSymbolCausalNodeSet().insert(baseClass);
            // DQ (4/12/2019): New code to uniformally support template instantiations that are referenced (shared), where they are shared.

            // DQ (4/12/2019): If this is a templated class then we have to save the name because its templated name might have template arguments that require name qualification.
               ASSERT_not_null(baseClass);
            // SgTemplateInstantiationDecl* templateInstantiationClassDeclaration = isSgTemplateInstantiationDecl(*i);
               SgTemplateInstantiationDecl* templateInstantiationClassDeclaration = isSgTemplateInstantiationDecl(baseClass->get_base_class());
               if (templateInstantiationClassDeclaration != NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                    mfprintf(mlog [ WARN ] ) ("Found a SgTemplateInstantiationDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationClassDeclaration->get_name().str());
#endif
                 // DQ (4/12/2019): When this is a function call in an array type index expression we can't identify an associated statement.
                    SgStatement* currentStatement = TransformationSupport::getStatement(baseClass);
                 // ASSERT_not_null(currentStatement);
                    if (currentStatement != NULL)
                       {
                         SgScopeStatement* currentScope = currentStatement->get_scope();
                         ASSERT_not_null(currentScope);

                      // traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement);
                      // traverseTemplatedFunction(functionRefExp,functionRefExp,currentScope,currentStatement);
                         traverseTemplatedClass(baseClass,baseClass,currentScope,currentStatement);
                       }
                      else
                       {
                         mfprintf(mlog [ WARN ] ) ("Note: Name qualification: parent statement could not be identified for baseClass = %p = %s \n",baseClass,baseClass->class_name().c_str());
                       }
                  }
               i++;
             }
        }

  // DQ (6/16/2017): It might be that this case should exclue the case of a SgTemplateInstantiationDecl so that it can be processed below.
  // Handle references to SgMemberFunctionDeclaration...
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(n);
     if (classDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In name qualification:\n");
          mfprintf(mlog [ WARN ] ) (" - classDeclaration   = %p = %s \n",classDeclaration, classDeclaration->class_name().c_str());
          mfprintf(mlog [ WARN ] ) (" -     ->get_parent() = %p = %s \n",classDeclaration->get_parent(), classDeclaration->get_parent() ? classDeclaration->get_parent()->class_name().c_str() : "");
#endif

       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(classDeclaration->get_parent());

       // DQ (6/16/2017): Handle the case of a template instantiation directive containing a template instantiation (which is also a SgClassDeclaration).
          if (currentScope == NULL)
             {
            // Check if this is a template class instantiation that is part of a template instantiation directive
               SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement = isSgTemplateInstantiationDirectiveStatement(classDeclaration->get_parent());
               if (templateInstantiationDirectiveStatement != NULL)
                  {
                 // currentScope is that of the parent of the templateInstantiationDirectiveStatement
                    currentScope = isSgScopeStatement(templateInstantiationDirectiveStatement->get_parent());
#if 0
                 // DQ (4/20/2018): Added debugging support.
                    mfprintf(mlog [ WARN ] ) ("In name qualification: processing SgClassDeclaration: found SgTemplateInstantiationDirectiveStatement \n");
#endif
                 // I think this has to be true.
                    ASSERT_not_null(currentScope);
                  }
                 else
                  {
                 // DQ (2/18/2019): Adding support for when the SgClassDeclaration is defined in another declaration (e.g. SgTypedefDeclaration).
                    SgNode* parent = classDeclaration->get_parent();
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(parent);
                 // currentScope = isSgScopeStatement(typedefDeclaration->get_parent());
                    if (typedefDeclaration != NULL)
                       {
                         currentScope = isSgScopeStatement(typedefDeclaration->get_parent());

                      // DQ (2/18/2019): We should have a valid currentScope at this point.
                         if (currentScope == NULL)
                            {
                              mfprintf(mlog [ WARN ] ) ("FATAL: Could not identify scope for class declaration: parent = %p = %s \n",parent,parent->class_name().c_str());
                              ROSE_ABORT();
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Found SgClassDeclaration in SgTypedefDeclaration: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                            }
                       }
                      else
                       {
                      // DQ (2/19/2019): This is frequently a SgLambdaExp or a SgVariableDeclaration
                      // Computing the current scope does not always seem possible.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("In name qualification: Cannot compute a valid scope for the classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
                         mfprintf(mlog [ WARN ] ) (" --- parent = %p = %s \n",parent,parent->class_name().c_str());
#endif
                       }
                  }
             }

       // ASSERT_not_null(currentScope);
          if (currentScope != NULL)
             {
            // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same
            // as the scope of the class declaration.  However, the analysis should work and determin that the
            // required name qualification length is zero.

            // DQ (7/22/2017): Refactored this code.
               SgScopeStatement* class_scope = classDeclaration->get_scope();

            // DQ (7/22/2017): I think we can assert this.
               ASSERT_not_null(class_scope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("currentScope                  = %p = %s \n",currentScope,currentScope->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("classDeclaration->get_scope() = %p = %s \n",class_scope,class_scope->class_name().c_str());
#endif
               if (currentScope != class_scope && !SgScopeStatement::isEquivalentScope(currentScope, class_scope))
                  {
                      // DQ (6/11/2013): Added test to make sure that name qualification is ignored for friend function where the class has not yet been seen.
                      // if (classDeclaration->get_declarationModifier().isFriend() == false)
                         SgDeclarationStatement* declarationForReferencedNameSet = classDeclaration->get_firstNondefiningDeclaration();
                         ASSERT_not_null(declarationForReferencedNameSet);
                         if (referencedNameSet.find(declarationForReferencedNameSet) != referencedNameSet.end())
                            {
                              int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,classDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("SgClassDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                              setNameQualification(classDeclaration,amountOfNameQualificationRequired);
                            }
                           else
                            {
                           // DQ (2/12/2019): This branch is taken within Cxx11_tests/test2019_107.C where the associated
                           // SgTemplateInstantiationDecl is a specialization and does require name qualification.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("This classDeclaration has not been seen before so skip the name qualification \n");
#endif
                           // DQ (2/12/2019): If this is a SgTemplateInstantiationDecl, it might require name qualification.
                              SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(classDeclaration);
                              if (templateInstantiationDecl != NULL)
                                 {
                                   int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,classDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("SgClassDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                                   setNameQualification(classDeclaration,amountOfNameQualificationRequired);
                                 }
                            }
                  }
                 else
                  {
                 // DQ (7/22/2017): I think the template arguments name qualification can be required. This fixes test2017_56.C.
                    int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,classDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) (" - amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                    setNameQualification(classDeclaration,amountOfNameQualificationRequired);
                  }
             }
        }


  // DQ (4/26/2019): Adding support to detect SgTemplateVariableDeclaration (see test2019_399.C).
     SgTemplateVariableDeclaration* templateVariableDeclaration = isSgTemplateVariableDeclaration(n);
     if (templateVariableDeclaration != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Case SgTemplateVariableDeclaration: name qualificaiton will be processed by the SgInitializedName support \n");
#endif
        }


  // Handle the types used in variable declarations...
  // A problem with this implementation is that it relies on there being one SgInitializedName per SgVariableDeclaration.
  // This is currently the case for C++, but we would like to fix this.  It is not clear if the SgInitializedName should carry its
  // own qualification or not (this violates the idea that the IR node that has the reference stored the name qualification data).
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n);
     if (variableDeclaration != NULL)
        {
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
          ASSERT_not_null(initializedName);

       // DQ (7/24/2011): if there is a bit-field width specifier then it could contain variable references that require name qualification.
          SgVariableDefinition* variableDefinition = isSgVariableDefinition(initializedName->get_declptr());
          if (variableDefinition != NULL)
             {
            // This is not always the correct current scope (see test2011_70.C for an example).
               SgScopeStatement* currentScope = SageInterface::getScope(variableDeclaration);
               ASSERT_not_null(currentScope);

               SgExpression* bitFieldWidthSpecifier = variableDefinition->get_bitfield();
               if (bitFieldWidthSpecifier != NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Traverse the bitFieldWidthSpecifier and add any required name qualification.\n");
#endif
                 // DQ (4/28/2019): We might need to add additional arguments to this function, such as in:
                 // generateNestedTraversalWithExplicitScope(type,currentScope,currentStatement,initializedName);
                    generateNestedTraversalWithExplicitScope(bitFieldWidthSpecifier,currentScope);
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("++++++++++++++++ DONE: Calling nameQualificationDepth to evaluate the name \n\n");
#endif
        }

  // DQ (4/18/2019): Added support for traversals over the type, so that we can support SgPointerMemberType which can exist within nested type.
  // Because this can happen in nexted types we need the type traversal to discover these.  I am hoping this will not be a performance issue
  // for long types stemming from template instatiations, if so we might want to detect these seperately and avoid name qualification for them.
     SgPointerMemberType* pointerMemberType = isSgPointerMemberType(n);
     if (pointerMemberType != NULL)
        {
          SgScopeStatement* currentScope = inheritedAttribute.get_currentScope();
          ASSERT_not_null(currentScope);

       // DQ (4/19/2019): This is not a good idea, I have modified the recursive step to allow us to pass the currentStatement as well (optionally).
       // DQ (4/18/2019): See if we can make the currentStatement just the currentScope.
          SgStatement* currentStatement = inheritedAttribute.get_currentStatement();
          ASSERT_not_null(currentStatement);

       // We need to have saved the referenceNode to use since this is associated with a shared type.
          SgNode* referenceNode = inheritedAttribute.get_referenceNode();
          ASSERT_not_null(referenceNode);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Case SgPointerMemberType: referenceNode = %p = %s \n",referenceNode,referenceNode->class_name().c_str());
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("###################################################### \n");
          mfprintf(mlog [ WARN ] ) ("Case SgPointerMemberType: Compute name qualification() \n");
          mfprintf(mlog [ WARN ] ) ("###################################################### \n");
#endif

          SgDeclarationStatement* classDeclaration = pointerMemberType->get_class_declaration_of();
          ASSERT_not_null(classDeclaration);

       // DQ (4/21/2019): This a SgExprStatement when we are processing a SgSizeOfOp IR node (see test2019_379.C).
          SgStatement* positionStatement = isSgStatement(currentStatement);
          if (positionStatement == NULL)
             {
               ASSERT_not_null(currentStatement);
               mfprintf(mlog [ WARN ] ) ("Error: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
             }
          ASSERT_not_null(positionStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Correcting associated declaration: classDeclaration  = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
          mfprintf(mlog [ WARN ] ) ("Correcting associated declaration: positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
          int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,positionStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("SgPointerMemberType: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
          setNameQualificationOnClassOf(pointerMemberType,classDeclaration,amountOfNameQualificationRequired);

       // DQ (4/20/2019): Now look at the base type, if it is not a SgPointerMemberType, then compute the associated name qualification.
       // We will of course traverse this type, but at the time of the traversal, we would not compute the name qualification since it
       // would be done from the context of the IR node that references the type.
          SgType* baseType = pointerMemberType->get_base_type();
          ASSERT_not_null(baseType);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Case SgPointerMemberType: baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif

       // DQ (4/21/2019): Reset the type so that we don't miss the SgPointerMemberType, but ignore other modifiers.
          unsigned char bit_array = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_RVALUE_REFERENCE_TYPE |
                                    SgType::STRIP_POINTER_TYPE  | SgType::STRIP_ARRAY_TYPE;
          baseType = baseType->stripType(bit_array);
          ASSERT_not_null(baseType);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Case SgPointerMemberType: after stripType(): baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif

          SgPointerMemberType* pointerMemberBaseType = isSgPointerMemberType(baseType);
          if (pointerMemberBaseType == NULL)
             {
            // Need to handle name qualification of this type (if there is an associated declaration).

               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(baseType);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Case SgPointerMemberType: declaration = %p = %s \n",declaration,(declaration != NULL) ? baseType->class_name().c_str() : "null");
#endif
               if (declaration != NULL)
                  {
                 // DQ (4/21/2019): Handle the base type of the SgPointerMemberType, if it is not another nested SgPointerMemberType IR node.
                    ASSERT_not_null(currentScope);
                    ASSERT_not_null(positionStatement);
                    int amountOfNameQualificationRequiredForType = nameQualificationDepth(declaration,currentScope,positionStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgPointerMemberType: base type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForType);
#endif
                    setNameQualificationOnBaseType(pointerMemberType,declaration,amountOfNameQualificationRequiredForType);
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("The base type of the SgPointerMemberType has no associated declaration (so cannot be name qualified) \n");
#endif
                  }
             }
            else
             {
            // This is a nested SgPointerMemberType in a SgPointerMemberType, it will be processed as part of the type traversal.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Detected a nested SgPointerMemberType, it will be processed as part of the recursion of the type traversal \n");
#endif
             }
        }

  // Handle SgType name qualification where SgInitializedName's appear outside of SgVariableDeclaration's (e.g. in function parameter declarations).
  // DQ (7/4/2021): Note that as of 2019 this code is used to handle the SgInitializedName IR node in variable declarations.
     SgInitializedName* initializedName = isSgInitializedName(n);
     if (initializedName != NULL)
        {

       // bool debugging = false;
#if DEBUG_INITIALIZED_NAME || 0
          mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: initializedName = %p = %s name = %s \n",initializedName,initializedName->class_name().c_str(),initializedName->get_name().str());
          mfprintf(mlog [ WARN ] ) (" --- initializedName->get_parent() = %p = %s \n",initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
#endif

#if DEBUG_NONTERMINATION
       // DQ (5/3/2024): Debugging non-terminating name qualification.
          printf("Case SgInitializedName: initializedName = %p = %s name = %s \n",initializedName,initializedName->class_name().c_str(),initializedName->get_name().str());
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
          mfprintf(mlog [ WARN ] ) ("Case of SgInitializedName: type = %p = %s name = %s \n",initializedName->get_type(),
               initializedName->get_type()->class_name().c_str(),initializedName->get_name().str());
          SgFunctionDeclaration* enclosingFunction = SageInterface::getEnclosingFunctionDeclaration(initializedName);
          if (enclosingFunction != NULL)
             {
               printf (" --- enclosingFunction = %p = %s name = %s \n",enclosingFunction,enclosingFunction->class_name().c_str(),enclosingFunction->get_name().str());
               printf (" --- enclosingFunction->get_scope() = %p = %s \n",enclosingFunction->get_scope(),enclosingFunction->get_scope()->class_name().c_str());
             }
#endif
       // DQ (3/31/2019): Adding name qualification for the SgInitialized name directly (then we need to remove
       // it from there it is introduced in the SgVariableDeclaration and the SgFunctionParameterList). The point
       // of adding it here is the it is required for pointer-to-member declarations that are not associated with
       // the base type of the pointer-to-member type and must be name qualified differently from the base type
       // (as demonstrated in C++11_tests/test2019_333.C).


       // DQ (4/26/2019): We need the currentScope where we are evaluating the name qualification, not the scope of the initialized name.
          SgNode* initializedNameParent = initializedName->get_parent();
          SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(initializedNameParent);
          if (declarationStatement == NULL)
             {
               mfprintf(mlog [ WARN ] ) ("What is this: initializedNameParent = %p = %s \n",initializedNameParent,initializedNameParent->class_name().c_str());
               ROSE_ABORT();
             }
          ASSERT_not_null(declarationStatement);
          SgScopeStatement* currentScope = declarationStatement->get_scope();

       // DQ (9/2/2020): Name qualification for the SgCtorInitializerList should use the scope of the associated class declaration.
          SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(initializedNameParent);
          if (ctorInitializerList != NULL)
             {
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(ctorInitializerList->get_parent());
               ROSE_ASSERT(memberFunctionDeclaration != NULL);
            // currentStatement = memberFunctionDeclaration->get_firstNondefiningDeclaration();
               currentScope = memberFunctionDeclaration->get_scope();
#if DEBUG_INITIALIZED_NAME
               printf ("Case of SgInitializedName: from SgCtorInitializerList: currentScope = %p = %s name = %s \n",currentScope,
                    currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
#endif
             }


#if DEBUG_INITIALIZED_NAME
          mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif

       // DQ (4/12/2019): Now that we have unified the SgInitializedName support, we need to handle this case here instead of in the SgfunctionParameterList support.
       // DQ (8/29/2014): This is a result of a transformation in the tutorial (codeCoverage.C) that does not appear to be implemeting a transformation correctly.
          if (currentScope == NULL)
             {
            // DQ (4/12/2019): Need to setup a local copy of the functionParameterList (since this code has been moved).
               SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(initializedName->get_parent());
               ASSERT_not_null(functionParameterList);

               mfprintf(mlog [ WARN ] ) ("Error: currentScope == NULL: functionParameterList = %p \n",functionParameterList);
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionParameterList->get_parent());
               ASSERT_not_null(functionDeclaration);
               mfprintf(mlog [ WARN ] ) ("Error: currentScope == NULL: functionDeclaration = %p = %s name = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
               ASSERT_not_null(functionDeclaration->get_file_info());
               functionDeclaration->get_file_info()->display("Error: currentScope == NULL: functionParameterList->get_parent(): debug");
               SgScopeStatement* temp_scope = SageInterface::getScope(functionDeclaration);
               ASSERT_not_null(temp_scope);

            // DQ (8/29/2014): It appears that we can't ask the SgFunctionParameterList for it's scope, but we can find the SgFunctionDeclaration from the parent and ask it; so this should be fixed.
               currentScope = temp_scope;

               mfprintf(mlog [ WARN ] ) ("It appears that in the case of a transforamtion, we can't always ask the SgFunctionParameterList for it's scope, but we can find the SgFunctionDeclaration from the parent and ask it; so this should be fixed. \n");
             }

          ASSERT_not_null(currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
          mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif

       // DQ (8/8/2020): Moved from the refactored code below.
          SgStatement* currentStatement = TransformationSupport::getStatement(initializedName);
          ASSERT_not_null(currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("case of SgInitializedName: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif

          SgType* type = initializedName->get_type();
          ASSERT_not_null(type);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
          printf ("From case SgIntializedName: calling nameQualificationTypeSupport() \n");
#endif
          nameQualificationTypeSupport (type,currentScope,initializedName);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
          mfprintf(mlog [ WARN ] ) ("initializedName->get_prev_decl_item() = %p \n",initializedName->get_prev_decl_item());
#endif

          bool initializedNameCouldRequireNameQualification = (initializedName->get_prev_decl_item() != NULL);
#if DEBUG_INITIALIZED_NAME
          printf ("Case SgInitializedName: initializedNameCouldRequireNameQualification = %s \n",initializedNameCouldRequireNameQualification ? "true" : "false");
#endif
          if (initializedNameCouldRequireNameQualification == true)
             {
               SgInitializedName* originallyDeclaredInitializedName = initializedName->get_prev_decl_item();

#if DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("originallyDeclaredInitializedName = %p = %s name = %s \n",
                    originallyDeclaredInitializedName,originallyDeclaredInitializedName->class_name().c_str(),originallyDeclaredInitializedName->get_name().str());
#endif
            // SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
               ASSERT_not_null(initializedName);
               ASSERT_not_null(initializedName->get_parent());

               ASSERT_not_null(originallyDeclaredInitializedName->get_parent());

            // SgDeclarationStatement* associatedDeclaration = isSgDeclarationStatement(initializedName->get_parent());
            // SgDeclarationStatement* associatedDeclaration = isSgDeclarationStatement(initializedName->get_parent());
               SgDeclarationStatement* associatedDeclaration = isSgDeclarationStatement(originallyDeclaredInitializedName->get_parent());
               if (associatedDeclaration == NULL)
                  {
#if DEBUG_INITIALIZED_NAME
                    mfprintf(mlog [ WARN ] ) ("Note: unexpected IR node: originallyDeclaredInitializedName->get_parent() = %p = %s \n",
                         originallyDeclaredInitializedName->get_parent(),originallyDeclaredInitializedName->get_parent()->class_name().c_str());
#endif
                 // DQ (4/27/2019): Address at least this specific case of a SgClassDefinition (should include case of SgTemplateClassDefinition).
                 // SgTemplateClassDefinition
                    SgClassDefinition * classDefinition = isSgClassDefinition(originallyDeclaredInitializedName->get_parent());
                    if (classDefinition != NULL)
                       {
                         associatedDeclaration = classDefinition->get_declaration();
                         ASSERT_not_null(associatedDeclaration);
                       }
                  }

            // DQ (6/27/2019): Added more debugging support.
               if (associatedDeclaration == NULL)
                  {
                    mfprintf(mlog [ WARN ] ) ("Note: unexpected IR node: originallyDeclaredInitializedName->get_parent() = %p = %s \n",
                         originallyDeclaredInitializedName->get_parent(),originallyDeclaredInitializedName->get_parent()->class_name().c_str());
                    mfprintf(mlog [ WARN ] ) (" --- originallyDeclaredInitializedName->get_name() = %s \n",originallyDeclaredInitializedName->get_name().str());
                    originallyDeclaredInitializedName->get_file_info()->display("unexpected IR node");
                  }
               ASSERT_not_null(associatedDeclaration);

            // Reuse the previously computed currentScope.
            // This is not always the correct current scope (see test2011_70.C for an example).
               ASSERT_not_null(currentScope);
#if DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("SgInitializedName: name: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("================ Calling nameQualificationDepthForType to evaluate the type \n");
#endif
            // Compute the depth of name qualification from the current statement:  variableDeclaration.
               int amountOfNameQualificationRequiredForName = nameQualificationDepth(initializedName,currentScope,associatedDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
               mfprintf(mlog [ WARN ] ) ("SgInitializedName: name: amountOfNameQualificationRequiredForName = %d \n",amountOfNameQualificationRequiredForName);
#endif

            // DQ (4/26/2019): Call this directly for the name qualificaiton of the SgInitializedName.
               bool skipGlobalNameQualification = false;
               setNameQualificationOnName(initializedName,associatedDeclaration,amountOfNameQualificationRequiredForName,skipGlobalNameQualification);
             }

#if DEBUG_INITIALIZED_NAME
          mfprintf(mlog [ WARN ] ) ("######################################################################## \n");
          mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: DONE: Processing the SgInitializedName IR's name \n");
          mfprintf(mlog [ WARN ] ) ("######################################################################## \n");
#endif
            // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

#if DEBUG_INITIALIZED_NAME && 0
          printf("333333333333333333333333333333333333333333333333333333333333333333333333333 \n");
          printf("333333333333333333333333333333333333333333333333333333333333333333333333333 \n");
          printf("333333333333333333333333333333333333333333333333333333333333333333333333333 \n");
          printf("333333333333333333333333333333333333333333333333333333333333333333333333333 \n");
          printf("333333333333333333333333333333333333333333333333333333333333333333333333333 \n");
#endif

       // DQ (4/28/2019): Trying to support the initializer here, so that we can support constructor
       // preinitialization lists, rather than through the SgConstructor initializer.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("initializedName->get_preinitialization() = %d \n",initializedName->get_preinitialization());
#endif
       // DQ (12/8/2019): If this is a simple data member then we don't need anme qualification on its type (which does not appear in the source code).
          bool is_simple_data_member = false;
          if (initializedName->get_preinitialization() == SgInitializedName::e_data_member)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("Found a data member used in pre-initialization list \n");
#endif
               is_simple_data_member = true;
             }

       // DQ (4/26/2019): The initializer should be processed as an expression to be name qualified seperately.
#if DEBUG_INITIALIZED_NAME
          mfprintf(mlog [ WARN ] ) ("############################################# \n");
          mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: Check for initializer \n");
          mfprintf(mlog [ WARN ] ) ("############################################# \n");
#endif
            // DQ (12/17/2013): Added support for name qualification of preinitialization list elements (see test codes: test2013_285-288.C).
               if (initializedName->get_initptr() != NULL && is_simple_data_member == false)
                  {
                 // DQ (2/7/2019): I think this can't be a SgPointerMemberType, so the code specific to this case does not go here.
                 // ROSE_ASSERT(isSgPointerMemberType(initializedName->get_type()) == NULL);
#if 0
                    mfprintf(mlog [ WARN ] ) ("Case of SgInitializedName: Commented out assertion: testing test2019_122.C \n");
#endif
#if DEBUG_INITIALIZED_NAME
                    mfprintf(mlog [ WARN ] ) ("############################################################################## \n");
                    mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: Processing the SgInitializedName IR node's initializer \n");
                    mfprintf(mlog [ WARN ] ) ("############################################################################## \n");
#endif
                 // DQ (4/28/2019): Added this variable declaration to support compiling this section that was previously commented out.
                    SgStatement* associatedStatement = currentScope;
#if DEBUG_INITIALIZED_NAME
                    mfprintf(mlog [ WARN ] ) ("initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
                    mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializedName->get_initptr());
                    if (constructorInitializer != NULL)
                       {
#if DEBUG_INITIALIZED_NAME
                         mfprintf(mlog [ WARN ] ) ("######################################################## \n");
                         mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: (constructorInitializer != NULL) \n");
                         mfprintf(mlog [ WARN ] ) ("######################################################## \n");
#endif
                      // DQ (12/8/2019): Note that "type" is a variable declared above and we don't what to hide that variable.
                      // SgType* type = initializedName->get_type();
                      // SgType* type = constructorInitializer->get_type();
                         SgType* constructorInitializer_type = constructorInitializer->get_type();
                         ASSERT_not_null(constructorInitializer_type);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("Test for special case of SgInitializedName used in SgCtorInitializerList: type = %p = %s \n",type,type->class_name().c_str());
#endif
                         SgFunctionType*        constructorInitializer_functionType       = isSgFunctionType(constructorInitializer_type);
                         SgMemberFunctionType*  constructorInitializer_memberFunctionType = isSgMemberFunctionType(constructorInitializer_type);
                         SgCtorInitializerList* ctor                                      = isSgCtorInitializerList(currentStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("Test for special case of SgInitializedName used in SgCtorInitializerList: ctor = %p functionType = %p memberFunctionType = %p \n",ctor,constructorInitializer_functionType,constructorInitializer_memberFunctionType);
#endif
                      // if (ctor != NULL)
                         if (ctor != NULL && (constructorInitializer_functionType != NULL || constructorInitializer_memberFunctionType != NULL))
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Calling setNameQualificationOnName() (operating DIRECTLY on the SgInitializedName) \n");
#endif
#if DEBUG_INITIALIZED_NAME
                              mfprintf(mlog [ WARN ] ) ("############################################################################################## \n");
                              mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: (ctor != NULL && (functionType != NULL || memberFunctionType != NULL)) \n");
                              mfprintf(mlog [ WARN ] ) ("############################################################################################## \n");
#endif

                           // DQ (2/2/2019): NOTE: constructorInitializer->get_declaration() == NULL when there is not associated
                           // constructor for the class (e.g. the case where the default constructor (compiler generated) is used).
                           // DQ (1/13/2014): This only get's qualification when the name being used matches the class name,
                           // else this is a data member and should not be qualified.  See test2014_01.C.
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(constructorInitializer->get_declaration());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Test for special case of SgInitializedName used in SgCtorInitializerList: functionDeclaration = %p \n",functionDeclaration);
#endif
                           // DQ (2/2/2019): This is non-null for all but EDG 5.0, so this is debugging support.
                              if (functionDeclaration == NULL)
                                 {
#if DEBUG_INITIALIZED_NAME
                                   mfprintf(mlog [ WARN ] ) ("############################################################################################################################### \n");
                                   mfprintf(mlog [ WARN ] ) ("Case SgInitializedName: SKIPPING CALL TO setNameQualificationOnName(): functionDeclaration == NULL \n");
                                   mfprintf(mlog [ WARN ] ) ("############################################################################################################################### \n");
#endif
                                 }
                                else
                                 {
                                // DQ (2/2/2019): Original code. Works for all but EDG 5.0.
                                   SgName functionName = functionDeclaration->get_name();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_INITIALIZED_NAME
                                   mfprintf(mlog [ WARN ] ) ("Test for special case of SgInitializedName used in SgCtorInitializerList: functionName = %s \n",functionName.str());
                                   mfprintf(mlog [ WARN ] ) ("Test for special case of SgInitializedName used in SgCtorInitializerList: initializedName->get_name() = %s \n",initializedName->get_name().str());
#endif
#if DEBUG_INITIALIZED_NAME
                                   mfprintf(mlog [ WARN ] ) ("@@@@@ currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif

#if DEBUG_INITIALIZED_NAME
                                   printf("444444444444444444444444444444444444444444444444444444444444444444444444444 \n");
                                   printf("444444444444444444444444444444444444444444444444444444444444444444444444444 \n");
                                   printf("444444444444444444444444444444444444444444444444444444444444444444444444444 \n");
                                   printf("444444444444444444444444444444444444444444444444444444444444444444444444444 \n");
                                   printf("444444444444444444444444444444444444444444444444444444444444444444444444444 \n");
#endif
#if DEBUG_INITIALIZED_NAME
                                   printf ("Case of SgInitializedName: functionDeclaration = %p = %s name = %s \n",
                                        functionDeclaration,functionDeclaration->class_name().c_str(),SageInterface::get_name(functionDeclaration).c_str());
                                   printf ("Case of SgInitializedName: currentScope = %p = %s name = %s \n",currentScope,
                                        currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
                                   printf ("Case of SgInitializedName: associatedStatement = %p = %s name = %s \n",associatedStatement,
                                        associatedStatement->class_name().c_str(),SageInterface::get_name(associatedStatement).c_str());
#endif
#if DEBUG_INITIALIZED_NAME
                                   printf ("6666666666666666666666666666666666666666666666666666666666666666666666666 \n");
                                   printf ("6666666666666666666666666666666666666666666666666666666666666666666666666 \n");
                                   printf ("Handling specific case of SgInitializedName from SgCtorInitializationList \n");
                                   printf ("6666666666666666666666666666666666666666666666666666666666666666666666666 \n");
                                   printf ("6666666666666666666666666666666666666666666666666666666666666666666666666 \n");
#endif
                                // DQ (9/3/2020): This is I think the only meaningful change to address Cxx11_tests/test2020_89.C.
                                // DQ (9/2/2020): If this is a SgInitializedName from a SgCtorInitializationList, then we want to search for the
                                // class associated with the member function in the parent of the scope of the class definition.
                                // However, this code might be too specific to this narrow case (something to look at in the morning).
                                   SgClassDefinition* classDefinition = isSgClassDefinition(functionDeclaration->get_scope());
                                   ROSE_ASSERT(classDefinition != NULL);
                                   SgDeclarationStatement* associatedDeclaration = isSgClassDeclaration(classDefinition->get_declaration());
                                   ROSE_ASSERT(associatedDeclaration != NULL);

                                // DQ (9/2/2020): use the scope of the current scope.
                                   currentScope = currentScope->get_scope();
                                   int amountOfNameQualificationRequiredForType = nameQualificationDepth(associatedDeclaration,currentScope,associatedStatement);
                                   if (initializedName->get_name() == functionName)
                                      {

                                     // DQ (10/18/2020): Moved declaration to where it is being used.
                                        SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
                                        ROSE_ASSERT(declaration != NULL);

                                     // DQ (4/28/2019): Added variable to allow this section to be compiled.
                                        bool skipGlobalNameQualification = false;
                                        setNameQualificationOnName(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);

                                     // DQ (3/31/2019): Uncomment this to trigger review because I now think we should be calling setNameQualification()
                                     // instead of setNameQualificationOnName().  Because setNameQualificationOnName() should use the new name qualification
                                     // fields for the SgInitializedName instead of the fields for the SgInitializedName's type.
                                      }
                                 }
                            }
                       }
                  }


            // DQ (4/26/2019): The initializer should be processed as an expression to be name qualified seperately.
            // DQ (10/18/2020): Moved declaration to where it is being used.
               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());

            // DQ (10/18/2020): Only insert into the referencedNameSet if this is a declaration that we have seen, must be non-null.
            // DQ (8/4/2012): Isolate that handling of the referencedNameSet from the use of skipGlobalNameQualification so that we can debug (test2012_96.C).
               if ((declaration != NULL) && (referencedNameSet.find(declaration) == referencedNameSet.end()))
                  {
                 // No qualification is required but we do want to count this as a reference to the class.
                    referencedNameSet.insert(declaration);
                  }
        }



  // Handle references to SgFunctionDeclaration...
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);

  // DQ (6/4/2011): Avoid processing as both member and non-member function...
  // if (functionDeclaration != NULL)
     if (functionDeclaration != NULL && isSgMemberFunctionDeclaration(n) == NULL)
        {
          SgScopeStatement* currentScope = isSgScopeStatement(functionDeclaration->get_parent());
#if 0
          mfprintf(mlog [ WARN ] ) ("Case of (functionDeclaration != NULL && isSgMemberFunctionDeclaration(n) == NULL): currentScope = %p \n",currentScope);
#endif

       // DQ (11/18/2017): When the parent is not a scope, it could be a SgTemplateInstantiationDirectiveStatement, in which
       // case we want the parent of that. See test2017_66.C (and previously test2006_08.C) for an example of this case.
          if (currentScope == NULL)
             {
               SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement = isSgTemplateInstantiationDirectiveStatement(functionDeclaration->get_parent());
               if (templateInstantiationDirectiveStatement != NULL)
                  {
                    currentScope = isSgScopeStatement(templateInstantiationDirectiveStatement->get_parent());
#if 0
                 // DQ (4/20/2018): Added debugging support.
                    mfprintf(mlog [ WARN ] ) ("In name qualification support: processing SgFunctionDeclaration (non-member): found SgTemplateInstantiationDirectiveStatement \n");
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Case of (functionDeclaration != NULL && isSgMemberFunctionDeclaration(n) == NULL): reset using SgTemplateInstantiationDirectiveStatement: currentScope = %p \n",currentScope);
#endif
                 // Now we should have a valid currentScope.
                    ASSERT_not_null(currentScope);
                  }
             }

#if 0
          mfprintf(mlog [ WARN ] ) ("currentScope = %p \n",currentScope);
#endif

       // ASSERT_not_null(currentScope);
          if (currentScope != NULL)
             {
            // Handle the function return type...
               ASSERT_not_null(functionDeclaration->get_orig_return_type());
               ASSERT_not_null(functionDeclaration->get_type());
               ASSERT_not_null(functionDeclaration->get_type()->get_return_type());
               SgType* returnType = functionDeclaration->get_type()->get_return_type();
               ASSERT_not_null(returnType);

               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(returnType);
               if (declaration != NULL)
                  {
                    int amountOfNameQualificationRequiredForReturnType = nameQualificationDepth(declaration,currentScope,functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgFunctionDeclaration's return type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForReturnType);
                    mfprintf(mlog [ WARN ] ) ("Putting the name qualification for the type into the return type of SgFunctionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());
#endif
                 // setNameQualificationReturnType(functionDeclaration,amountOfNameQualificationRequiredForReturnType);
                    setNameQualificationReturnType(functionDeclaration,declaration,amountOfNameQualificationRequiredForReturnType);
                  }
                 else
                  {
                 // This case is common for builtin functions such as: __builtin_powi
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("declaration == NULL: could not put name qualification for the type into the return type of SgFunctionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());
#endif
                  }

            // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
               traverseType(returnType,functionDeclaration,currentScope,functionDeclaration);

            // Handle the function name...
            // DQ (6/20/2011): Friend function can be qualified...sometimes...
            // if (functionDeclaration->get_declarationModifier().isFriend() == true || functionDeclaration->get_specialFunctionModifier().isOperator() == true)
               if (functionDeclaration->get_specialFunctionModifier().isOperator() == true)
                  {
                 // DQ (6/19/2011): We sometimes have to qualify friends if it is to avoid ambiguity (see test2006_159.C) (but we never qualify an operator, I think).
                 // Maybe a friend declaration should add an SgAliasSymbol to the class definition scope's symbol table.
                 // Then simpler rules (no special case) would cause the name qualification to be generated properly.

                 // Old comment
                 // Never use name qualification for friend functions or operators. I am more sure of the case of friend functions than operators.
                 // Friend functions will have a global scope (though this might change in the future; google "friend global scope injection").
                 // mfprintf(mlog [ WARN ] ) ("Detected a friend or operator function, these are not provided with name qualification. \n");
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Detected a operator function, these are not provided with name qualification. \n");
#endif
                  }
                 else
                  {
                 // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same
                 // as the scope of the function declaration.  However, the analysis should work and determin that the
                 // required name qualification length is zero.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("I would like to not have to have this SgFunctionDeclaration logic, we should get the name qualification correct more directly. \n");
#endif
                 // DQ (6/20/2011): Friend function can be qualified and a fix to add a SgAliasSymbol to the class definition scope's symbol table
                 // should allow it to be handled with greater precission.
                 // DQ (6/25/2011): Friend functions can require global qualification as well (see test2011_106.C).
                 // Not clear how to handle this case.
                    if (functionDeclaration->get_declarationModifier().isFriend() == true)
                       {
                      // This is the case of a friend function declaration which requires more name qualification than expected.
                      // Note that this might be compiler dependent but at least GNU g++ required more qualification than expected.
                         SgScopeStatement* scope = functionDeclaration->get_scope();
                         SgGlobal* globalScope = isSgGlobal(scope);
                         if (globalScope != NULL)
                            {
                           // We want to specify global qualification when the friend function is in global scope (see test2011_106.C).
                           // DQ (6/25/2011): This will output the name qualification correctly AND cause the output of the outlined function to be supressed.
                              int amountOfNameQualificationRequired = 0;

                           // Check if this function declaration has been seen already...
                           // if (functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration())

                           // This is the same code as below so it could be refactored (special handling for function declarations
                           // that are defining declaration and don't have an associated nondefining declaration).

                           // DQ (8/4/2012): This is a case using the referencedNameSet that should be refactored out of this location.
                              SgDeclarationStatement* declarationForReferencedNameSet = functionDeclaration->get_firstNondefiningDeclaration();

                              if (declarationForReferencedNameSet == NULL)
                                 {
                                // Note that a function with only a defining declaration will not have a nondefining declaration
                                // automatically constructed in the AST (unlike classes and some onther sorts of declarations).
                                   declarationForReferencedNameSet = functionDeclaration->get_definingDeclaration();

                                // DQ (6/22/2011): I think this is true.  This assertion fails for test2006_78.C (a template example code).
                                // ROSE_ASSERT(declarationForReferencedNameSet == declaration);

                                // DQ (6/23/2011): This assertion fails for the LoopProcessor on tests/nonsmoke/functional/roseTests/loopProcessingTests/mm.C
                                // ASSERT_not_null(declarationForReferencedNameSet);
                                   if (declarationForReferencedNameSet == NULL)
                                      {
                                        declarationForReferencedNameSet = functionDeclaration;
                                        ASSERT_not_null(declarationForReferencedNameSet);
                                      }
                                   ASSERT_not_null(declarationForReferencedNameSet);
                                 }
                              ASSERT_not_null(declarationForReferencedNameSet);

                           // DQ (8/4/2012): We would like to refactor this code (I think).
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                              mfprintf(mlog [ WARN ] ) ("Name qualification for SgFunctionDeclaration: I think this should be using the defined function xxx so that we isolate references to the referencedNameSet \n");
#endif
                              if (referencedNameSet.find(declarationForReferencedNameSet) == referencedNameSet.end())
                                 {
                                // No global qualification is required.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("No qualification should be used for this friend function. \n");
#endif
                                 }
                                else
                                 {
                                   amountOfNameQualificationRequired = 1;
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("Force global qualification for friend function: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                                 }
                              setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
                            }
                           else
                            {
                           // Not clear what to do with this case, I guess we just want standard qualification rules.
                              int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
                              setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
                            }
                       }
                      else
                       {
                      // DQ (3/31/2018): Added assertion.
                         ASSERT_not_null(functionDeclaration->get_scope());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("currentScope = %p functionDeclaration->get_scope() = %p \n",currentScope,functionDeclaration->get_scope());
                         mfprintf(mlog [ WARN ] ) ("functionDeclaration->get_scope() = %s \n",functionDeclaration->get_scope()->class_name().c_str());
#endif
                      // Case of non-member functions (more logical name qualification rules).
                         if (currentScope != functionDeclaration->get_scope())
                            {
                           // DQ (1/21/2013): Added support for testing the more general equivalence of scopes (where the pointers are not equal, applies only to namespaces, I think).
                              bool isSameNamespace = SgScopeStatement::isEquivalentScope(currentScope,functionDeclaration->get_scope());
#if 0
                              mfprintf(mlog [ WARN ] ) ("isSameNamespace = %s \n",isSameNamespace ? "true" : "false");
#endif
                              if (isSameNamespace == false)
                                 {
                                   int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                                   mfprintf(mlog [ WARN ] ) ("isSameNamespace == false: SgFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                                // DQ (21/2011): test2011_89.C demonstrates a case where name qualification of a functionRef expression is required.
                                // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in
                                // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
                                // definition could not live in the SgBasicBlock.
                                   bool skipNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("isSameNamespace == false: Test of functionDeclaration: skipNameQualification = %s \n",skipNameQualification ? "true" : "false");
#endif
                                   if (skipNameQualification == false)
                                      {
                                        setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
                                      }
                                 }
                                else
                                 {
#if 0
                                   printf ("isSameNamespace == true: functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());
#endif
                                // DQ (3/31/2018): Note that we still might require name qualification on any template arguments in the template function instantiation.
                                // Ignore the case fo a SgTemplateFunctionDeclaration.
                                   if (isSgTemplateInstantiationFunctionDecl(functionDeclaration) != NULL)
                                      {
                                     // This point of calling this function is to just have the template arguments evaluated for name qualification (see Cxx11_tests/test2018_68.C).
                                        int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);

                                     // Add this to make sure that amountOfNameQualificationRequired is referenced to avoid a compiler warning.
                                        ROSE_ASSERT(amountOfNameQualificationRequired >= 0);
                                      }
                                 }
                            }
                           else
                            {
                              SgTemplateInstantiationFunctionDecl* templateFunction = isSgTemplateInstantiationFunctionDecl(functionDeclaration);
                              if (templateFunction != NULL)
                                 {
                                   nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
                                 }
                            }
                       }
                  }
            // DQ (4/14/2018): Add the name qualification computation to the parameterList_syntax (since it will be used by preference in the unparser).
            // generateNestedTraversalWithExplicitScope( SgNode* node, SgScopeStatement* input_currentScope )
               if (functionDeclaration->get_type_syntax_is_available() == true)
                  {
                 // DQ (4/20/2018): This is an error reported by Charles, but in a reproducer testcode that does
                 // not generate the error for me.  I expect that it might be an issue of not recompiling the
                 // build tree after the header files have been changed between versions that fixed a previous
                 // bug (unrelated) and was checked in recently. I prefer the assertion, but I will remove it and
                 // support a conditional check for now (before I leave on vacation).
                 // ASSERT_not_null(functionDeclaration->get_parameterList_syntax());
                 // generateNestedTraversalWithExplicitScope(functionDeclaration->get_parameterList_syntax(),currentScope);
                    if (functionDeclaration->get_parameterList_syntax() != NULL)
                       {
                         generateNestedTraversalWithExplicitScope(functionDeclaration->get_parameterList_syntax(),currentScope);
                       }
                  }
             }
            else
             {
            // Note that test2005_57.C presents an example that triggers this case and so might be a relevant
            // test code.  Example: "template<typename T> void foobar (T x){ }".
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("WARNING: SgFunctionDeclaration -- currentScope is not available, not clear why! \n");
#endif
             }
        }

  // Handle references to SgMemberFunctionDeclaration...
     SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(n);
     if (memberFunctionDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
        // DQ (9/7/2014): Added debugging to verify that this case is supporting name qualification of SgTemplateMemberFunctionDeclaration IR node.
           if (isSgTemplateMemberFunctionDeclaration(memberFunctionDeclaration) != NULL)
              {
                mfprintf(mlog [ WARN ] ) ("Note: This case supports SgTemplateMemberFunctionDeclaration as well: memberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
             // ROSE_ASSERT(false);
              }
#endif

       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(memberFunctionDeclaration->get_parent());
#if 0
          mfprintf(mlog [ WARN ] ) ("case SgMemberFunctionDeclaration: currentScope = %p \n",currentScope);
#endif

       // DQ (4/20/2018): Added new code to support where member functions are used in SgTemplateInstantiationDirectiveStatement.
       // DQ (4/20/2018): When the parent is not a scope, it could be a SgTemplateInstantiationDirectiveStatement, in which
       // case we want the parent of that. See test2017_66.C (and previously test2006_08.C) for an example of this case.
          if (currentScope == NULL)
             {
               SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement = isSgTemplateInstantiationDirectiveStatement(memberFunctionDeclaration->get_parent());
               if (templateInstantiationDirectiveStatement != NULL)
                  {
                    currentScope = isSgScopeStatement(templateInstantiationDirectiveStatement->get_parent());
#if 0
                 // DQ (4/20/2018): Added debugging support.
                    mfprintf(mlog [ WARN ] ) ("In name qualification support: processing SgMemberFunctionDeclaration: found SgTemplateInstantiationDirectiveStatement \n");
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Case of (memberFunctionDeclaration != NULL): reset using SgTemplateInstantiationDirectiveStatement: currentScope = %p \n",currentScope);
#endif
                 // Now we should have a valid currentScope.
                    ASSERT_not_null(currentScope);
                  }
             }

#if 0
          mfprintf(mlog [ WARN ] ) ("currentScope = %p \n",currentScope);
#endif

       // ASSERT_not_null(currentScope);
          if (currentScope != NULL)
             {
            // Handle the function return type...
               ASSERT_not_null(memberFunctionDeclaration->get_orig_return_type());
               ASSERT_not_null(memberFunctionDeclaration->get_type());
               ASSERT_not_null(memberFunctionDeclaration->get_type()->get_return_type());
               SgType* returnType = memberFunctionDeclaration->get_type()->get_return_type();
               ASSERT_not_null(returnType);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("case SgMemberFunctionDeclaration: returnType = %p = %s = %s \n",returnType,returnType->class_name().c_str(),returnType->unparseToString().c_str());
               SgType* return_syntax_type = NULL;
            // DQ (2/25/2019): Use the type syntax when it is available.
               if (memberFunctionDeclaration->get_type_syntax_is_available() == true)
                  {
                    mfprintf(mlog [ WARN ] ) ("case SgMemberFunctionDeclaration: Using the type_syntax since it is available: memberFunctionDeclaration->get_type_syntax() = %p \n",
                         memberFunctionDeclaration->get_type_syntax());
                    SgFunctionType* functionType = isSgFunctionType(memberFunctionDeclaration->get_type_syntax());
                    ASSERT_not_null(functionType);
                 // return_syntax_type = memberFunctionDeclaration->get_type_syntax();
                    if (functionType->get_orig_return_type() != NULL)
                       {
                         return_syntax_type = functionType->get_orig_return_type();
                       }
                      else
                       {
                         return_syntax_type = functionType->get_return_type();
                       }
                    ASSERT_not_null(return_syntax_type);
                  }

               if (return_syntax_type != NULL)
                  {
                    mfprintf(mlog [ WARN ] ) ("case SgMemberFunctionDeclaration: return_syntax_type = %p = %s = %s \n",return_syntax_type,return_syntax_type->class_name().c_str(),return_syntax_type->unparseToString().c_str());
                  }
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("case SgMemberFunctionDeclaration: returnType = %p = %s \n",returnType,returnType->class_name().c_str());
               SgTemplateType* template_returnType = isSgTemplateType(returnType);
               if (template_returnType != NULL)
                  {
                    mfprintf(mlog [ WARN ] ) ("template_returnType                                    = %p \n",template_returnType);
                    mfprintf(mlog [ WARN ] ) ("template_returnType->get_name()                        = %s \n",template_returnType->get_name().str());
                    mfprintf(mlog [ WARN ] ) ("template_returnType->get_template_parameter_position() = %d \n",template_returnType->get_template_parameter_position());

                  }
               mfprintf(mlog [ WARN ] ) ("   --- memberFunctionDeclaration->get_firstNondefiningDeclaration() = %p = %s \n",memberFunctionDeclaration->get_firstNondefiningDeclaration(),memberFunctionDeclaration->get_firstNondefiningDeclaration()->class_name().c_str());
               if (memberFunctionDeclaration->get_definingDeclaration() != NULL)
                  {
                    mfprintf(mlog [ WARN ] ) ("   --- memberFunctionDeclaration->get_definingDeclaration() = %p = %s \n",memberFunctionDeclaration->get_definingDeclaration(),memberFunctionDeclaration->get_definingDeclaration()->class_name().c_str());
                  }
               mfprintf(mlog [ WARN ] ) ("memberFunctionDeclaration->get_type() = %p = %s \n",memberFunctionDeclaration->get_type(),memberFunctionDeclaration->get_type()->class_name().c_str());
#endif

               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(returnType);
               if (declaration != NULL)
                  {
                    int amountOfNameQualificationRequiredForReturnType = nameQualificationDepth(declaration,currentScope,memberFunctionDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgMemberFunctionDeclaration's return type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForReturnType);
                    mfprintf(mlog [ WARN ] ) ("Putting the name qualification for the type into the return type of SgMemberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
#endif
                    setNameQualificationReturnType(memberFunctionDeclaration,declaration,amountOfNameQualificationRequiredForReturnType);
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("declaration == NULL: could not put name qualification for the type into the return type of SgMemberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
#endif
                  }

#if 0
               mfprintf(mlog [ WARN ] ) ("Calling traverseType on SgMemberFunctionDeclaration = %p = %s name = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),memberFunctionDeclaration->get_name().str());
#endif
            // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
               traverseType(returnType,memberFunctionDeclaration,currentScope,memberFunctionDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Don't forget possible covariant return types for SgMemberFunctionDeclaration IR nodes \n");

            // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same
            // as the scope of the function declaration.  However, the analysis should work and determin that the
            // required name qualification length is zero.
               mfprintf(mlog [ WARN ] ) ("I would like to not have to have this SgMemberFunctionDeclaration logic, we should get the name qualification correct more directly. \n");
               mfprintf(mlog [ WARN ] ) ("   --- memberFunctionDeclaration->get_scope() = %p = %s \n",memberFunctionDeclaration->get_scope(),memberFunctionDeclaration->get_scope()->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("   --- currentScope                           = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
               if (currentScope != memberFunctionDeclaration->get_scope())
                  {
                 // DQ (1/21/2013): Note that the concept of equivalent scope is fine here if it only tests the equivalence of the pointers.
                 // We can't have member functions in namespaces so we don't require that more general test for scope equivalence.

                    int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgMemberFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                    setNameQualification(memberFunctionDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                 // DQ (9/7/2014): This branch is taken by the non-defining template member functions defined outside of their
                 // associated template class declarations. There are also other cases where this branch is taken.

                 // Don't know what test code exercises this case (see test2005_73.C).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                    mfprintf(mlog [ WARN ] ) ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available through predicate (currentScope != memberFunctionDeclaration->get_scope()), not clear why! \n");
                    mfprintf(mlog [ WARN ] ) ("   --- memberFunctionDeclaration->get_scope() = %p = %s \n",memberFunctionDeclaration->get_scope(),memberFunctionDeclaration->get_scope()->class_name().c_str());
                    mfprintf(mlog [ WARN ] ) ("   --- currentScope                           = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                  }

            // DQ (4/14/2018): Add the name qualification computation to the parameterList_syntax (since it will be used by preference in the unparser).
            // generateNestedTraversalWithExplicitScope( SgNode* node, SgScopeStatement* input_currentScope )
               if (memberFunctionDeclaration->get_type_syntax_is_available() == true)
                  {
#if 0
                    mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@ Process the member function parameter syntax @@@@@@@@@@@@@@@@@@ \n");
                    mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
                    ASSERT_not_null(memberFunctionDeclaration->get_parameterList_syntax());
                    generateNestedTraversalWithExplicitScope(memberFunctionDeclaration->get_parameterList_syntax(),currentScope);
#if 0
                    mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@######@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@ DONE: Process the member function parameter syntax @@@@@@@@@@@@@@@@@@ \n");
                    mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@######@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
                  }
             }
            else
             {
            // Note that test2005_63.C presents an example that triggers this case and so might be a relevant.
            // This is also the reason why test2005_73.C is failing!!!  Fix it tomorrow!!! (SgTemplateInstantiationDirectiveStatement)
               SgDeclarationStatement* currentStatement = isSgDeclarationStatement(memberFunctionDeclaration->get_parent());

            // DQ (9/4/2014): Lambda functions (in SgLambdaExp) are an example where this fails.
            // ASSERT_not_null(currentStatement);
               if (currentStatement != NULL)
                  {
                    SgScopeStatement* currentScope = isSgScopeStatement(currentStatement->get_parent());
                    if (currentScope != NULL)
                       {
                         int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("SgMemberFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                         setNameQualification(memberFunctionDeclaration,amountOfNameQualificationRequired);

                      // DQ (4/14/2018): Add the name qualification computation to the parameterList_syntax (since it will be used by preference in the unparser).
                      // generateNestedTraversalWithExplicitScope( SgNode* node, SgScopeStatement* input_currentScope )
                         if (memberFunctionDeclaration->get_type_syntax_is_available() == true)
                            {
                           // DQ (4/14/2018): I can't detect that we have any test codes that reach here!
                           // This might be a subject to ingestigate later.
#if 0
                              mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                              mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@ Process the member function parameter syntax: scope computed from parent @@@@@@@@@@@@@@@@@@ \n");
                              mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
                              ASSERT_not_null(memberFunctionDeclaration->get_parameterList_syntax());
                              generateNestedTraversalWithExplicitScope(memberFunctionDeclaration->get_parameterList_syntax(),currentScope);
#if 0
                              mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@######@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                              mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@ DONE: Process the member function parameter syntax: scope computed from parent @@@@@@@@@@@@@@@@@@ \n");
                              mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@######@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
                            }
                       }
                      else
                       {
                         mfprintf(mlog [ WARN ] ) ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available through parent SgDeclarationStatement, not clear why! \n");
                         ROSE_ABORT();
                       }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available, not clear why! \n");
#endif
                 // ROSE_ASSERT(false);
                  }
                 else
                  {
                 // This should only be a lambda function defined in a SgLambdaExp.
                    ASSERT_not_null(isSgLambdaExp(memberFunctionDeclaration->get_parent()));
                  }
             }
        }

  // DQ (6/3/2017): Since the underling template instantiation is not shared, and becasue it is traversed explicitly,
  // We can (I think) reserve the name qualification of the template instantiation to the instantiated template function
  // directly and need not support an additional (redundant) evaluation of name qualification here.

  // DQ (4/3/2014): Added new case to address no longer traversing this IR node's member.
  // See test2005_73.C.
     SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement = isSgTemplateInstantiationDirectiveStatement(n);
     if (templateInstantiationDirectiveStatement != NULL)
        {
       // DQ (4/3/2014): We no longer traverse the declaration referenced in this IR node so we
       // have to handle the name qualification requiredments for the associated declaration directly.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("################ Processing SgTemplateInstantiationDirectiveStatement (name qualification is handled within the nested template instantiation) \n");
#endif
        }

  // DQ (5/14/2011): Added support for the name qualification of the base type used in typedefs.
  // Handle references to SgTypedefDeclaration...
     SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(n);
     if (typedefDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

#define DEBUG_TYPEDEF (DEBUG_NAME_QUALIFICATION_LEVEL > 3)

       // We need the structural location in scope (not the semantic one).
       // SgScopeStatement* currentScope = isSgScopeStatement(typedefDeclaration->get_parent());
          SgScopeStatement* currentScope = typedefDeclaration->get_scope();
          ASSERT_not_null(currentScope);

          SgType* baseType = typedefDeclaration->get_base_type();
          ASSERT_not_null(baseType);
          SgDeclarationStatement* baseTypeDeclaration = associatedDeclaration(baseType);

       // DQ (4/10/2019): Handle the case when this is a typedef of a SgPointrMemberType.
          SgDeclarationStatement* pointerMemberClassDeclaration = NULL;

          SgPointerMemberType* pointerMemberType = isSgPointerMemberType(baseType);
          if (pointerMemberType != NULL)
             {
#if DEBUG_TYPEDEF
               mfprintf(mlog [ WARN ] ) ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
               mfprintf(mlog [ WARN ] ) ("Found a SgPointerMemberType in the base type of a SgTypedefDeclaration \n");
               mfprintf(mlog [ WARN ] ) ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");

               mfprintf(mlog [ WARN ] ) ("We need to make a recursive type traversal for this case! \n");
#endif

#if DEBUG_TYPEDEF
               mfprintf(mlog [ WARN ] ) ("############################################################################# \n");
               mfprintf(mlog [ WARN ] ) ("Case SgTypedefDeclaration: Calling generateNestedTraversalWithExplicitScope() \n");
               mfprintf(mlog [ WARN ] ) ("############################################################################# \n");
#endif
               generateNestedTraversalWithExplicitScope(baseType,currentScope,typedefDeclaration,typedefDeclaration);

            // DQ (4/19/2019): It might be that we should call this after the traveral over each type instead of before we traverse the type.
            // This way we save the correctly computed string for each type after the different parts of name qualificaiton are in place.
               traverseType(baseType,typedefDeclaration,currentScope,typedefDeclaration);

#if DEBUG_TYPEDEF
               mfprintf(mlog [ WARN ] ) ("####################################################################################################### \n");
               mfprintf(mlog [ WARN ] ) ("Case SgTypedefDeclaration: DONE: Processing the recursive evaluation of the SgInitializedName IR's type \n");
               mfprintf(mlog [ WARN ] ) ("####################################################################################################### \n");
#endif
             }
            else
             {
            // DQ (4/28/2019): (refactored) Put the rest of the non-SgPointerMemberType support into the false block.

            // If the base type is defined in the typedef directly then it should need no name qualification by definition.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() = %s \n",typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() ? "true" : "false");
#endif

            // DQ (4/10/2019): Handle the case when this is a typedef of a SgPointrMemberType.
               if (pointerMemberClassDeclaration != NULL)
                  {
                    int amountOfNameQualificationRequiredOnPointerMemberClass = nameQualificationDepth(pointerMemberClassDeclaration,currentScope,typedefDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgTypedefDeclaration: amountOfNameQualificationRequiredOnPointerMemberClass = %d \n",amountOfNameQualificationRequiredOnPointerMemberClass);
#endif

                    ASSERT_not_null(pointerMemberClassDeclaration);
                    setNameQualificationOnPointerMemberClass(typedefDeclaration,pointerMemberClassDeclaration,amountOfNameQualificationRequiredOnPointerMemberClass);
                  }

            // This is NULL if the base type is not associated with a declaration (e.g. not a SgNamedType).
            // ASSERT_not_null(baseTypeDeclaration);
            // if (baseTypeDeclaration != NULL)
               if ( (baseTypeDeclaration != NULL) && (typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() == false) )
                  {
                 // int amountOfNameQualificationRequiredForBaseType = nameQualificationDepth(baseTypeDeclaration,currentScope,typedefDeclaration);
                    int amountOfNameQualificationRequiredForBaseType = nameQualificationDepth(baseType,currentScope,typedefDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgTypedefDeclaration: amountOfNameQualificationRequiredForBaseType = %d \n",amountOfNameQualificationRequiredForBaseType);
#endif
                    ASSERT_not_null(baseTypeDeclaration);
                 // setNameQualification(typedefDeclaration,baseTypeDeclaration,amountOfNameQualificationRequiredForBaseType);
                    setNameQualificationOnBaseType(typedefDeclaration,baseTypeDeclaration,amountOfNameQualificationRequiredForBaseType);
                  }

#if DEBUG_TYPEDEF || 0
            // DQ (2/15/2017): Note that this output will be the way to identify the start of a failing infinite loop
            // for tests/nonsmoke/functional/CompileTests/RoseExample_tests/testRoseHeaders_11.C.
               mfprintf(mlog [ WARN ] ) ("Calling traverseType on SgTypedefDeclaration = %p name = %s \n",typedefDeclaration,typedefDeclaration->get_name().str());
#endif

#if DEBUG_TYPEDEF || 0
               mfprintf(mlog [ WARN ] ) ("typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() = %s \n",
                    typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() ? "true" : "false");
#endif
            // DQ (5/2/2019): check the defining declaration for the associated declaration (see test2019_427.C).
            // For example, a typedef with multiple declarations ("typedef struct {} A,*Aptr;").
            // bool skipTraverseType = typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration();
               SgTypedefDeclaration*   definingTypedefDeclaration = typedefDeclaration;
               SgDeclarationStatement* assocaitedDeclaration = typedefDeclaration->get_declaration();
               SgDeclarationStatement* definingDeclaration = assocaitedDeclaration;
               if (assocaitedDeclaration != NULL)
                  {
                    definingDeclaration = assocaitedDeclaration->get_definingDeclaration();
                 // ASSERT_not_null(definingDeclaration);
                    if (definingDeclaration != NULL)
                       {
                         definingTypedefDeclaration = isSgTypedefDeclaration(definingDeclaration->get_parent());

                      // DQ (5/2/2019): If this is NULL, then use the original version.
                         if (definingTypedefDeclaration == NULL)
                            {
                              definingTypedefDeclaration = typedefDeclaration;
                            }
                       }
                    ASSERT_not_null(definingTypedefDeclaration);
                  }

               bool skipTraverseType = definingTypedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration();
#if 0
               mfprintf(mlog [ WARN ] ) ("222222222222222222222 skipTraverseType = %s \n",skipTraverseType ? "true" : "false");
#endif
            // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
            // traverseType(baseType,typedefDeclaration,currentScope,typedefDeclaration);
               if (skipTraverseType == false)
                  {
                    traverseType(baseType,typedefDeclaration,currentScope,typedefDeclaration);

#if DEBUG_TYPEDEF || 0
                    mfprintf(mlog [ WARN ] ) ("DONE: Calling traverseType on SgTypedefDeclaration = %p name = %s \n",typedefDeclaration,typedefDeclaration->get_name().str());
#endif
                  }
                 else
                  {
#if DEBUG_TYPEDEF || 0
                    mfprintf(mlog [ WARN ] ) ("Skipped call to traverseType for case SgTypedefDeclaration = %p name = %s \n",typedefDeclaration,typedefDeclaration->get_name().str());
#endif
                  }

            // DQ (4/14/2018): Adding support for name qualification of template arguments (though it should not be requirted for the tyepdef directly).
               SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(typedefDeclaration);
               if (templateInstantiationTypedefDeclaration != NULL)
                  {
                 // This point of calling this function is to just have the template arguments evaluated for name qualification (see Cxx11_tests/test2018_68.C).

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    int amountOfNameQualificationRequired = nameQualificationDepth(templateInstantiationTypedefDeclaration,currentScope,templateInstantiationTypedefDeclaration);
                    mfprintf(mlog [ WARN ] ) ("SgTemplateInstantiationTypedefDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

                 // DQ (4/14/2018): Report anything that is unusual, i.e. non-zero name qualification length.
                    if (amountOfNameQualificationRequired > 0)
                       {
                         mfprintf(mlog [ WARN ] ) ("Warning: name qualification length should be zero for a templateInstantiationTypedefDeclaration declared in the same scope: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

                         mfprintf(mlog [ WARN ] ) ("templateInstantiationTypedefDeclaration = %p = %s = %s \n",templateInstantiationTypedefDeclaration,templateInstantiationTypedefDeclaration->class_name().c_str(),functionDeclaration->get_mangled_name().str());
                       }
#endif
                  }
             }
        }


  // Handle references in SgUsingDirectiveStatement...
     SgUsingDirectiveStatement* usingDirective = isSgUsingDirectiveStatement(n);
     if (usingDirective != NULL)
        {
          SgNamespaceDeclarationStatement* namespaceDeclaration = usingDirective->get_namespaceDeclaration();
          ASSERT_not_null(namespaceDeclaration);
          SgScopeStatement* currentScope = usingDirective->get_scope();
          ASSERT_not_null(currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
#endif

          int amountOfNameQualificationRequired = nameQualificationDepth(namespaceDeclaration,currentScope,usingDirective);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("SgUsingDirectiveStatement's SgNamespaceDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
          setNameQualification(usingDirective,namespaceDeclaration,amountOfNameQualificationRequired);
        }

     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(n);
     if (usingDeclaration != NULL)
        {
          SgDeclarationStatement* associatedDeclaration     = usingDeclaration->get_declaration();
          SgInitializedName*      associatedInitializedName = usingDeclaration->get_initializedName();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In case for SgUsingDeclarationStatement: associatedDeclaration = %p associatedInitializedName = %p \n",associatedDeclaration,associatedInitializedName);
          if (associatedDeclaration != NULL)
             mfprintf(mlog [ WARN ] ) ("associatedDeclaration = %p = %s = %s = %s \n",associatedDeclaration,associatedDeclaration->class_name().c_str(),SageInterface::get_name(associatedDeclaration).c_str(),SageInterface::generateUniqueName(associatedDeclaration,true).c_str());
          if (associatedInitializedName != NULL)
             mfprintf(mlog [ WARN ] ) ("associatedInitializedName = %p = %s = %s = %s \n",associatedInitializedName,associatedInitializedName->class_name().c_str(),SageInterface::get_name(associatedInitializedName).c_str(),SageInterface::generateUniqueName(associatedInitializedName,true).c_str());
#endif
          SgScopeStatement* currentScope = usingDeclaration->get_scope();
          ASSERT_not_null(currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
#endif

          int amountOfNameQualificationRequired = 0;
          if (associatedDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("associatedDeclaration != NULL: associatedDeclaration = %p = %s = %s \n",associatedDeclaration,associatedDeclaration->class_name().c_str(),SageInterface::get_name(associatedDeclaration).c_str());
               mfprintf(mlog [ WARN ] ) ("associatedDeclaration != NULL: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               if (currentScope->get_scope() != NULL)
                    mfprintf(mlog [ WARN ] ) ("associatedDeclaration != NULL: currentScope->get_scope() = %p = %s \n",currentScope->get_scope(),currentScope->get_scope()->class_name().c_str());
#endif
               amountOfNameQualificationRequired = depthOfGlobalNameQualification(associatedDeclaration);

            // DQ (1/11/2019): Don't outout added global qualification for the case of a inheriting constructor.
            // DQ (6/22/2011): If the amountOfNameQualificationRequired is zero then add one to force at least global qualification.
            // See test2004_80.C for an example.
            // if (isSgGlobal(currentScope->get_scope()) != NULL && amountOfNameQualificationRequired == 0)
               bool is_inheriting_constructor = usingDeclaration->get_is_inheriting_constructor();
               if (is_inheriting_constructor == false && isSgGlobal(currentScope->get_scope()) != NULL && amountOfNameQualificationRequired == 0)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Handling special case to force at least global qualification. \n");
#endif
                    amountOfNameQualificationRequired += 1;
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("SgUsingDeclarationStatement's associatedDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

            // DQ (1/10/2019): If this is a constructor, member function, then we must qualifiy it to distinquish it from the base class name.
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(associatedDeclaration);
               if (memberFunctionDeclaration != NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Found a member function = %p name = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
#endif
                    if (memberFunctionDeclaration->get_specialFunctionModifier().isConstructor() == true)
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("Found a constructor = %p name = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
#endif
                       }
                  }

               setNameQualification(usingDeclaration,associatedDeclaration,amountOfNameQualificationRequired);
             }
            else
             {
               ASSERT_not_null(associatedInitializedName);
               amountOfNameQualificationRequired = nameQualificationDepth(associatedInitializedName,currentScope,usingDeclaration);

               setNameQualification(usingDeclaration,associatedInitializedName,amountOfNameQualificationRequired);
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("SgUsingDeclarationStatement's SgVarRefExp: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
        }


  // DQ (7/8/2014): Adding support for name qualification of the SgNamespaceDeclarationStatement referenced by a SgNamespaceAliasDeclarationStatement.
  // Handle references in SgNamespaceAliasDeclarationStatement...
     SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(n);
     if (namespaceAliasDeclaration != NULL)
        {
#if 0
          mfprintf(mlog [ WARN ] ) ("In namespace qualification: namespaceAliasDeclaration = %p name = %s \n",namespaceAliasDeclaration,namespaceAliasDeclaration->get_name().str());
          mfprintf(mlog [ WARN ] ) ("   --- is_alias_for_another_namespace_alias = %s \n",namespaceAliasDeclaration->get_is_alias_for_another_namespace_alias() ? "true" : "false");
#endif
          string namespaceDeclarationName;
       // SgNamespaceDeclarationStatement* namespaceDeclaration = namespaceAliasDeclaration->get_namespaceDeclaration();
          SgDeclarationStatement* namespaceDeclaration = NULL;
          if (namespaceAliasDeclaration->get_is_alias_for_another_namespace_alias() == true)
             {
               namespaceDeclaration = namespaceAliasDeclaration->get_namespaceAliasDeclaration();

            // DQ (8/1/2020): Set the name.
               namespaceDeclarationName = namespaceAliasDeclaration->get_namespaceAliasDeclaration()->get_name();
             }
            else
             {
               namespaceDeclaration = namespaceAliasDeclaration->get_namespaceDeclaration();

            // DQ (8/1/2020): Set the name.
               namespaceDeclarationName = namespaceAliasDeclaration->get_namespaceDeclaration()->get_name();
             }
          ASSERT_not_null(namespaceDeclaration);

       // DQ (8/1/2020): Record the associated NamespaceAliasDeclarationStatement so it can be used instead in namequalification.
       // inheritedAttribute.get_namespaceAliasDeclarationMap().insert(pair<SgDeclarationStatement*,SgNamespaceAliasDeclarationStatement*>(namespaceDeclaration,namespaceAliasDeclaration));

          namespaceAliasDeclarationMap.insert(pair<SgDeclarationStatement*,SgNamespaceAliasDeclarationStatement*>(namespaceDeclaration,namespaceAliasDeclaration));

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 0
          printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
          printf ("In evaluateInheritedAttribute(): Saved namespace alias: %s to namespace declaration = %s \n",namespaceAliasDeclaration->get_name().str(),namespaceDeclarationName.c_str());
#if 0
          printf (" --- inheritedAttribute.get_namespaceAliasDeclarationMap().size() = %zu \n",inheritedAttribute.get_namespaceAliasDeclarationMap().size());
          ROSE_ASSERT(namespaceAliasDeclarationMapFromInheritedAttribute != NULL);
          printf (" --- namespaceAliasDeclarationMapFromInheritedAttribute->size() = %zu \n",namespaceAliasDeclarationMapFromInheritedAttribute->size());
#else
          printf (" --- namespaceAliasDeclarationMap.size() = %zu \n",namespaceAliasDeclarationMap.size());
#endif
          printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
          SgScopeStatement* currentScope = namespaceAliasDeclaration->get_scope();
          ASSERT_not_null(currentScope);

          int amountOfNameQualificationRequired = nameQualificationDepth(namespaceDeclaration,currentScope,namespaceAliasDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("SgNamespaceAliasDeclarationStatement's SgNamespaceDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
          setNameQualification(namespaceAliasDeclaration,namespaceDeclaration,amountOfNameQualificationRequired);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("DONE: SgNamespaceAliasDeclarationStatement's SgNamespaceDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
        }

     SgNonrealRefExp * nrRefExp = isSgNonrealRefExp(n);
     if (nrRefExp != NULL) {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
        mfprintf(mlog [ WARN ] ) ("case SgNonrealRefExp: nrRefExp = %p\n", nrRefExp);
#endif
       SgNonrealSymbol * nrsym = nrRefExp->get_symbol();
       ASSERT_not_null(nrsym);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
       mfprintf(mlog [ WARN ] ) (" --- nrsym = %p : %s\n", nrsym, nrsym->get_name().str());
#endif

       SgNonrealDecl * nrdecl = nrsym->get_declaration();
       ASSERT_not_null(nrdecl);

       SgStatement* currentStatement = TransformationSupport::getStatement(nrRefExp);
       if (currentStatement != NULL) {
         SgScopeStatement* currentScope = currentStatement->get_scope();

         evaluateNameQualificationForTemplateArgumentList(nrdecl->get_tpl_args(), currentScope, currentStatement);

         SgDeclarationStatement * declstmt = nrdecl;
         if (nrdecl->get_templateDeclaration() != NULL) {
           declstmt = nrdecl->get_templateDeclaration();
         }

         int amountOfNameQualificationRequired = nameQualificationDepth(declstmt, currentScope, currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
         mfprintf(mlog [ WARN ] ) (" --- amountOfNameQualificationRequired = %d\n", amountOfNameQualificationRequired);
#endif
         setNameQualification(nrRefExp, declstmt, amountOfNameQualificationRequired);
       }
     }

  // DQ (5/12/2011): We want to located name qualification information about referenced functions
  // at the SgFunctionRefExp and SgMemberFunctionRefExp IR node instead of the SgFunctionCallExp IR node.
     SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(n);
     if (functionRefExp != NULL)
        {

          SgFunctionDeclaration* functionDeclaration = functionRefExp->getAssociatedFunctionDeclaration();
       // ASSERT_not_null(functionDeclaration);
          if (functionDeclaration != NULL)
             {
               SgStatement* currentStatement = TransformationSupport::getStatement(functionRefExp);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("!!!!!!!!!!!!!!! case SgFunctionRefExp: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif
            // DQ (9/17/2011); Added escape for where the currentStatement == NULL (fails for STL code when the original expression trees are used to eliminate the constant folded values).
            // ASSERT_not_null(currentStatement);
               if (currentStatement != NULL)
                  {
                 // DQ (3/15/2019): If this is part of an recursive call then the inheritedAttribute.get_currentScope()
                 // is set and we should use it as the currentScope.
                 // DQ (9/17/2011): this is the original case we want to restore later...
                 // SgScopeStatement* currentScope = currentStatement->get_scope();
                    SgScopeStatement* currentScope = NULL;
                    if (inheritedAttribute.get_currentScope() != NULL)
                       {
#if 0
                         mfprintf(mlog [ WARN ] ) ("Using scope set from a recursive call to name qualification \n");
#endif
                         currentScope = inheritedAttribute.get_currentScope();
                       }
                      else
                       {
#if 0
                         mfprintf(mlog [ WARN ] ) ("Using scope set from the scope of the currentStatement \n");
#endif
                         currentScope = currentStatement->get_scope();
                       }
                 // ASSERT_not_null(currentScope);

                 // DQ (1/31/2019): If this is a member function or template member function instantiation, AND it is definted
                 // outside of the class scope THEN we need to use the structural scope instead of the logical scope.
                    if (currentScope != NULL)
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("!!!!!!!!!!!!!!! currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                         SgStatement* parentStatement = isSgStatement(currentStatement->get_parent());
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("!!!!!!!!!!!!!!! parentStatement = %p = %s \n",parentStatement,parentStatement->class_name().c_str());
#endif
                         if (parentStatement != currentScope)
                            {
                              currentScope = parentStatement->get_scope();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("!!!!!!!!!!!!!!! RESETTING VIA PARENT: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                            }
                       }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("case SgFunctionRefExp: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                    int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgFunctionCallExp's function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                 // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in
                 // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
                 // definition could not live in the SgBasicBlock.
                    bool skipNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Test of functionRefExp: skipNameQualification = %s \n",skipNameQualification ? "true" : "false");
#endif
                    if (skipNameQualification == false)
                       {
                         setNameQualification(functionRefExp,functionDeclaration,amountOfNameQualificationRequired);
                       }
                  }
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("WARNING: functionDeclaration == NULL in SgFunctionCallExp for name qualification support! \n");
#endif
             }

       // If this is a templated function then we have to save the name because its templated name might have template arguments that require name qualification.
          SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(functionRefExp->getAssociatedFunctionDeclaration());
          if (templateInstantiationFunctionDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Found a SgTemplateInstantiationFunctionDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationFunctionDeclaration->get_name().str());
#endif
            // DQ (12/18/2016): When this is a function call in an array type index expression we can't identify an associated statement.
               SgStatement* currentStatement = TransformationSupport::getStatement(functionRefExp);
               if (currentStatement != NULL)
                  {
                    SgScopeStatement* currentScope = currentStatement->get_scope();
                    ASSERT_not_null(currentScope);

                    traverseTemplatedFunction(functionRefExp,functionRefExp,currentScope,currentStatement);
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Note: Name qualification: parent statement could not be identified (may be hidden in array type index) for functionRefExp = %p = %s \n",functionRefExp,functionRefExp->class_name().c_str());
#endif
                  }
             }
        }


#define PSEUDO_DESTRUCTOR_REF_SUPPORT 1

#if PSEUDO_DESTRUCTOR_REF_SUPPORT
  // DQ (1/18/2020): Adding support for SgPseudoDestructorRefExp (see C++11_tests/test2020_56.C).
     SgPseudoDestructorRefExp* pseudoDestructorRefExp = isSgPseudoDestructorRefExp(n);
     if (pseudoDestructorRefExp != NULL)
        {

#define DEBUG_PSEUDO_DESTRUCTOR_REF (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0

#if DEBUG_PSEUDO_DESTRUCTOR_REF
          mfprintf(mlog [ WARN ] ) ("Detected SgPseudoDestructorRefExp: pseudoDestructorRefExp = %p \n",pseudoDestructorRefExp);
#endif
          SgType* type = pseudoDestructorRefExp->get_object_type();
          ASSERT_not_null(type);

          SgNamedType* namedType = isSgNamedType(type);
          ASSERT_not_null(namedType);

          SgDeclarationStatement* declarationStatement = namedType->get_declaration();
          ASSERT_not_null(declarationStatement);

       // if (memberFunctionDeclaration != NULL)
          if (declarationStatement != NULL)
             {
            // DQ (2/17/2019): Adding support for pointers to member functions.
            // if (isMemberFunctionMemberReference == false)
            // if (isMemberFunctionMemberReference == false || isAddressTaken == true)

            // DQ (2/23/2019): Except that this code works in all cases that I can see at the moment, I think that the
            // current scope should be taken from the type of the pointer being dereferenced instead of from the location
            // of the statement containing the memberFunctionRefExp.  But I can't build a counter example that fails.

               SgStatement* currentStatement = TransformationSupport::getStatement(pseudoDestructorRefExp);

#if DEBUG_PSEUDO_DESTRUCTOR_REF
               mfprintf(mlog [ WARN ] ) ("Compute the currentStatement: currentStatement = %p \n",currentStatement);
#endif
               if (currentStatement == NULL)
                  {
                    mfprintf(mlog [ WARN ] ) ("Error: Location of where we can NOT associate the expression to a statement \n");
                    pseudoDestructorRefExp->get_file_info()->display("Error: currentStatement == NULL: memberFunctionRefExp: debug");
                    declarationStatement->get_file_info()  ->display("Error: currentStatement == NULL: memberFunctionDeclaration: debug");
                  }
               ASSERT_not_null(currentStatement);

#if DEBUG_PSEUDO_DESTRUCTOR_REF
               mfprintf(mlog [ WARN ] ) ("case of SgPseudoDestructorRefExp: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif
               SgScopeStatement* currentScope = currentStatement->get_scope();
               ASSERT_not_null(currentScope);

#if DEBUG_PSEUDO_DESTRUCTOR_REF
               mfprintf(mlog [ WARN ] ) ("case of SgPseudoDestructorRefExp: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("***** case of SgPseudoDestructorRefExp: Calling nameQualificationDepth() ***** \n");
#endif
            // int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,currentStatement);
               int amountOfNameQualificationRequired = nameQualificationDepth(declarationStatement,currentScope,currentStatement);

#if DEBUG_PSEUDO_DESTRUCTOR_REF
               mfprintf(mlog [ WARN ] ) ("***** case of SgPseudoDestructorRefExp: DONE: Calling nameQualificationDepth() ***** \n");
               mfprintf(mlog [ WARN ] ) ("SgPseudoDestructorRefExp's name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                 // setNameQualification(memberFunctionRefExp,memberFunctionDeclaration,amountOfNameQualificationRequired);
                    setNameQualification(pseudoDestructorRefExp,declarationStatement,amountOfNameQualificationRequired);
                 // DQ (2/17/2019): Case of xxx !(isDataMemberReference == true && isAddressTaken == true)
             }
            else
             {
#if DEBUG_PSEUDO_DESTRUCTOR_REF || 0
               mfprintf(mlog [ WARN ] ) ("WARNING: declarationStatement == NULL in SgPseudoDestructorRefExp for name qualification support! \n");
#endif
             }
        }
#endif


     SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(n);
     if (memberFunctionRefExp != NULL)
        {
          SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionRefExp->getAssociatedMemberFunctionDeclaration();
       // ASSERT_not_null(functionDeclaration);

#define DEBUG_MEMBER_FUNCTION_REF (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0

#if DEBUG_MEMBER_FUNCTION_REF
          mfprintf(mlog [ WARN ] ) ("case of SgMemberFunctionRefExp: memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
#endif

       // DQ (2/7/2019): Adding support for name qualification induced from SgPointerMemberType function paramters.
          bool nameQualificationInducedFromPointerMemberType = false;

          bool isMemberFunctionMemberReference = SageInterface::isMemberFunctionMemberReference(memberFunctionRefExp);
          bool isAddressTaken                  = SageInterface::isAddressTaken(memberFunctionRefExp);

#if DEBUG_MEMBER_FUNCTION_REF
          mfprintf(mlog [ WARN ] ) ("isMemberFunctionMemberReference = %s \n",isMemberFunctionMemberReference ? "true" : "false");
#endif
       // DQ (2/23/2019): I think that the test code test2019_191.C is not setting this correctly. The logic for
       // member function pointers (references) is not yet worked out as well as for data membr references.
#if DEBUG_MEMBER_FUNCTION_REF
          mfprintf(mlog [ WARN ] ) ("Explicitly setting isMemberFunctionMemberReference == false \n");
#endif
       // isMemberFunctionMemberReference = false;

#if DEBUG_MEMBER_FUNCTION_REF || 0
          mfprintf(mlog [ WARN ] ) ("Case of SgMemberFunctionRefExp: isMemberFunctionMemberReference = %s isAddressTaken = %s \n",isMemberFunctionMemberReference ? "true" : "false",isAddressTaken ? "true" : "false");
#endif
          if (isMemberFunctionMemberReference == true && isAddressTaken == true)
             {
#if DEBUG_MEMBER_FUNCTION_REF || 0
               mfprintf(mlog [ WARN ] ) ("Detected case of name qualification required due to pointer to member function reference \n");
#endif
               nameQualificationInducedFromPointerMemberType = true;
             }
            else
             {
            // bool isMemberFunctionMemberReference = SageInterface::isMemberFunctionMemberReference(memberFunctionRefExp);
               if (isMemberFunctionMemberReference == true)
                  {
#if DEBUG_MEMBER_FUNCTION_REF
                    mfprintf(mlog [ WARN ] ) ("This is a member function member reference requiring name qualification to the class where the data member reference is referenced \n");
#endif
                    ROSE_ASSERT(isAddressTaken == false);

                 // DQ (2/17/2019): Debugging pointer to membr function (similar to pointer to member data).
                 // bool isAddressTaken = SageInterface::isAddressTaken(memberFunctionRefExp);
                 // mfprintf(mlog [ WARN ] ) ("isAddressTaken = %s \n",isAddressTaken ? "true" : "false");

                 // std::list<SgClassType*> classChain = SageInterface::getClassTypeChainForDataMemberReference(memberFunctionRefExp);
                    std::list<SgClassType*> classChain = SageInterface::getClassTypeChainForMemberReference(memberFunctionRefExp);

#if DEBUG_MEMBER_FUNCTION_REF
                    mfprintf(mlog [ WARN ] ) ("case SgMemberFunctionRefExp: classChain.size() = %zu \n",classChain.size());
                    std::list<SgClassType*>::iterator classChain_iterator = classChain.begin();
                    while (classChain_iterator != classChain.end())
                       {
                         mfprintf(mlog [ WARN ] ) (" --- *classChain_iterator = %p = %s name = %s \n",*classChain_iterator,(*classChain_iterator)->class_name().c_str(),(*classChain_iterator)->get_name().str());

                         classChain_iterator++;
                       }
#endif
                 // DQ (2/16/2019): We need to call something like this, but specialized to just use the single class in the classChain.
                 // setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);

                    if (classChain.empty() == false)
                       {
                         std::list<SgClassType*>::iterator classChain_first = classChain.begin();
                         std::string qualifier = std::string((*classChain_first)->get_name().str()) + "::";

#if DEBUG_MEMBER_FUNCTION_REF
                         mfprintf(mlog [ WARN ] ) ("data member qualifier = %s \n",qualifier.c_str());
#endif
                      // DQ (2/16/2019): Mark this as at least non-zero, but it is computed based on where the ambiguity is instead
                      // of as a length of the chain of scope from the variable referenced's variable declaration scope.
                         memberFunctionRefExp->set_name_qualification_length(1);

                         memberFunctionRefExp->set_global_qualification_required(false);
                         memberFunctionRefExp->set_type_elaboration_required(false);

                         if (qualifiedNameMapForNames.find(memberFunctionRefExp) == qualifiedNameMapForNames.end())
                            {
#if DEBUG_MEMBER_FUNCTION_REF
                              mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),memberFunctionRefExp,memberFunctionRefExp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
                              printf("TEST 1: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),memberFunctionRefExp,memberFunctionRefExp->class_name().c_str());
#endif
                              qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(memberFunctionRefExp,qualifier));
                            }
                           else
                            {
                           // DQ (6/20/2011): We see this case in test2011_87.C.
                           // If it already existes then overwrite the existing information.
                           // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(memberFunctionRefExp);
                              NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(memberFunctionRefExp);
                              ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if DEBUG_MEMBER_FUNCTION_REF
                              string previousQualifier = i->second.c_str();
                              mfprintf(mlog [ WARN ] ) ("WARNING: test 0: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
                              if (i->second != qualifier)
                                 {
                                // DQ (7/23/2011): Multiple uses of the SgVarRefExp expression in SgArrayType will cause
                                // the name qualification to be reset each time.  This is OK since it is used to build
                                // the type name that will be saved.
                                   i->second = qualifier;
#if 0
                                   mfprintf(mlog [ WARN ] ) ("Note: name in qualifiedNameMapForNames already exists and is different... \n");
#endif
                                 }
                            }
                       }
                      else
                       {
#if DEBUG_MEMBER_FUNCTION_REF
                         mfprintf(mlog [ WARN ] ) ("This has an empty class chain: classChain.size() = %zu \n",classChain.size());
#endif

                 // DQ (6/1/2019): When the function called is from a base class and conflicts with a member function
                 // in the derived class then we need additional name qualification.

#if DEBUG_MEMBER_FUNCTION_REF
                    mfprintf(mlog [ WARN ] ) (" (isMemberFunctionMemberReference == true && isAddressTaken == false) == true \n");
#endif
                 // ROSE_ASSERT (isMemberFunctionMemberReference == true && isAddressTaken == false);

                    SgStatement* currentStatement = TransformationSupport::getStatement(memberFunctionRefExp);
                 // ASSERT_not_null(currentStatement);
                    if (currentStatement != NULL)
                       {
                         SgScopeStatement* currentScope = currentStatement->get_scope();
                         ASSERT_not_null(currentScope);

                         ASSERT_not_null(memberFunctionDeclaration);

                         int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,currentStatement);

#if DEBUG_MEMBER_FUNCTION_REF
                         mfprintf(mlog [ WARN ] ) ("***** case of SgMemberFunctionRefExp: DONE: Calling nameQualificationDepth() ***** \n");
                         mfprintf(mlog [ WARN ] ) ("SgMemberFunctionCallExp's member function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                         ASSERT_not_null(memberFunctionRefExp);

                         setNameQualification(memberFunctionRefExp,memberFunctionDeclaration,amountOfNameQualificationRequired);
                       }
                      else
                       {
#if DEBUG_MEMBER_FUNCTION_REF || 0
                         mfprintf(mlog [ WARN ] ) ("case of SgMemberFunctionRefExp: currentStatement == NULL (could be a function call hidden in a decltype()) \n");
#endif
                       }
                       }
                  }
                 else
                  {
                    ROSE_ASSERT (isMemberFunctionMemberReference == false);
                    if (isAddressTaken == true)
                       {
                       }
                      else
                       {
                         ROSE_ASSERT(isAddressTaken == false);
                       }
                  }
             }

#if DEBUG_MEMBER_FUNCTION_REF
          mfprintf(mlog [ WARN ] ) ("Case of SgMemberFunctionRefExp: memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
#endif

          if (memberFunctionDeclaration != NULL)
             {
            // DQ (2/17/2019): Adding support for pointers to member functions.
            // if (isMemberFunctionMemberReference == false)
               if (isMemberFunctionMemberReference == false || isAddressTaken == true)
                  {

                 // DQ (2/23/2019): Except that this code works in all cases that I can see at the moment, I think that the
                 // current scope should be taken from the type of the pointer being dereferenced instead of from the location
                 // of the statement containing the memberFunctionRefExp.  But I can't build a counter example that fails.

                    SgStatement* currentStatement = TransformationSupport::getStatement(memberFunctionRefExp);

#if DEBUG_MEMBER_FUNCTION_REF
                    mfprintf(mlog [ WARN ] ) ("Compute the currentStatement: currentStatement = %p \n",currentStatement);
#endif
                    if (currentStatement == NULL)
                       {
                      // DQ (8/19/2014): Because we know where this can happen we don't need to always output debugging info.
                      // A better test might be to find the type that embeds the expression and make sure it is a SgArrayType.
                      // DQ (7/11/2014): test2014_83.C demonstrates how this can happen because the SgMemberFunctionRefExp
                      // appears in an index expression of an array type in a variable declaration.
                         SgType* associatedType = TransformationSupport::getAssociatedType(memberFunctionRefExp);
                         if (associatedType != NULL)
                            {
                              SgArrayType* arrayType = isSgArrayType(associatedType);
                              if (arrayType == NULL)
                                 {
#if DEBUG_MEMBER_FUNCTION_REF
                                   mfprintf(mlog [ WARN ] ) ("Warning: Location of where we can NOT associate the expression to a SgArrayType \n");
                                   memberFunctionRefExp->get_file_info()     ->display("Error: currentStatement == NULL: memberFunctionRefExp: debug");
                                   memberFunctionDeclaration->get_file_info()->display("Error: currentStatement == NULL: memberFunctionDeclaration: debug");
#endif
                                 }
                                else
                                 {
#if DEBUG_MEMBER_FUNCTION_REF
                                   mfprintf(mlog [ WARN ] ) ("Note: Location of where we CAN associate the expression to a statement: confirmed unassociated expression is buried in a type: associatedType = %p = %s \n",associatedType,associatedType->class_name().c_str());
#endif
                                 }
                            }
                           else
                            {
                              mfprintf(mlog [ WARN ] ) ("Error: Location of where we can NOT associate the expression to a statement \n");
#if DEBUG_MEMBER_FUNCTION_REF
                              memberFunctionRefExp->get_file_info()     ->display("Error: currentStatement == NULL: memberFunctionRefExp: debug");
                              memberFunctionDeclaration->get_file_info()->display("Error: currentStatement == NULL: memberFunctionDeclaration: debug");
#endif
                            }

                      // DQ (7/11/2014): Added support for when this is a nested call and the scope where the call is made from is essential.
                         if (explictlySpecifiedCurrentScope != NULL)
                            {
#if DEBUG_MEMBER_FUNCTION_REF
                              mfprintf(mlog [ WARN ] ) ("explictlySpecifiedCurrentScope = %p = %s \n",explictlySpecifiedCurrentScope,explictlySpecifiedCurrentScope->class_name().c_str());
#endif

                           // DQ (4/19/2019): Now that we (optionally) also pass in the explictlySpecifiedCurrentStatement, we might want to use it directly.
#if 0
                              mfprintf(mlog [ WARN ] ) ("case of SgMemberFunctionRefExp: Using explictlySpecifiedCurrentScope for the value of currentStatement: need to check this! \n");
#endif
                              currentStatement = explictlySpecifiedCurrentScope;
                            }
                           else
                            {
                              mfprintf(mlog [ WARN ] ) ("Error: explictlySpecifiedCurrentScope == NULL \n");

                              mfprintf(mlog [ WARN ] ) ("Exiting as a test! \n");
                              ROSE_ABORT();
                            }
                       }
                    ASSERT_not_null(currentStatement);

#if DEBUG_MEMBER_FUNCTION_REF
                    mfprintf(mlog [ WARN ] ) ("case of SgMemberFunctionRefExp: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif
                    SgScopeStatement* currentScope = currentStatement->get_scope();
                    ASSERT_not_null(currentScope);

#if DEBUG_MEMBER_FUNCTION_REF
                    mfprintf(mlog [ WARN ] ) ("case of SgMemberFunctionRefExp: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
                    mfprintf(mlog [ WARN ] ) ("***** case of SgMemberFunctionRefExp: Calling nameQualificationDepth() ***** \n");
#endif
                    int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,currentStatement);

#if DEBUG_MEMBER_FUNCTION_REF
                    mfprintf(mlog [ WARN ] ) ("***** case of SgMemberFunctionRefExp: DONE: Calling nameQualificationDepth() ***** \n");
                    mfprintf(mlog [ WARN ] ) ("SgMemberFunctionCallExp's member function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                 // DQ (2/7/2019): Add an extra level of name qualification if this is pointer-to-member type induced.
                    if (nameQualificationInducedFromPointerMemberType == true)
                       {
                      // DQ (2/8/2019): Only add name qualification if not present (else we can get over qualification
                      // that can show up as pointer names in the name qualification, see Cxx11_tests/test2019_86.C).
                         if (amountOfNameQualificationRequired == 0)
                            {
                              amountOfNameQualificationRequired++;
                            }
#if DEBUG_MEMBER_FUNCTION_REF
                         mfprintf(mlog [ WARN ] ) ("Found case of name qualification required because the variable is associated with SgPointerMemberType: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                       }

                    setNameQualification(memberFunctionRefExp,memberFunctionDeclaration,amountOfNameQualificationRequired);
                 // DQ (2/17/2019): Case of xxx !(isDataMemberReference == true && isAddressTaken == true)
                  }
             }
            else
             {
#if DEBUG_MEMBER_FUNCTION_REF || 0
               mfprintf(mlog [ WARN ] ) ("WARNING: memberFunctionDeclaration == NULL in SgMemberFunctionCallExp for name qualification support! \n");
#endif
             }

       // If this is a templated function then we have to save the name because its templated name might have template arguments that require name qualification.
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionRefExp->getAssociatedMemberFunctionDeclaration());
          if (templateInstantiationMemberFunctionDeclaration != NULL)
             {
#if DEBUG_MEMBER_FUNCTION_REF
               mfprintf(mlog [ WARN ] ) ("Found a SgTemplateInstantiationMemberFunctionDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationMemberFunctionDeclaration->get_name().str());
               mfprintf(mlog [ WARN ] ) ("Must handle templated SgMemberFunctionRefExp! \n");
#endif

            // DQ (5/24/2013): Added support for member function template argument lists to have similar handling, such as to
            // SgTemplateInstantiationFunctionDecl IR nodes.  This is required to support test codes such as test2013_188.C.
               SgStatement* currentStatement = TransformationSupport::getStatement(memberFunctionRefExp);

            // DQ (4/15/2019): This fails for EDG 5.0 only, on Cxx_tests/test2004_149.C (as a result of recent work Sunday afternoon).
            // ASSERT_not_null(currentStatement);
               if (currentStatement != NULL)
                  {
               SgScopeStatement* currentScope = currentStatement->get_scope();
               ASSERT_not_null(currentScope);

#if DEBUG_MEMBER_FUNCTION_REF
               mfprintf(mlog [ WARN ] ) ("case of SgMemberFunctionRefExp: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("case of SgMemberFunctionRefExp: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("***** calling traverseTemplatedMemberFunction() \n");
#endif

            // traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement);
            // traverseTemplatedFunction(functionRefExp,functionRefExp,currentScope,currentStatement);
               traverseTemplatedMemberFunction(memberFunctionRefExp,memberFunctionRefExp,currentScope,currentStatement);

#if DEBUG_MEMBER_FUNCTION_REF
               mfprintf(mlog [ WARN ] ) ("***** DONE: calling traverseTemplatedMemberFunction() \n");
#endif
                  }
             }
        }


  // DQ (5/31/2011): This is a derived class from SgExpression and SgInitializer...
     SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(n);
     if (constructorInitializer != NULL)
        {
          SgMemberFunctionDeclaration* memberFunctionDeclaration = constructorInitializer->get_declaration();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("Case of SgConstructorInitializer: memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
#endif

          SgStatement* currentStatement = TransformationSupport::getStatement(constructorInitializer);

#if 0
          printf ("Case of SgConstructorInitializer: before if(currentStatement == NULL): currentStatement = %p = %s name = %s \n",currentStatement,
               currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
#endif

          if (currentStatement == NULL)
             {
            // DQ (1/28/2019): This can happen when the expression is used in an array type declaration (e.g. within a variable declaration for an array).
            // NOTE: this will be possibly incorrect if there is a using declaration in the scope that would be important to the name qualification.
            // We would then need to know if the declarration declaring the array type was before or after the using declaration.
            // Not clear what would be the best way to solve that problem (though it would not be in the set of directived already processed, so it might be fine).
#if 0
               mfprintf(mlog [ WARN ] ) ("In name qualification: not possible to locate statement containing constructorInitializer = %p (using first statement from current scope) \n",constructorInitializer);
#endif
               ASSERT_not_null(constructorInitializer->get_parent());

               SgScopeStatement* tmp_currentScope = inheritedAttribute.get_currentScope();
               ASSERT_not_null(tmp_currentScope);
            // If we don't have a statement derived from the expression to reference, then use the first statement in the current scope.
               currentStatement = tmp_currentScope->firstStatement();
               ASSERT_not_null(currentStatement);
             }
          ASSERT_not_null(currentStatement);

       // If this could occur in a SgForStatement then this should be fixed up as it is elsewhere...
          SgScopeStatement* currentScope = currentStatement->get_scope();
          ASSERT_not_null(currentScope);

#if 0
          printf ("Case of SgConstructorInitializer: memberFunctionDeclaration = %p = %s name = %s \n",
               memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),SageInterface::get_name(memberFunctionDeclaration).c_str());
          printf ("Case of SgConstructorInitializer: currentScope = %p = %s name = %s \n",currentScope,
               currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
          printf ("Case of SgConstructorInitializer: currentStatement = %p = %s name = %s \n",currentStatement,
               currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
#endif
#if 0
          printf ("Case of SgConstructorInitializer: currentScope: symbol_table: \n");
          currentScope->get_symbol_table()->print("Case of SgConstructorInitializer: currentScope: symbol_table");
#endif

          if (memberFunctionDeclaration != NULL)
             {
               int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,currentStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("SgConstructorInitializer's constructor member function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
               setNameQualification(constructorInitializer,memberFunctionDeclaration,amountOfNameQualificationRequired);
#if 0
               mfprintf(mlog [ WARN ] ) ("In name qualification: Case of SgConstructorInitializer: memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
#endif
             }
            else
             {
            // DQ (6/1/2011): This happens when there is no explicit constructor that can be used to build a class, in this case the class name must be used to define a default constructor.
            // This is a problem for test2004_130.C (at line 165 col = 14 file = /home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.2.4-dq-cxx-rc/include-staging/g++_HEADERS/hdrs3/bits/stl_iterator_base_types.h).
            // Need to investigate this later (it is strange that it is not an issue in test2011_63.C, but it is a struct instead of a class and that might be why).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("WARNING: memberFunctionDeclaration == NULL in SgConstructorInitializer for name qualification support! \n");
#endif
            // ROSE_ASSERT(false);

            // DQ (6/4/2011): Added support for this case.
               SgClassDeclaration* classDeclaration = constructorInitializer->get_class_decl();
            // ASSERT_not_null(classDeclaration);
               if (classDeclaration != NULL)
                  {
                 // An example of the problem is test2005_42.C, where the class name is used to generate the constructor initializer name.
                    int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("SgConstructorInitializer's constructor (class default constructor) name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                 // This will attach the new type string to the classDeclaration.
                    setNameQualification(constructorInitializer,classDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                 // This is a strange error: see test2004_77.C
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("WARNING: In SgConstructorInitializer name qualification support: neither memberFunctionDeclaration or classDeclaration are valid pointers. \n");
#endif
                  }
             }

       // After processing the name qualification for the class declaration, we need to also process the
       // reference to the type for any name qualification on possible template arguments.
          ASSERT_not_null(constructorInitializer->get_type());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Calling traverseType() on constructorInitializer = %p class type = %p = %s \n",
                    constructorInitializer,constructorInitializer->get_type(),constructorInitializer->get_type()->class_name().c_str());
#endif
       // DQ (8/19/2013): Added the call to associate the name qualified class name with the constructorInitializer.
       // DQ (8/19/2013): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
          traverseType(constructorInitializer->get_type(),constructorInitializer,currentScope,currentStatement);
        }



  // DQ (3/21/2018): This is a derived class from SgConstructorInitializer...
     SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(n);
     if (aggregateInitializer != NULL)
        {
       // DQ (3/21/2018): Ignore the member function for the case of a SgAggregateInitializer.
       // SgMemberFunctionDeclaration* memberFunctionDeclaration = aggregateInitializer->get_declaration();
       // SgMemberFunctionDeclaration* memberFunctionDeclaration = NULL;
          SgType* aggregateInitializerType = aggregateInitializer->get_type();
          ASSERT_not_null(aggregateInitializerType);
#if 0
          mfprintf(mlog [ WARN ] ) ("Case of SgAggregateInitializer: aggregateInitializerType = %p = %s \n",aggregateInitializerType,aggregateInitializerType->class_name().c_str());
#endif
          SgClassType* aggregateInitializerClassType = isSgClassType(aggregateInitializerType);
#if 0
          mfprintf(mlog [ WARN ] ) ("Case of SgAggregateInitializer: aggregateInitializerClassType = %p \n",aggregateInitializerClassType);
#endif
          SgClassDeclaration* aggregateInitializerClassDeclaration = NULL;
          if (aggregateInitializerClassType != NULL)
             {
               aggregateInitializerClassDeclaration = isSgClassDeclaration(aggregateInitializerClassType->get_declaration());
               ASSERT_not_null(aggregateInitializerClassDeclaration);
#if 0
               mfprintf(mlog [ WARN ] ) ("Case of SgAggregateInitializer: classDeclaration = %p = %s name = %s \n",
                    aggregateInitializerClassDeclaration,aggregateInitializerClassDeclaration->class_name().c_str(),aggregateInitializerClassDeclaration->get_name().str());
#endif
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Case of SgAggregateInitializer: aggregateInitializerClassDeclaration = %p \n",aggregateInitializerClassDeclaration);
#endif

          SgStatement* currentStatement = TransformationSupport::getStatement(aggregateInitializer);
          if (currentStatement == NULL)
             {
            // DQ (1/28/2019): This can happen when the expression is used in an array type declaration (e.g. within a variable declaration for an array).
            // NOTE: this will be possibly incorrect if there is a using declaration in the scope that would be important to the name qualification.
            // We would then need to know if the declarration declaring the array type was before or after the using declaration.
            // Not clear what would be the best wayy to solve that problem (though it would not be in the set of directived already processed, so it might be fine).
#if 0
               mfprintf(mlog [ WARN ] ) ("In name qualification: not possible to locate statement containing aggregateInitializer = %p (using first statement from current scope) \n",aggregateInitializer);
#endif
               ASSERT_not_null(aggregateInitializer->get_parent());

               SgScopeStatement* tmp_currentScope = inheritedAttribute.get_currentScope();
               ASSERT_not_null(tmp_currentScope);
            // If we don't have a statement derived from the expression to reference, then use the first statement in the current scope.
               currentStatement = tmp_currentScope->firstStatement();
               ASSERT_not_null(currentStatement);
             }
          ASSERT_not_null(currentStatement);

       // If this could occur in a SgForStatement then this should be fixed up as it is elsewhere...
          SgScopeStatement* currentScope = currentStatement->get_scope();
          ASSERT_not_null(currentScope);

          if (aggregateInitializerClassDeclaration != NULL)
             {
#if 0
               mfprintf(mlog [ WARN ] ) ("Case of SgAggregateInitializer: aggregateInitializerClassDeclaration = %p = %s name = %s \n",
                    aggregateInitializerClassDeclaration,aggregateInitializerClassDeclaration->class_name().c_str(),aggregateInitializerClassDeclaration->get_name().str());
#endif
               int amountOfNameQualificationRequired = nameQualificationDepth(aggregateInitializerClassDeclaration,currentScope,currentStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("SgAggregateInitializer's class declaration name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
               setNameQualification(aggregateInitializer,aggregateInitializerClassDeclaration,amountOfNameQualificationRequired);
#if 0
               mfprintf(mlog [ WARN ] ) ("In name qualification: Case of SgAggregateInitializer: aggregateInitializerClassDeclaration = %p \n",aggregateInitializerClassDeclaration);
#endif

            // After processing the name qualification for the class declaration, we need to also process the
            // reference to the type for any name qualification on possible template arguments.
               ASSERT_not_null(aggregateInitializer->get_type());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Calling traverseType() on aggregateInitializer = %p class type = %p = %s \n",
                    aggregateInitializer,aggregateInitializer->get_type(),aggregateInitializer->get_type()->class_name().c_str());
#endif
            // DQ (8/19/2013): Added the call to associate the name qualified class name with the constructorInitializer.
            // DQ (8/19/2013): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
               traverseType(aggregateInitializer->get_type(),aggregateInitializer,currentScope,currentStatement);
             }
        }



     SgVarRefExp* varRefExp = isSgVarRefExp(n);
     if (varRefExp != NULL)
        {
       // We need to store the information about the required name qualification in the SgVarRefExp IR node.

          SgStatement* currentStatement = TransformationSupport::getStatement(varRefExp);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Case of SgVarRefExp: varRefExp = %p currentStatement = %p = %s \n",varRefExp,currentStatement,currentStatement != NULL ? currentStatement->class_name().c_str() : "null");
#endif

       // DQ (2/7/2019): Adding support for name qualification induced from SgPointerMemberType function paramters.
          bool nameQualificationInducedFromPointerMemberType = false;

       // DQ (2/8/2019): And then I woke up in the morning and had a better idea.
       // DQ (2/8/2019): An alternative to supporting pointer-to-member name qualification would be to detect member data accessed via a pointer.
       // so we need to look at the parent of a SgVarRefExp and see if it is a SgAddressOfOp when it is a reference to a data member.
          bool isDataMemberReference = SageInterface::isDataMemberReference(varRefExp);
          bool isAddressTaken        = SageInterface::isAddressTaken(varRefExp);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("Case of SgVarRefExp: isDataMemberReference = %s isAddressTaken = %s \n",isDataMemberReference ? "true" : "false",isAddressTaken ? "true" : "false");
#endif
          if (isDataMemberReference == true && isAddressTaken == true)
             {
               nameQualificationInducedFromPointerMemberType = true;
             }
            else
             {
            // DQ (2/15/2019): Debugging Cxx11_tests/test2019_129.C.  Data member references should have a current
            // statement that starts in the class XXX (instead of where XXX.yyy is located.
            // If this is a data member reference, then we need to change the perspective to a currentStatement of
            // that of the class where it is a data member reference.  So XXX.yyy would have yyy be a data member
            // of XXX and so the current statement would be the scope represent by XXX.

            // DQ (2/15/2019): Unfortunately data member name qualification happens top down, instaed of bottom up
            // like all other name qualification.  So this is going to complicate things.

            // Need to change the name of this function to be more specific.
               if (isDataMemberReference == true)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Change the starting location for name qualification to the class where the data member reference is referenced \n");
#endif
                    ROSE_ASSERT(isAddressTaken == false);
                 // reset the current statement.

                 // Insead of returning the SgClassType at the end of the chain, we actaully need to generate the chain
                 // of SgClassType the reflects the path taken (represented by the chain of SgCastExp expressions).

                 // Then for each element of the chain, we need to lookup the symbol (a in "X x; x.a;") in the scope
                 // representing each class, and see if the number of causal nodes is more than one. The name qualification
                 // length is the longest chain between scopes where two of the associated SgAliasSymbols have 2 or more
                 // causal nodes.

                 // DQ (2/16/2019): We need to look for both variable and base class ambiguity.  I don't think we need
                 // base class ambiguity, since that is not allowed in the langauge.

                 // Second generation of this function.
                 // std::list<SgClassType*> classChain = SageInterface::getClassTypeChainForDataMemberReference(varRefExp);
                    std::list<SgClassType*> classChain = SageInterface::getClassTypeChainForMemberReference(varRefExp);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("case SgVarRefExp: classChain.size() = %zu \n",classChain.size());
                    std::list<SgClassType*>::iterator classChain_iterator = classChain.begin();
                    while (classChain_iterator != classChain.end())
                       {
                         mfprintf(mlog [ WARN ] ) (" --- *classChain_iterator = %p = %s name = %s \n",*classChain_iterator,(*classChain_iterator)->class_name().c_str(),(*classChain_iterator)->get_name().str());

                         classChain_iterator++;
                       }
#endif
                 // DQ (11/10/2020): This assertion failed (classChain.size() <= 2) where the value was 3 for some of the target code for codeSegregation.
                 // DQ (11/8/2020): I now think that Cxx11_tests/test_2019_120.C could be used to make this arbitrarily long.
                 // DQ (12/11/2019): Modified to provide a larger upper bound for classChain.size().
                 // DQ (2/16/2019): I think this is always true, since base class abiguity is not allowed in the C++ language.
                 // ROSE_ASSERT(classChain.size() == 1);
                 // ROSE_ASSERT(classChain.empty() == true || classChain.size() == 1);
                 // ROSE_ASSERT(classChain.empty() == true || classChain.size() <= 2);
                    if (classChain.size() > 3)
                       {
                         printf ("In name qualification: Case of SgVarRefExp: classChain.size() > 3: classChain.size() = %zu \n",classChain.size());
                       }

                 // DQ (2/16/2019): We need to call something like this, but specialized to just use the single class in the classChain.
                 // setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);

                    if (classChain.empty() == false)
                       {
                      // DQ (1/19/2020): Might need to recursively call the name qualification on the classChain_first
                      // since it can be a class that required name qualificaiton to resolve an ambiguity.
#if 0
                         mfprintf(mlog [ WARN ] ) ("Might need a recursive call to resolve the possible ambiguity of this class defined by classChain_first \n");
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         std::list<SgClassType*>::iterator classChain_first = classChain.begin();
                         mfprintf(mlog [ WARN ] ) ("(*classChain_first)->get_name().str() = %s \n",(*classChain_first)->get_name().str());
#endif
                      // This is much more complex code, but it satisfies all of the test codes including the ones
                      // for codeSegragation when the the symbol table for the global scope across file is cleared.
#if 0
                         printf ("################################################# \n");
                         printf ("Iterate over the classChain: size = %zu \n",classChain.size());
                         printf ("################################################# \n");
#endif
                      // DQ (11/8/2020): Potential bug fix for name qualification error that only happens in
                      // transformations (e.g. codeSegregation and outlining). Note that it appears as an issue
                      // to fix only when the buildSourceFile() function is used and the symbol table associated
                      // with the global scope across files is cleared (recently implementd in SageInterface::buildFile()
                      // (called by SageInterface::getSourceFile()).
                         std::list<SgClassType*>::iterator classChain_last;
                         std::list<SgClassType*>::iterator classChain_target = classChain.begin();
                         std::list<SgClassType*>::iterator i = classChain.begin();

                         bool useNextClass = false;
                      // Note that the start of the chain may not be the most apropriate class to use.
                      // This is demonstrated by Cxx_tests/test2019_130.C and the codeSegragation tool test_93.cpp.
                         while (i != classChain.end())
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                              mfprintf(mlog [ WARN ] ) (" --- (*i)->get_name().str() = %s \n",(*i)->get_name().str());
#endif
                           // Review the length of the causal nodes for the alias symbol.  If is is one then no name qualification
                           // is needed, if it is more than one then name qualification is required to disambiguate the member access.
                              SgClassType* classType = *i;
                              ROSE_ASSERT(classType != NULL);

                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                              ROSE_ASSERT(classDeclaration != NULL);
#if 0
                              printf (" ---  --- classDeclaration = %p = %s name = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
#endif
                              SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                              if (definingClassDeclaration != NULL)
                                 {
                                   SgSymbol* varRefExp_symbol = varRefExp->get_symbol();
                                   ROSE_ASSERT(varRefExp_symbol != NULL);

                                   SgName varRefExp_name = varRefExp_symbol->get_name();
#if 0
                                   printf (" ---  --- varRefExp_name = %s \n",varRefExp_name.str());
#endif
                                   SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                   ROSE_ASSERT(classDefinition != NULL);

                                   size_t number_of_alias_symbols = classDefinition->count_alias_symbol(varRefExp_name);
#if 0
                                   printf (" ---  --- number_of_alias_symbols = %zu \n",number_of_alias_symbols);
#endif
                                   if (number_of_alias_symbols > 0)
                                      {
                                        SgAliasSymbol* aliasSymbol = classDefinition->lookup_alias_symbol(varRefExp_name,varRefExp_symbol);
#if 0
                                        printf (" ---  --- number_of_alias_symbols > 0: aliasSymbol = %p \n",aliasSymbol);
#endif
                                        if (aliasSymbol != NULL)
                                           {
#if 0
                                             printf (" ---  --- number_of_alias_symbols > 0: Found an alias symbol: aliasSymbol = %p = %s \n",aliasSymbol,aliasSymbol->class_name().c_str());
                                             printf (" ---  --- number_of_alias_symbols > 0: Found an alias symbol: causal_nodes list size = %zu \n",aliasSymbol->get_causal_nodes().size());
#endif
                                             if (aliasSymbol->get_causal_nodes().size() == 1)
                                                {
                                               // Reset to where there will be no ambiguity.
#if 0
                                                  printf ("Reset because aliasSymbol->get_causal_nodes().size() == 1 \n");
#endif
                                                  classChain_target = i;
                                                }
                                               else
                                                {
                                                  if (aliasSymbol->get_causal_nodes().size() > 1)
                                                     {
                                                    // Use the next element in the chain.
                                                       useNextClass = true;
                                                     }
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Reset to where there will be no ambiguity.
#if 0
                                        printf ("Reset because number_of_alias_symbols == 0: useNextClass = %s \n",useNextClass ? "true" : "false");
#endif
                                        if (useNextClass == true)
                                           {
                                             classChain_target = i;
                                             useNextClass = false;
                                           }
                                      }
                                 }

                           // qualifier += std::string((*i)->get_name().str()) + "::";
                              classChain_last = i;
                              i++;
                            }
                         std::string qualifier = std::string((*classChain_target)->get_name().str()) + "::";

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("data member qualifier = %s \n",qualifier.c_str());
#endif
                      // DQ (2/16/2019): Mark this as at least non-zero, but it is computed based on where the ambiguity is instead
                      // of as a length of the chain of scope from the variable referenced's variable declaration scope.
                         varRefExp->set_name_qualification_length(1);

                         varRefExp->set_global_qualification_required(false);
                         varRefExp->set_type_elaboration_required(false);

                         if (qualifiedNameMapForNames.find(varRefExp) == qualifiedNameMapForNames.end())
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),varRefExp,varRefExp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
                              printf("TEST 2: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),varRefExp,varRefExp->class_name().c_str());
#endif
                              qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(varRefExp,qualifier));
                            }
                           else
                            {
                           // DQ (6/20/2011): We see this case in test2011_87.C.
                           // If it already existes then overwrite the existing information.
                           // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(varRefExp);
                              NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(varRefExp);
                              ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              string previousQualifier = i->second.c_str();
                              mfprintf(mlog [ WARN ] ) ("WARNING: test 1: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
                              if (i->second != qualifier)
                                 {
                                // DQ (7/23/2011): Multiple uses of the SgVarRefExp expression in SgArrayType will cause
                                // the name qualification to be reset each time.  This is OK since it is used to build
                                // the type name that will be saved.
                                   i->second = qualifier;
#if 0
                                   mfprintf(mlog [ WARN ] ) ("Note: name in qualifiedNameMapForNames already exists and is different... \n");
#endif
                                 }
                            }
                       }
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Case of SgVarRefExp: nameQualificationInducedFromPointerMemberType = %s \n",nameQualificationInducedFromPointerMemberType ? "true" : "false");
          mfprintf(mlog [ WARN ] ) (" --- isDataMemberReference = %s isAddressTaken = %s \n",isDataMemberReference ? "true" : "false",isAddressTaken ? "true" : "false");
          mfprintf(mlog [ WARN ] ) (" --- currentStatement = %p \n",currentStatement);
#endif

          if (isDataMemberReference == false || isAddressTaken == true)
             {
            // DQ (6/23/2011): This test fails for the new name qualification after a transformation in tests/nonsmoke/functional/roseTests/programTransformationTests/test1.C
               if (currentStatement != NULL)
                  {
                 // DQ (5/30/2011): Handle the case of test2011_58.C (index declaration in for loop construct).
                    SgScopeStatement* currentScope = isSgScopeStatement(currentStatement);
                    if (currentScope == NULL)
                       {
                         currentScope = currentStatement->get_scope();
                       }
                    ASSERT_not_null(currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Case SgVarRefExp: (could this be in an array type?) currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                    SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
                    ASSERT_not_null(variableSymbol);
                    SgInitializedName* initializedName = variableSymbol->get_declaration();
                    ASSERT_not_null(initializedName);

                 // DQ (7/18/2012): Added test as part of debugging test2011_75.C.
                    ASSERT_not_null(initializedName->get_parent());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Case of SgVarRefExp: varRefExp = %p : initializedName name = %s parent = %p = %s \n",
                         varRefExp,initializedName->get_name().str(),initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
#endif
                 // DQ (7/24/3030): This variable declaration hides an outer declaration using the same variable name.
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedName->get_parent());

                 // DQ (7/24/2020): Debugging Cxx20_tests/test2020_122.C and Cxx_tests/test2020_14.C.
                    if ((variableDeclaration != NULL) && (variableDeclaration != variableDeclaration->get_definingDeclaration()) )
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 0
                         printf ("SgVarRefExp is not in a defining SgVariableDeclaration: might be in a SgDesignatedInitializer \n");
#endif
                         SgExprListExp* exprListExp = isSgExprListExp(varRefExp->get_parent());
                         if (exprListExp != NULL)
                            {
                              SgDesignatedInitializer* designatedInitializer = isSgDesignatedInitializer(exprListExp->get_parent());
                              if (designatedInitializer != NULL)
                                 {
                                // This is not a SgInitializedName being declared by a variable declaration. Setting
                                // this to NULL will force the TRUE case below to be taken, which after the check
                                // for some builtin predefined names, will cause not name qualification to be added.
                                   variableDeclaration = NULL;
                                 }
                            }
                       }

                    if (variableDeclaration != NULL)
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("In case SgVarRefExp: (variableDeclaration != NULL) Calling nameQualificationDepth() \n");
#endif
                      // DQ (12/21/2015): When this is a data member of a class/struct then we are consistatnly overqualifying the SgVarRefExp
                      // because we are not considering the case of a variable of type class that is being used with the SgArrowExp or SgDotExp
                      // which would not require the name qualification.  The only case where we would still need the name qualification is the
                      // relatively rare case of multiple inheritance (which must be detected seperately).

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("variableDeclaration = %p \n",variableDeclaration);
                         mfprintf(mlog [ WARN ] ) ("currentScope        = %p = %s \n",currentScope,currentScope->class_name().c_str());
                         mfprintf(mlog [ WARN ] ) ("currentStatement    = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
                         variableDeclaration->get_file_info()->display("Before nameQualificationDepth()");
#endif
                         int amountOfNameQualificationRequired = nameQualificationDepth(variableDeclaration,currentScope,currentStatement);

                      // DQ (2/7/2019): Add an extra level of name qualification if this is pointer-to-member type induced.
                         if (nameQualificationInducedFromPointerMemberType == true)
                            {
                           // DQ (2/8/2019): Only add name qualification if not present (else we can get over qualification
                           // that can show up as pointer names in the name qualification, see Cxx11_tests/test2019_86.C).
                              if (amountOfNameQualificationRequired == 0)
                                 {
                                // DQ (3/30/2019): Experiment with commenting this out!
                                   amountOfNameQualificationRequired++;
                                 }
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("Found case of name qualification required because the variable is associated with SgPointerMemberType: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                            }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("SgVarRefExp's SgDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                         setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);

                      // DQ (12/23/2015): If there are multiple symbols with the same name then we require the name qualification.
                      // See test2015_140.C for an example.
                         SgName name = initializedName->get_name().str();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("SgVarRefExp's SgDeclarationStatement: initializedName->get_name() = %s \n",name.str());
#endif

                         int numberOfAliasSymbols = currentScope->count_alias_symbol(name);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("SgVarRefExp's SgDeclarationStatement: numberOfAliasSymbols              = %d \n",numberOfAliasSymbols);
                         mfprintf(mlog [ WARN ] ) ("SgVarRefExp's SgDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

                         if (numberOfAliasSymbols > 1 && amountOfNameQualificationRequired == 0)
                            {
                           // DQ (3/15/2017): Added support to use message streams.
                              mfprintf(mlog [ WARN ] ) ("WARNING: name qualification can be required when there are multiple base classes with the same referenced variable via SgAliasSymbol \n");
                            }
                           else
                            {
                           // DQ (12/23/2015): Note that this is not a count of the SgVariableSymbol IR nodes.
                              int numberOfSymbolsWithSameName = (int)currentScope->count_symbol(name);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("SgVarRefExp's SgDeclarationStatement: numberOfSymbolsWithSameName       = %d \n",numberOfSymbolsWithSameName);
#endif

                              if ((numberOfSymbolsWithSameName - numberOfAliasSymbols) > 1 && amountOfNameQualificationRequired == 0)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("WARNING: name qualification can be required when there are multiple base classes with the same referenced variable via SgVariableSymbol \n");
                                 }
                              if ((numberOfSymbolsWithSameName - numberOfAliasSymbols) > 1 && amountOfNameQualificationRequired == 0)
                                 {
                                   mfprintf(mlog [ WARN ] ) ("   --- numberOfSymbolsWithSameName       = %d \n",numberOfSymbolsWithSameName);
                                   mfprintf(mlog [ WARN ] ) ("   --- amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
                                 }
                            }
                         ROSE_ASSERT(numberOfAliasSymbols <= 1 || amountOfNameQualificationRequired > 0);
                       }
                  }
                 else
                  {
                 // DQ (7/23/2011): This case happens when the SgVarRefExp can not be associated with a statement.
                 // I think this only happens when a constant variable is used in an array index of an array type.
#if 0
                    mfprintf(mlog [ WARN ] ) ("Case of TransformationSupport::getStatement(varRefExp) == NULL explictlySpecifiedCurrentScope = %p \n",explictlySpecifiedCurrentScope);
#endif
                 // DQ (7/24/2011): This fails for the tests/nonsmoke/functional/CompileTests/OpenMP_tests/objectLastprivate.cpp test code.
                 // ASSERT_not_null(explictlySpecifiedCurrentScope);
                    if (explictlySpecifiedCurrentScope != NULL)
                       {
                      // DQ (4/19/2019): Now that we (optionally) also pass in the explictlySpecifiedCurrentStatement, we might want to use it directly.
#if 0
                         mfprintf(mlog [ WARN ] ) ("case of SgVarRefExp: Using explictlySpecifiedCurrentScope for the value of currentStatement: need to check this! \n");
#endif
                         currentStatement = explictlySpecifiedCurrentScope;

                         SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
                         ASSERT_not_null(variableSymbol);

                         SgInitializedName* initializedName = variableSymbol->get_declaration();
                         ASSERT_not_null(initializedName);

                         SgNode * parent = initializedName->get_parent();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("In case SgVarRefExp: (currentStatement == NULL) Calling nameQualificationDepth() variableDeclaration = %p initializedName->get_parent() = %p = %s \n", variableDeclaration, parent, parent ? parent->class_name().c_str() : "");
#endif

                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(parent);
                         SgDeclarationScope * decl_scope = isSgDeclarationScope(parent);
                         if (variableDeclaration != NULL) {
                           int amountOfNameQualificationRequired = nameQualificationDepth(variableDeclaration,explictlySpecifiedCurrentScope,currentStatement);
                           setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);
                         } else if (decl_scope != NULL) {
                           // NOP that is a nontype template parameter
                         } else {
                           SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(parent);
                           SgTemplateClassDefinition * tpldef = isSgTemplateClassDefinition(parent);
                           SgTemplateParameter* tplParam = isSgTemplateParameter(parent);
                           SgTemplateInstantiationDefn * templateInstantiationDefn = isSgTemplateInstantiationDefn(parent);
                           SgEnumDeclaration * enumDecl = isSgEnumDeclaration(parent);

                           int amountOfNameQualificationRequired = nameQualificationDepth(initializedName,explictlySpecifiedCurrentScope,currentStatement);
                           if (functionParameterList != NULL) {
                             setNameQualification(varRefExp,functionParameterList,amountOfNameQualificationRequired);
                           } else if (tpldef != NULL) {
                             mfprintf(mlog [ WARN ] )("WARNING: In NameQualificationTraversal::evaluateInheritedAttribute: Found SgInitializedName whose parent is a template class definition. It does not sound right!!!\n");
                             ASSERT_not_null(tpldef->get_parent());
                             SgDeclarationStatement * tpldecl = isSgDeclarationStatement(tpldef->get_parent());
                             ASSERT_not_null(tpldecl);
                             ROSE_ASSERT(isSgTemplateClassDeclaration(tpldecl));
                             setNameQualification(varRefExp,tpldecl,amountOfNameQualificationRequired);
                           } else if (templateInstantiationDefn != NULL) {
                             setNameQualification(varRefExp,templateInstantiationDefn->get_declaration(),amountOfNameQualificationRequired);
                           } else if (tplParam != NULL) {
#if 0
                             mfprintf(mlog [ WARN ] )("tplParam = %p (%s)\n", tplParam, tplParam ? tplParam->class_name().c_str() : "");
#endif
                             ASSERT_not_null(tplParam->get_parent());
                             SgDeclarationStatement * tpldecl = isSgDeclarationStatement(tplParam->get_parent());
                             ASSERT_not_null(tpldecl);
#if 0
                             mfprintf(mlog [ WARN ] )("tpldecl = %p (%s)\n", tpldecl, tpldecl ? tpldecl->class_name().c_str() : "");
#endif
                             ROSE_ASSERT(
                                 isSgTemplateFunctionDeclaration(tpldecl) ||
                                 isSgTemplateMemberFunctionDeclaration(tpldecl) ||
                                 isSgTemplateClassDeclaration(tpldecl) ||
                                 isSgTemplateTypedefDeclaration(tpldecl) ||
                                 isSgTemplateVariableDeclaration(tpldecl) ||
                                 isSgNonrealDecl(tpldecl)
                             );
                             setNameQualification(varRefExp,tpldecl,amountOfNameQualificationRequired);
                           } else if (enumDecl != NULL) {
                             setNameQualification(varRefExp, enumDecl, amountOfNameQualificationRequired);
//                             setNameQualification(varRefExp, isSgScopeStatement(enumDecl->get_parent()), amountOfNameQualificationRequired);
                           } else {
                          // mfprintf(mlog [ WARN ] )("ERROR: Unexpected parent for SgInitializedName: parent = %p (%s)\n", parent, parent ? parent->class_name().c_str() : "");
                             printf("ERROR: Unexpected parent for SgInitializedName: parent = %p (%s)\n", parent, parent ? parent->class_name().c_str() : "");
                             ROSE_ABORT();
                           }
                         }
                       }
                      else if (variableDeclaration != NULL)
                       {
                         int amountOfNameQualificationRequired = nameQualificationDepth(variableDeclaration,explictlySpecifiedCurrentScope,currentStatement);
                         setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);
                       }
                      else
                       {
                         mfprintf(mlog [ WARN ] )("WARNING: Unexpected conditions in NameQualificationTraversal::evaluateInheritedAttribute.\n");
                       }
                  }
             }
        }

  // DQ (6/9/2011): Added support for test2011_79.C (enum values can require name qualification).
     SgEnumVal* enumVal = isSgEnumVal(n);
     if (enumVal != NULL)
        {
          SgScopeStatement* currentScope = NULL;

          SgEnumDeclaration* enumDeclaration = enumVal->get_declaration();
          ASSERT_not_null(enumDeclaration);

          SgStatement* currentStatement = TransformationSupport::getStatement(enumVal);
       // ASSERT_not_null(currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("case of SgEnumVal: currentStatement = %p \n",currentStatement);
#endif


          if (currentStatement != NULL)
             {
               currentScope = isSgScopeStatement(currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("case of SgEnumVal: currentStatement = %p = %s currentScope = %p = %s \n",
                    currentStatement,currentStatement->class_name().c_str(),currentScope,currentScope != NULL ? currentScope->class_name().c_str() : "NULL");
#endif
            // If the current statement was not a scope, then what scope contains the current statement.
               if (currentScope == NULL)
                  {
                 // DQ (5/24/2013): This is a better way to set the scope (see test2013_187.C).
                 // currentScope = currentStatement->get_scope();
                    ASSERT_not_null(inheritedAttribute.get_currentScope());
                    currentScope = inheritedAttribute.get_currentScope();
                  }
               ASSERT_not_null(currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("case of SgEnumVal (after setting currentScope): currentStatement = %p = %s currentScope = %p = %s \n",
                    currentStatement,currentStatement->class_name().c_str(),currentScope,currentScope != NULL ? currentScope->class_name().c_str() : "NULL");
#endif
               ASSERT_not_null(inheritedAttribute.get_currentScope());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("case of SgEnumVal : inheritedAttribute.get_currentScope() = %p = %s \n",
                       inheritedAttribute.get_currentScope(),inheritedAttribute.get_currentScope()->class_name().c_str());
#endif
             }
            else
             {
            // If the enum value is contained in an index expression then currentStatement will be NULL.
            // But then the current scope should be known explicitly.
               currentScope = explictlySpecifiedCurrentScope;

            // DQ (9/17/2011); Added escape for where the currentScope == NULL (fails for STL code when the original expression trees are used to eliminate the constant folded values).
            // ASSERT_not_null(currentScope);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
            // DQ (4/19/2019): Now that we (optionally) also pass in the explictlySpecifiedCurrentStatement, we might want to use it directly.
               mfprintf(mlog [ WARN ] ) ("case of SgEnumVal: Using explictlySpecifiedCurrentScope for the value of currentStatement: need to check this! \n");
#endif
            // Use the currentScope as the currentStatement
               currentStatement = currentScope;
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("currentStatement = %p \n",currentStatement);
          if (currentStatement != NULL)
             {
               mfprintf(mlog [ WARN ] ) ("currentStatement = %p = %s = %s \n",currentStatement,currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
             }


          mfprintf(mlog [ WARN ] ) ("currentScope = %p \n",currentScope);
          if (currentScope != NULL)
             {
               mfprintf(mlog [ WARN ] ) ("currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
             }
#endif

       // DQ (9/17/2011); Added escape for where the currentScope == NULL (fails for STL code when the original expression trees are used to eliminate the constant folded values).
       // ASSERT_not_null(currentScope);
          if (currentScope != NULL)
             {
            // DQ (9/17/2011): this is the original case we waant to restore later...
               ASSERT_not_null(currentScope);

            // We need to look up the qualification for the enum name and not the enum declaration (which may have a different name (or no name).

            // DQ (7/8/2019): Ideally this would form an iteration over the scopes from the current scope through the scopes connect via the base class.

            // DQ (7/8/2019): Added varialbe to store the contribution to name qualification from ambiguity, as uposed to visability.
               int additionalNameQualificationToResolveAmbiguity = 0;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("######################################################## \n");
               mfprintf(mlog [ WARN ] ) ("Testing the EnumVal name instead of the Enum declaration \n");
               mfprintf(mlog [ WARN ] ) ("######################################################## \n");
#endif
               SgName enumVal_name = enumVal->get_name();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("enumVal_name = %s \n",enumVal_name.str());
#endif
                  {
                 // If there was no symbol, then there was no ambiguity to force the name qualification.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("If there was no symbol, then there was no ambiguity to force the name qualification in the current scope directly \n");
#endif
                 // DQ (8/16/2013): Build the template parameters and template arguments as appropriate (will be NULL pointers for some types of declarations).
                    SgTemplateParameterPtrList* templateParameterList = NULL; // SageBuilder::getTemplateParameterList(declaration);
                    SgTemplateArgumentPtrList*  templateArgumentList  = NULL; // SageBuilder::getTemplateArgumentList(declaration);

                    SgEnumDeclaration* enumDeclaration = enumVal->get_declaration();
                    ASSERT_not_null(enumDeclaration);
                    SgScopeStatement* enumDeclarationScope = enumDeclaration->get_scope();
                    ASSERT_not_null(enumDeclarationScope);

                    SgSymbol* symbolFromEnumDeclarationScope = enumDeclarationScope->lookup_enum_field_symbol(enumVal_name);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    if (symbolFromEnumDeclarationScope == NULL)
                       {
                         mfprintf(mlog [ WARN ] ) ("name qualification: case of SgEnumVal: Found case of enumVal_name = %s not in scope of enumDeclaration = %p = %s \n",
                                 enumVal_name.str(),enumDeclaration,enumDeclaration->class_name().c_str());
                         mfprintf(mlog [ WARN ] ) (" --- symbolFromEnumDeclarationScope == NULL \n");
                       }
#endif
                 // ASSERT_not_null(symbolFromEnumDeclarationScope);

                    SgSymbol* symbolFromParents = SageInterface::lookupSymbolInParentScopes(enumVal_name,currentScope,templateParameterList,templateArgumentList);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("symbolFromParents = %p \n",symbolFromParents);
#endif
                    if (symbolFromParents != NULL && symbolFromEnumDeclarationScope != NULL && symbolFromParents != symbolFromEnumDeclarationScope)
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("Found a reason for adding name qualification \n");
#endif
                         additionalNameQualificationToResolveAmbiguity++;
                       }
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("############################################################## \n");
               mfprintf(mlog [ WARN ] ) ("DONE: Testing the EnumVal name instead of the Enum declaration \n");
               mfprintf(mlog [ WARN ] ) ("############################################################## \n");
#endif

               int amountOfNameQualificationRequired = nameQualificationDepth(enumDeclaration,currentScope,currentStatement);

               if (amountOfNameQualificationRequired == 0)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Specify name qualification to resolve ambiguity: additionalNameQualificationToResolveAmbiguity = %d \n",additionalNameQualificationToResolveAmbiguity);
#endif
                    amountOfNameQualificationRequired = additionalNameQualificationToResolveAmbiguity;
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("SgEnumVal: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
               setNameQualification(enumVal,enumDeclaration,amountOfNameQualificationRequired);
             }
            else
             {
            // DQ (9/17/2011): Added this case, print a warning and fix thiat after debugging the constant folding value elimination..
               mfprintf(mlog [ WARN ] ) ("WARNING: SgEnumVal name qualification not handled for the case of currentScope == NULL \n");
             }

        }


  // DQ (6/2/2011): Handle the range of expressions that can reference types that might require name qualification...
     SgNewExp*   newExp   = isSgNewExp(n);
     SgSizeOfOp* sizeOfOp = isSgSizeOfOp(n);
     SgCastExp*  castExp  = isSgCastExp(n);
     SgTypeIdOp* typeIdOp = isSgTypeIdOp(n);
     if (newExp != NULL || sizeOfOp != NULL || castExp != NULL || typeIdOp != NULL)
        {
          SgExpression* referenceToType = isSgExpression(n);

          bool skipQualification = false;

          SgType* qualifiedType = NULL;
          switch(n->variantT())
             {
               case V_SgNewExp:
                  {
                    qualifiedType = newExp->get_specified_type();
                    break;
                  }

               case V_SgSizeOfOp:
                  {
                    qualifiedType = sizeOfOp->get_operand_type();
                    if (qualifiedType == NULL) {
                      ASSERT_not_null(sizeOfOp->get_operand_expr());
                      skipQualification = true;
                    }
                    break;
                  }

               case V_SgTypeIdOp:
                  {
                    qualifiedType = typeIdOp->get_operand_type();
                    if (qualifiedType == NULL) {
                      ASSERT_not_null(typeIdOp->get_operand_expr());
                      skipQualification = true;
                    }
                    break;
                  }

               case V_SgCastExp:
                  {
                    qualifiedType = castExp->get_type();
                    break;
                  }

               default:
                  {
                 // Anything else should not make it this far...
                    mfprintf(mlog [ WARN ] ) ("Error: default reached in switch... n = %p = %s \n",n,n->class_name().c_str());
                    ROSE_ABORT();
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          if (qualifiedType == NULL)
             {
            // We see this case for test2006_139.C  (code is: "sizeof("string")" or "sizeof(<SgVarRefExp>)" ).
               mfprintf(mlog [ WARN ] ) ("Note: qualifiedType == NULL for n = %p = %s \n",n,n->class_name().c_str());
             }
#endif
       // ASSERT_not_null(qualifiedType);

          if (skipQualification == false)
             {
            // DQ (1/26/2013): added assertion.
               ASSERT_not_null(qualifiedType);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("before stripType(): qualifiedType = %p = %s \n",qualifiedType,qualifiedType->class_name().c_str());
#endif
            // DQ (5/19/2019): Comment this out since it causes the cast to loose the information about casts of pointer types.
            // See test2019_433.C.
#if 0
               mfprintf(mlog [ WARN ] ) ("Comment out strip type: qualifiedType->stripType(SgType::STRIP_POINTER_TYPE) \n");
#endif
            // qualifiedType = qualifiedType->stripType(SgType::STRIP_POINTER_TYPE);
               SgType* strippedQualifiedType = qualifiedType->stripType(SgType::STRIP_POINTER_TYPE);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("after stripType():  qualifiedType         = %p = %s \n",qualifiedType,qualifiedType->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("after stripType():  strippedQualifiedType = %p = %s \n",strippedQualifiedType,strippedQualifiedType->class_name().c_str());
#endif

            // DQ (4/26/2019): This variable is not used.
            // DQ (4/16/2019): If the qualifiedType is a SgPointerMemberType, then we need to have this return the declaration
            // associated with the base type.
            // SgDeclarationStatement* associatedTypeDeclaration = associatedDeclaration(qualifiedType);
               SgDeclarationStatement* associatedTypeDeclaration = associatedDeclaration(strippedQualifiedType);

            // DQ (4/15/2019): Adding SgPointerMemberType support for the few expressions that contain explicit references to types
            // SgPointerMemberType* pointerMemberType = isSgPointerMemberType(qualifiedType);
               SgPointerMemberType* pointerMemberType = isSgPointerMemberType(strippedQualifiedType);
               if (pointerMemberType != NULL)
                  {
                    SgStatement*      currentStatement = TransformationSupport::getStatement(referenceToType);
                    SgScopeStatement* currentScope     = currentStatement->get_scope();

                    ASSERT_not_null(currentScope);
                    generateNestedTraversalWithExplicitScope(pointerMemberType,currentScope,currentStatement,referenceToType);
                 // DQ (4/19/2019): It might be that we should call this after the traveral over each type instead of before we traverse the type.
                 // This way we save the correctly computed string for each type after the different parts of name qualificaiton are in place.
                    traverseType(pointerMemberType,referenceToType,currentScope,currentStatement);
                  }
                 else
                  {
                 // DQ (4/27/2019): Turn on this code that was previously disabled (does not address SgPointerMemberType support requirements.
#if 1
                 // SgDeclarationStatement* associatedTypeDeclaration = associatedDeclaration(qualifiedType);
                    if (associatedTypeDeclaration != NULL)
                       {
#if 0
                         mfprintf(mlog [ WARN ] ) ("##################################################################################### \n");
                         mfprintf(mlog [ WARN ] ) ("Case SgExpression: containing reference to type: processing associatedTypeDeclaration \n");
                         mfprintf(mlog [ WARN ] ) ("##################################################################################### \n");
#endif
#if 0
                         mfprintf(mlog [ WARN ] ) ("associatedTypeDeclaration = %p = %s = %s \n",associatedTypeDeclaration,
                              associatedTypeDeclaration->class_name().c_str(),SageInterface::get_name(associatedTypeDeclaration).c_str());
#endif
                         SgStatement* currentStatement = TransformationSupport::getStatement(n);

                      // ASSERT_not_null(currentStatement);
                         if (currentStatement != NULL)
                            {
                              SgScopeStatement* currentScope = currentStatement->get_scope();
                              if (currentScope != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("INFO: currentStatement = %p (%s)\n", currentStatement, currentStatement->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("INFO: currentScope     = %p (%s)\n", currentScope, currentScope->class_name().c_str());
#endif

                                   int amountOfNameQualificationRequiredForType = nameQualificationDepth(associatedTypeDeclaration,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("SgExpression (name = %s) type: amountOfNameQualificationRequiredForType = %d \n",referenceToType->class_name().c_str(),amountOfNameQualificationRequiredForType);
#endif
                                   setNameQualification(referenceToType,associatedTypeDeclaration,amountOfNameQualificationRequiredForType);
#if 0
                                   mfprintf(mlog [ WARN ] ) ("Calling traverseType on referenceToType = %p = %s \n",referenceToType,referenceToType->class_name().c_str());
                                   mfprintf(mlog [ WARN ] ) ("Calling traverseType on qualifiedType   = %p = %s \n",qualifiedType,qualifiedType->class_name().c_str());
#endif
                                // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
                                   traverseType(qualifiedType,referenceToType,currentScope,currentStatement);
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   mfprintf(mlog [ WARN ] ) ("WARNING: currentStatement->get_scope() == NULL for case of referenceToType = %p = %s \n",referenceToType,referenceToType->class_name().c_str());
#endif
                                 }
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("WARNING: currentStatement == NULL for case of referenceToType = %p = %s \n",referenceToType,referenceToType->class_name().c_str());
#endif
                            }
                       }
                      else
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("Note: associatedTypeDeclaration == NULL in SgExpression for name qualification support! referenceToType = %s \n",referenceToType->class_name().c_str());
#endif
                       }
#endif
#if 1
                 // DQ (4/27/2019): This has been moved from above since it should be extended to include this code which is supporting the non-SgPointerMemberType.
                  }
#endif
             }
        }


  // DQ (6/21/2011): Added support for name qualification of expressions contained in originalExpressionTree's where they are stored.
     SgExpression* expression = isSgExpression(n);
     if (expression != NULL)
        {
          SgExpression* originalExpressionTree = expression->get_originalExpressionTree();
          if (originalExpressionTree != NULL)
             {
            // Note that we have to pass the local copy of the referencedNameSet so that the same set will be used for all recursive calls (see test2011_89.C).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@ Recursive call to the originalExpressionTree = %p = %s \n",originalExpressionTree,originalExpressionTree->class_name().c_str());
#endif
               SgStatement* currentStatement = TransformationSupport::getStatement(n);
            // DQ (9/14/2015): Added debugging code.
            // DQ (9/14/2015): This can be an expression in a type, in which case we don't have an associated scope.
               if (currentStatement == NULL)
                  {
                 // This can be an expression in a type, in which case we don't have an associated scope.
#if 0
                    mfprintf(mlog [ WARN ] ) ("Note: This can be an expression in a type, in which case we don't have an associated scope: expression = %p = %s originalExpressionTree = %p = %s \n",
                         expression,expression->class_name().c_str(),originalExpressionTree,originalExpressionTree->class_name().c_str());
#endif
                  }
                 else
                  {
                    ASSERT_not_null(currentStatement);
                    SgScopeStatement* currentScope = currentStatement->get_scope();
                    ASSERT_not_null(currentScope);

                    generateNestedTraversalWithExplicitScope(originalExpressionTree,currentScope);
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@ DONE: Recursive call to the originalExpressionTree = %p = %s \n",originalExpressionTree,originalExpressionTree->class_name().c_str());
#endif
             }
        }

  // DQ (6/25/2011): Added support for use from unparseToString().
  // I don't think that we need this case since the unparser handles the case of using
  // the fully qualified name directly when called from the unparseToString() function.
     SgType* type = isSgType(n);
     if (type != NULL)
        {
#if 0
          mfprintf(mlog [ WARN ] ) ("Found a type in the evaluation of name qualification type = %p = %s \n",type,type->class_name().c_str());
#endif
       // void NameQualificationTraversal::traverseType ( SgType* type, SgNode* nodeReferenceToType, SgScopeStatement* currentScope, SgStatement* positionStatement )
       // SgNode* nodeReferenceToType    = NULL;
       // SgScopeStatement* currentScope = NULL;
       // SgStatement* positionStatement = NULL;
       // traverseType(type,initializedName,currentScope,currentStatement);
        }

  // ******************************************************************************
  // Now that this declaration is processed, mark it as being seen (place into set).
  // ******************************************************************************

  // DQ (2/13/2019): I think that this kind of declaration was not previously processed for name qualification.
  // Likely missed because enums previously could not have a prototype declaration (but can under C++11).
     SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(n);
     if (enumDeclaration != NULL)
        {
#if 0
          mfprintf(mlog [ WARN ] ) ("Case of SgEnumDeclaration: enumDeclaration              = %p \n",enumDeclaration);
          mfprintf(mlog [ WARN ] ) (" --- enumDeclaration->get_firstNondefiningDeclaration() = %p \n",enumDeclaration->get_firstNondefiningDeclaration());
          mfprintf(mlog [ WARN ] ) (" --- enumDeclaration->get_definingDeclaration()         = %p \n",enumDeclaration->get_definingDeclaration());
#endif
       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(enumDeclaration->get_parent());

          if (currentScope == NULL)
             {
            // DQ (2/18/2019): Adding support for when the SgEnumDeclaration is defined in another declaration (e.g. SgTypedefDeclaration).
               SgNode* parent = enumDeclaration->get_parent();
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(parent);
               if (typedefDeclaration != NULL)
                  {
                    currentScope = isSgScopeStatement(typedefDeclaration->get_parent());

                 // DQ (2/18/2019): We should have a valid currentScope at this point.
                    if (currentScope == NULL)
                       {
                         mfprintf(mlog [ WARN ] ) ("NOTE: Could not identify scope for enum declaration: parent = %p = %s \n",parent,parent->class_name().c_str());
                         ROSE_ABORT();
                       }
                      else
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("Found SgEnumDeclaration in SgTypedefDeclaration: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                       }
                  }
                 else
                  {
                 // DQ (2/19/2019): This is frequently a SgLambdaExp or a SgVariableDeclaration
                 // Computing the current scope does not always seem possible.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("In name qualification: Cannot compute a valid scope for the enumDeclaration = %p = %s \n",enumDeclaration,enumDeclaration->class_name().c_str());
                    mfprintf(mlog [ WARN ] ) (" --- parent = %p = %s \n",parent,parent->class_name().c_str());
#endif
                  }
             }

       // ASSERT_not_null(currentScope);
          if (currentScope != NULL)
             {
            // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same
            // as the scope of the class declaration.  However, the analysis should work and determin that the
            // required name qualification length is zero.

            // DQ (7/22/2017): Refactored this code.
               SgScopeStatement* enum_scope = enumDeclaration->get_scope();

            // DQ (7/22/2017): I think we can assert this.
               ASSERT_not_null(enum_scope);
#if 0
               mfprintf(mlog [ WARN ] ) ("currentScope                 = %p = %s \n",currentScope,currentScope->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("enumDeclaration->get_scope() = %p = %s \n",enum_scope,enum_scope->class_name().c_str());
#endif
            // if (currentScope != classDeclaration->get_scope())
               if (currentScope != enum_scope)
                  {
                 // DQ (1/21/2013): We should be able to assert this.
                    ASSERT_not_null(enumDeclaration->get_scope());

                 // DQ (1/21/2013): Added new static function to support testing for equivalent when the scopes are namespaces.
                    bool isSameNamespace = SgScopeStatement::isEquivalentScope(currentScope,enumDeclaration->get_scope());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("isSameNamespace = %s \n",isSameNamespace ? "true" : "false");
#endif
                 // DQ (1/21/2013): Added code to support when equivalent namespaces are detected.
                    if (isSameNamespace == false)
                       {
                      // DQ (6/11/2013): Added test to make sure that name qualification is ignored for friend function where the class has not yet been seen.
                      // if (classDeclaration->get_declarationModifier().isFriend() == false)
                         SgDeclarationStatement* declarationForReferencedNameSet = enumDeclaration->get_firstNondefiningDeclaration();
                         ASSERT_not_null(declarationForReferencedNameSet);
                         if (referencedNameSet.find(declarationForReferencedNameSet) != referencedNameSet.end())
                            {
                              int amountOfNameQualificationRequired = nameQualificationDepth(enumDeclaration,currentScope,enumDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("SgEnumDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                              setNameQualification(enumDeclaration,amountOfNameQualificationRequired);
                            }
                       }
                  }
                 else
                  {
                 // Don't know what test code exercises this case (see test2011_62.C).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("WARNING: SgEnumDeclaration -- currentScope is not available through predicate (currentScope != enumDeclaration->get_scope()), not clear why! \n");
                    mfprintf(mlog [ WARN ] ) ("Commenting out: enumDeclaration->get_parent() == enumDeclaration->get_scope() in name qualitication \n");
                    mfprintf(mlog [ WARN ] ) ("name qualification for enumDeclaration->get_scope()  = %p = %s \n",enumDeclaration->get_scope(),enumDeclaration->get_scope()->class_name().c_str());
                    mfprintf(mlog [ WARN ] ) ("enumDeclaration->get_parent()                        = %p = %s \n",enumDeclaration->get_parent(),enumDeclaration->get_parent()->class_name().c_str());
#endif
                 // DQ (7/22/2017): I think the template arguments name qualification can be required. This fixes test2017_56.C.
                    int amountOfNameQualificationRequired = nameQualificationDepth(enumDeclaration,currentScope,enumDeclaration);
                 // We only really wanted to make sure that any template arguments were properly name qualified.
                    amountOfNameQualificationRequired = 0;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("NEW CASE: currentScope != enumDeclaration->get_scope(): SgEnumDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                    setNameQualification(enumDeclaration,amountOfNameQualificationRequired);
                  }
             }
            else
             {
            // DQ (2/13/2019): I think this can happen if the enum declaration is in a typedef declaration
            // or parameter list, etc (less common places to find enum declarations).

            // NOTE: Cxx_tests/test2019_125.C demonstrates where this kind of enum requires name qualification.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("WARNING: SgEnumDeclaration -- currentScope is not available, not clear why! \n");
#endif
#if 0
               mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@ Cannot determine current scope for SgEnumDeclaration (likely a enum in a typedef declaration) \n");
#endif
            // enumDeclaration->get_file_info()->display("Cannot determine current scope for SgEnumDeclaration");

               SgDeclarationStatement* outerDeclaration = isSgDeclarationStatement(enumDeclaration->get_parent());
               ASSERT_not_null(outerDeclaration);
               currentScope = isSgScopeStatement(outerDeclaration->get_parent());
               ASSERT_not_null(currentScope);

               int amountOfNameQualificationRequired = nameQualificationDepth(enumDeclaration,currentScope,enumDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("NEW CASE: currentScope != enumDeclaration->get_scope(): SgEnumDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
               setNameQualification(enumDeclaration,amountOfNameQualificationRequired);
             }
        }


     SgDeclarationStatement* declaration = isSgDeclarationStatement(n);
     if (declaration != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
          mfprintf(mlog [ WARN ] ) ("Found a SgDeclarationStatement in the evaluation of name qualification declaration = %p = %s \n",declaration,declaration->class_name().c_str());
          printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
#endif
       // If this is a declaration of something that has a name then we need to mark it as having been seen.

       // In some cases of C++ name qualification depending on if the defining declaration (or a forward
       // declaration is in a scope that would define the declaration to a scope where the declaration could be
       // present).  This detail is handled by reporting if such a declaration has been seen yet.  Since the
       // preorder traversal is the same as the traversal used in the unparsing it is sufficient to record
       // the order of the processing here and not complicate the unparser directly.  Note that the use of
       // function declarations follow these rules and so are a problem when the prototype is defined in a
       // function (where it does not communicate the defining declarations location) instead of in a global
       // scope or namespace scope (where it does appear to communicate its position.

       // SgDeclarationStatement* firstNondefiningDeclaration   = declaration->get_firstNondefiningDeclaration();
          SgDeclarationStatement* declarationForReferencedNameSet = declaration->get_firstNondefiningDeclaration();
       // ROSE_ASSERT(declarationForReferencedNameSet == NULL);
       // declarationForReferencedNameSet = declaration->get_firstNondefiningDeclaration();

          if (declarationForReferencedNameSet == NULL)
             {
            // Note that a function with only a defining declaration will not have a nondefining declaration
            // automatically constructed in the AST (unlike classes and some other sorts of declarations).
               declarationForReferencedNameSet = declaration->get_definingDeclaration();

            // DQ (6/22/2011): I think this is true.  This assertion fails for test2006_78.C (a template example code).
            // ROSE_ASSERT(declarationForReferencedNameSet == declaration);

            // DQ (6/23/2011): This assertion fails for the LoopProcessor on tests/nonsmoke/functional/roseTests/loopProcessingTests/mm.C
            // ASSERT_not_null(declarationForReferencedNameSet);
               if (declarationForReferencedNameSet == NULL)
                  {
                    declarationForReferencedNameSet = declaration;
                    ASSERT_not_null(declarationForReferencedNameSet);
                  }
               ASSERT_not_null(declarationForReferencedNameSet);
             }
          ASSERT_not_null(declarationForReferencedNameSet);
       // Look at each declaration, but as soon as we find an acceptable one put the declarationForReferencedNameSet
       // into the set so that we can search on a uniform representation of the declaration. Note that we want the
       // scope of where it is located and not it's scope if it were name qualified...
       // SgScopeStatement* scopeOfNondefiningDeclaration = isSgScopeStatement(firstNondefiningDeclaration->get_parent());
          ASSERT_not_null(declaration->get_parent());
          SgScopeStatement* scopeOfDeclaration = isSgScopeStatement(declaration->get_parent());

       // DQ (5/19/2017): added support for test2017_39.C (SgTemplateInstantiationDirectiveStatement support).
       // In this case the SgTemplateInstantiation is a declaration hidden inside of the SgTemplateInstantiationDirectiveStatement.
          SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(declaration->get_parent());
          if (templateInstantiationDirective != NULL)
             {
               scopeOfDeclaration = isSgScopeStatement(templateInstantiationDirective->get_parent());
               ASSERT_not_null(scopeOfDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("************* Found SgTemplateInstantiationDirectiveStatement: reset scope to that of the template instantiation directive: scope = %p = %s  \n",
                    scopeOfDeclaration,scopeOfDeclaration->class_name().c_str());
#endif
             }

          bool acceptableDeclarationScope = false;

       // I think that some declarations might not appear in a scope properly (e.g pointer to function, etc.)
       // ASSERT_not_null(scopeOfNondefiningDeclaration);
          if (scopeOfDeclaration != NULL)
             {
               switch(scopeOfDeclaration->variantT())
                  {
                 // At least this case is not allowed.
                    case V_SgBasicBlock: acceptableDeclarationScope = false;
                         break;

                 // Everything else is OK!
                 // DQ (6/22/2011): Note that a declaration in a typedef is an acceptable scope under some cases (not clear on the limits of this case).
                    default:
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         mfprintf(mlog [ WARN ] ) ("scopeOfNondefiningDeclaration = %p = %s \n",scopeOfDeclaration,scopeOfDeclaration->class_name().c_str());
#endif
                         acceptableDeclarationScope = true;
                       }
                  }
             }
            else
             {
            // This appears to fail for something in rose_edg_required_macros_and_functions.h.

            // DQ (2/18/2019): This case happens when an enum declaration is contained as the base type in a typedef declaration.
            // In which case the scope is just the scope of the enclosing typedef declaration.
               SgNode* parent = declaration->get_parent();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("scopeOfDeclaration == NULL: declaration               = %p = %s \n",declaration,declaration->class_name().c_str());
               mfprintf(mlog [ WARN ] ) ("scopeOfDeclaration == NULL: declaration->get_parent() = %p = %s \n",parent,parent->class_name().c_str());
#endif

            // DQ (2/18/2019): Chasing down all the things that can be the parent when the scope of a declaration computed via the parent is not clear.
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(parent);
               if (typedefDeclaration != NULL)
                  {
                    scopeOfDeclaration = isSgScopeStatement(typedefDeclaration->get_parent());
                  }
                 else
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parent);
                    if (functionDeclaration != NULL)
                       {
                         ASSERT_not_null(functionDeclaration);
                         scopeOfDeclaration = isSgScopeStatement(functionDeclaration->get_parent());
                         if (scopeOfDeclaration == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                              mfprintf(mlog [ WARN ] ) ("test 1: SgFunctionDeclaration: scopeOfDeclaration == NULL: cannot support name qualification: functionDeclaration->get_parent() = %p = %s \n",
                                   functionDeclaration->get_parent(),functionDeclaration->get_parent()->class_name().c_str());
#endif
                            }
                       }
                      else
                       {
                         SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(parent);
                         if (functionParameterList != NULL)
                            {
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionParameterList->get_parent());
                              ASSERT_not_null(functionDeclaration);
                              scopeOfDeclaration = isSgScopeStatement(functionDeclaration->get_parent());
                              if (scopeOfDeclaration == NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                                   mfprintf(mlog [ WARN ] ) ("test 2: SgFunctionParameterList: SgFunctionDeclaration: scopeOfDeclaration == NULL: cannot support name qualification: functionDeclaration->get_parent() = %p = %s \n",
                                        functionDeclaration->get_parent(),functionDeclaration->get_parent()->class_name().c_str());
#endif
                                 }
                            }
                       }
                  }

               if (scopeOfDeclaration == NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                    mfprintf(mlog [ WARN ] ) ("scopeOfDeclaration == NULL: Could not identify scope of declaration to support name qualification: parent = %p = %s \n",parent,parent->class_name().c_str());
#endif
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("lost scope: scopeOfDeclaration = %p = %s \n",scopeOfDeclaration,scopeOfDeclaration->class_name().c_str());
#endif
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("I hope that we can make this an error (scopeOfDeclaration == NULL) declaration = %p = %s declaration->get_parent() = %p = %s \n",
                    declaration,declaration->class_name().c_str(),declaration->get_parent(),declaration->get_parent()->class_name().c_str());
#endif
             }
#if 0
          printf ("acceptableDeclarationScope                                                         = %s \n",acceptableDeclarationScope ? "true" : "false");
          printf ("referencedNameSet.find(declarationForReferencedNameSet) == referencedNameSet.end() = %s \n",referencedNameSet.find(declarationForReferencedNameSet) == referencedNameSet.end() ? "true" : "false");
#endif
          ASSERT_not_null(declarationForReferencedNameSet);
          if (acceptableDeclarationScope == true && referencedNameSet.find(declarationForReferencedNameSet) == referencedNameSet.end())
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Adding declarationForReferencedNameSet = %p = %s to set of visited declarations \n",declarationForReferencedNameSet,declarationForReferencedNameSet->class_name().c_str());
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               printf ("INSERTING INTO referencedNameSet: declarationForReferencedNameSet = %p = %s name = %s \n",
                    declarationForReferencedNameSet,declarationForReferencedNameSet->class_name().c_str(),SageInterface::get_name(declarationForReferencedNameSet).c_str());
               printf (" --- Could have used: declaration = %p = %s name = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
               referencedNameSet.insert(declarationForReferencedNameSet);
             }
            else
             {
            // mfprintf(mlog [ WARN ] ) ("firstNondefiningDeclaration = %p NOT added to referencedNameSet \n",firstNondefiningDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
               mfprintf(mlog [ WARN ] ) ("declarationForReferencedNameSet = %p NOT added to referencedNameSet \n",declarationForReferencedNameSet);
               printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
#endif
             }
        }

  // DQ (7/12/2014): Add any possible nodes that can generate SgAliasSymbols to the SgSymbolTable::p_aliasSymbolCausalNodeSet .
  // This is used by the symbol table to know when to use or ignore SgAliasSymbols in symbol table lookups.
     if (isSgUsingDirectiveStatement(n) != NULL || isSgUsingDeclarationStatement(n) != NULL || isSgBaseClass(n) != NULL)
        {
          SgSymbolTable::get_aliasSymbolCausalNodeSet().insert(n);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::evaluateInheritedAttribute(): Added SgAliasSymbols causal node = %p = %s to SgSymbolTable::p_aliasSymbolCausalNodeSet size = %" PRIuPTR " \n",
               n,n->class_name().c_str(),SgSymbolTable::get_aliasSymbolCausalNodeSet().size());
#endif

          if (isSgBaseClass(n) != NULL)
             {
               mfprintf(mlog [ WARN ] ) ("NameQualificationTraversal::evaluateInheritedAttribute(): Identified SgBaseClass in traversal \n");
               ROSE_ABORT();
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("****************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("Leaving NameQualificationTraversal::evaluateInheritedAttribute(): node = %p = %s \n",n,n->class_name().c_str());
     mfprintf(mlog [ WARN ] ) ("******************************************************\n\n\n");
#endif

     return NameQualificationInheritedAttribute(inheritedAttribute);
   }


NameQualificationSynthesizedAttribute
NameQualificationTraversal::evaluateSynthesizedAttribute(SgNode* n, NameQualificationInheritedAttribute inheritedAttribute, SynthesizedAttributesList synthesizedAttributeList)
   {
  // This is not used now but will likely be used later.
  // NameQualificationSynthesizedAttribute returnAttribute;
     NameQualificationSynthesizedAttribute returnAttribute(n);

  // DQ (8/2/2020): Added assertion.
     ROSE_ASSERT(n != NULL);

// #if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
#if 0
     mfprintf(mlog [ WARN ] ) ("\n\n****************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("****************************************************** \n");
     mfprintf(mlog [ WARN ] ) ("Inside of NameQualificationTraversal::evaluateSynthesizedAttribute(): node = %p = %s = %s \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str());
     mfprintf(mlog [ WARN ] ) ("****************************************************** \n");
#endif

  // DQ (8/14/2025): This is an optimization to skip the traversal of the AST outside of what is in the source tree.
     if (suppressNameQualificationAcrossWholeTranslationUnit == true)
        {
       // SgStatement* statement = isSgStatement(n);
          SgLocatedNode* locatedNode = isSgLocatedNode(n);
       // if (statement != NULL)
          if (locatedNode != NULL)
             {
            // DQ (8/14/2025): Adding support to count the number of statements traversed in the name qualification when using traverseInputFile().
            // It should be only the statements in the source file, but it appears to include statements marked as compilerGenerated.
            // AstPerformance::numberOfStatementsProcessedInNameQualificationUsingTraverseInputFile++;

            // if (statement->get_file_info()->get_filenameString() != "compilerGenerated")
            // if (statement->isCompilerGenerated() == true)
               if (locatedNode->isCompilerGenerated() == false)
                  {
                 // We could just check is the nearest parent statement is compiler generated.
                 // Or we could see if this is from a header file...(let's not do that).
                    SgStatement* statement = SageInterface::getEnclosingStatement(locatedNode);
                    if (statement->isCompilerGenerated() == false)
                       {
#if 0
                         printf("Inside of NameQualificationTraversal::evaluateSynthesizedAttribute(): counter = %d node = %s = %s = %p \n",
                              AstPerformance::numberOfStatementsProcessedInNameQualificationUsingTraverseInputFile,n->class_name().c_str(),SageInterface::get_name(n).c_str(),n);
                         printf(" --- statement->get_file_info()->get_filenameString() = %s \n",statement->get_file_info()->get_filenameString().c_str());
#endif
                       }
                      else
                       {
                         return returnAttribute;
                       }
                  }
                 else
                  {
                    return returnAttribute;
                  }
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
  // DQ (6/23/2013): Output the generated name with required name qualification for debugging.
     switch(n->variantT())
        {
       // DQ (8/19/2013): Added case to support debugging.
          case V_SgConstructorInitializer:

          case V_SgMemberFunctionRefExp:
             {
               mfprintf(mlog [ WARN ] ) ("************************************************************************************** \n");
               mfprintf(mlog [ WARN ] ) ("In evaluateSynthesizedAttribute(): node = %p = %s node->unparseToString() = %s \n",n,n->class_name().c_str(),n->unparseToString().c_str());
               mfprintf(mlog [ WARN ] ) ("************************************************************************************** \n");
               break;
             }

          default:
             {
            // do nothing
             }
        }
#endif

  // DQ (4/21/2019): I now think that we don't need this, but how is the referenceNode a SgPointerMemberType.
  // NOTE: We might also just call this once when n is the same as a saved referenceNode.
     SgNode* referenceNode = inheritedAttribute.get_referenceNode();
     if (n == referenceNode)
        {
          ROSE_ABORT();
        }

  // Iterate over the synthesizedAttributeList.
     SynthesizedAttributesList::iterator i = synthesizedAttributeList.begin();
     while (i != synthesizedAttributeList.end())
        {
          NameQualificationSynthesizedAttribute synthesizedAttribute = *i;
       // ROSE_ASSERT(synthesizedAttribute != NULL);
          SgNode* synthesizedAttributeNode = synthesizedAttribute.node;

       // DQ (8/2/2020): Not clear why this can be NULL.
       // ROSE_ASSERT(synthesizedAttributeNode != NULL);

          if (synthesizedAttributeNode != NULL)
             {
               SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(synthesizedAttributeNode);
               if (namespaceAliasDeclaration != NULL)
                  {
                    SgDeclarationStatement* declaration = namespaceAliasDeclaration->get_is_alias_for_another_namespace_alias() ?
                                                               isSgDeclarationStatement(namespaceAliasDeclaration->get_namespaceAliasDeclaration()) :
                                                               isSgDeclarationStatement(namespaceAliasDeclaration->get_namespaceDeclaration());
                    ROSE_ASSERT(declaration != NULL);
                    if (namespaceAliasDeclarationMap.find(declaration) != namespaceAliasDeclarationMap.end())
                       {
                         namespaceAliasMapType::iterator i = namespaceAliasDeclarationMap.find(declaration);
                         ROSE_ASSERT(i != namespaceAliasDeclarationMap.end());
                         namespaceAliasDeclarationMap.erase(i);
                       }
                  }
             }

          i++;
        }

     ROSE_ASSERT(returnAttribute.node != NULL);

     return returnAttribute;
   }


#define DEBUG_TRAVERSE_NONREAL_FOR_SCOPE 0

// Dealing with nonreal's scope: the scope is where the nonreal is instantiated this code either:
//  - Uses the scope of the associated template, if it exists
//  - Traverse nonreal parent using while loop, else
SgScopeStatement * traverseNonrealDeclForCorrectScope(SgDeclarationStatement * declaration) {
  SgScopeStatement * scope = declaration->get_scope();
  ASSERT_not_null(scope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_NONREAL_FOR_SCOPE
  mfprintf(mlog [ WARN ] ) ("In traverseNonrealDeclForCorrectScope():\n");
  mfprintf(mlog [ WARN ] ) (" --- declaration = %p (%s)\n", declaration, declaration->class_name().c_str());
  mfprintf(mlog [ WARN ] ) (" --- scope = %p (%s)\n", scope, scope->class_name().c_str());
#endif

  SgNonrealDecl * nrdecl = isSgNonrealDecl(declaration);
  while (nrdecl != NULL) {
    if (nrdecl->get_templateDeclaration() == NULL) {
      SgDeclarationScope * decl_scope = isSgDeclarationScope(nrdecl->get_scope());
      ASSERT_not_null(decl_scope);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_NONREAL_FOR_SCOPE
      mfprintf(mlog [ WARN ] ) (" --- decl_scope = %p (%s)\n", decl_scope,decl_scope->class_name().c_str());
#endif

      SgNode * decl_scope_parent = decl_scope->get_parent();
      ASSERT_not_null(decl_scope_parent);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_NONREAL_FOR_SCOPE
      mfprintf(mlog [ WARN ] ) (" --- decl_scope_parent = %p (%s)\n", decl_scope_parent, decl_scope_parent->class_name().c_str());
#endif

      SgTemplateClassDeclaration * tcdecl_parent = isSgTemplateClassDeclaration(decl_scope_parent);
      SgTemplateFunctionDeclaration * tfdecl_parent = isSgTemplateFunctionDeclaration(decl_scope_parent);
      SgTemplateMemberFunctionDeclaration * tmfdecl_parent = isSgTemplateMemberFunctionDeclaration(decl_scope_parent);
      SgTemplateTypedefDeclaration * ttddecl_parent = isSgTemplateTypedefDeclaration(decl_scope_parent);
      SgTemplateVariableDeclaration * tvdecl_parent = isSgTemplateVariableDeclaration(decl_scope_parent);
      SgNonrealDecl * nr_parent = isSgNonrealDecl(decl_scope_parent);
      if (nr_parent != NULL) {
        ROSE_ASSERT(nr_parent != nrdecl); // LOOP in the nonreal declaration: forbidden
        nrdecl = nr_parent;
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_NONREAL_FOR_SCOPE
        mfprintf(mlog [ WARN ] ) (" --- nrdecl = %p (%s)\n", nrdecl, nrdecl->class_name().c_str());
#endif
      } else {
        ROSE_ASSERT(tcdecl_parent || tfdecl_parent || tmfdecl_parent || ttddecl_parent || tvdecl_parent);
        break;
      }
    } else {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_NONREAL_FOR_SCOPE
      mfprintf(mlog [ WARN ] ) (" --- nrdecl->get_templateDeclaration() = %p (%s)\n", nrdecl->get_templateDeclaration(), nrdecl->get_templateDeclaration()->class_name().c_str());
#endif

      scope = nrdecl->get_templateDeclaration()->get_scope();
      ASSERT_not_null(scope);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TRAVERSE_NONREAL_FOR_SCOPE
      mfprintf(mlog [ WARN ] ) (" --- scope = %p (%s)\n", scope, scope->class_name().c_str());
#endif

      break;
    }
  }

  return scope;
}

// ************************************************************************************
//    These overloaded functions, setNameQualification(), support references to IR
// nodes that require name qualification.  Each function inserts a qualified name
// (string) into a map stored as a static data member in SgNode. For each IR node
// that is qualified, the reference to the IR node carries the name qualification
// (is used as a key in the map of qualified names).  There are two maps, one for
// the qualification of names and one for qualification of types.  Note that, since
// types are shared, it is more clear that the type can't carry the qualified name
// because it could be different at each location where the type is referenced; thus
// the reference to the type carries the qualified name (via the map).  The case of
// why named IR constructs have to have there qualified name in the IR node referencing
// the named construct is similar.
//
// They are only a few IR nodes that reference IR nodes that can be qualified:
//    SgExpression IR nodes:
//       SgVarRefExp
//       SgFunctionRefExp
//       SgMemberFunctionRefExp
//       SgConstructorInitializer
//       SgNewExp
//       SgCastExp
//       SgSizeOfOp
//       SgTypeIdOp
//
//    SgDeclarationStatement IR nodes:
//       SgFunctionDeclaration (for the function name)
//       SgFunctionDeclaration (for the return type)
//       SgUsingDeclarationStatement (for references to a declaration (e.g. namespace or class))
//       SgUsingDeclarationStatement (for references to a SgInitializedName)
//       SgUsingDirectiveStatement
//       SgVariableDeclaration
//       SgTypedefDeclaration
//       SgClassDeclaration
//       SgEnumDeclaration
//
//    SgStatement IR nodes:
//       SgForInitStatement is not a problems since it is a list of SgInitializedName
//
//    SgLocatedNode nodes:
//       SgInitializedName
//
//    SgSupport nodes:
//       SgBaseClass
//       SgTemplateArgument
//
// Other (not yet supported) IR nodes recognized to reference types that could require name
// qualification support:
//    SgExpression IR nodes:
//
//    And maybe also:
//       SgPseudoDestructorRefExp
//       SgTemplateParameter
//
// Note also that name qualifiction can be required on expressions that are a part of
// the originalExpressionTree that represent the expanded representation from constant
// folding.  Thus we have to make a recursive call on all valid originalExpressionTree
// pointers where they are present:
//     SgBinaryOp
//     SgValueExp
//     SgFunctionRefExp
//     SgValueExp
//     SgCastExp
// ************************************************************************************


// DQ (4/20/2019): For where the input is a type (SgPointerMemberType) we need to pass in a
// referenceNode for what will become the key in the map for the name qualification prefix.
void NameQualificationTraversal::setNameQualificationOnClassOf ( SgPointerMemberType* pointerMemberType, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // DQ (4/19/2019): Adding support for chains of SpPointerMemberType types (requires type traversal).

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgPointerMemberType*) \n");
#endif
#if DEBUG_NONTERMINATION
     printf("In setNameQualification(SgPointerMemberType*): calling setNameQualificationSupport() \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pointerMemberType: outputNameQualificationLength  = %d \n",outputNameQualificationLength);
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pointerMemberType: outputTypeEvaluation           = %s \n",outputTypeEvaluation ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pointerMemberType: outputGlobalQualification      = %s \n",outputGlobalQualification ? "true" : "false");

     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): qualifier = %s \n",qualifier.c_str());
#endif

     if (qualifiedNameMapForNames.find(pointerMemberType) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for (SgPointerMemberType) name = %s into list at IR node = %p = %s \n",qualifier.c_str(),pointerMemberType,pointerMemberType->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(pointerMemberType,qualifier));

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Testing name in map: for SgPointerMemberType = %p qualified name = %s \n",pointerMemberType,pointerMemberType->get_qualified_name_prefix_for_class_of().str());
          mfprintf(mlog [ WARN ] ) ("SgNode::get_globalQualifiedNameMapForNames().size() = %" PRIuPTR " \n",SgNode::get_globalQualifiedNameMapForNames().size());
#endif
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(pointerMemberType);
       // ROSE_ASSERT (i != qualifiedNameMapForNames.end());
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(pointerMemberType);
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(pointerMemberType);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(pointerMemberType);
       // ROSE_ASSERT (i != qualifiedNameMapForTypes.end());
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 3: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
            // DQ (3/15/2019): We need to disable the assertion below because it can happen (see Cxx11_tests/test2019_214.C).
               i->second = qualifier;
             }
        }
   }



void NameQualificationTraversal::setNameQualificationOnBaseType ( SgPointerMemberType* pointerMemberType, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // DQ (4/19/2019): Adding support for chains of SpPointerMemberType types (requires type traversal).

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgPointerMemberType*) \n");
#endif
#if DEBUG_NONTERMINATION
     printf("In setNameQualification(SgPointerMemberType*): calling setNameQualificationSupport() \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pointerMemberType: outputNameQualificationLength  = %d \n",outputNameQualificationLength);
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pointerMemberType: outputTypeEvaluation           = %s \n",outputTypeEvaluation ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pointerMemberType: outputGlobalQualification      = %s \n",outputGlobalQualification ? "true" : "false");

     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): qualifier = %s \n",qualifier.c_str());
#endif

#if 0
     mfprintf(mlog [ WARN ] ) ("Checking qualifiedNameMapForTypes using pointerMemberType (should be what pointed to this type) \n");
#endif

     if (qualifiedNameMapForTypes.find(pointerMemberType) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for (SgPointerMemberType) name = %s into list at IR node = %p = %s \n",qualifier.c_str(),pointerMemberType,pointerMemberType->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST T1: Inserting qualifier for (SgPointerMemberType) name = %s into list at IR node = %p = %s \n",qualifier.c_str(),pointerMemberType,pointerMemberType->class_name().c_str());
#endif
       // qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(pointerMemberType,qualifier));
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(pointerMemberType,qualifier));

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
       // mfprintf(mlog [ WARN ] ) ("Testing name in map: for SgPointerMemberType = %p qualified name = %s \n",pointerMemberType,pointerMemberType->get_qualified_name_prefix().str());
          mfprintf(mlog [ WARN ] ) ("Testing name in map: for SgPointerMemberType = %p qualified name = %s \n",pointerMemberType,pointerMemberType->get_qualified_name_prefix_for_base_type().str());
       // mfprintf(mlog [ WARN ] ) ("SgNode::get_globalQualifiedNameMapForNames().size() = %" PRIuPTR " \n",SgNode::get_globalQualifiedNameMapForNames().size());
          mfprintf(mlog [ WARN ] ) ("SgNode::get_globalQualifiedNameMapForTypes().size() = %" PRIuPTR " \n",SgNode::get_globalQualifiedNameMapForTypes().size());
#endif
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(pointerMemberType);
       // ROSE_ASSERT (i != qualifiedNameMapForNames.end());
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(pointerMemberType);
          NameQualificationMapType::iterator i = qualifiedNameMapForTypes.find(pointerMemberType);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 3: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
            // DQ (3/15/2019): We need to disable the assertion below because it can happen (see Cxx11_tests/test2019_214.C).
               i->second = qualifier;
             }
        }
   }




void
NameQualificationTraversal::setNameQualification(SgVarRefExp* varRefExp, SgVariableDeclaration* variableDeclaration, int amountOfNameQualificationRequired)
   {
  // This is where we hide the details of translating the intepretation of the amountOfNameQualificationRequired
  // which can be greater than the number of nested scopes to a representation that is bounded by the number of
  // nested scopes and sets the global qualification to be true. If I decide I don't like this here, then we
  // might find a way to handling this point more directly later. This at least gets it set properly in the AST.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // DQ (7/31/2012): check if this is a SgVarRefExp that is associated with a class that is un-named, if so then
  // supress the name qualification (which would use the internally generated name).  Note that all constructs
  // that are un-named have names generated internally for them so that we can support the AST merge process
  // and generally reference multiple un-named constructs that may exist in a single compilation unit.
  // SgClassDeclaration* classDeclaration = isSgClassDeclaration(varRefExp->parent());

     ASSERT_not_null(varRefExp);
     SgBinaryOp* dotExp   = isSgDotExp(varRefExp->get_parent());
     SgBinaryOp* arrowExp = isSgArrowExp(varRefExp->get_parent());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgVarRefExp*) \n");
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("dotExp = %p arrowExp = %p \n",dotExp,arrowExp);
#endif

     SgVarRefExp* possibleClassVarRefExp = NULL;
     if (dotExp != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Note that this code is overly sensitive to the local structure of the AST expressions \n");
#endif
          possibleClassVarRefExp = isSgVarRefExp(dotExp->get_lhs_operand());

          if (possibleClassVarRefExp == NULL)
             {
               SgPntrArrRefExp* possiblePntrArrRefExp = isSgPntrArrRefExp(dotExp->get_lhs_operand());
               if (possiblePntrArrRefExp != NULL)
                  {
                    possibleClassVarRefExp = isSgVarRefExp(possiblePntrArrRefExp->get_lhs_operand());
                  }
             }
        }

     if (arrowExp != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Note that this code is overly sensitive to the local structure of the AST expressions \n");
#endif
          possibleClassVarRefExp = isSgVarRefExp(arrowExp->get_lhs_operand());

          if (possibleClassVarRefExp == NULL)
             {
               SgPntrArrRefExp* possiblePntrArrRefExp = isSgPntrArrRefExp(arrowExp->get_lhs_operand());
               if (possiblePntrArrRefExp != NULL)
                  {
                    possibleClassVarRefExp = isSgVarRefExp(possiblePntrArrRefExp->get_lhs_operand());
                  }
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("possibleClassVarRefExp = %p \n",possibleClassVarRefExp);
#endif

     SgClassType* classType = NULL;
     if (possibleClassVarRefExp != NULL)
        {
          SgType* varRefExpType = possibleClassVarRefExp->get_type();
          ASSERT_not_null(varRefExpType);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): varRefExpType = %p = %s \n",varRefExpType,varRefExpType->class_name().c_str());
#endif

       // DQ (8/2/2012): test2007_06.C and test2012_156.C show that we need to strip past the typedefs.
       // Note that we don't want to strip typedefs, since that could take us past public types and into private types.
       // SgType* possibleClassType = varRefExpType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE); // Excluding SgType::STRIP_TYPEDEF_TYPE
          SgType* possibleClassType = varRefExpType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_TYPEDEF_TYPE);
          classType = isSgClassType(possibleClassType);
        }


     bool isAnUnamedConstructs = false;
     if (classType != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): classType = %p = %s \n",classType,classType->class_name().c_str());
#endif
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
          ASSERT_not_null(classDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
#endif

       // DQ (9/4/2012): I don't think that the defining declaration should have to exist.
       // However this was a previously passing test for all of the regression tests.
       // ASSERT_not_null(classDeclaration->get_definingDeclaration());
          if (classDeclaration->get_definingDeclaration() != NULL)
             {
               SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
               if (definingClassDeclaration == NULL)
                  {
                    mfprintf(mlog [ WARN ] ) ("ERROR: definingClassDeclaration == NULL: classDeclaration->get_definingDeclaration() = %p = %s \n",classDeclaration->get_definingDeclaration(),classDeclaration->get_definingDeclaration()->class_name().c_str());
                  }
               ASSERT_not_null(definingClassDeclaration);

            // This should be true so assert this here.
               ROSE_ASSERT(classDeclaration->get_isUnNamed() == definingClassDeclaration->get_isUnNamed());
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("WARNING: classDeclaration->get_definingDeclaration() == NULL: This was a previously passing test, but not now that we have force SgTemplateTypes to be handled in the local type table. \n");
#endif
             }

          if (classDeclaration->get_isUnNamed() == true)
             {
               isAnUnamedConstructs = true;
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): isAnUnamedConstructs = %s \n",isAnUnamedConstructs ? "true" : "false");
#endif

  // DQ (7/31/2012): If this is an un-named construct then no qualifiaction can be used since there is no associated name.
     if (isAnUnamedConstructs == false)
        {
          SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(variableDeclaration);
          string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

          varRefExp->set_global_qualification_required(outputGlobalQualification);
          varRefExp->set_name_qualification_length(outputNameQualificationLength);

       // There should be no type evaluation required for a variable reference, as I recall.
          ROSE_ASSERT(outputTypeEvaluation == false);
          varRefExp->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_name_qualification_length()     = %d \n",varRefExp->get_name_qualification_length());
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_type_elaboration_required()     = %s \n",varRefExp->get_type_elaboration_required() ? "true" : "false");
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_global_qualification_required() = %s \n",varRefExp->get_global_qualification_required() ? "true" : "false");
#endif

          if (qualifiedNameMapForNames.find(varRefExp) == qualifiedNameMapForNames.end())
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),varRefExp,varRefExp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
               printf("TEST 5: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),varRefExp,varRefExp->class_name().c_str());
#endif
               qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(varRefExp,qualifier));
             }
            else
             {
            // DQ (6/20/2011): We see this case in test2011_87.C.
            // If it already existes then overwrite the existing information.
            // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(varRefExp);
               NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(varRefExp);
               ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               string previousQualifier = i->second.c_str();
               mfprintf(mlog [ WARN ] ) ("WARNING: test 2: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
               if (i->second != qualifier)
                  {
                 // DQ (7/23/2011): Multiple uses of the SgVarRefExp expression in SgArrayType will cause
                 // the name qualification to be reset each time.  This is OK since it is used to build
                 // the type name that will be saved.
                    i->second = qualifier;
#if 0
                    mfprintf(mlog [ WARN ] ) ("Note: name in qualifiedNameMapForNames already exists and is different... \n");
#endif
                  }
             }
        }
       else
        {
               SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(variableDeclaration);
               string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

               varRefExp->set_global_qualification_required(outputGlobalQualification);
               varRefExp->set_name_qualification_length(outputNameQualificationLength);

            // There should be no type evaluation required for a variable reference, as I recall.
               ROSE_ASSERT(outputTypeEvaluation == false);
               varRefExp->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_name_qualification_length()     = %d \n",varRefExp->get_name_qualification_length());
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_type_elaboration_required()     = %s \n",varRefExp->get_type_elaboration_required() ? "true" : "false");
               mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_global_qualification_required() = %s \n",varRefExp->get_global_qualification_required() ? "true" : "false");
#endif

               if (qualifiedNameMapForNames.find(varRefExp) == qualifiedNameMapForNames.end())
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),varRefExp,varRefExp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
                    printf("TEST 6: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),varRefExp,varRefExp->class_name().c_str());
#endif
                    qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(varRefExp,qualifier));
                  }
                 else
                  {
                 // DQ (6/20/2011): We see this case in test2011_87.C.
                 // If it already existes then overwrite the existing information.
                 // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(varRefExp);
                    NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(varRefExp);
                    ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    string previousQualifier = i->second.c_str();
                    mfprintf(mlog [ WARN ] ) ("WARNING: test 2: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
                    if (i->second != qualifier)
                       {
                      // DQ (7/23/2011): Multiple uses of the SgVarRefExp expression in SgArrayType will cause
                      // the name qualification to be reset each time.  This is OK since it is used to build
                      // the type name that will be saved.
                         i->second = qualifier;
                         mfprintf(mlog [ WARN ] ) ("Note: name in qualifiedNameMapForNames already exists and is different... \n");
                       }
                  }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgFunctionRefExp* functionRefExp, SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
   {
  // This is where we hide the details of translating the intepretation of the amountOfNameQualificationRequired
  // which can be greater than the number of nested scopes to a representation that is bounded by the number of
  // nested scopes and sets the global qualification to be true. If I decide I don't like this here, then we
  // might find a way to handling this point more directly later. This at least gets it set properly in the AST.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgFunctionRefExp*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(functionDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     functionRefExp->set_global_qualification_required(outputGlobalQualification);
     functionRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     functionRefExp->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionRefExp->get_name_qualification_length()     = %d \n",functionRefExp->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionRefExp->get_type_elaboration_required()     = %s \n",functionRefExp->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionRefExp->get_global_qualification_required() = %s \n",functionRefExp->get_global_qualification_required() ? "true" : "false");
#endif

  // DQ (5/2/2012): I don't think that global qualification is allowed for friend functions (so test for this).
  // test2012_59.C is an example of this issue.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_declarationModifier().isFriend() = %s \n",functionDeclaration->get_declarationModifier().isFriend() ? "true" : "false");
     if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_firstNondefiningDeclaration()->get_declarationModifier().isFriend() = %s \n",functionDeclaration->get_firstNondefiningDeclaration()->get_declarationModifier().isFriend() ? "true" : "false");
     if (functionDeclaration->get_definingDeclaration() != NULL)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_definingDeclaration()->get_declarationModifier().isFriend()         = %s \n",functionDeclaration->get_definingDeclaration()->get_declarationModifier().isFriend() ? "true" : "false");
#endif

  // Look for friend declaration on both declaration (defining and non-defining).
     bool isFriend = false;
     if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
        {
          isFriend = isFriend || functionDeclaration->get_firstNondefiningDeclaration()->get_declarationModifier().isFriend();
        }
     if (functionDeclaration->get_definingDeclaration() != NULL)
        {
          isFriend = isFriend || functionDeclaration->get_definingDeclaration()->get_declarationModifier().isFriend();
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("isFriend                                       = %s \n",isFriend ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("functionDeclaration->get_definingDeclaration() = %p \n",functionDeclaration->get_definingDeclaration());
#endif

  // DQ (4/2/2014): After discusion with Markus, this is a problem that is a significant
  // issue and requires a general solution that would be useful more generally than just
  // to this specific problem.  We need to build a data stucture and hide it behind a
  // class with an appropriate API.  The data structure should have a container of
  // non-defining declarations for each first-non-defining declaration.  Thus we would
  // have a way to find all of the non-defining declarations associated with any
  // function and thus query if one of them was declard in a scope that defined its
  // scope definatively.  This class should be a part of an AST Information sort of
  // object that we would use to collect similar analysis information that would be
  // seperate from the AST, but might be used with the AST for certain purposes
  // (e.g. removning all functions including all associated non-defining declarations).

  // DQ (4/6/2014): Adding support for new analysis results. Fails for test2013_242.C.
     ASSERT_not_null(declarationSet);
     if (qualifiedNameMapForNames.find(functionRefExp) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for (SgFunctionRefExp) name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionRefExp,functionRefExp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 7: Inserting qualifier for (SgFunctionRefExp) name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionRefExp,functionRefExp->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionRefExp,qualifier));

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Testing name in map: for SgFunctionRefExp = %p qualified name = %s \n",functionRefExp,functionRefExp->get_qualified_name_prefix().str());
          mfprintf(mlog [ WARN ] ) ("SgNode::get_globalQualifiedNameMapForNames().size() = %" PRIuPTR " \n",SgNode::get_globalQualifiedNameMapForNames().size());
#endif
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(functionRefExp);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(functionRefExp);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 3: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
            // DQ (3/15/2019): We need to disable the assertion below because it can happen (see Cxx11_tests/test2019_214.C).
               i->second = qualifier;
             }
        }
   }

void
NameQualificationTraversal::setNameQualification(SgMemberFunctionRefExp* functionRefExp, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
   {
  // This is where we hide the details of translating the intepretation of the amountOfNameQualificationRequired
  // which can be greater than the number of nested scopes to a representation that is bounded by the number of
  // nested scopes and sets the global qualification to be true. If I decide I don't like this here, then we
  // might find a way to handling this point more directly later. This at least gets it set properly in the AST.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgMemberFunctionRefExp*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(functionDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     functionRefExp->set_global_qualification_required(outputGlobalQualification);
     functionRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     functionRefExp->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): memberFunctionRefExp->get_name_qualification_length()     = %d \n",functionRefExp->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): memberFunctionRefExp->get_type_elaboration_required()     = %s \n",functionRefExp->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): memberFunctionRefExp->get_global_qualification_required() = %s \n",functionRefExp->get_global_qualification_required() ? "true" : "false");
#endif
     if (qualifiedNameMapForNames.find(functionRefExp) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting (memberFunction) qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionRefExp,functionRefExp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 8: Inserting (memberFunction) qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionRefExp,functionRefExp->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionRefExp,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(functionRefExp);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(functionRefExp);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 4: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 1
               mfprintf(mlog [ WARN ] ) ("NOTE: test 4: replacing previousQualifier = %s with new qualifier = %s \n",i->second.c_str(),qualifier.c_str());
#endif
               i->second = qualifier;
               mfprintf(mlog [ WARN ] ) (" --- Name qualificaiton was previously and error: we may need to set it to something different: qualifier = %s \n",qualifier.c_str());
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgPseudoDestructorRefExp* pseudoDestructorRefExp, SgDeclarationStatement* declarationStatement, int amountOfNameQualificationRequired)
   {
  // This is where we hide the details of translating the intepretation of the amountOfNameQualificationRequired
  // which can be greater than the number of nested scopes to a representation that is bounded by the number of
  // nested scopes and sets the global qualification to be true. If I decide I don't like this here, then we
  // might find a way to handling this point more directly later. This at least gets it set properly in the AST.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgPseudoDestructorRefExp*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declarationStatement);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     pseudoDestructorRefExp->set_global_qualification_required(outputGlobalQualification);
     pseudoDestructorRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     pseudoDestructorRefExp->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pseudoDestructorRefExp->get_name_qualification_length()     = %d \n",pseudoDestructorRefExp->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pseudoDestructorRefExp->get_type_elaboration_required()     = %s \n",pseudoDestructorRefExp->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): pseudoDestructorRefExp->get_global_qualification_required() = %s \n",pseudoDestructorRefExp->get_global_qualification_required() ? "true" : "false");
#endif
     if (qualifiedNameMapForNames.find(pseudoDestructorRefExp) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting (pseudoDestructorRefExp) qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),pseudoDestructorRefExp,pseudoDestructorRefExp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 9: Inserting (pseudoDestructorRefExp) qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),pseudoDestructorRefExp,pseudoDestructorRefExp->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(pseudoDestructorRefExp,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(pseudoDestructorRefExp);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(pseudoDestructorRefExp);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 4.5: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("NOTE: test 4.5: replacing previousQualifier = %s with new qualifier = %s \n",i->second.c_str(),qualifier.c_str());
#endif
               i->second = qualifier;
               mfprintf(mlog [ WARN ] ) (" --- Name qualificaiton was previously and error: we may need to set it to something different: qualifier = %s \n",qualifier.c_str());
             }
        }
   }


// DQ (6/4/2011): This function handles a specific case that is demonstrated by test2005_42.C.
// DQ (6/1/2011): Added support for qualification of the SgConstructorInitializer.
void
NameQualificationTraversal::setNameQualification(SgConstructorInitializer* constructorInitializer, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // DQ (6/4/2011): This handles the case of both the declaration being a SgMemberFunctionDeclaration and a SgClassDeclaration.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgConstructorInitializer*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope, amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     constructorInitializer->set_global_qualification_required(outputGlobalQualification);
     constructorInitializer->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     constructorInitializer->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): constructorInitializer->get_name_qualification_length()     = %d \n",constructorInitializer->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): constructorInitializer->get_type_elaboration_required()     = %s \n",constructorInitializer->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): constructorInitializer->get_global_qualification_required() = %s \n",constructorInitializer->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(constructorInitializer) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),constructorInitializer,constructorInitializer->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 10: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),constructorInitializer,constructorInitializer->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(constructorInitializer,qualifier));
        }
       else
        {
       // DQ (2/12/2012): Fixing support where the name qualification must be rewritten where it is used in a different context.
       // this appears to be a common requirement.  This case appears to not have been a problem before but is now with the
       // new EDG 4.3 support.  This has been added because of the requirements of that support.

       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(constructorInitializer);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(constructorInitializer);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 5: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               mfprintf(mlog [ WARN ] ) ("Error: name in qualifiedNameMapForNames already exists and is different...\n");
               mfprintf(mlog [ WARN ] ) (">>>> %s\n", i->second.c_str());
               mfprintf(mlog [ WARN ] ) (">>>> %s\n", qualifier.c_str());
               i->second = qualifier;
             }
        }

  // DQ (6/4/2011): Added test...
     ROSE_ASSERT(SgNode::get_globalQualifiedNameMapForNames().find(constructorInitializer) != SgNode::get_globalQualifiedNameMapForNames().end());
   }


void
NameQualificationTraversal::setNameQualification(SgEnumVal* enumVal, SgEnumDeclaration* enumDeclaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgEnumVal*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(enumDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     enumVal->set_global_qualification_required(outputGlobalQualification);
     enumVal->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     enumVal->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): enumVal->get_name_qualification_length()     = %d \n",enumVal->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): enumVal->get_type_elaboration_required()     = %s \n",enumVal->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): enumVal->get_global_qualification_required() = %s \n",enumVal->get_global_qualification_required() ? "true" : "false");
#endif
     if (qualifiedNameMapForNames.find(enumVal) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),enumVal,enumVal->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 11: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),enumVal,enumVal->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(enumVal,qualifier));
        }
       else
        {
       // If it already exists then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(enumVal);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(enumVal);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test6: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
            // DQ (5/3/2013): Note that this happens for test2013_144.C where the enumValue is used as the size
            // in the array type (and the SgArrayType is shared). See comments in the test code for how this
            // might be improved (forcing name qualification).
               i->second = qualifier;
             }
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgBaseClass* baseClass, SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgBaseClass*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(classDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     baseClass->set_global_qualification_required(outputGlobalQualification);
     baseClass->set_name_qualification_length(outputNameQualificationLength);
     baseClass->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): baseClass->get_name_qualification_length()     = %d \n",baseClass->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): baseClass->get_type_elaboration_required()     = %s \n",baseClass->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): baseClass->get_global_qualification_required() = %s \n",baseClass->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(baseClass) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),baseClass,baseClass->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 12: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),baseClass,baseClass->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(baseClass,qualifier));
        }
       else
        {
       // DQ (6/17/2013): I think it is reasonable that this might have been previously set and
       // we have to overwrite the last value as we handle it again in a different context.

       // If it already exists then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(baseClass);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(baseClass);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 7: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               ROSE_ABORT();
             }
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired )
   {
  // This takes only a SgFunctionDeclaration since it is where we locate the name qualification information AND
  // is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // mfprintf(mlog [ WARN ] ) ("\n************************************************ \n");

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgFunctionDeclaration*): functionDeclaration = %p \n",functionDeclaration);
     mfprintf(mlog [ WARN ] ) (" --- functionDeclaration->get_name()   = %s \n",functionDeclaration->get_name().str());
     mfprintf(mlog [ WARN ] ) (" --- amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
     mfprintf(mlog [ WARN ] ) (" --- functionDeclaration->get_definingDeclaration() = %p \n",functionDeclaration->get_definingDeclaration());
     mfprintf(mlog [ WARN ] ) (" --- functionDeclaration->get_firstNondefiningDeclaration() = %p \n",functionDeclaration->get_firstNondefiningDeclaration());
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(functionDeclaration);

  // DQ (5/28/2022): The problem is that for a member function build from scratch and added to the AST,
  // there is one level of name qualification too mucyh being requested, and it is an error to use global
  // qualification in these cases with at least GNU v10.
  // Introduce error checking to detect when there is too much name qualification being requested.
  // This needs to be fixed correctly before this point.
     ROSE_ASSERT(scope != NULL);
     SgScopeStatement* outer_scope = scope->get_scope();
     if (isSgGlobal(outer_scope) != NULL)
        {
          amountOfNameQualificationRequired = 1;
        }

     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgFunctionDeclaration*): scope = %p qualifier = %s \n",scope,qualifier.c_str());
     mfprintf(mlog [ WARN ] ) (" --- outputGlobalQualification         = %s \n",outputGlobalQualification ? "true" : "false");
     mfprintf(mlog [ WARN ] ) (" --- outputNameQualificationLength     = %d \n",outputNameQualificationLength);
     mfprintf(mlog [ WARN ] ) (" --- amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

  // DQ (9/7/2014): Added suppor for where this is a template member or non-member function declaration and we need to genrate the name with the associated template header.
     string template_header;
     SgTemplateFunctionDeclaration*       templateFunctionDeclaration       = isSgTemplateFunctionDeclaration(functionDeclaration);
     SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = isSgTemplateMemberFunctionDeclaration(functionDeclaration);
     bool buildTemplateHeaderString = (templateFunctionDeclaration != NULL || templateMemberFunctionDeclaration != NULL);
     if (buildTemplateHeaderString == true)
        {
       // DQ (9/7/2014): First idea, but not likely to work...and too complex.
       // Note that another aspect of this implementation might be that we save a set of template class
       // declarations so that we can match types in the function's parameter list against the template class declaration
       // set so that we know when to build function parameter types as template types vs. template instantiation types.
       // This would require that we save a more complex data structure than a simple string.  It is also not clear if
       // all references to a template class instantiation could be assumed to be references to it's template declaration?
       // Or maybe the problem is that there is some other function parameter lis that we need to consult.

       // DQ (9/7/2014): Better:
       // A better solution would be to make sure that we generate type in the EDG/ROSE translation using the template
       // function's paramter list associated with the first non-defining declaration (instead of the one being generated
       // as part of building the defining declaration (which is using the same a_routine_ptr as that used to build the
       // template instantiation.  As a result we a mixing the types in the defining template declaration with that of the
       // defining template instantiation (which is always wrong).  So the simple solution is to just use the types from
       // the non-defining template member or non-member function declaration.  The same should apply to the function
       // return type.  This is the simplest solution to date.

       // DQ (9/8/2014): The best solution was to translate the defining non-template function declarations when we saw them
       // as defining declarations, but only put the non-defining declaration into the class template (to match the normalization
       // done by EDG) and then attach the defining template declaration ahead of the first associated template instantiation.
       // This appears to work well and will soon be evaluated for further tests.
          template_header = setTemplateHeaderNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired);
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_declarationModifier().isFriend() = %s \n",functionDeclaration->get_declarationModifier().isFriend() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): outputNameQualificationLength                             = %d \n",outputNameQualificationLength);
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): outputGlobalQualification                                 = %s \n",outputGlobalQualification ? "true" : "false");
#endif

  // DQ (2/18/2024): Note that there may not be a nondefining declaration, and then we still don't want to output the global name qualification.
  // DQ (2/16/2013): Note that test2013_67.C is a case where name qualification of the friend function is required.
  // I think it is because it is a non defining declaration instead of a defining declaration.
  // DQ (3/31/2012): I don't think that global qualification is allowed for friend functions (so test for this).
  // test2012_57.C is an example of this issue.
  // if (outputGlobalQualification == true && functionDeclaration->get_declarationModifier().isFriend() == true)
  // if ( (outputGlobalQualification == true) && (functionDeclaration->get_declarationModifier().isFriend() == true) && (functionDeclaration == functionDeclaration->get_definingDeclaration()))
     if ( (outputGlobalQualification == true) &&
          (functionDeclaration->get_declarationModifier().isFriend() == true) &&
          ( (functionDeclaration->get_definingDeclaration() == NULL) || (functionDeclaration == functionDeclaration->get_definingDeclaration()) ) )
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("WARNING: We can't specify global qualification of friend function (qualifier reset to be empty string) \n");
#endif
       // Note that I think this might only be an issue where outputNameQualificationLength == 0.
          ROSE_ASSERT (outputNameQualificationLength == 0);

       // Reset the values (and the qualifier string).
       // outputNameQualificationLength = 0;
          outputGlobalQualification = false;
          qualifier = "";
        }

     functionDeclaration->set_global_qualification_required(outputGlobalQualification);
     functionDeclaration->set_name_qualification_length(outputNameQualificationLength);
     functionDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_name_qualification_length()     = %d \n",functionDeclaration->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_type_elaboration_required()     = %s \n",functionDeclaration->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_global_qualification_required() = %s \n",functionDeclaration->get_global_qualification_required() ? "true" : "false");

     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration = %p firstNondefiningDeclaration() = %p \n",functionDeclaration,functionDeclaration->get_firstNondefiningDeclaration());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration = %p definingDeclaration()         = %p \n",functionDeclaration,functionDeclaration->get_definingDeclaration());
#endif

     if (qualifiedNameMapForNames.find(functionDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 13: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionDeclaration,qualifier));
        }
       else
        {
       // If it already exists then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(functionDeclaration);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(functionDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());
          if (i->second != qualifier)
             {
               mfprintf(mlog [ WARN ] ) ("WARNING: test 8: replacing previousQualifier = %s with new qualifier = %s \n",i->second.c_str(),qualifier.c_str());
               mfprintf(mlog [ WARN ] ) (" --- functionDeclaration = %p = %s name = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
               ROSE_ABORT();
             }
        }

     if (buildTemplateHeaderString == true)
        {
       // Add the template header string to a new map.
#if 0
          mfprintf(mlog [ WARN ] ) ("Add the template header string to a new map: template_header = %s \n",template_header.c_str());
#endif

          if (qualifiedNameMapForTemplateHeaders.find(functionDeclaration) == qualifiedNameMapForTemplateHeaders.end())
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Inserting qualifier for template header = %s into list at IR node = %p = %s \n",template_header.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
#endif
               qualifiedNameMapForTemplateHeaders.insert(std::pair<SgNode*,std::string>(functionDeclaration,template_header));
             }
            else
             {
            // If it already exists then overwrite the existing information.
            // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTemplateHeaders.find(functionDeclaration);
               NameQualificationMapType::iterator i = qualifiedNameMapForTemplateHeaders.find(functionDeclaration);
               ROSE_ASSERT (i != qualifiedNameMapForTemplateHeaders.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               string previous_template_header = i->second.c_str();
               mfprintf(mlog [ WARN ] ) ("WARNING: test 9: replacing previousQualifier = %s with new qualifier = %s \n",previous_template_header.c_str(),template_header.c_str());
#endif
               if (i->second != template_header)
                  {
                    ROSE_ABORT();
                  }
             }
        }


  // mfprintf(mlog [ WARN ] ) ("****************** DONE ******************** \n\n");
   }

// void NameQualificationTraversal::setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired )
void
NameQualificationTraversal::setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // This takes only a SgFunctionDeclaration since it is where we locate the name qualification information AND
  // is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationReturnType(SgFunctionDeclaration*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     functionDeclaration->set_global_qualification_required_for_return_type(outputGlobalQualification);
     functionDeclaration->set_name_qualification_length_for_return_type(outputNameQualificationLength);
     functionDeclaration->set_type_elaboration_required_for_return_type(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_name_qualification_length_for_return_type()     = %d \n",functionDeclaration->get_name_qualification_length_for_return_type());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_type_elaboration_required_for_return_type()     = %s \n",functionDeclaration->get_type_elaboration_required_for_return_type() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_global_qualification_required_for_return_type() = %s \n",functionDeclaration->get_global_qualification_required_for_return_type() ? "true" : "false");
#endif

     if (qualifiedNameMapForTypes.find(functionDeclaration) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST T2: Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(functionDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(functionDeclaration);
          NameQualificationMapType::iterator i = qualifiedNameMapForTypes.find(functionDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 10: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               i->second = qualifier;
               mfprintf(mlog [ WARN ] ) ("Commented out reset of name qualification: replacing previousQualifier = %s with new qualifier = %s \n",i->second.c_str(),qualifier.c_str());
             }
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgUsingDeclarationStatement*,SgDeclarationStatement*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     usingDeclaration->set_global_qualification_required(outputGlobalQualification);
     usingDeclaration->set_name_qualification_length(outputNameQualificationLength);
     usingDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_name_qualification_length()     = %d \n",usingDeclaration->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_type_elaboration_required()     = %s \n",usingDeclaration->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_global_qualification_required() = %s \n",usingDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(usingDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDeclaration,usingDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 14: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDeclaration,usingDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDeclaration,qualifier));
        }
       else
        {
       // If it already exists then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(usingDeclaration);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(usingDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 11: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               i->second = qualifier;
               ROSE_ABORT();
             }
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgInitializedName* associatedInitializedName, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgUsingDeclarationStatement*,SgInitializedName*) \n");
#endif

     string qualifier = setNameQualificationSupport(associatedInitializedName->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     usingDeclaration->set_global_qualification_required(outputGlobalQualification);
     usingDeclaration->set_name_qualification_length(outputNameQualificationLength);
     usingDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_name_qualification_length()     = %d \n",usingDeclaration->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_type_elaboration_required()     = %s \n",usingDeclaration->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_global_qualification_required() = %s \n",usingDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(usingDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDeclaration,usingDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 15: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDeclaration,usingDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDeclaration,qualifier));
        }
       else
        {
          mfprintf(mlog [ WARN ] ) ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ABORT();
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgUsingDirectiveStatement* usingDirective, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgUsingDirectiveStatement*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     usingDirective->set_global_qualification_required(outputGlobalQualification);
     usingDirective->set_name_qualification_length(outputNameQualificationLength);
     usingDirective->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDirective->get_name_qualification_length()     = %d \n",usingDirective->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDirective->get_type_elaboration_required()     = %s \n",usingDirective->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): usingDirective->get_global_qualification_required() = %s \n",usingDirective->get_global_qualification_required() ? "true" : "false");
#endif


     if (qualifiedNameMapForNames.find(usingDirective) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDirective,usingDirective->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 16: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDirective,usingDirective->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDirective,qualifier));
        }
       else
        {
          ROSE_ASSERT(qualifiedNameMapForNames[usingDirective] == qualifier);
        }
   }


// DQ (7/8/2014): Adding support for name qualification of SgNamespaceDeclarations within a SgNamespaceAliasDeclarationStatement.
void
NameQualificationTraversal::setNameQualification ( SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgNamespaceAliasDeclarationStatement*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     namespaceAliasDeclaration->set_global_qualification_required(outputGlobalQualification);
     namespaceAliasDeclaration->set_name_qualification_length(outputNameQualificationLength);
     namespaceAliasDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): namespaceAliasDeclaration->get_name_qualification_length()     = %d \n",namespaceAliasDeclaration->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): namespaceAliasDeclaration->get_type_elaboration_required()     = %s \n",namespaceAliasDeclaration->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): namespaceAliasDeclaration->get_global_qualification_required() = %s \n",namespaceAliasDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(namespaceAliasDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),namespaceAliasDeclaration,namespaceAliasDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 17: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),namespaceAliasDeclaration,namespaceAliasDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(namespaceAliasDeclaration,qualifier));
        }
       else
        {
          mfprintf(mlog [ WARN ] ) ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ABORT();
        }
   }

// DQ (7/7/2021): Added function to trim the leading "::" from the qualified name.
static void trimLeadingGlobalNameQualification(std::string &s)
   {
     s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return ch != ':'; }));
   }

// DQ (3/31/2019): Renamed this function to make it more clear now that we have two versions, one to name
// qualify the SgInitializedName and one to name qualify the type used in the SgInitializedName.
// DQ (8/4/2012): Added support to permit global qualification to be skipped explicitly (see test2012_164.C and test2012_165.C for examples where this is important).
void
NameQualificationTraversal::setNameQualificationOnType(SgInitializedName* initializedName,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired, bool skipGlobalQualification)
   {
  // This is used to set the name qualification on the type referenced by the SgInitializedName, and not on the SgInitializedName IR node itself.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // DQ (4/28/2019): Added assertion.
     ASSERT_not_null(initializedName);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnType(SgInitializedName*): initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif

  // DQ (4/28/2019): Added assertion.
     ASSERT_not_null(declaration);

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnType(SgInitializedName*): qualifier = %s \n",qualifier.c_str());
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("declaration = %p = %s \n",declaration,declaration->class_name().c_str());
     mfprintf(mlog [ WARN ] ) ("declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
     mfprintf(mlog [ WARN ] ) ("declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());
#endif

  // unsigned int sourceSequenceForInitializedName = initializedName->get_file_info()->get_source_sequence_number();
  // unsigned int sourceSequenceForTypeDeclaration = declaration->get_file_info()->get_source_sequence_number();

  // DQ (5/15/2018): Test code test2018_65.C demonstrates that we need to suppress the
  // name qualification of the type if the defining declaration has not been seen yet.
     unsigned int sourceSequenceForInitializedName = 0;
     unsigned int sourceSequenceForTypeDeclaration = 0;
     SgDeclarationStatement* definingDeclaration = declaration->get_definingDeclaration();
     if (definingDeclaration != NULL)
        {
       // If we have a defining declaration, then query the source sequence numbers.
          ASSERT_not_null(initializedName->get_file_info());
          ASSERT_not_null(declaration->get_file_info());
          sourceSequenceForTypeDeclaration = definingDeclaration->get_file_info()->get_source_sequence_number();
          sourceSequenceForInitializedName = initializedName->get_file_info()->get_source_sequence_number();
        }
       else
        {
       // DQ (7/7/2021): If these is no defining declaration then use the nondefining declaration (important for typedefs and namespaces).
          sourceSequenceForTypeDeclaration = declaration->get_firstNondefiningDeclaration()->get_file_info()->get_source_sequence_number();
          sourceSequenceForInitializedName = initializedName->get_file_info()->get_source_sequence_number();
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("sourceSequenceForInitializedName = %u \n",sourceSequenceForInitializedName);
     mfprintf(mlog [ WARN ] ) ("sourceSequenceForTypeDeclaration = %u \n",sourceSequenceForTypeDeclaration);
#endif

  // DQ (7/7/2021): For a transformation (e.g. outlining), the values will be zero, so test for this explicitly
  // and if so, then we want to output the name qualification. Note that zero for the source sequence implaies
  // that this is a transformation.
     bool outputNameQualification = (sourceSequenceForInitializedName == 0) || sourceSequenceForTypeDeclaration < sourceSequenceForInitializedName;

  // DQ (5/15/2018): If this is a SgTemplateInstantiationTypedefDeclaration then output the name qualification.
     if (isSgTemplateInstantiationTypedefDeclaration(declaration) != NULL)
        {
          outputNameQualification = true;
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnType(SgInitializedName*): skipGlobalQualification = %s \n",skipGlobalQualification ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnType(SgInitializedName*): outputNameQualification = %s \n",outputNameQualification ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnType(SgInitializedName*): qualifier = %s \n",qualifier.c_str());
#endif

     if (skipGlobalQualification == true && outputNameQualification == false)
        {
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(SgInitializedName* initializedName): skipGlobalQualification has caused global qualification to be ignored \n");
#endif
          qualifier = "";

          outputNameQualificationLength = 0;
          outputGlobalQualification     = false;

       // Note clear if this is what we want.
          outputTypeEvaluation          = false;
        }
       else if (skipGlobalQualification)
        {
          trimLeadingGlobalNameQualification(qualifier);
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnType(SgInitializedName*): after modification: qualifier = %s \n",qualifier.c_str());
#endif

     initializedName->set_global_qualification_required_for_type(outputGlobalQualification);
     initializedName->set_name_qualification_length_for_type(outputNameQualificationLength);
     initializedName->set_type_elaboration_required_for_type(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnType(): initializedName->get_name_qualification_length_for_type()     = %d \n",initializedName->get_name_qualification_length_for_type());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnType(): initializedName->get_type_elaboration_required_for_type()     = %s \n",initializedName->get_type_elaboration_required_for_type() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnType(): initializedName->get_global_qualification_required_for_type() = %s \n",initializedName->get_global_qualification_required_for_type() ? "true" : "false");
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnType(SgInitializedName*): qualifier = %s \n",qualifier.c_str());
#endif

     if (qualifiedNameMapForTypes.find(initializedName) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for type = %s into list at SgInitializedName IR node = %p = %s \n",qualifier.c_str(),initializedName,initializedName->get_name().str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST T3: Inserting qualifier for type = %s into list at SgInitializedName IR node = %p = %s \n",qualifier.c_str(),initializedName,initializedName->get_name().str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(initializedName,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(initializedName);
          NameQualificationMapType::iterator i = qualifiedNameMapForTypes.find(initializedName);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 12: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               i->second = qualifier;

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               mfprintf(mlog [ WARN ] ) ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
#endif
             }
        }
   }


// DQ (12/17/2013): Added support for the name qualification of the SgInitializedName object when used in the context of the preinitialization list.
void
NameQualificationTraversal::setNameQualificationOnName(SgInitializedName* initializedName,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired, bool skipGlobalQualification)
   {
  // This is used to set the name qualification on the SgInitializedName directly, and not on the type referenced by the SgInitializedName IR node.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnName(SgInitializedName*): amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

  // DQ (8/4/2012): In rare cases we have to eliminate qualification only if it is going to be global qualification.
  // if (skipGlobalQualification == true && qualifier == "::")
     if (skipGlobalQualification == true)
        {
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnName(SgInitializedName* initializedName): skipGlobalQualification has caused global qualification to be ignored \n");
#endif
          qualifier = "";

          outputNameQualificationLength = 0;
          outputGlobalQualification     = false;

       // Note clear if this is what we want.
          outputTypeEvaluation          = false;
        }

     initializedName->set_global_qualification_required(outputGlobalQualification);
     initializedName->set_name_qualification_length(outputNameQualificationLength);
     initializedName->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnName(): initializedName->get_name_qualification_length()     = %d \n",initializedName->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnName(): initializedName->get_type_elaboration_required()     = %s \n",initializedName->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnName(): initializedName->get_global_qualification_required() = %s \n",initializedName->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(initializedName) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for SgInitializedName = %s into list at SgInitializedName IR node = %p = %s \n",qualifier.c_str(),initializedName,initializedName->get_name().str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 18: Inserting qualifier for SgInitializedName = %s into list at SgInitializedName IR node = %p = %s \n",qualifier.c_str(),initializedName,initializedName->get_name().str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(initializedName,qualifier));
        }
       else
        {
       // If it already exists then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(initializedName);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(initializedName);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 13: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               mfprintf(mlog [ WARN ] ) ("WARNING: name in qualifiedNameMapForNames already exists and is different... \n");
#endif
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgVariableDeclaration* variableDeclaration,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // This is used to set the name qualification on the associated SgInitializedName (there is only one per SgVariableDeclaration at present, but this may be changed (fixed) in the future.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgVariableDeclaration*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     variableDeclaration->set_global_qualification_required(outputGlobalQualification);
     variableDeclaration->set_name_qualification_length(outputNameQualificationLength);
     variableDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): variableDeclaration->get_name_qualification_length()     = %d \n",variableDeclaration->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): variableDeclaration->get_type_elaboration_required()     = %s \n",variableDeclaration->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): variableDeclaration->get_global_qualification_required() = %s \n",variableDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

  // std::map<SgNode*,std::string>::iterator it_qualifiedNameMapForNames = qualifiedNameMapForNames.find(variableDeclaration);
     NameQualificationMapType::iterator it_qualifiedNameMapForNames = qualifiedNameMapForNames.find(variableDeclaration);
     if (it_qualifiedNameMapForNames == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at SgVariableDeclaration IR node = %p = %s \n",qualifier.c_str(),variableDeclaration,variableDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 19: Inserting qualifier for name = %s into list at SgVariableDeclaration IR node = %p = %s \n",qualifier.c_str(),variableDeclaration,variableDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(variableDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(variableDeclaration);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(variableDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 14: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               i->second = qualifier;

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               mfprintf(mlog [ WARN ] ) ("WARNING: name in qualifiedNameMapForNames already exists and is different... \n");
#endif
             }
        }
   }

void
NameQualificationTraversal::setNameQualificationOnBaseType(SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnBaseType(SgTypedefDeclaration*) \n");
     mfprintf(mlog [ WARN ] ) (" --- typedefDeclaration = %p (%s)\n", typedefDeclaration, typedefDeclaration->class_name().c_str());
     mfprintf(mlog [ WARN ] ) (" --- declaration = %p (%s)\n", declaration, declaration->class_name().c_str());
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     typedefDeclaration->set_global_qualification_required_for_base_type(outputGlobalQualification);
     typedefDeclaration->set_name_qualification_length_for_base_type(outputNameQualificationLength);
     typedefDeclaration->set_type_elaboration_required_for_base_type(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnBaseType(): typedefDeclaration->get_name_qualification_length_for_base_type()     = %d \n",typedefDeclaration->get_name_qualification_length_for_base_type());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnBaseType(): typedefDeclaration->get_type_elaboration_required_for_base_type()     = %s \n",typedefDeclaration->get_type_elaboration_required_for_base_type() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnBaseType(): typedefDeclaration->get_global_qualification_required_for_base_type() = %s \n",typedefDeclaration->get_global_qualification_required_for_base_type() ? "true" : "false");
#endif

     if (qualifiedNameMapForTypes.find(typedefDeclaration) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),typedefDeclaration,typedefDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST T4: Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),typedefDeclaration,typedefDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(typedefDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(typedefDeclaration);
          NameQualificationMapType::iterator i = qualifiedNameMapForTypes.find(typedefDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 15: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
            // ROSE_ASSERT(false);

               SgName testNameInMap = typedefDeclaration->get_qualified_name_prefix();
               mfprintf(mlog [ WARN ] ) ("testNameInMap = %s \n",testNameInMap.str());
#endif
             }
        }
   }


void
NameQualificationTraversal::setNameQualificationOnPointerMemberClass(SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationOnPointerMemberClass(SgTypedefDeclaration*) \n");
     mfprintf(mlog [ WARN ] ) (" --- typedefDeclaration = %p (%s)\n", typedefDeclaration, typedefDeclaration->class_name().c_str());
     mfprintf(mlog [ WARN ] ) (" --- declaration = %p (%s)\n", declaration, declaration->class_name().c_str());
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     typedefDeclaration->set_global_qualification_required(outputGlobalQualification);
     typedefDeclaration->set_name_qualification_length(outputNameQualificationLength);
     typedefDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnPointerMemberClass(): typedefDeclaration->get_name_qualification_length()     = %d \n",typedefDeclaration->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnPointerMemberClass(): typedefDeclaration->get_type_elaboration_required()     = %s \n",typedefDeclaration->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationOnPointerMemberClass(): typedefDeclaration->get_global_qualification_required() = %s \n",typedefDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(typedefDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),typedefDeclaration,typedefDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 20: Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),typedefDeclaration,typedefDeclaration->class_name().c_str());
#endif
       // qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(typedefDeclaration,qualifier));
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(typedefDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(typedefDeclaration);
       // ROSE_ASSERT (i != qualifiedNameMapForTypes.end());
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(typedefDeclaration);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(typedefDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 16: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("WARNING: name in qualifiedNameMap already exists and is different... \n");
            // ROSE_ASSERT(false);

               SgName testNameInMap = typedefDeclaration->get_qualified_name_prefix();
               mfprintf(mlog [ WARN ] ) ("testNameInMap = %s \n",testNameInMap.str());
#endif
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgTemplateArgument* templateArgument, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // This function will generate the qualified name prefix (without the name of the declaration) and add it to
  // the map of name qualification strings referenced via the IR node that references the SgTemplateArgument.

  // DQ (6/1/2011): Note that the name qualification could be more complex than this function presently supports.
  // The use of derivation can permit there to be multiple legal qualified names for a single construct.  There
  // could also be some qualified names using using type names that are private or protected and thus can only
  // be used in restricted contexts.  This sumbject of multiple qualified names or selecting amongst them for
  // where each may be used is not handled presently.

  // DQ (9/23/2012): Note that the template arguments of the defining declaration don't appear to be set (only for
  // the nondefining declaration).  This was a problem for test2012_220.C.  The fix was to make sure that the
  // unparsing of the SgClassType consistantly uses the nondefining declaration, and it's template arguments.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#define DEBUG_TEMPLATE_ARGUMENT_NAME_QUALIFICATION 0

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPLATE_ARGUMENT_NAME_QUALIFICATION
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgTemplateArgument*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(declaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPLATE_ARGUMENT_NAME_QUALIFICATION
     mfprintf(mlog [ WARN ] ) (" - qualifier = %s\n", qualifier.c_str());
#endif

#if DEBUG_NONTERMINATION || 0
     printf ("In setNameQualification(SgTemplateArgument*): qualifier.length() = %zu \n",qualifier.length());
#endif

#if 1
  // DQ (5/23/2024): When the names start getting too long, use this as a way to short-circuit the runaway process.
     const size_t qualifier_length_limit = 6000;
     if (qualifier.length() > qualifier_length_limit)
        {
          static bool messageOutput = false;
          if (messageOutput == false)
             {
            // mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgTemplateArgument*): qualifier.length() length exceeded (qualifier_length_limit = %zu) Setting disableNameQualification == true \n",qualifier_length_limit);
               printf ("In setNameQualification(SgTemplateArgument*): qualifier.length() length exceeded (qualifier_length_limit = %zu) Setting disableNameQualification == true \n",qualifier_length_limit);
               messageOutput = true;

               disableNameQualification = true;
             }

       // Zero out the qualifier.
          qualifier = "";
        }
#endif

  // These may not be important under the newest version of name qualification that uses the qualified
  // name string map to IR nodes that reference the construct using the name qualification.
     templateArgument->set_global_qualification_required(outputGlobalQualification);
     templateArgument->set_name_qualification_length(outputNameQualificationLength);
     templateArgument->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_TEMPLATE_ARGUMENT_NAME_QUALIFICATION
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): templateArgument                                      = %p \n",templateArgument);
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): templateArgument->get_name_qualification_length()     = %d \n",templateArgument->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): templateArgument->get_type_elaboration_required()     = %s \n",templateArgument->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): templateArgument->get_global_qualification_required() = %s \n",templateArgument->get_global_qualification_required() ? "true" : "false");
#endif

  // DQ (9/25/2012): The code below is more complex than I would like because it has to set the name qualification
  // on both the template arguments of the defining and nondefining declarations.

  // DQ (9/22/2012): This is the bug to fix tomorrow morning...
  // XXX:  Either we should be setting the name_qualification_length on the SgTemplateArgument for the defining declaration (as well as the (first?) nondefining declaration)
  //       or we should be sharing the SgTemplateArgument across both the non-defining and defining declarations.
  //       I think I would like to share the SgTemplateArgument (this this problem would take care of itself).

  // TV (04/04/2018): Look for matching defining template argument.
  // For non-real template instantiation, i.e. member of a template parameter: "T0::template T1<A>", the template argument
  // ("A") parent is the global scope (FIXME or is it the scope of the template parameter ("T0")??? FIXME)
     SgTemplateArgument* defining_templateArgument = NULL;
     SgNode * tpl_arg_parent = templateArgument->get_parent();

#if DEBUG_TEMPLATE_ARGUMENT_NAME_QUALIFICATION
     mfprintf(mlog [ WARN ] ) ("tpl_arg_parent = %p = %s \n", tpl_arg_parent, tpl_arg_parent ? tpl_arg_parent->class_name().c_str() : "");
#endif

     ASSERT_not_null(tpl_arg_parent);

     SgDeclarationStatement* associatedDeclaration = isSgDeclarationStatement(tpl_arg_parent);
     if (associatedDeclaration != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("associatedDeclaration = %p = %s \n",associatedDeclaration,associatedDeclaration->class_name().c_str());
#endif
          SgDeclarationStatement* firstNondefining_associatedDeclaration = associatedDeclaration->get_firstNondefiningDeclaration();
          SgDeclarationStatement* defining_associatedDeclaration         = associatedDeclaration->get_definingDeclaration();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("firstNondefining_associatedDeclaration = %p \n",firstNondefining_associatedDeclaration);
#endif
          SgTemplateInstantiationDecl* firstDefining_classTemplateInstantiationDeclaration = isSgTemplateInstantiationDecl(firstNondefining_associatedDeclaration);

       // SgTemplateArgument* nondefining_templateArgument = templateArgument;

          int nondefiningDeclaration_templateArgument_position = 0;
          int definingDeclaration_templateArgument_position    = 0;

          bool found = false;
          if (firstDefining_classTemplateInstantiationDeclaration != NULL)
             {
            // Find the index position of the current template argument.
               SgTemplateArgumentPtrList & l = firstDefining_classTemplateInstantiationDeclaration->get_templateArguments();
               for (SgTemplateArgumentPtrList::iterator i = l.begin(); i != l.end(); i++)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("--- template argument = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
                    if (found == false)
                       {
                         if (*i == templateArgument)
                              found = true;
                           else
                              nondefiningDeclaration_templateArgument_position++;
                       }
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("defining_associatedDeclaration                   = %p \n",defining_associatedDeclaration);
          mfprintf(mlog [ WARN ] ) ("nondefiningDeclaration_templateArgument_position = %d \n",nondefiningDeclaration_templateArgument_position);
          mfprintf(mlog [ WARN ] ) ("definingDeclaration_templateArgument_position    = %d \n",definingDeclaration_templateArgument_position);
#endif

          SgTemplateInstantiationDecl* defining_classTemplateInstantiationDeclaration      = isSgTemplateInstantiationDecl(defining_associatedDeclaration);
          if (defining_classTemplateInstantiationDeclaration != NULL)
             {
            // Find the associated template argument (matching position) in the template argument list of the defining declaration.
#if 0
            // This is simpler code (but it causes some sort of error in the stack).
               defining_templateArgument = defining_classTemplateInstantiationDeclaration->get_templateArguments()[nondefiningDeclaration_templateArgument_position];
#else
            // This code is better tested and works well.
               SgTemplateArgumentPtrList & l = defining_classTemplateInstantiationDeclaration->get_templateArguments();
               for (SgTemplateArgumentPtrList::iterator i = l.begin(); i != l.end(); i++)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("--- template argument = %p = %s \n",*i,(*i)->class_name().c_str());
                    mfprintf(mlog [ WARN ] ) ("In loop: nondefiningDeclaration_templateArgument_position = %d \n",nondefiningDeclaration_templateArgument_position);
                    mfprintf(mlog [ WARN ] ) ("In loop: definingDeclaration_templateArgument_position    = %d \n",definingDeclaration_templateArgument_position);
#endif
                    if (definingDeclaration_templateArgument_position == nondefiningDeclaration_templateArgument_position)
                       {
                      // This is the template argument in the coresponding defining declaration.
                         defining_templateArgument = *i;
                       }

                    definingDeclaration_templateArgument_position++;
                  }
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("defining_templateArgument = %p \n",defining_templateArgument);
#endif

            // This is false when the template arguments are shared (which appears to happen sometimes, see test2004_38.C).
            // ASSERT_not_null(defining_templateArgument);
            // if (defining_templateArgument != NULL)
               if (defining_templateArgument != NULL && defining_templateArgument != templateArgument)
                  {
                 // Mark the associated template argument in the defining declaration so that it can be output with qualification (see test2012_220.C).
                    defining_templateArgument->set_global_qualification_required(outputGlobalQualification);
                    defining_templateArgument->set_name_qualification_length(outputNameQualificationLength);
                    defining_templateArgument->set_type_elaboration_required(outputTypeEvaluation);

                 // DQ (9/24/2012): Make sure these are different.
                    ROSE_ASSERT(defining_templateArgument != templateArgument);
                  }
             }
        }

  // Look for the template argument in the IR node map and either reset it or add it to the map.
  // The support for the template argument from the defining declaration makes this a bit more
  // complex, but both are set to always be the same (since we will prefer to use that from the
  // defining declaration in the unparsing).  Note that the preference for the defining declaration
  // use in the unparsing comes from supporting the corner case of type declarations nested in
  // other declarations; e.g. "struct X { int a; } Y;" where the declaration of the type "X" is
  // nested in the declaration of the variable "Y" (there are several different forms of this).
     if (qualifiedNameMapForTypes.find(templateArgument) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name or type = %s into list at IR node = %p = %s \n",qualifier.c_str(),templateArgument,templateArgument->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(templateArgument,qualifier));

       // Handle the defining declaration's template argument.
       // if (defining_templateArgument != NULL)
          if (defining_templateArgument != NULL && defining_templateArgument != templateArgument)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Insert qualified name = %s for defining_templateArgument = %p \n",qualifier.c_str(),defining_templateArgument);
#endif
#if DEBUG_NONTERMINATION || 0
               printf("TEST T6: Insert qualifier (length = %zu) for name = %s for defining_templateArgument = %p \n",qualifier.length(),qualifier.c_str(),defining_templateArgument);
#endif
               qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(defining_templateArgument,qualifier));
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                mfprintf(mlog [ WARN ] ) ("NOTE: defining_templateArgument != NULL && defining_templateArgument != templateArgument (qualified not inserted into qualifiedNameMapForTypes using defining_templateArgument = %p \n",defining_templateArgument);
#endif
             }
        }
       else
        {
       // If it already exists then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(templateArgument);
          NameQualificationMapType::iterator i = qualifiedNameMapForTypes.find(templateArgument);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 17: for templateArgument = %p replacing previousQualifier = %s with new qualifier = %s \n",templateArgument,previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
            // ROSE_ASSERT(false);
#endif
#if DEBUG_NONTERMINATION
               printf ("Calling templateArgument->get_qualified_name_prefix(): setting testNameInMap \n");
#endif
               SgName testNameInMap = templateArgument->get_qualified_name_prefix();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("testNameInMap = %s \n",testNameInMap.str());
#endif

            // DQ (9/24/2012): Check that the defining declaration's template argument is uniformally set.
            // if (defining_templateArgument != NULL)
               if (defining_templateArgument != NULL && defining_templateArgument != templateArgument)
                  {
                    ROSE_ASSERT(qualifiedNameMapForTypes.find(defining_templateArgument) != qualifiedNameMapForTypes.end());
                 // std::map<SgNode*,std::string>::iterator j = qualifiedNameMapForTypes.find(defining_templateArgument);
                    NameQualificationMapType::iterator j = qualifiedNameMapForTypes.find(defining_templateArgument);
                    ROSE_ASSERT (j != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("For defining_templateArgument = %p j->second = %s qualifier = %s \n",defining_templateArgument,j->second.c_str(),qualifier.c_str());
#endif
                 // ROSE_ASSERT(j->second != qualifier);

                    if (j->second != qualifier)
                       {
#if DEBUG_NONTERMINATION || 0
                         printf ("TEST B: Resetting qualifier via assignment: qualifier.length() = %zu \n",qualifier.length());
#endif
                         j->second = qualifier;
                       }

#if DEBUG_NONTERMINATION
                    printf ("Calling templateArgument->get_qualified_name_prefix(): setting defining_testNameInMap \n");
#endif
                    SgName defining_testNameInMap = defining_templateArgument->get_qualified_name_prefix();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("defining_testNameInMap = %s \n",defining_testNameInMap.str());
#endif
                    ROSE_ASSERT(defining_testNameInMap == testNameInMap);
                  }
             }
            else
             {
            // DQ (5/30/2019): Need to handle the case where the name qualification stored in the qualifiedNameMapForTypes
            // are the same for templateArgument, but different for defining_templateArgument.  This is a bugfix for test2019_444.C
            // reported by Charles as part of reproducers for bugs in ROSE from ASC codes.
#if 0
               mfprintf(mlog [ WARN ] ) ("The name qualifier is the same in the qualifiedNameMapForTypes for templateArgument, but need to check for defining_templateArgument \n");
#endif
               if (defining_templateArgument != NULL && defining_templateArgument != templateArgument)
                  {
                    ROSE_ASSERT(qualifiedNameMapForTypes.find(defining_templateArgument) != qualifiedNameMapForTypes.end());
                 // std::map<SgNode*,std::string>::iterator j = qualifiedNameMapForTypes.find(defining_templateArgument);
                    NameQualificationMapType::iterator j = qualifiedNameMapForTypes.find(defining_templateArgument);
                    ROSE_ASSERT (j != qualifiedNameMapForTypes.end());
                 // ROSE_ASSERT(j->second == qualifier);
                    if (j->second != qualifier)
                       {
#if 0
                         mfprintf(mlog [ WARN ] ) ("Setting the qualified for defining_templateArgument: j->second = %s qualifier = %s \n",j->second.c_str(),qualifier.c_str());
#endif
#if DEBUG_NONTERMINATION || 0
                         printf ("TEST C: Resetting qualifier via assignment: qualifier.length() = %zu \n",qualifier.length());
#endif
                         j->second = qualifier;
                       }
                    ROSE_ASSERT(j->second == qualifier);
                  }
             }
        }
   }


// void NameQualificationTraversal::setNameQualification(SgCastExp* castExp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
void
NameQualificationTraversal::setNameQualification(SgExpression* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // DQ (6/4/2011): This should not be a SgConstructorInitializer since that uses the qualifiedNameMapForNames instead of the qualifiedNameMapForTypes.
     ROSE_ASSERT(isSgConstructorInitializer(exp) == NULL);

  // DQ (11/22/2016): Added assertion.
     ASSERT_not_null(typeDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgExpression*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(typeDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     exp->set_global_qualification_required(outputGlobalQualification);
     exp->set_name_qualification_length(outputNameQualificationLength);

  // DQ (6/2/2011): I think that type elaboration could be required for casts, but I am not certain.
     exp->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): exp->get_name_qualification_length()     = %d \n",exp->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): exp->get_type_elaboration_required()     = %s \n",exp->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): exp->get_global_qualification_required() = %s \n",exp->get_global_qualification_required() ? "true" : "false");
#endif
     if (qualifiedNameMapForTypes.find(exp) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST T7: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(exp,qualifier));
        }
       else
        {
       // DQ (6/21/2011): Now we are catching this case...

       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(exp);
          NameQualificationMapType::iterator i = qualifiedNameMapForTypes.find(exp);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 18: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               i->second = qualifier;
             }
        }
   }


void
NameQualificationTraversal::setNameQualificationForPointerToMember(SgExpression* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // DQ (6/4/2011): This should not be a SgConstructorInitializer since that uses the qualifiedNameMapForNames instead of the qualifiedNameMapForTypes.
     ROSE_ASSERT(isSgConstructorInitializer(exp) == NULL);

  // DQ (11/22/2016): Added assertion.
     ASSERT_not_null(typeDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualificationForPointerToMember(SgExpression*): exp = %p = %s \n",exp,exp->class_name().c_str());
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(typeDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

  // DQ (4/16/2019): These are virtual functions defined for a subset of IR nodes to be valid, but defined for SgExpression to be an explicit error.
     exp->set_global_qualification_for_pointer_to_member_class_required(outputGlobalQualification);
     exp->set_name_qualification_for_pointer_to_member_class_length(outputNameQualificationLength);

  // DQ (6/2/2011): I think that type elaboration could be required for casts, but I am not certain.
     exp->set_type_elaboration_for_pointer_to_member_class_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationForPointerToMember(): exp->get_name_qualification_length()     = %d \n",exp->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationForPointerToMember(): exp->get_type_elaboration_required()     = %s \n",exp->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationForPointerToMember(): exp->get_global_qualification_required() = %s \n",exp->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(exp) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(exp,qualifier));
        }
       else
        {
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(exp);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(exp);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 18: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               ROSE_ABORT();
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgNonrealRefExp* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // DQ (11/22/2016): Added assertion.
     ASSERT_not_null(typeDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgNonrealRefExp*) \n");
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(typeDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     exp->set_global_qualification_required(outputGlobalQualification);
     exp->set_name_qualification_length(outputNameQualificationLength);

  // DQ (6/2/2011): I think that type elaboration could be required for casts, but I am not certain.
     exp->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): exp->get_name_qualification_length()     = %d \n",exp->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): exp->get_type_elaboration_required()     = %s \n",exp->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): exp->get_global_qualification_required() = %s \n",exp->get_global_qualification_required() ? "true" : "false");
#endif
     if (qualifiedNameMapForNames.find(exp) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 22: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(exp,qualifier));
        }
       else
        {
       // DQ (6/21/2011): Now we are catching this case...

       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(exp);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(exp);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 19: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               ROSE_ABORT();
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgAggregateInitializer* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
   {
  // DQ (3/22/2018): This is a special version required for the SgAggregateInitializer, becuase it uses the set_global_qualification_required_for_type()
  // named functions instead of the set_global_qualification_required() named functions.  The alternative to to reusing the version of setNameQualification()
  // that takes a SgExpression would force special unparser support for the outputType function.  And since the name qualification for the case of a class type
  // is handled as name qualification on the output of a type, fixing up the name qualification is the better solution (so it seems to be after trying the
  // alternative).

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // DQ (6/4/2011): This should not be a SgConstructorInitializer since that uses the qualifiedNameMapForNames instead of the qualifiedNameMapForTypes.
     ROSE_ASSERT(isSgConstructorInitializer(exp) == NULL);

  // DQ (11/22/2016): Added assertion.
     ASSERT_not_null(typeDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(SgAggregateInitializer*): TOP of function: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(typeDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     exp->set_global_qualification_required_for_type(outputGlobalQualification);
     exp->set_name_qualification_length_for_type(outputNameQualificationLength);

  // DQ (6/2/2011): I think that type elaboration could be required for casts, but I am not certain.
     exp->set_type_elaboration_required_for_type(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(SgAggregateInitializer*): exp->get_name_qualification_length()     = %d \n",exp->get_name_qualification_length_for_type());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(SgAggregateInitializer*): exp->get_type_elaboration_required()     = %s \n",exp->get_type_elaboration_required_for_type() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(SgAggregateInitializer*): exp->get_global_qualification_required() = %s \n",exp->get_global_qualification_required_for_type() ? "true" : "false");
#endif

     if (qualifiedNameMapForTypes.find(exp) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST T8: Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(exp,qualifier));
        }
       else
        {
       // DQ (6/21/2011): Now we are catching this case...

       // If it already existes then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(exp);
          NameQualificationMapType::iterator i = qualifiedNameMapForTypes.find(exp);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 20: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               mfprintf(mlog [ WARN ] ) ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
               i->second = qualifier;

               SgName testNameInMap = exp->get_qualified_name_prefix();
               mfprintf(mlog [ WARN ] ) ("testNameInMap = %s \n",testNameInMap.str());
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired)
   {
  // This is used to set the name qualification on the associated SgClassDeclaration.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgClassDeclaration*) \n");
     mfprintf(mlog [ WARN ] ) (" - classDeclaration = %p (%s)\n", classDeclaration, classDeclaration->class_name().c_str());
     mfprintf(mlog [ WARN ] ) (" - amountOfNameQualificationRequired = %d\n", amountOfNameQualificationRequired);
#endif

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(classDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) (" - qualifier = %s\n", qualifier.c_str());
#endif

     classDeclaration->set_global_qualification_required(outputGlobalQualification);
     classDeclaration->set_name_qualification_length(outputNameQualificationLength);
     classDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) (" - classDeclaration->get_name_qualification_length()     = %d \n",classDeclaration->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) (" - classDeclaration->get_type_elaboration_required()     = %s \n",classDeclaration->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) (" - classDeclaration->get_global_qualification_required() = %s \n",classDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(classDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at SgClassDeclaration IR node = %p = %s \n",qualifier.c_str(),classDeclaration,classDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 23: Inserting qualifier for name = %s into list at SgClassDeclaration IR node = %p = %s \n",qualifier.c_str(),classDeclaration,classDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(classDeclaration,qualifier));
        }
       else
        {
       // If it already exists then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(classDeclaration);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(classDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 21: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               ROSE_ABORT();
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgEnumDeclaration* enumDeclaration, int amountOfNameQualificationRequired)
   {
  // This is used to set the name qualification on the associated SgEnumDeclaration.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In setNameQualification(SgEnumDeclaration*) \n");
#endif

  // DQ (2/22/2019): Adding assertion to debug GNU 4.9.3 issue.
     ASSERT_not_null(enumDeclaration);

     SgScopeStatement * scope = traverseNonrealDeclForCorrectScope(enumDeclaration);
     string qualifier = setNameQualificationSupport(scope,amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     enumDeclaration->set_global_qualification_required(outputGlobalQualification);
     enumDeclaration->set_name_qualification_length(outputNameQualificationLength);
     enumDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): enumDeclaration->get_name_qualification_length()     = %d \n",enumDeclaration->get_name_qualification_length());
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): enumDeclaration->get_type_elaboration_required()     = %s \n",enumDeclaration->get_type_elaboration_required() ? "true" : "false");
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualification(): enumDeclaration->get_global_qualification_required() = %s \n",enumDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(enumDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("Inserting qualifier for name = %s into list at SgEnumDeclaration IR node = %p = %s \n",qualifier.c_str(),enumDeclaration,enumDeclaration->class_name().c_str());
#endif
#if DEBUG_NONTERMINATION
          printf("TEST 24: Inserting qualifier for name = %s into list at SgEnumDeclaration IR node = %p = %s \n",qualifier.c_str(),enumDeclaration,enumDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(enumDeclaration,qualifier));
        }
       else
        {
       // If it already exists then overwrite the existing information.
       // std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(enumDeclaration);
          NameQualificationMapType::iterator i = qualifiedNameMapForNames.find(enumDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          mfprintf(mlog [ WARN ] ) ("WARNING: test 22: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               ROSE_ABORT();
             }
        }
   }


string
NameQualificationTraversal::setNameQualificationSupport(SgScopeStatement* scope, const int inputNameQualificationLength, int & output_amountOfNameQualificationRequired , bool & outputGlobalQualification, bool & outputTypeEvaluation )
   {
  // This is lower level support for the different overloaded setNameQualification() functions.
  // This function builds up the qualified name as a string and then returns it to be used in
  // either the map to names or the map to types (two different hash maps).
     string qualifierString;

#define DEBUG_NQ_SCOPE 0

     output_amountOfNameQualificationRequired = inputNameQualificationLength;
     outputGlobalQualification                = false;
     outputTypeEvaluation                     = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): scope = %p = %s = %s inputNameQualificationLength = %d \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str(),inputNameQualificationLength);
     mfprintf(mlog [ WARN ] ) (" --- outputGlobalQualification = %s \n",outputGlobalQualification ? "true" : "false");
#endif

#if 0
     printf ("test 6: qualifierString = %s \n",qualifierString.c_str());
#endif
#if DEBUG_NQ_SCOPE
  // DQ (5/5/2024): Debugging non-termination.
     printf("\nSTART: In NameQualificationTraversal::setNameQualificationSupport(): scope = %p = %s = %s inputNameQualificationLength = %d \n",
          scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str(),inputNameQualificationLength);
#endif

#if 0
     printf ("In setNameQualificationSupport(): namespaceAliasDeclarationMap.size() = %zu \n",namespaceAliasDeclarationMap.size());
#endif

     for (int i = 0; i < inputNameQualificationLength; i++)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NQ_SCOPE
          mfprintf(mlog [ WARN ] ) ("   --- In loop: i = %d scope = %p = %s = %s \n",i,scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
#endif
#if DEBUG_NQ_SCOPE
       // DQ (5/5/2024): Debugging non-termination.
          printf ("In setNameQualificationSupport(): in loop: i = %d scope = %p = %s = %s \n",i,scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
#endif
          string scope_name;

       // DQ (8/9/2020): When we process a namespace alias, we can break out of the loop over the length of
       // the required namespace name qualification depth (I think).  See Cxx_tests/test2020_24.C for an example.
          bool breakOutOfLoop = false;

       // DQ (8/19/2014): This is used to control the generation of qualified names for un-named namespaces
       // (and maybe also other un-named language constructs).
          bool skip_over_scope = false;
#if DEBUG_NQ_SCOPE
          printf ("test 7: qualifierString = %s \n",qualifierString.c_str());
#endif
       // This requirement to visit the template arguments occurs for templaed functions and templated member functions as well.
          SgTemplateInstantiationDefn* templateClassDefinition = isSgTemplateInstantiationDefn(scope);
          if (templateClassDefinition != NULL)
             {
            // Need to investigate how to generate a better quality name.
               SgTemplateInstantiationDecl* templateClassDeclaration = isSgTemplateInstantiationDecl(templateClassDefinition->get_declaration());
               ASSERT_not_null(templateClassDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
            // This is the normalized name (without name qualification for internal template arguments)
               mfprintf(mlog [ WARN ] ) ("templateClassDeclaration->get_name()          = %s \n",templateClassDeclaration->get_name().str());

            // This is the name of the template (without and internal template arguments)
               mfprintf(mlog [ WARN ] ) ("templateClassDeclaration->get_templateName() = %s \n",templateClassDeclaration->get_templateName().str());
#endif
#if DEBUG_NQ_SCOPE
            // DQ (5/5/2024): Debugging non-termination.
               printf("templateClassDeclaration->get_name()          = %s \n",templateClassDeclaration->get_name().str());
#endif
               SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
               ASSERT_not_null(unparseInfoPointer);
               unparseInfoPointer->set_outputCompilerGeneratedStatements();

            // templateClassDeclaration->get_file_info()->display("SgTemplateInstantiationDecl trying to generate the qualified name: debug");

               string template_name = templateClassDeclaration->get_templateName();

            // DQ (2/22/2019): Note: the same moderately more complex handling for template arguments in the unparser,
            // might need to be used here for the support of the name qualification.
               SgTemplateArgumentPtrList & templateArgumentList = templateClassDeclaration->get_templateArguments();
#if 0
               mfprintf(mlog [ WARN ] ) ("In name qualification: templateArgumentList.size() = %zu \n",templateArgumentList.size());
#endif
#if DEBUG_NQ_SCOPE
               printf("In name qualification: templateArgumentList.size() = %zu \n",templateArgumentList.size());
#endif
               bool isEmptyTemplateArgumentList = templateArgumentList.empty();

            // template_name += "< ";
               if (isEmptyTemplateArgumentList == false)
                  {
                    template_name += "< ";
                  }

            // mfprintf(mlog [ WARN ] ) ("START: template_name = %s \n",template_name.c_str());
               SgTemplateArgumentPtrList::iterator i = templateArgumentList.begin();

               bool previousTemplateArgumentOutput = false;
               while (i != templateArgumentList.end())
                  {
                    bool skipTemplateArgument = false;
                    bool stopTemplateArgument = false;
                    (*i)->outputTemplateArgument(skipTemplateArgument, stopTemplateArgument);

                    if (stopTemplateArgument) {
                      break;
                    }

#if 0
                    mfprintf(mlog [ WARN ] ) ("filterTemplateArgument = %s \n",filterTemplateArgument ? "true" : "false");
#endif
                 // if ((*i)->get_argumentType() != SgTemplateArgument::start_of_pack_expansion_argument)
                    if (skipTemplateArgument == false)
                       {
                      // if (i != templateArgumentList.begin())
                         if (previousTemplateArgumentOutput == true)
                            {
                           // DQ (2/11/2019): Adding debugging information for test2019_93.C.
                           // template_name += " /* output comma: part 3 */ ";
                              template_name += ",";
                            }
                         string template_argument_name = globalUnparseToString(*i,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || DEBUG_NQ_SCOPE
                         mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ In name qualification: templateArgument = %p template_argument_name (globalUnparseToString()) = %s \n",*i,template_argument_name.c_str());
                         mfprintf(mlog [ WARN ] ) ("   --- template_argument_name.length() = %zu \n",template_argument_name.length());
#endif
#if DEBUG_NQ_SCOPE
                      // printf("   --- template_name = %s \n",template_name.c_str());
                         printf("   --- template_name.length() = %zu \n",template_name.length());
                      // printf("   --- template_argument_name.length() = %zu template_argument_name.substr(0,50) = %s \n",template_argument_name.length(),template_argument_name.substr(0,50).c_str());
                         printf("   --- template_argument_name.length() = %zu \n",template_argument_name.length());
                         printf("   --- template_argument_name = %s \n",template_argument_name.c_str());
#if 0
                         printf("   --- template_argument_name.substr(0,50) = %s \n",
                              template_argument_name.substr(0,50).c_str());
                         printf("   --- template_argument_name.substr(template_argument_name.length()-50,template_argument_name.length()) = %s \n",
                              template_argument_name.substr(template_argument_name.length()-50,template_argument_name.length()-1).c_str());
#endif
#endif
                         template_name += template_argument_name;
                         previousTemplateArgumentOutput = true;
                       }

                    i++;
#if 0
                 // DQ (5/12/2024): Added to force a single iteration through this loop.
                    printf ("Added to force a single iteration through this loop \n");
                    break;
#endif
                  }

            // template_name += "> ";
               if (isEmptyTemplateArgumentList == false)
                  {
                    template_name += "> ";
                  }

               scope_name = template_name;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("NAME OF SCOPE: scope name -- template_name = %s \n",template_name.c_str());
#endif
#if DEBUG_NQ_SCOPE
            // printf("NAME OF SCOPE: scope name -- template_name = %s \n",template_name.c_str());
               printf("NAME OF SCOPE: scope name -- template_name.length() = %zu \n",template_name.length());
#endif
            // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
               delete unparseInfoPointer;
             }
            else
             {
            // scope_name = scope->class_name().c_str();
               scope_name = SageInterface::get_name(scope).c_str();
#if DEBUG_NQ_SCOPE
            // DQ (5/5/2024): Debugging non-termination.
               printf ("templateClassDefinition != NULL: scope_name = %s \n",scope_name.c_str());
#endif
#if DEBUG_NQ_SCOPE
               printf ("test 8: qualifierString = %s \n",qualifierString.c_str());
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("Before test for __anonymous_ un-named scopes: scope_name = %s \n",scope_name.c_str());
#endif
            // DQ (4/6/2013): Test this scope name for that of n un-named scope so that we can avoid name qualification
            // using an internally generated scope name.
            // Note that the pointer is from an EDG object (e.g. a_type_ptr), so we can't reproduce it in ROSE.
            // This might be something to fix if we want to be able to reproduce it.
               if (scope_name.substr(0,14) == "__anonymous_0x")
                  {
                 // DQ (4/6/2013): Added test (this would be better to added to the AST consistancy tests).
                    SgClassDefinition* classDefinition = isSgClassDefinition(scope);
                    if (classDefinition != NULL)
                       {
                         if (classDefinition->get_declaration()->get_isUnNamed() == false)
                            {
                           // DQ (4/11/2017): Klockworks reports that classDeclaration may be NULL, so make sure that adding an assertion will fix the issue.
                              SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                              ASSERT_not_null(classDeclaration);
                              mfprintf(mlog [ WARN ] ) ("Error: class should be marked as unnamed: classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
                              mfprintf(mlog [ WARN ] ) ("   --- classDeclaration name = %s \n",classDeclaration->get_name().str());
                            }
                         ROSE_ASSERT(classDefinition->get_declaration()->get_isUnNamed() == true);
                       }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): Detected scope_name of un-named scope: scope_name = %s (reset to empty string for name qualification) \n",scope_name.c_str());
#endif
                    scope_name = "";

                 // DQ (5/3/2013): If this case was detected then we can't use the qualified name.
                 // The test2013_145.C demonstrates this case where one part of the name qualification
                 // is empty string (unnamed scope, specifically a union in the test code).
                    qualifierString = "";

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): skip over this un-named declaration in the generation of the more complete qualified name... \n");
#endif
                 // DQ (4/14/2019): If there is an un-named declaration we just want to not use that name, but
                 // we want to continue to iterate to collect the associated scopes to build the qualified name.
                 // Note that Cxx11_tests/test2019_373.C is an example where this is required.
                 // break;
                  }
                 else
                  {
                 // DQ (4/6/2013): Added test (this would be better to add to the AST consistancy tests).
                 // ROSE_ASSERT(scope->get_isUnNamed() == false);
#if DEBUG_NQ_SCOPE
                    printf("test 9: scope name = %s \n",scope_name.c_str());
#endif
#if DEBUG_NQ_SCOPE
                    printf ("test 9: qualifierString = %s \n",qualifierString.c_str());
#endif
#if DEBUG_NQ_SCOPE
                 // DQ (5/5/2024): Debugging non-termination.
                    printf ("In setNameQualificationSupport(): not anonymous \n");
#endif
                    SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(scope);
                    if (namespaceDefinition != NULL)
                       {
#if 0
                         mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): Detected a SgNamespaceDefinition: namespaceDefinition = %p \n",namespaceDefinition);
#endif
#if DEBUG_NQ_SCOPE
                      // DQ (5/5/2024): Debugging non-termination.
                         printf ("In setNameQualificationSupport(): namespaceDefinition != NULL \n");
#endif

                         SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(namespaceDefinition->get_namespaceDeclaration());
                         ASSERT_not_null(namespaceDeclaration);
                         if (namespaceDeclaration->get_isUnnamedNamespace() == true)
                            {
#if 0
                              mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): found un-named namespace: namespaceDefinition = %p \n",namespaceDefinition);
#endif
                              skip_over_scope = true;
                            }
                           else
                            {
                           // DQ (8/1/2020): Adding support for references to the NamespaceAlias (required for new failing test code).
#if 0
                              printf ("Adding support for references to the NamespaceAlias \n");
#endif
                           // DQ (8/1/2020): This should always be true.
                           // ROSE_ASSERT(namespaceAliasDeclarationMapFromInheritedAttribute != NULL);
#if 0
                           // printf ("namespaceAliasDeclarationMapFromInheritedAttribute->size() = %zu \n",namespaceAliasDeclarationMapFromInheritedAttribute->size());
                              printf ("namespaceAliasDeclarationMap.size() = %zu \n",namespaceAliasDeclarationMap.size());
#endif
                           // if (namespaceAliasDeclarationMapFromInheritedAttribute->find(namespaceDeclaration) != namespaceAliasDeclarationMapFromInheritedAttribute->end())
                              if (namespaceAliasDeclarationMap.find(namespaceDeclaration) != namespaceAliasDeclarationMap.end())
                                 {
                                // SgDeclarationStatement* declaration = namespaceAliasDeclarationMapFromInheritedAttribute->operator[](namespaceDeclaration);
                                // SgDeclarationStatement* declaration = namespaceAliasDeclarationMap[namespaceDeclaration];
                                   SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = namespaceAliasDeclarationMap[namespaceDeclaration];
                                   ROSE_ASSERT(namespaceAliasDeclaration != NULL);
#if DEBUG_NQ_SCOPE
                                   printf ("test 10: qualifierString = %s \n",qualifierString.c_str());
#endif
                                // qualifierString = " /* use the namspace alias */ ";

                                // DQ (8/2/2020): Reset the name of the scope.
                                   scope_name = namespaceAliasDeclaration->get_name();
#if DEBUG_NQ_SCOPE
                                   printf("namespaceAliasDeclaration: scope name = %s \n",scope_name.c_str());
#endif

                                   breakOutOfLoop = true;
                                 }
                            }
                       }
                      else
                       {
                      // DQ (9/7/2014): Added case for template class definitions (which we were not using and thus
                      // it was not a problem that we didn't compute them quite right).  These were being computed
                      // as "class-name::class-name", but we need then to be computed to be:
                      // "class-name<template-parameter>::class-name<template-parameter>" instead.
                      // Other logic will have to add the template header where these are used (not clear how to
                      // do that if we don't do it here).

#if DEBUG_NQ_SCOPE
                      // DQ (5/5/2024): Debugging non-termination.
                         printf ("In setNameQualificationSupport(): namespaceDefinition == NULL \n");
#endif
                         SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(scope);
                         if (templateClassDefinition != NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): Found SgTemplateClassDefinition: templateClassDefinition = %p = %s \n",templateClassDefinition,templateClassDefinition->class_name().c_str());
#endif
#if DEBUG_NQ_SCOPE
                           // DQ (5/5/2024): Debugging non-termination.
                              printf ("In setNameQualificationSupport(): templateClassDefinition != NULL \n");
#endif
                              SgTemplateClassDeclaration* templateClassDeclaration = templateClassDefinition->get_declaration();
                              ASSERT_not_null(templateClassDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                           // This is the normalized name (without name qualification for internal template arguments)
                              mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): templateClassDeclaration->get_name()          = %s \n",templateClassDeclaration->get_name().str());

                           // This is the name of the template (without and internal template arguments)
                              mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): templateClassDeclaration->get_templateName() = %s \n",templateClassDeclaration->get_templateName().str());
#endif

                              SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
                              ASSERT_not_null(unparseInfoPointer);
                              unparseInfoPointer->set_outputCompilerGeneratedStatements();

                           // templateClassDeclaration->get_file_info()->display("SgTemplateInstantiationDecl trying to generate the qualified name: debug");

                              string template_name = templateClassDeclaration->get_templateName();

                           // DQ (9/12/2014): If we have template specialization arguments then we wnat to use these instead of the template parameters (I think).
                           // See test2014_222.C for an example.
                              SgTemplateArgumentPtrList & templateSpecializationArgumentList = templateClassDeclaration->get_templateSpecializationArguments();
#if 0
                              mfprintf(mlog [ WARN ] ) ("templateSpecializationArgumentList.size() = %zu \n",templateSpecializationArgumentList.size());
                              mfprintf(mlog [ WARN ] ) ("specialization = %d \n",templateClassDeclaration->get_specialization());
#endif
                              if (templateSpecializationArgumentList.empty() == false)
                                 {
                                // DQ (9/13/2014): I have build overloaded versions of globalUnparseToString() to handle that case of SgTemplateArgumentPtrList.
                                   string template_specialization_argument_list_string = globalUnparseToString(&templateSpecializationArgumentList,unparseInfoPointer);
#if 0
                                   mfprintf(mlog [ WARN ] ) ("template_specialization_argument_list_string = %s \n",template_specialization_argument_list_string.c_str());
#endif
                                   template_name += template_specialization_argument_list_string;
                                 }
                                else
                                 {
                             // DQ (9/9/2014): Modified to support empty name template parameter lists as what appear if none are present
                             // (and this is a non-template function in a template class which we consider to be a template function
                             // because it can be instantiated).
                                SgTemplateParameterPtrList & templateParameterList = templateClassDeclaration->get_templateParameters();
                                if (templateParameterList.empty() == false)
                                   {
                                  // DQ (9/13/2014): I have build overloaded versions of globalUnparseToString() to handle that case of SgTemplateParameterPtrList.
                                     string template_parameter_list_string = globalUnparseToString(&templateParameterList,unparseInfoPointer);
                                     template_name += template_parameter_list_string;
                                   }
                                 }

                              scope_name = template_name;

#if DEBUG_NQ_SCOPE
                              printf("templateClassDefinition: scope name = %s \n",scope_name.c_str());
#endif
#if DEBUG_NQ_SCOPE
                           // DQ (5/5/2024): Debugging non-terminating behavior.
                              if (scope_name.length() > 2000)
                                 {
                                   printf ("Warning: scope_name.length() > 10000 \n");
                                 }
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              mfprintf(mlog [ WARN ] ) ("setNameQualificationSupport(): case of SgTemplateClassDefinition: scope_name = %s \n",scope_name.c_str());
#endif
#if 0
                              mfprintf(mlog [ WARN ] ) ("Exiting as a test! \n");
                              ROSE_ABORT();
#endif
                            }
                           else
                            {
#if DEBUG_NQ_SCOPE
                           // DQ (5/5/2024): Debugging non-termination.
                              printf ("In setNameQualificationSupport(): templateClassDefinition == NULL \n");
#endif
                           // DQ (4/27/2019): Need to add case for SgClassDefinition, to support test2019_102.C.
                              SgClassDefinition* classDefinition = isSgClassDefinition(scope);
                              if (classDefinition != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                                   mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): Found SgClassDefinition: classDefinition = %p = %s \n",classDefinition,classDefinition->class_name().c_str());
#endif
#if 0
                                // DQ (4/27/2019): Look into this in the morning.
                                   mfprintf(mlog [ WARN ] ) ("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                                   mfprintf(mlog [ WARN ] ) ("Unclear what do do for this case! \n");
                                   mfprintf(mlog [ WARN ] ) ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n\n\n");
#endif
                                 }
                            }
                       }
#if DEBUG_NQ_SCOPE
                    printf("test 11: scope name = %s \n",scope_name.c_str());
#endif
#if DEBUG_NQ_SCOPE
                    printf ("test 11: qualifierString = %s \n",qualifierString.c_str());
#endif
                    if (scope_name.substr(0,2) == "0x" && isSgGlobal(scope) == NULL)
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                         mfprintf(mlog [ WARN ] ) ("WARNING: Detected scope name generated from pointer: i = %d scope = %p = %s skip_over_scope = %s \n",i,scope,scope->class_name().c_str(),skip_over_scope ? "true" : "false");
                         mfprintf(mlog [ WARN ] ) (" --- qualifierString = %s \n",qualifierString.c_str());
#endif
                         skip_over_scope = true;
                       }
                  }
             }

          SgDeclarationScope * decl_scope = isSgDeclarationScope(scope);
          if (decl_scope != NULL) {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
            mfprintf(mlog [ WARN ] ) ("setNameQualificationSupport(): case of SgDeclarationScope:\n");
            mfprintf(mlog [ WARN ] ) (" --- scope_name         = %s \n",scope_name.c_str());
            mfprintf(mlog [ WARN ] ) (" --- scope->get_scope() = %p (%s) \n", scope->get_scope(), scope->get_scope() != NULL ? scope->get_scope()->class_name().c_str() : "");
            mfprintf(mlog [ WARN ] ) (" --- scope->get_parent() = %p (%s) \n", scope->get_parent(), scope->get_parent() != NULL ? scope->get_parent()->class_name().c_str() : "");
#endif
          }

          SgGlobal* globalScope = isSgGlobal(scope);
          if (globalScope != NULL)
             {
            // If we have iterated beyond the number of nested scopes, then set the global
            // qualification and reduce the name_qualification_length correspondingly by one.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) ("!!!!! We have iterated beyond the number of nested scopes: setting outputGlobalQualification == true \n");
#endif
               outputGlobalQualification = true;
               output_amountOfNameQualificationRequired = inputNameQualificationLength-1;

               scope_name = "::";
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) (" --- scope_name = %s skip_over_scope = %s \n",scope_name.c_str(),skip_over_scope ? "true" : "false");
#endif

          if (skip_over_scope == false)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) (" --- outputGlobalQualification = %s \n",outputGlobalQualification ? "true" : "false");
#endif
               if (outputGlobalQualification)
                  {
                    qualifierString = "::" + qualifierString;
                  }
                 else if (scope_name.length() > 0)
                  {
                    qualifierString = scope_name + "::" + qualifierString;
                  }
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) (" --- Case of skip_over_scope == true!\n");
#endif
             }
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
               mfprintf(mlog [ WARN ] ) (" --- qualifierString = %s \n",qualifierString.c_str());
#endif
          if (globalScope != NULL) break;

       // We have to loop over scopes that are not named scopes!
          scope = scope->get_scope();

          if (breakOutOfLoop == true)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 1
               printf ("breakOutOfLoop == true: short curcuit this loop over the name qualification depth (becasue we used a namespace alias) \n");
#endif
               break;
             }
#if DEBUG_NQ_SCOPE
       // DQ (5/5/2024): Debugging non-termination.
          printf ("BASE of loop: i = %d scope = %p = %s = %s \n",i,scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
#endif
        }

#if DEBUG_NQ_SCOPE
     printf ("test 15: qualifierString = %s \n",qualifierString.c_str());
     printf ("test 15: qualifierString.length() = %zu \n",qualifierString.length());
#endif
#if 0
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setNameQualificationSupport(): After loop over name qualifiction depth: inputNameQualificationLength = %d \n",inputNameQualificationLength);
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) && 1
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
     mfprintf(mlog [ WARN ] ) ("Leaving NameQualificationTraversal::setNameQualificationSupport(): outputGlobalQualification = %s output_amountOfNameQualificationRequired = %d qualifierString = %s \n",
          outputGlobalQualification ? "true" : "false",output_amountOfNameQualificationRequired,qualifierString.c_str());
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
#endif

  // DQ (6/12/2011): Make sure we have not generated a qualified name with "::::" because of an scope translated to an empty name.
     ROSE_ASSERT(qualifierString.find("::::") == string::npos);

  // DQ (6/23/2011): Never generate a qualified name from a pointer value.
  // This is a bug in the inlining support where the symbol tables are not setup just right.
     if (qualifierString.substr(0,2) == "0x")
        {
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("WARNING: Detected qualified name generated from pointer value 0x..., reset to empty string (inlining does not fixup symbol tables) \n");
#endif
          qualifierString = "";
        }
     ROSE_ASSERT(qualifierString.substr(0,2) != "0x");

     return qualifierString;
   }

string
NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(SgScopeStatement* scope, const int inputNameQualificationLength )
   {
  // DQ (9/7/2014): This function generates a string that is used with name qualification of template declarations.
  // For example:
  //      template < typename T >
  //      template < typename S >
  //      void X<T>::A<S>::foobar (int x) { int a_value; }
  // Requires the template header string: "template < typename T > template < typename S >"
  // in addition to the usual name qualification (which here is in terms of template parameters
  // instead of template arguments (as in a template instantiation), namely "X<T>::A<S>::").
  // This new support for template headers also requires a new map of names to template declarations.

  // This is lower level support for the different overloaded setNameQualification() functions.
  // This function builds up the qualified name as a string and then returns it to be used in
  // either the map to names or the map to types (two different hash maps).
     string accumulated_template_header_name;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): scope = %p = %s = %s inputNameQualificationLength = %d \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str(),inputNameQualificationLength);
#endif

     for (int i = 0; i < inputNameQualificationLength; i++)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("   --- In loop: i = %d scope = %p = %s = %s \n",i,scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
#endif
          string template_header_name;

       // DQ (9/7/2014): Added case for template class definitions (which we were not using and thus
       // it was not a problem that we didn't compute them quite right).  These were being computed
       // as "class-name::class-name", but we need then to be computed to be:
       // "class-name<template-parameter>::class-name<template-parameter>" instead.
       // Other logic will have to add the template header where these are used (not clear how to
       // do that if we don't do it here).
          SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(scope);
          if (templateClassDefinition != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               mfprintf(mlog [ WARN ] ) ("Found SgTemplateClassDefinition: templateClassDefinition = %p = %s \n",templateClassDefinition,templateClassDefinition->class_name().c_str());
#endif
               SgTemplateClassDeclaration* templateClassDeclaration = templateClassDefinition->get_declaration();
               ASSERT_not_null(templateClassDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
            // This is the normalized name (without name qualification for internal template arguments)
               mfprintf(mlog [ WARN ] ) ("templateClassDeclaration->get_name()          = %s \n",templateClassDeclaration->get_name().str());

            // This is the name of the template (without and internal template arguments)
               mfprintf(mlog [ WARN ] ) ("templateClassDeclaration->get_templateName() = %s \n",templateClassDeclaration->get_templateName().str());
#endif
#if 0
               SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
               ASSERT_not_null(unparseInfoPointer);
               unparseInfoPointer->set_outputCompilerGeneratedStatements();
#endif
            // templateClassDeclaration->get_file_info()->display("SgTemplateInstantiationDecl trying to generate the qualified name: debug");

               SgTemplateParameterPtrList & templateParameterList = templateClassDeclaration->get_templateParameters();
               if (templateParameterList.empty() == false)
                  {
               string template_name = buildTemplateHeaderString(templateParameterList);
               template_header_name = template_name;
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): template_header_name = %s accumulated_template_header_name = %s \n",template_header_name.c_str(),accumulated_template_header_name.c_str());
#endif
          accumulated_template_header_name = template_header_name + accumulated_template_header_name;

       // We have to loop over scopes that are not named scopes!
          scope = scope->get_scope();
        }

     ROSE_ASSERT(accumulated_template_header_name.substr(0,2) != "0x");

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): accumulated_template_header_name = %s \n",accumulated_template_header_name.c_str());
#endif

     return accumulated_template_header_name;
   }


string
NameQualificationTraversal::buildTemplateHeaderString ( SgTemplateParameterPtrList & templateParameterList )
   {
     SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
     ASSERT_not_null(unparseInfoPointer);
     unparseInfoPointer->set_outputCompilerGeneratedStatements();

#if 0
     mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::buildTemplateHeaderString(): templateParameterList.size() = %zu \n",templateParameterList.size());
#endif

     string template_name = "template < ";
  // mfprintf(mlog [ WARN ] ) ("START: template_name = %s \n",template_name.c_str());
     SgTemplateParameterPtrList::iterator i = templateParameterList.begin();
     while (i != templateParameterList.end())
        {
#if 0
          mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::buildTemplateHeaderString(): Check for the type of the template parameters (could be non-type, etc.) \n");
#endif
          SgTemplateParameter* templateParameter = *i;
          ASSERT_not_null(templateParameter);

       // Maybe the unparser support should optionally insert the "typename" or other parameter kind support.
          string template_parameter_name = globalUnparseToString(templateParameter,unparseInfoPointer);

          bool template_parameter_name_has_been_output = false;

       // DQ (9/10/2014): We only want to output the "typename" when it is required (and exactly when it is required is not clear).
       // Note that in C++ using "class" or "typename" is equivalent.
       // template_name += "typename ";
          switch(templateParameter->get_parameterType())
             {
            // Only type parameters should require "typename" (but not if the type was explicit).
               case SgTemplateParameter::type_parameter:
                  {
                 // DQ (9/10/2014): Added support for case SgTemplateParameter::type_parameter.
                    SgType* type = templateParameter->get_type();
                    ASSERT_not_null(type);
#if 0
                    mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::buildTemplateHeaderString(): case SgTemplateParameter::type_parameter: type = %p = %s \n",type,type->class_name().c_str());
#endif
                 // If the type was explicit then don't output a redundant "typename".
                    SgTemplateType* templateType = isSgTemplateType(type);
                    if (templateType == NULL)
                       {
                      // This might tell us when to use "class: instead of "typename" but since they are equivalent we can prefer to output "typename".
                         SgClassType* classType = isSgClassType(type);
                         if (classType != NULL)
                            {
                              template_name += "typename ";
                            }
                       }
                      else
                       {
                         template_name += "typename ";
                       }

                    break;
                  }

            // Non-type parameters should not require "typename".
               case SgTemplateParameter::nontype_parameter:
                  {
                    if (templateParameter->get_expression() != NULL)
                       {
                      // unp->u_exprStmt->unparseExpression(templateParameter->get_expression(),info);
                       }
                      else
                       {
                         if (templateParameter->get_initializedName() == NULL)
                            {
                           // Not clear what this is?
                            }
                         ASSERT_not_null(templateParameter->get_initializedName());

                         SgType* type = templateParameter->get_initializedName()->get_type();
                         ASSERT_not_null(type);
#if 0
                         mfprintf(mlog [ WARN ] ) ("In NameQualificationTraversal::buildTemplateHeaderString(): case SgTemplateParameter::nontype_parameter: type = %p = %s \n",type,type->class_name().c_str());
#endif
                      // unp->u_type->outputType<SgInitializedName>(templateParameter->get_initializedName(),type,info);
                         SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
                         ASSERT_not_null(unparseInfoPointer);
                         unparseInfoPointer->set_outputCompilerGeneratedStatements();

                         unparseInfoPointer->set_isTypeFirstPart();

                         string template_parameter_name_1stpart = globalUnparseToString(type,unparseInfoPointer);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                         mfprintf(mlog [ WARN ] ) ("case SgTemplateParameter::nontype_parameter: templateParameter = %p template_parameter_name_1stpart (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name_1stpart.c_str());
#endif
                         unparseInfoPointer->unset_isTypeFirstPart();

                      // DQ (9/11/2014): Need to add a space.
                         template_parameter_name_1stpart += " ";

                         template_name += template_parameter_name_1stpart;

                      // Put out the template parameter name.
                         template_name += template_parameter_name;

                         template_parameter_name_has_been_output = true;

                         unparseInfoPointer->set_isTypeSecondPart();

                      // Output the second part of the type.
                         string template_parameter_name_2ndpart = globalUnparseToString(type,unparseInfoPointer);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                         mfprintf(mlog [ WARN ] ) ("case SgTemplateParameter::nontype_parameter: templateParameter = %p template_parameter_name_2ndpart (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name_2ndpart.c_str());
#endif
                         template_name += template_parameter_name_2ndpart;

                      // This is not really required since the SgUnparse_Info object will not be used further.
                         unparseInfoPointer->unset_isTypeSecondPart();
                       }
                    break;
                  }

               case SgTemplateParameter::template_parameter:
                  {
                    ASSERT_not_null(templateParameter->get_templateDeclaration());
                    SgNonrealDecl* nrdecl = isSgNonrealDecl(templateParameter->get_templateDeclaration());
                    ASSERT_not_null(nrdecl);

                    SgTemplateParameterPtrList & templateParameterList = nrdecl->get_tpl_params();

                    template_name += buildTemplateHeaderString(templateParameterList);

                    // FIXME when is it necessary?
                    template_name += "class ";
                    break;
                  }

               default:
                  {
                    mfprintf(mlog [ WARN ] ) ("Error: buildTemplateHeaderString(): default reached \n");
                    ROSE_ABORT();
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          mfprintf(mlog [ WARN ] ) ("templateParameter = %p template_parameter_name (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name.c_str());
#endif
       // template_name += template_parameter_name;
          if (template_parameter_name_has_been_output == false)
             {
               template_name += template_parameter_name;
             }
          i++;

          if (i != templateParameterList.end())
               template_name += ",";
        }

     template_name += " > ";

#if 0
     mfprintf(mlog [ WARN ] ) ("Leaving buildTemplateHeaderString(): template_name = %s \n",template_name.c_str());
#endif

     return template_name;
   }



// DQ (3/31/2014): Adding support for global qualifiction.
size_t
NameQualificationTraversal::depthOfGlobalNameQualification(SgDeclarationStatement* declaration)
   {
     ASSERT_not_null(declaration);
#if 0
     mfprintf(mlog [ WARN ] ) ("In depthOfGlobalNameQualification(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
     size_t depthOfNameQualification = 0;

     SgScopeStatement* scope = declaration->get_scope();
     while (isSgGlobal(scope) == NULL)
        {
#if 0
          mfprintf(mlog [ WARN ] ) ("   --- scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
          if (scope->isNamedScope() == true)
             {
               depthOfNameQualification++;
#if 0
               mfprintf(mlog [ WARN ] ) ("Incrementing depthOfNameQualification = %" PRIuPTR " \n",depthOfNameQualification);
#endif
             }

          scope = scope->get_scope();
#if 0
          mfprintf(mlog [ WARN ] ) ("   --- (after scope->get_scope()): scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
        }

#if 0
     mfprintf(mlog [ WARN ] ) ("In depthOfGlobalNameQualification(): depthOfNameQualification = %" PRIuPTR " \n",depthOfNameQualification);
#endif

     return depthOfNameQualification;
   }


// DQ (1/24/2019): display accumulated private base class map.
void
NameQualificationTraversal::displayBaseClassMap ( const string & label, BaseClassSetMap & x )
   {
  // std::map<SgClassDeclaration*,std::set<SgClassDeclaration*> > privateBaseClassSets );

     mfprintf(mlog [ WARN ] ) ("In displayBaseClassMap(): label = %s \n",label.c_str());

  // std::map<SgClassDeclaration*,std::set<SgClassDeclaration*> >::iterator i = x.begin();
     BaseClassSetMap::iterator i = x.begin();
     while (i != x.end())
        {
          SgClassDeclaration*             derivedClassDeclaration = i->first;
          std::set<SgClassDeclaration*> & privateBaseClasses      = i->second;

          mfprintf(mlog [ WARN ] ) ("  --- derivedClassDeclaration = %p = %s name = %s \n",derivedClassDeclaration,derivedClassDeclaration->class_name().c_str(),derivedClassDeclaration->get_name().str());
          mfprintf(mlog [ WARN ] ) ("  --- privateBaseClasses.size() = %zu \n",privateBaseClasses.size());

          std::set<SgClassDeclaration*>::const_iterator j = privateBaseClasses.begin();
          while (j != privateBaseClasses.end())
             {
               SgClassDeclaration* privateBaseClassDeclaration = *j;
               ASSERT_not_null(privateBaseClassDeclaration);

               mfprintf(mlog [ WARN ] ) ("  --- --- privateBaseClassDeclaration = %p = %s name = %s \n",privateBaseClassDeclaration,privateBaseClassDeclaration->class_name().c_str(),privateBaseClassDeclaration->get_name().str());

               j++;
             }
          i++;
        }

     mfprintf(mlog [ WARN ] ) ("Leaving displayBaseClassMap(): label = %s \n",label.c_str());
   }


// DQ (2/17/2019): Moved this to the name qualification file so we can work on it more easily.
bool
SgScopeStatement::hasAmbiguity(SgName & name, SgSymbol* symbol)
   {
  // DQ (2/16/2019): Added to support detection of ambiguity that drives the generation of name qualification.

  // NOTE: in the case where the declaration associated with the symbol is declared in the current scope, we can't have any ambiguity.

#define DEBUG_HAS_AMBIGUITY 0

#if DEBUG_HAS_AMBIGUITY
     mfprintf(mlog [ WARN ] ) ("\nIn SgScopeStatement::hasAmbiguity(): name = %s symbol = %p = %s \n",name.str(),symbol,symbol->class_name().c_str());
#endif

#if DEBUG_HAS_AMBIGUITY
     mfprintf(mlog [ WARN ] ) ("Print the symbol table for the current scope = %p = %s \n",this,this->class_name().c_str());
     this->get_symbol_table()->print();
#endif

  // DQ (2/23/2019): This might be a possible alternative way (maybe a better way) to detect possible ambiguity.
     size_t numberOfSymbols = this->count_symbol (name);

#if DEBUG_HAS_AMBIGUITY
     mfprintf(mlog [ WARN ] ) ("numberOfSymbols = %zu \n",numberOfSymbols);
#endif

     size_t numberOfAliasSymbols = this->count_alias_symbol(name);

#if DEBUG_HAS_AMBIGUITY
     mfprintf(mlog [ WARN ] ) ("numberOfAliasSymbols = %zu \n",numberOfAliasSymbols);
#endif

     bool ambiguityDetected = false;
     if (numberOfAliasSymbols > 1)
        {
       // Detected ambiguity that will require some name qualification.

       // If there are multiple SgAliasSymbols then we need to know if they are associated with the same
       // base class or different base classes. If all from the same base class then there is no ambiguity.
       // else if they are from multiple base classes then it is the derivation that is providing the
       // possible ambiguity, which should be resolved via additional name qualification.

          std::vector<SgNode*> causalNodeList;

          rose_hash_multimap * internal_table = this->get_symbol_table()->get_table();
          ASSERT_not_null(internal_table);

#if DEBUG_HAS_AMBIGUITY
               mfprintf(mlog [ WARN ] ) ("Before loop over symbols \n");
#endif
          std::pair<rose_hash_multimap::iterator, rose_hash_multimap::iterator> range = internal_table->equal_range (name);
          for (rose_hash_multimap::iterator i = range.first; i != range.second; ++i)
             {
               SgSymbol * orig_current_symbol = i->second;
               ASSERT_not_null(orig_current_symbol);

#if DEBUG_HAS_AMBIGUITY
               mfprintf(mlog [ WARN ] ) ("Top of loop over symbols \n");
               mfprintf(mlog [ WARN ] ) ("@@@@@@ orig_current_symbol = %p = %s \n",orig_current_symbol,orig_current_symbol->class_name().c_str());
#endif
               SgAliasSymbol* aliasSymbol = isSgAliasSymbol(orig_current_symbol);
               if (aliasSymbol != NULL)
                  {
                    size_t causalNodeCount = aliasSymbol->get_causal_nodes().size();
#if DEBUG_HAS_AMBIGUITY
                    mfprintf(mlog [ WARN ] ) (" --- causalNodeCount = %zu \n",causalNodeCount);
#endif
                    if (causalNodeCount == 1)
                       {
                      // We need to know if each of the alias symbols has a different causal node.
                         SgNode* causalNode = aliasSymbol->get_causal_nodes()[0];
#if DEBUG_HAS_AMBIGUITY
                         mfprintf(mlog [ WARN ] ) (" --- causalNode = %p \n",causalNode);
                         mfprintf(mlog [ WARN ] ) (" --- find(causalNodeList.begin(),causalNodeList.end(),causalNode) == causalNodeList.end() = %s \n",
                              find(causalNodeList.begin(),causalNodeList.end(),causalNode) == causalNodeList.end() ? "true" : "false");
#endif
                         if (find(causalNodeList.begin(),causalNodeList.end(),causalNode) == causalNodeList.end())
                            {
                              causalNodeList.push_back(causalNode);
                            }
                       }
                      else
                       {
                      // We have identified an ambiguity.

                         ROSE_ASSERT (causalNodeCount > 1);
#if DEBUG_HAS_AMBIGUITY
                         mfprintf(mlog [ WARN ] ) (" --- We have identified an ambiguity (causalNodeCount > 1): causalNodeCount = %zu \n",causalNodeCount);
#endif
                         ambiguityDetected = true;
                       }
                  }
                 else
                  {
                 // DQ (2/17/2019): This case should be addressed.
                 // I think this means that there is no ambiguity, since it would be through a single class
                 // (else the base class is mixing alias symbols with the non-alias symbols and it is less
                 // clear if there is an ambiguity (but there still could be and we would not detect it).
                 // I need a test code to demonstrate this before it can be properly addressed.
#if DEBUG_HAS_AMBIGUITY
                    mfprintf(mlog [ WARN ] ) ("Note: In SgScopeStatement::hasAmbiguity(): Found a non SgAliasSymbol: orig_current_symbol = %p = %s \n",
                         orig_current_symbol,orig_current_symbol->class_name().c_str());
#endif
                  }
#if DEBUG_HAS_AMBIGUITY
               mfprintf(mlog [ WARN ] ) ("Bottom of loop over symbols: causalNodeList.size() = %zu \n",causalNodeList.size());
#endif
             }

#if DEBUG_HAS_AMBIGUITY
          mfprintf(mlog [ WARN ] ) ("After loop over symbols: causalNodeList.size() = %zu \n",causalNodeList.size());
#endif

          if (causalNodeList.size() > 1)
             {
               ambiguityDetected = true;
             }

#if DEBUG_HAS_AMBIGUITY
          mfprintf(mlog [ WARN ] ) ("Using count_alias_symbol(): Detected ambiguity that will require some name qualification \n");
#endif
       // ambiguityDetected = true;
        }
       else
        {
       // No ambiguity that will require any name qualification.

#if DEBUG_HAS_AMBIGUITY
          mfprintf(mlog [ WARN ] ) ("Using count_alias_symbol(): No ambiguity that will require any name qualification \n");
#endif
       // Lookup the SgAliasSymbol in the class scope.
          SgAliasSymbol* aliasSymbol = this->lookup_alias_symbol(name,symbol);
       // ASSERT_not_null(aliasSymbol);
          if (aliasSymbol != NULL)
             {
               ROSE_ASSERT(aliasSymbol->get_causal_nodes().empty() == false);
               if (aliasSymbol->get_causal_nodes().size() > 1)
                  {
                 // Detected ambiguity that will require some name qualification.

#if DEBUG_HAS_AMBIGUITY
                    mfprintf(mlog [ WARN ] ) ("Detected ambiguity that will require some name qualification \n");
#endif
                    ambiguityDetected = true;
                  }
                 else
                  {
                    if (numberOfSymbols > 1)
                       {
#if DEBUG_HAS_AMBIGUITY
                         mfprintf(mlog [ WARN ] ) ("Detected multible SgSymbol are available: consider this an ambiguity: numberOfSymbols = %zu \n",numberOfSymbols);
#endif
                         ambiguityDetected = true;
                       }
                      else
                       {
                      // No ambiguity that will require any name qualification.

#if DEBUG_HAS_AMBIGUITY
                         mfprintf(mlog [ WARN ] ) ("No ambiguity that will require any name qualification \n");
#endif
                       }
                  }
             }
            else
             {
#if DEBUG_HAS_AMBIGUITY
               mfprintf(mlog [ WARN ] ) ("No SgAliasSymbol is available \n");
#endif
               if (numberOfSymbols > 1)
                  {
#if DEBUG_HAS_AMBIGUITY
                    mfprintf(mlog [ WARN ] ) ("Detected multible SgSymbol are available: consider this an ambiguity: numberOfSymbols = %zu \n",numberOfSymbols);
#endif
                    ambiguityDetected = true;
                  }
                 else
                  {
#if DEBUG_HAS_AMBIGUITY
                    mfprintf(mlog [ WARN ] ) ("No SgSymbol or only one SgSymbol is available \n");
#endif
                  }
             }
        }

#if DEBUG_HAS_AMBIGUITY
     mfprintf(mlog [ WARN ] ) ("ambiguityDetected = %s \n",ambiguityDetected ? "true" : "false");
#endif

     return ambiguityDetected;
   }

void
clearNameQualificationAda()
   {
      SgNode::get_globalQualifiedNameMapForNames().clear();
      SgNode::get_globalQualifiedNameMapForMapsOfTypes().clear();
   }
