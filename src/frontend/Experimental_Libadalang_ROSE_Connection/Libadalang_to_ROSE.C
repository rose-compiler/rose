#include "sage3basic.h"

#include <type_traits>
#include <algorithm>
#include <deque>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/algorithm/string.hpp>

#include "Rose/Diagnostics.h"
#include "rose_config.h"
#include "sageGeneric.h"
#include "sageBuilder.h"
#include "Libadalang_to_ROSE.h"
#include "AdaMaker.h"
#include "LibadalangExpression.h"
#include "LibadalangStatement.h"
#include "LibadalangType.h"
#include "cmdline.h"

#include "sageInterfaceAda.h"


// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Libadalang_ROSE_Translation {

namespace{
  /// stores a mapping from hash to SgInitializedName
  map_t<int, SgInitializedName*> libadalangVarsMap;

  /// stores a mapping from hash to Exception declaration
  map_t<int, SgInitializedName*> libadalangExcpsMap;

  /// stores a mapping from hash to SgInitializedName
  map_t<int, SgDeclarationStatement*> libadalangDeclsMap;

  /// stores a mapping from hash to ROSE type declaration
  map_t<int, SgDeclarationStatement*> libadalangTypesMap;

  map_t<int, SgBasicBlock*> libadalangBlocksMap;

  /// stores a mapping from hash to builtin type nodes
  map_t<int, SgType*> adaTypesMap;

  /// stores a mapping from string to builtin type nodes
  map_t<AdaIdentifier, SgType*> adaTypesByNameMap;

  /// stores a mapping from hash to builtin exception nodes
  map_t<int, SgInitializedName*> adaExcpsMap;

  /// stores a mapping from string to builtin pkgs (std, ascii)
  map_t<int, SgAdaPackageSpecDecl*> adaPkgsMap;

  /// stores variables defined in Standard or Ascii
  map_t<int, SgInitializedName*> adaVarsMap;

  /// map of inherited symbols
  std::map<InheritedSymbolKey, SgAdaInheritedFunctionSymbol*> inheritedSymbolMap;

  /// maps generated operators
  map_t<OperatorKey, std::vector<OperatorDesc> > operatorSupportMap;

  std::vector<SgExpression*> operatorExprsVector;

} //end unnamed namespace

map_t<int, SgInitializedName*>&                                libadalangVars() { return libadalangVarsMap;   }
map_t<int, SgInitializedName*>&                               libadalangExcps() { return libadalangExcpsMap;  }
map_t<int, SgDeclarationStatement*>&                          libadalangDecls() { return libadalangDeclsMap;  }
map_t<int, SgDeclarationStatement*>&                          libadalangTypes() { return libadalangTypesMap;  }
map_t<int, SgBasicBlock*>&                                   libadalangBlocks() { return libadalangBlocksMap; }
map_t<int, SgType*>&                                                 adaTypes() { return adaTypesMap;         }
map_t<AdaIdentifier, SgType*>&                                 adaTypesByName() { return adaTypesByNameMap;   }
map_t<int, SgInitializedName*>&                                      adaExcps() { return adaExcpsMap;         }
map_t<int, SgAdaPackageSpecDecl*>&                                    adaPkgs() { return adaPkgsMap;          }
map_t<int, SgInitializedName*>&                                       adaVars() { return adaVarsMap;          }
std::map<InheritedSymbolKey, SgAdaInheritedFunctionSymbol*>& inheritedSymbols() { return inheritedSymbolMap;  }
std::vector<SgExpression*>&                                     operatorExprs() { return operatorExprsVector; }
map_t<OperatorKey, std::vector<OperatorDesc> >&               operatorSupport() { return operatorSupportMap;  }

/// Function to turn an ada_text_type into a string
std::string dot_ada_text_type_to_string(ada_text_type input_text){
    //ada_text_type does not have a func to convert to string,
    // so we convert to ada_text using LibadalangText
    LibadalangText value_text(input_text);
    return value_text.string_value();
}

/// Function to turn an ada_unbounded_text_type_array into a string
std::string dot_ada_unbounded_text_type_to_string(ada_unbounded_text_type_array input_text){
    //ada_unbounded_text_type_array does not have a func to convert to string,
    // so we convert to ada_text using LibadalangText
    LibadalangText value_text(input_text);
    return value_text.string_value();
}

/// Function to get the source location of an ada node as a string
std::string dot_ada_full_sloc(ada_base_entity *node){
    ada_text_type file_name;
    ada_source_location_range line_numbers;

    //Get the location of the text corresponding to this node
    ada_node_sloc_range(node, &line_numbers);
    //Get the file name this node is from
    ada_ada_node_full_sloc_image(node, &file_name);
    std::string file_name_string = dot_ada_text_type_to_string(file_name);
    std::string::size_type pos = file_name_string.find(':');
    if(pos != std::string::npos){
        file_name_string = file_name_string.substr(0, pos);
    }

    std::string full_sloc = file_name_string + " - ";
    full_sloc += std::to_string(line_numbers.start.line) + ":" + std::to_string(line_numbers.start.column) + " .. ";
    full_sloc += std::to_string(line_numbers.end.line) + ":" + std::to_string(line_numbers.end.column);
    return full_sloc;
}

//
// logger

extern Sawyer::Message::Facility mlog;

static bool fail_on_error = true;

/// returns true of the kind is of interest
static inline
bool traceKind(const char* /* kind */)
{
  return true;
}

/// Gets the p_canonical_text field for a node and returns it as a string
std::string canonical_text_as_string(ada_base_entity* lal_element){
  ada_symbol_type p_canonical_text;
  ada_single_tok_node_p_canonical_text(lal_element, &p_canonical_text);
  LibadalangText ada_canonical_text(&p_canonical_text);
  std::string canonical_text_string = ada_canonical_text.string_value();
  return canonical_text_string;
}

void logKind(const char* kind, int elemID)
{
  if (!traceKind(kind)) return;

  logTrace() << kind;
  if (elemID > 0) logTrace() << ' ' << elemID;
  logTrace() << std::endl;
}

LabelAndLoopManager::~LabelAndLoopManager()
{
  for (GotoContainer::value_type el : gotos)
  {
    //~ el.first->set_label(&lookupNode(labels, el.second));
    //ADA_ASSERT(el.first->get_label_expression() == nullptr);
    el.first->set_label_expression(&mkLabelRefExp(lookupNode(labels, el.second)));
  }
}


void LabelAndLoopManager::label(int hash, SgLabelStatement& lblstmt)
{
  SgLabelStatement*& mapped = labels[hash];

  //ADA_ASSERT(mapped == nullptr);
  if(mapped != nullptr){
    logWarn() << "Overwriting value in labels!\n";
  }
  mapped = &lblstmt;
}

void LabelAndLoopManager::gotojmp(int hash, SgGotoStatement& gotostmt)
{
  gotos.emplace_back(&gotostmt, hash);
}

AstContext
AstContext::unscopedBlock(SgAdaUnscopedBlock& blk) const
{
  AstContext tmp{*this};

  tmp.stmtHandler = [&blk](AstContext, SgStatement& stmt)
                    {
                      sg::linkParentChild(blk, stmt, &SgAdaUnscopedBlock::append_statement);
                    };

  return tmp;
}

AstContext AstContext::pragmas(PragmaContainer& allPragmas) const
{
  AstContext tmp{*this};

  tmp.all_pragmas = &allPragmas;
  return tmp;
}

AstContext AstContext::deferredUnitCompletionContainer(DeferredCompletionContainer& cont) const
{
  AstContext tmp{*this};

  tmp.unit_completions = &cont;
  return tmp;
}

void AstContext::storeDeferredUnitCompletion(DeferredCompletion completion) const
{
  SG_DEREF(unit_completions).emplace_back(std::move(completion));
}


AstContext
AstContext::instantiation(SgAdaGenericInstanceDecl& instance) const
{
  AstContext tmp{*this};

  tmp.enclosing_instantiation = &instance;
  return tmp;
}

AstContext
AstContext::scope(SgScopeStatement& s) const
{
  //ADA_ASSERT(s.get_parent());

  AstContext tmp{*this};

  tmp.the_scope = &s;
  return tmp;
}

AstContext
AstContext::labelsAndLoops(LabelAndLoopManager& lm) const
{
  AstContext tmp{*this};

  tmp.all_labels_loops = &lm;
  return tmp;
}

AstContext
AstContext::sourceFileName(std::string& file) const
{
  AstContext tmp{*this};

  tmp.unit_file_name = &file;
  return tmp;
}

AstContext
AstContext::pragmaAspectAnchor(SgDeclarationStatement& dcl) const
{
  AstContext tmp{*this};

  tmp.pragma_aspect_anchor = &dcl;
  return tmp;
}

AstContext AstContext::unit_root(ada_base_entity* unit_root_lal) const
{
  AstContext tmp{*this};

  tmp.lal_unit_root = unit_root_lal;
  return tmp;
}

// static
void
AstContext::defaultStatementHandler(AstContext ctx, SgStatement& s)
{
  SgScopeStatement& scope = ctx.scope();

  scope.append_statement(&s);
  //ADA_ASSERT(s.get_parent() == &scope);
}

void handleElement(ada_base_entity* lal_element, AstContext ctx, bool isPrivate)
{
    //Get the kind of this node
    ada_node_kind_enum kind = ada_node_kind(lal_element);

    LibadalangText kind_name(kind);
    std::string kind_name_string = kind_name.string_value();
    logTrace() << "handleElement called on a " << kind_name_string << std::endl;

    switch (kind)
    {
        case ada_subp_body:             // Asis.Declarations
        case ada_subp_decl:
        case ada_null_subp_decl:
        case ada_subp_body_stub:
        case ada_abstract_subp_decl:
        case ada_package_body:
        case ada_package_decl:
        case ada_object_decl:
        case ada_subtype_decl:
        case ada_type_decl:
        case ada_task_type_decl:
        case ada_task_body:
        case ada_entry_decl:
        case ada_component_decl:
        case ada_exception_decl:
        case ada_number_decl:
        case ada_single_protected_decl:
        case ada_protected_body:
        {
          handleDeclaration(lal_element, ctx, isPrivate);
          break;
        }

      /*case A_Clause:                  // Asis.Clauses
        {
          handleClause(elem, ctx);
          break;
        }

      case A_Defining_Name:           // Asis.Declarations
        {
          // handled by getName
          ROSE_ABORT();
        }

      case A_Statement:               // Asis.Statements
        {
          // handled in StmtCreator
          ROSE_ABORT();
        }

      case An_Expression:             // Asis.Expressions
        {
          // handled by getExpr
          ROSE_ABORT();
        }

      case An_Exception_Handler:      // Asis.Statements
        {
          // handled by handleExceptionHandler
          ROSE_ABORT();
        }*/

      case ada_variant_part:          // Asis.Definitions
      case ada_null_component_decl:
        {
          handleDefinition(lal_element, ctx);
          break;
        }

      /*case A_Pragma:                  // Asis.Elements
        {
          handlePragma(elem, nullptr, ctx);
          break;
        }

      case Not_An_Element:  // Nil_Element
      case A_Path:                    // Asis.Statements
      case An_Association:            // Asis.Expressions */ //TODO Figure out the rest of these mappings
      default:
        logWarn() << "Unhandled element " << kind_name_string << std::endl;
        //ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }
}

namespace{
  /// clears all mappings created during translation
  void clearMappings()
  {
    libadalangVars().clear();
    //libadalangExcps().clear();
    libadalangDecls().clear();
    libadalangTypes().clear();
    //libadalangBlocks().clear();
    adaTypes().clear();
    //adaExcps().clear();
    adaPkgs().clear();
    adaVars().clear();
    //~ adaFuncs().clear();

    inheritedSymbols().clear();
    operatorSupport().clear();
    operatorExprs().clear();
  }

  void handleUnit(ada_base_entity* lal_unit, AstContext context, const std::string& src_file_name)
  {

    //First, make sure this is a unit node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_unit);

    if(kind != ada_compilation_unit){
        logError() << "handleUnit provided incorrect node kind!\n";
    }


    bool        processUnit   = true;
    //bool        logParentUnit = false;
    //bool        logBodyUnit   = false;

    LibadalangText kind_name(kind);
    std::string kind_name_string = kind_name.string_value();

    if (processUnit)
    {
      //Get the body node
      ada_base_entity unit_body;
      ada_compilation_unit_f_body(lal_unit, &unit_body);

     /*//Get the name of the file this unit comes from
     ada_text_type file_name;
     ada_ada_node_full_sloc_image(&unit_body, &file_name);
     std::string file_name_string = dot_ada_text_type_to_string(file_name);
     std::string::size_type pos = file_name_string.find(':');
     if(pos != std::string::npos){
         file_name_string = file_name_string.substr(0, pos);
     }
      
      ada_unbounded_text_type_array p_syntactic_fully_qualified_name;
      int result = ada_compilation_unit_p_syntactic_fully_qualified_name(lal_unit, &p_syntactic_fully_qualified_name);

      std::string                             unitFile = dot_ada_unbounded_text_type_to_string(p_syntactic_fully_qualified_name);*/
      //TODO Find a way to get the full file path for the unit from the root node

      std::string                             unitFile = src_file_name;
      unitFile = unitFile;
      logInfo() << "Unit name is " << unitFile << std::endl;
      AstContext::PragmaContainer             pragmalist;
      AstContext::DeferredCompletionContainer compls;
      AstContext                              ctx = context.sourceFileName(unitFile)
                                                           .pragmas(pragmalist)
                                                           .deferredUnitCompletionContainer(compls);

      logTrace()   << "handleUnit called on a " << kind_name_string << std::endl;

      /*if (logParentUnit)
        logTrace() << "\n " << adaUnit.Corresponding_Parent_Declaration << " (Corresponding_Parent_Declaration)";

      if (logBodyUnit)
        logTrace() << "\n " << adaUnit.Corresponding_Body << " (Corresponding_Body)";

      logTrace()   << std::endl;*/ //TODO Add these features

      //ElemIdRange range = idRange(adaUnit.Context_Clause_Elements); //TODO ???
      bool        privateDecl = false;


      //The body node can be either ada_library_item or ada_subunit
      kind = ada_node_kind(&unit_body);

      ada_base_entity unit_declaration;
      //Fetch the unit declaration based on which kind we got
      if(kind == ada_library_item){
          ada_library_item_f_item(&unit_body, &unit_declaration);
          //Update the privateDecl field
          ada_base_entity ada_private_node;
          ada_library_item_f_has_private(&unit_body, &ada_private_node);
          ada_node_kind_enum ada_private_kind = ada_node_kind(&ada_private_node);
          privateDecl = (ada_private_kind == ada_private_present);
      } else {
          ada_subunit_f_body(&unit_body, &unit_declaration);
      }
      
      //traverseIDs(range, elemMap(), ElemCreator{ctx}); handle the pragmas/prelude/with
      handleElement(&unit_declaration, ctx, privateDecl);

      //processAndPlacePragmas(adaUnit.Compilation_Pragmas, { &ctx.scope() }, ctx);

      for (AstContext::DeferredCompletion& c : compls) c();
    }
  }

  const SgFunctionCallExp* callNode(const SgFunctionRefExp& fnref)
  {
    return isSgFunctionCallExp(fnref.get_parent());
  }

/*
  const SgExpressionPtrList& paramList(const SgCallExpression* call)
  {
    ASSERT_not_null(call);

    return SG_DEREF(call->get_args()).get_expressions();
  }
*/

  const SgFunctionCallExp* parentCallNode(const SgExprListExp* arglst)
  {
    return arglst ? isSgFunctionCallExp(arglst->get_parent()) : nullptr;
  }

  const SgFunctionCallExp* parentCallNode(const SgCallExpression* call)
  {
    ASSERT_not_null(call);

    const SgNode* parent = call->get_parent();

    if (const SgActualArgumentExpression* actarg = isSgActualArgumentExpression(parent))
      parent = actarg->get_parent();

    return parentCallNode(isSgExprListExp(parent));
  }

  using OverloadSet = std::vector<SgFunctionSymbol*>;

  struct OverloadInfo : std::tuple<SgFunctionSymbol*, OverloadSet, bool>
  {
    using base = std::tuple<SgFunctionSymbol*, OverloadSet, bool>;
    using base::base;

    /// the symbol which was originally in place
    SgFunctionSymbol*  orig_sym() const { return std::get<0>(*this); }

    /// the overload set
    /// \{
          OverloadSet& ovlset()         { return std::get<1>(*this); }
    const OverloadSet& ovlset() const   { return std::get<1>(*this); }
    /// \}

    /// \todo reconsider whether foldable is needed ..
    /// true, iff all arguments are literals or calls to literal-equivalent functions
    /// \{
          bool&        literalEquivalent()       { return std::get<2>(*this); }
    const bool&        literalEquivalent() const { return std::get<2>(*this); }
    /// \}
  };

  using OverloadMap = std::map<SgFunctionRefExp*, OverloadInfo>;
  using WorkItem    = OverloadMap::iterator;

  struct WorkItems : private std::deque<WorkItem>
  {
    using base = std::deque<WorkItem>;

    WorkItems() = default;

    using base::size;

    void add(WorkItem item) { base::push_back(item); }

    WorkItem next()
    {
      WorkItem item = base::front();

      base::pop_front();
      return item;
    }

    bool empty() const { return base::size() == 0; }
  };

  bool
  testProperty(const Sg_File_Info* n, bool (Sg_File_Info::*property)() const)
  {
    return n && (n->*property)();
  }

  bool
  testProperty(const SgLocatedNode& n, bool (Sg_File_Info::*property)() const)
  {
    return (  testProperty(n.get_file_info(),        property)
           || testProperty(n.get_startOfConstruct(), property)
           || testProperty(n.get_endOfConstruct(),   property)
           );
  }

  bool
  isAdaLiteralList(const SgExpression* arg);

  bool isAdaLiteralExp(const SgExpression* e)
  {
    if (const SgActualArgumentExpression* act = isSgActualArgumentExpression(e))
      return isAdaLiteralExp(act->get_expression());

    return isSgValueExp(e) || isAdaLiteralList(e);
  }

  bool
  isAdaLiteralList(const SgExpression* arg)
  {
    return isSgAggregateInitializer(arg);
    //~ const SgExprListExp* lst = isSgExprListExp(arg);
    //~ if (!lst) return false;

    //~ const SgExpressionPtrList& exprs = lst->get_expressions();

    //~ return std::all_of( exprs.begin(), exprs.end(),
                        //~ [](const SgExpression* e) { return isAdaLiteralExp(e); }
                      //~ );
  }

  bool isLiteralEquivalent(const OverloadMap& m, const SgFunctionCallExp& call, const SgFunctionRefExp& fnref)
  {
    // function has been compiler generated
    //   and has no definition ...
    const SgFunctionSymbol*      fnsym  = fnref.get_symbol();

    if (fnsym == nullptr)
    {
      logWarn() << "function symbol not available" << std::endl;
      return false;
    }

    const SgFunctionDeclaration* fndcl  = fnsym->get_declaration();
    if (!fndcl || !testProperty(*fndcl, &Sg_File_Info::isCompilerGenerated) || fndcl->get_definingDeclaration())
      return false;

    // all arguments are literals or literal equivalent
    const SgExprListExp&         args   = SG_DEREF(call.get_args());
    const SgExpressionPtrList&   arglst = args.get_expressions();
    auto  isLitEquCall = [&m](SgFunctionRefExp* r) -> bool
                         {
                           if (r == nullptr) return false;

                           auto pos = m.find(r);

                           return (pos != m.end()) && pos->second.literalEquivalent();
                         };

    auto  isLiteralEquExp = [isLitEquCall](SgExpression* e)-> bool
                            {
                              return isAdaLiteralExp(e) || isLitEquCall(isSgFunctionRefExp(e));
                            };

    return std::all_of(arglst.begin(), arglst.end(), isLiteralEquExp);
  }


  struct AmbiguousCallExtractor : AstSimpleProcessing
  {
      explicit
      AmbiguousCallExtractor(decltype(inheritedSymbolMap)& inhsymbols)
      : AstSimpleProcessing(), m(), inhsyms(inhsymbols)
      {}

      void visit(SgNode* sageNode) override
      {
        using iterator   = decltype(inhsyms.begin());
        using value_type = decltype(*inhsyms.begin());

        SgFunctionCallExp*           call = isSgFunctionCallExp(sageNode);
        if (call == nullptr) return;

        SgFunctionRefExp*            fnref = isSgFunctionRefExp(call->get_function());
        if (fnref == nullptr) return;

        const SgFunctionDeclaration* fndcl = fnref->getAssociatedFunctionDeclaration();
        if (fndcl == nullptr) return;

        iterator const beg = inhsyms.lower_bound(std::make_pair(fndcl,   nullptr));
        iterator const lim = inhsyms.lower_bound(std::make_pair(fndcl+1, nullptr));

        // we add all calls to the overload map, b/c they may be needed for
        //   ambiguity resolution in context.
        if (beg != lim) ++reqdisambig;

        OverloadSet       overloads;
        SgFunctionSymbol* fnsym = fnref->get_symbol();

        overloads.emplace_back(fnsym);
        std::transform( beg, lim,
                        std::back_inserter(overloads),
                        [](value_type& val) -> SgFunctionSymbol*
                        {
                          return val.second;
                        }
                      );

        m.emplace(fnref, OverloadInfo{fnsym, std::move(overloads), isLiteralEquivalent(m, *call, *fnref)});
        //~ logTrace() << "adding " << fnref << std::endl;
      }

      operator OverloadMap () &&
      {
        return std::move(m);
      }

      int                           reqdisambig = 0;
    private:
      OverloadMap                   m;
      decltype(inheritedSymbolMap)& inhsyms;
  };

  OverloadMap collectAllFunctionRefExp(SgGlobal& scope)
  {
    AmbiguousCallExtractor extractor{inheritedSymbols()};

    extractor.traverse(&scope, postorder);

    logInfo() << "Calls requiring disambiguation: " << extractor.reqdisambig << std::endl;
    return extractor;
  }

  WorkItems createWorksetFrom(OverloadMap& m)
  {
    WorkItems res;

    for (WorkItem pos = m.begin(), lim = m.end(); pos != lim; ++pos)
      res.add(pos);

    return res;
  }

  struct ArgParamTypeCompatibility
  {
    static
    bool areCompatible(const SgType& arg, const SgType& prm)
    {
      // a constant's type is determined by the context
      //   \note returning always true oversimplifies...
      if (isSgAutoType(&arg)) return true;

      // currently we do not resolve pointer types
      // \note not sure if type derivation can be pointer based
      // \todo revise and complete as needed
      if (isSgAdaAccessType(&arg)) return true;

      si::Ada::TypeDescription prmRoot = si::Ada::typeRoot(const_cast<SgType&>(prm));

      // \todo this assumes that the argument is a subtype of prmRoot
      if (prmRoot.polymorphic())
      {
        logInfo() << "polymorphic arg type" << std::endl;
        return true;
      }

      si::Ada::TypeDescription argRoot = si::Ada::typeRoot(const_cast<SgType&>(arg));
      const bool               res = (  (argRoot.typerep() != nullptr)
                                     && (argRoot.typerep() == prmRoot.typerep())
                                     );
      if (false)
      {
        logFlaw() << res << '\n'
                  << " * a " << (argRoot.typerep() ? typeid(*argRoot.typerep()).name() : std::string{})
                  << " " << argRoot.typerep() << " / "
                  << typeid(arg).name() << " " << &arg << '\n'
                  << " * p " << (prmRoot.typerep() ? typeid(*prmRoot.typerep()).name() : std::string{})
                  << " " << prmRoot.typerep() << " / "
                  << typeid(prm).name() << " " << &prm << '\n'
                  << std::flush;

        if (SgTypedefType* tydef = isSgTypedefType(prmRoot.typerep()))
        {
          logFlaw() << "prmtydef: " << tydef->get_name() << " -> "
                    << typeid(*isSgTypedefDeclaration(tydef->get_declaration())->get_base_type()).name()
                    << std::endl;
        }

        if (SgTypedefType* tydef = isSgTypedefType(argRoot.typerep()))
        {
          logFlaw() << "argtydef: " << tydef->get_name() << " -> "
                    << typeid(*isSgTypedefDeclaration(tydef->get_declaration())->get_base_type()).name()
                    << std::endl;
        }
      }

      return res;
    }

    bool operator()(const std::set<const SgType*>& args, const SgType* prm) const
    {
      ASSERT_not_null(prm);

      // when the set is empty, the argument does not participate in overload resolution
      //   e.g., the set is empty when an argument is defaulted.
      if (args.empty()) return true;

      auto areTypeCompatible = [prm](const SgType* arg) -> bool
                               {
                                 return areCompatible(SG_DEREF(arg), *prm);
                               };

      return std::any_of( args.begin(), args.end(), areTypeCompatible );
    }

    bool operator()(const SgType* arg, const std::set<const SgType*>& parms) const
    {
      if (arg == nullptr)
      {
        logFlaw() << "null function return" << std::endl;
        return true;
      }

      // when the set is empty, the argument does not participate in overload resolution
      //   e.g., the set is empty when an argument is defaulted.
      if (parms.empty()) return true;

      auto areTypeCompatible = [arg](const SgType* prm) -> bool
                               {
                                 return areCompatible(*arg, SG_DEREF(prm));
                               };

      return std::any_of( parms.begin(), parms.end(), areTypeCompatible );
    }
  };


  std::set<const SgType*>
  simpleExpressionType(const SgExpression& arg)
  {
    // literals are convertible to derived types
    //    without further type resolution, it is unclear what type this
    //    literals have. Thus, exclude literals from participating in
    //    overload resolution for now.
    if (isAdaLiteralExp(&arg))
      return {};

    return { si::Ada::typeOfExpr(arg).typerep() };
    // return { arg.get_type() };
  }

  const SgType*
  functionReturnType(const SgFunctionSymbol* sym)
  {
    return si::Ada::functionType(SG_DEREF(sym)).get_return_type();
/*
 *  was:
    const SgFunctionType* fnty = isSgFunctionType(sym->get_type());

    return fnty ? fnty->get_return_type() : nullptr;
 */
  }

  const std::vector<SgType*>&
  parameterTypes(const SgFunctionSymbol* sym)
  {
    return si::Ada::functionType(SG_DEREF(sym)).get_arguments();
  }


  std::set<const SgType*>
  resultTypes(const SgExpression* parg, const OverloadMap& allrefs)
  {
    using ResultType = decltype(resultTypes(parg, allrefs));

    if (parg == nullptr)
      return { /* empty set */ };

    const SgExpression& arg   = *parg;
    SgFunctionRefExp*   fnref = nullptr;

    if (const SgCallExpression* call = isSgCallExpression(&arg))
      fnref = isSgFunctionRefExp(call->get_function());

    if (fnref == nullptr)
      return simpleExpressionType(arg);

    const SgFunctionDeclaration* fndcl = fnref->getAssociatedFunctionDeclaration();
    if (fndcl && testProperty(*fndcl, &Sg_File_Info::isCompilerGenerated))
      return {};

    OverloadMap::const_iterator pos = allrefs.find(fnref);

    if (pos == allrefs.end()) // fallback ..
      return simpleExpressionType(arg);

    ResultType res;

    for (const SgFunctionSymbol* sym : pos->second.ovlset())
      res.insert(functionReturnType(sym));

    return res;
  }

  std::vector<std::set<const SgType*> >
  argumentTypes(const SgExpressionPtrList& args, OverloadMap& allrefs)
  {
    decltype(argumentTypes(args, allrefs)) res;

    for (const SgExpression* arg : args)
      res.emplace_back(resultTypes(arg, allrefs));

    return res;
  }

  bool isComparisonOperator(const SgFunctionDeclaration& fn)
  {
    static const std::string eq  = si::Ada::roseOperatorPrefix + "=";
    static const std::string neq = si::Ada::roseOperatorPrefix + "/=";

    const std::string name = fn.get_name();

    return (  boost::iequals(name, eq)
           || boost::iequals(name, neq)
           );
  }

  const SgFunctionParameterList&
  parameterList(const SgFunctionSymbol& fnsym)
  {
    if (const SgAdaInheritedFunctionSymbol* inhsym = isSgAdaInheritedFunctionSymbol(&fnsym))
      if (const SgFunctionSymbol* pubsym = inhsym->get_publiclyVisibleFunctionSymbol())
      {
        return parameterList(*pubsym);
      }

    const SgFunctionDeclaration& fndcl = SG_DEREF(fnsym.get_declaration());
    return SG_DEREF(fndcl.get_parameterList());
  }

  SgExpressionPtrList
  normalizedArguments(const SgFunctionCallExp& fncall, const OverloadMap& allrefs)
  {
    SgFunctionRefExp* fnref = isSgFunctionRefExp(fncall.get_function());
    if (fnref == nullptr) return {};

    for (const SgFunctionSymbol* fnsym : allrefs.at(fnref).ovlset())
    {
      try
      {
        return si::Ada::normalizedCallArguments2(fncall, parameterList(SG_DEREF(fnsym)));
      }
      catch (const std::logic_error& e) {}
    }

    logError() << "Unable to normalize argument list in call "
               << fncall.unparseToString()
               << std::endl;
    return {};
  }

  SgExpressionPtrList
  normalizedArguments(const SgFunctionCallExp* fncall, const OverloadMap& allrefs)
  {
    return normalizedArguments(SG_DEREF(fncall), allrefs);
  }

/*
  std::set<const SgType*>
  typesFromCallContext( const SgFunctionCallExp& parentCall,
                        const SgFunctionCallExp& childCall,
                        const OverloadMap& allrefs
                      );
*/

  std::set<const SgType*>
  callTypeEqualityConstraint( const SgFunctionCallExp& parentCall,
                              std::size_t pos,
                              const OverloadMap& allrefs
                            )
  {
    const SgExpression* arg = normalizedArguments(parentCall, allrefs).at(pos);

    return resultTypes(arg, allrefs);
  }

  /// returns eligible return types of a childcall when the
  ///   child is part of an argument list of a parent call.
  /// returns the empty set when the contextual types shall not be used
  /// (due to current inaccuracies, for example, when the parent call
  /// calls a compiler generated operator.)
  std::set<const SgType*>
  typesFromCallContext( const SgFunctionCallExp& parentCall,
                        const SgFunctionCallExp& childCall,
                        const OverloadMap& allrefs
                      )
  {
    SgFunctionRefExp*            fnref = isSgFunctionRefExp(parentCall.get_function());
    OverloadMap::const_iterator  ovpos = allrefs.find(fnref);

    if (ovpos == allrefs.end())
      return {};

    const SgFunctionDeclaration* fndcl = fnref->getAssociatedFunctionDeclaration();

    if (fndcl == nullptr)
      return {};

    // do not trust arguments of compiler generated functions
    const bool compilerGenerated     = testProperty(*fndcl, &Sg_File_Info::isCompilerGenerated);
    const bool compilerGenComparison = compilerGenerated && isComparisonOperator(*fndcl);

    if (compilerGenerated && !compilerGenComparison)
      return {};

    std::set<const SgType*> res;

    try
    {
      // \todo instead of computing the normalizedArgumentPosition every time,
      //       the information could be memoized.
      //       TODO_MEMOIZE_NORMALIZED_ARGPOS
      std::size_t argpos = si::Ada::normalizedArgumentPosition(parentCall, childCall);

      if (!compilerGenComparison)
      {
        for (SgFunctionSymbol* fnsym : ovpos->second.ovlset())
          res.insert(parameterTypes(fnsym).at(argpos));
      }
      else
      {

        res = callTypeEqualityConstraint(parentCall, 1 - argpos, allrefs);
      }
    }
    catch (const std::logic_error& e)
    {
      /* catches exceptions from normalizedArgumentPosition. */
      logWarn() << "ex: " << e.what() << std::endl;
    }

    return res;
  }

  std::set<const SgType*>
  typesFromAssignContext(const SgAssignOp& parentAssign, const SgFunctionCallExp& childCall)
  {
    if(parentAssign.get_rhs_operand() != &childCall){
      logWarn() << "Parent & child do not match in typesFromAssignContext.\n";
    }

    const SgExpression& lhs = SG_DEREF(parentAssign.get_lhs_operand());

    return { lhs.get_type() };
  }

  std::set<const SgType*>
  typesFromAssignInitializer( const SgAssignInitializer& assignIni,
                              const SgFunctionCallExp& /* childCall */
                            )
  {
    if (const SgInitializedName* var = isSgInitializedName(assignIni.get_parent()))
      return { var->get_type() };

    return { };
  }

  std::set<const SgType*>
  typesFromExpression(const SgExpression* exp)
  {
    const SgType* ty = si::Ada::typeOfExpr(const_cast<SgExpression*>(exp)).typerep();

    if (ty == nullptr)
      return {};

    return { ty };
  }

  std::set<const SgType*>
  typesFromResolvedCallContext(const SgFunctionCallExp& parentCall, const SgFunctionCallExp& childCall)
  {
    std::set<const SgType*> res;

    try
    {
      // \todo instead of computing the normalizedArgumentPosition every time,
      //       the information could be memoized.
      //       TODO_MEMOIZE_NORMALIZED_ARGPOS
      const std::size_t       argpos = si::Ada::normalizedArgumentPosition(parentCall, childCall);
      const SgFunctionRefExp* fnref  = isSgFunctionRefExp(parentCall.get_function());

      if (fnref == nullptr)
      {
        logFlaw() << "resolve context for fn-pointer calls .. [incomplete]"
                  << std::endl;
      }
      else
      {
        res = { parameterTypes(fnref->get_symbol()).at(argpos) };
      }
    }
    catch (std::logic_error& e)
    {
      logTrace() << "typesFromResolvedCallContext: ex " << e.what() << std::endl;
    }

    return res;
  }

  struct ExpectedTypes : sg::DispatchHandler<std::set<const SgType*> >
  {
    using base = sg::DispatchHandler<std::set<const SgType*> >;
    using CallContextHandler = std::function<std::set<const SgType*>(const SgFunctionCallExp&, const SgFunctionCallExp&)>;

    ExpectedTypes(const SgFunctionCallExp& call, CallContextHandler callCtxHandler)
    : base(), origCall(call), callContextHandler(std::move(callCtxHandler))
    {}

    void handle(const SgNode& n)
    {
      // SG_UNEXPECTED_NODE(n);
      logFlaw() << "unrecognizedCall context: " << typeid(n).name()
                << std::endl;
    }

    //
    // Expression contexts

    // assignment and initialization
    void handle(const SgAssignOp& n)                 { res = typesFromAssignContext(n, origCall); }
    void handle(const SgAssignInitializer& n)        { res = typesFromAssignInitializer(n, origCall); };

    // calls
    // void handle(const SgFunctionCallExp& n)          {  }

    // logical operators
    void handle(const SgOrOp&)                       { res = { adaTypesByName().at(AdaIdentifier{"BOOLEAN"}) }; }
    void handle(const SgAndOp&)                      { res = { adaTypesByName().at(AdaIdentifier{"BOOLEAN"}) }; }

    // other expressions
    void handle(const SgCastExp& n)
    {
      const bool    qualexpr = n.get_cast_type() == SgCastExp::e_ada_type_qualification;

      // for true casts, we cannot make any assumption.
      if (!qualexpr) return;

      res = { n.get_type() };
    }

    void handle(const SgActualArgumentExpression& n) { res = sg::dispatch(*this, n.get_parent()); }
    void handle(const SgMembershipOp& n)             { res = typesFromExpression(n.get_rhs_operand()); }
    void handle(const SgNonMembershipOp& n)          { res = typesFromExpression(n.get_rhs_operand()); }

    void handle(const SgExprListExp& n)
    {
      if (const SgFunctionCallExp* call = isSgFunctionCallExp(n.get_parent()))
      {
        res = callContextHandler(*call, origCall);
        return;
      }

      handle(static_cast<const SgNode&>(n));
    }

    //
    // Relevant statements

    void handle(const SgExprStatement&)              { /* procedure call */ }

    void handle(const SgReturnStmt& n)
    {
      const SgFunctionDeclaration& fndcl = sg::ancestor<SgFunctionDeclaration>(n);
      const SgFunctionType&        fnty  = SG_DEREF(fndcl.get_type());

      res = { fnty.get_return_type() };
    }

    private:
      const SgFunctionCallExp& origCall;
      CallContextHandler       callContextHandler;
  };

  // computes the types as expected from the call context and all currently viable functions
  //   if the type should not be used, or the call is to a procedure
  //   return the empty set.
  std::set<const SgType*>
  expectedTypes(const SgFunctionCallExp* call, const OverloadMap& allrefs)
  {
    auto callHandler = [&allrefs]
                       (const SgFunctionCallExp& parentCall, const SgFunctionCallExp& childCall) -> std::set<const SgType*>
                       {
                         return typesFromCallContext(parentCall, childCall, allrefs);
                       };

    return sg::dispatch(ExpectedTypes{SG_DEREF(call), std::move(callHandler)}, call->get_parent());
  }

  // computes the expected types from the call context.
  std::set<const SgType*>
  expectedTypes(const SgFunctionCallExp* call)
  {
    auto callHandler = []
                       (const SgFunctionCallExp& parentCall, const SgFunctionCallExp& childCall) -> std::set<const SgType*>
                       {
                         return typesFromResolvedCallContext(parentCall, childCall);
                       };

    return sg::dispatch(ExpectedTypes{SG_DEREF(call), std::move(callHandler)}, call->get_parent());
  }

  template <class SageNode>
  void setFileInfo( SageNode& n,
                    void (SageNode::*setter)(Sg_File_Info*),
                    Sg_File_Info* (SageNode::*getter)() const,
                    const std::string& filename,
                    int line,
                    int col
                  )
  {
    Sg_File_Info* info = (n.*getter)();

    if (info == nullptr)
    {
      info = &mkFileInfo(filename, line, col);
      (n.*setter)(info);
    }

    info->set_parent(&n);

    info->unsetCompilerGenerated();
    info->unsetTransformation();
    info->unsetShared();
    info->set_physical_filename(filename);
    info->set_filenameString(filename);
    info->set_line(line);
    info->set_physical_line(line);
    info->set_col(col);

    info->setOutputInCodeGeneration();
  }

  /// \private
  template <class SageNode>
  void attachSourceLocation_internal(SageNode& n, ada_base_entity* lal_element, AstContext ctx)
  {
    const std::string&      unit = ctx.sourceFileName();
    ada_source_location_range line_numbers;

    //Get the location of the text corresponding to this node
    ada_node_sloc_range(lal_element, &line_numbers);

    setFileInfo( n,
                 &SageNode::set_startOfConstruct, &SageNode::get_startOfConstruct,
                 unit, line_numbers.start.line, line_numbers.start.column );

    setFileInfo( n,
                 &SageNode::set_endOfConstruct,   &SageNode::get_endOfConstruct,
                 unit, line_numbers.end.column,  line_numbers.end.column );
  }
}


/// attaches the source location information from \ref elem to
///   the AST node \ref n.
/// \note to avoid useless instantiations, the template function has two
///       front functions for Sage nodes with location information.
/// \note If an expression has decayed to a located node, the operator position will not be set.
/// \{
void attachSourceLocation(SgLocatedNode& n, ada_base_entity* lal_element, AstContext ctx)
{
  attachSourceLocation_internal(n, lal_element, ctx);
}

void attachSourceLocation(SgExpression& n, ada_base_entity* lal_element, AstContext ctx)
{
  const std::string&      unit = ctx.sourceFileName();
  ada_source_location_range line_numbers;

  //Get the location of the text corresponding to this node
  ada_node_sloc_range(lal_element, &line_numbers);

  setFileInfo( n,
               &SgExpression::set_operatorPosition, &SgExpression::get_operatorPosition,
               unit, line_numbers.start.line, line_numbers.start.column );

  attachSourceLocation(static_cast<SgLocatedNode&>(n), lal_element, ctx);
}

void attachSourceLocation(SgPragma& n, ada_base_entity* lal_element, AstContext ctx)
{
  attachSourceLocation_internal(n, lal_element, ctx);
}
/// \}

namespace{
  bool _hasLocationInfo(SgLocatedNode* n)
  {
    if (!n) return false;

    // this only asks for get_startOfConstruct assuming that
    // get_endOfConstruct is consistent.
    Sg_File_Info* info = n->get_startOfConstruct();

    return info && !info->isCompilerGenerated();
  }

  bool hasLocationInfo(SgNode* n)
  {
    return _hasLocationInfo(isSgLocatedNode(n));
  }

  void cpyFileInfo( SgLocatedNode& n,
                    void (SgLocatedNode::*setter)(Sg_File_Info*),
                    Sg_File_Info* (SgLocatedNode::*getter)() const,
                    const SgLocatedNode& src
                  )
  {
    const Sg_File_Info& info  = SG_DEREF((src.*getter)());

    setFileInfo(n, setter, getter, info.get_filenameString(), info.get_line(), info.get_col());
  }

  void computeSourceRangeFromChildren_internal(std::vector<SgNode*> successors, SgLocatedNode& n)
  {
    auto beg    = successors.begin();
    auto lim    = successors.end();
    auto first  = std::find_if(beg, lim, hasLocationInfo);
    auto rbeg   = successors.rbegin();
    auto rlim   = std::make_reverse_iterator(first);
    auto last   = std::find_if(rbeg, rlim, hasLocationInfo);

    if ((first == lim) || (last == rlim))
    {
      if (SgExpression* ex = isSgExpression(&n))
        markCompilerGenerated(*ex);
      else
        markCompilerGenerated(n);

      return;
    }

    //~ logTrace() << "set srcloc for " << typeid(n).name() << std::endl;

    cpyFileInfo( n,
                 &SgLocatedNode::set_startOfConstruct, &SgLocatedNode::get_startOfConstruct,
                 SG_DEREF(isSgLocatedNode(*first)) );

    cpyFileInfo( n,
                 &SgLocatedNode::set_endOfConstruct,   &SgLocatedNode::get_endOfConstruct,
                 SG_DEREF(isSgLocatedNode(*last)) );

    if (SgExpression* ex = isSgExpression(&n))
    {
      Sg_File_Info* oppos = ex->get_operatorPosition();

      ASSERT_require(oppos);
      (*oppos) = *ex->get_startOfConstruct();
    }
  }

  void computeSourceRangeFromChildren_internal(SgLocatedNode& n)
  {
    computeSourceRangeFromChildren_internal(n.get_traversalSuccessorContainer(), n);
  }

  struct SourceLocationCalc
  {
    void handle(SgNode& n)        { SG_UNEXPECTED_NODE(n); }
    void handle(SgLocatedNode& n) { computeSourceRangeFromChildren_internal(n); }

    void handle(SgFunctionParameterList& n)
    {
      const SgInitializedNamePtrList& args = n.get_args();

      if (args.size())
      {
        SgLocatedNode* first = args.front()->get_declptr();
        SgLocatedNode* last  = args.back()->get_declptr();

        computeSourceRangeFromChildren_internal({first, last}, n);
      }
      else
        computeSourceRangeFromChildren_internal({}, n);
    }
  };
} //end anonymous namespace

void computeSourceRangeFromChildren(SgLocatedNode& n)
{
  sg::dispatch(SourceLocationCalc{}, &n);
}

void computeSourceRangeFromChildren(SgLocatedNode* n)
{
  if (n == nullptr) return;

  computeSourceRangeFromChildren(*n);
}

/// initialize translation settins
void initialize(const Rose::Cmdline::Ada::CmdlineSettings& settings)
{
  // settings.failhardAdb and fail_on_error are obsolete
  if (settings.failhardAdb) fail_on_error = true;
}

struct GenFileInfo : AstSimpleProcessing
{
    void visit(SgNode* sageNode) override
    {
      SgLocatedNode* n = isSgLocatedNode(sageNode);

      if (n == nullptr || !n->isTransformation()) return;

      logError() << n << " " << typeid(*n).name() << "has isTransformation" << n->isTransformation();

      computeSourceRangeFromChildren(*n);

      if (n->isTransformation())
      {
        logError() << n << " " << typeid(*n).name() << "STILL has isTransformation" << n->isTransformation()
                   << "  c=" << n->get_startOfConstruct()
                   << " " << isSgVarRefExp(n)->get_symbol()->get_name()
                   << std::endl;
      }

      //ADA_ASSERT(!n->isTransformation());
    }
};

/// sets the file info to the parents file info if not set otherwise
void genFileInfo(SgSourceFile* file)
{
    logTrace() << "check and generate missing file info" << std::endl;

    GenFileInfo fixer;

    fixer.traverse(file, postorder);
    //~ fixer.traverse(file, preorder);
}

template <class SageParent>
void setChildIfNull( std::size_t& ctr,
                       SageParent& parent,
                       SgExpression* (SageParent::*getter)() const,
                       void (SageParent::*setter)(SgExpression*)
                     )
  {
    if ((parent.*getter)()) return;

    (parent.*setter)(&mkNullExpression());
    ++ctr;
}
namespace{
  void replaceNullptrWithNullExpr()
  {
    std::size_t ctr = 0;

    auto nullrepl = [&ctr](SgExpression* e)->void
                    {
                      if (SgBinaryOp* binop = isSgBinaryOp(e))
                      {
                        setChildIfNull(ctr, *binop, &SgBinaryOp::get_lhs_operand, &SgBinaryOp::set_lhs_operand);
                        setChildIfNull(ctr, *binop, &SgBinaryOp::get_rhs_operand, &SgBinaryOp::set_rhs_operand);
                        return;
                      }

                      if (SgUnaryOp* unop = isSgUnaryOp(e))
                      {
                        setChildIfNull(ctr, *unop, &SgUnaryOp::get_operand, &SgUnaryOp::set_operand);
                        return;
                      }
                    };


    std::for_each( operatorExprs().begin(), operatorExprs().end(),
                   nullrepl
                 );

    logInfo() << "Replaced " << ctr << " nullptr with SgNullExpression." << std::endl;
  }

  /// checks if the scope of the type returned by \ref is the same
  ///   as the scope where \ref ty was declared.
  /// \todo implement full type check and rename to typeCheckCallContext ..
  bool scopeCheckCallContext(SgFunctionCallExp& exp, const SgType& ty)
  {
    SgFunctionRefExp* fnref    = isSgFunctionRefExp(exp.get_function());
    std::string       rosename = SG_DEREF(fnref).get_symbol()->get_name();
    std::string       opname   = si::Ada::convertRoseOperatorNameToAdaOperator(rosename);
    SgScopeStatement* expScope = si::Ada::operatorScope(opname, &ty);
    SgScopeStatement* typScope = si::Ada::operatorScope(opname, si::Ada::typeOfExpr(exp).typerep());
    const bool        res      = si::Ada::sameCanonicalScope(expScope, typScope);

    //~ logTrace() << "liteq: scope " << res
               //~ << "(" << expScope << " | " << typScope << ") " << exp.unparseToString()
               //~ << std::endl;
    return res;
  }

  void decorateWithTypecast(SgFunctionCallExp& exp, const SgType& ty)
  {
    // Another alternative would be to place the operator into a different
    //   scope. This seems to be more appropriate because it would
    //   avoid casts to some composite types, and the code would be unparsed
    //   with the operator being properly scope qualified.

    // if operator syntax is used, it cannot be scope qualified ..
    if (exp.get_uses_operator_syntax()) return;

    // if the operator is not defined in the standard scope, we use it
    //   without further casting ..
    if (SgFunctionSymbol* fnsym = exp.getAssociatedFunctionSymbol())
      if (fnsym->get_scope() != si::Ada::pkgStandardScope())
        return;

    SgNullExpression& dummy = mkNullExpression();

    si::replaceExpression(&exp, &dummy, true /* keep exp */);

    SgType&    castty = SG_DEREF( ty.stripType(SgType::STRIP_MODIFIER_TYPE) );
    SgCastExp& castex = mkCastExp(exp, castty);

    si::replaceExpression(&dummy, &castex, false /* delete dummy */);
  }

  void typecastLiteralEquivalentFunctionsIfNeeded(const OverloadMap& m)
  {
    std::for_each( m.begin(), m.end(),
                   [](const OverloadMap::value_type& el) -> void
                   {
                     if (el.first == nullptr)
                     {
                       //~ logWarn() << "liteq: null" << std::endl;
                       return;
                     }

                     if (!el.second.literalEquivalent())
                     {
                       //~ logTrace() << "liteq: " << el.first->unparseToString()
                                  //~ << " - " << el.second.literalEquivalent()
                                  //~ << std::endl;
                       return;
                     }

                     SgFunctionCallExp* callexp = isSgFunctionCallExp(el.first->get_parent());
                     if (callexp == nullptr)
                     {
                       //~ logTrace() << "liteq: " << typeid(*el.first->get_parent()).name() << std::endl;
                       return;
                     }

                     std::set<const SgType*> typeCandidates = expectedTypes(callexp);

                     if (typeCandidates.size() > 1)
                       ; // logTrace() << "liteq: multiple type candidates" << std::endl;
                     else if (typeCandidates.size() == 0)
                       ; // logTrace() << "liteq: 0 candidates" << std::endl;
                     else if (*typeCandidates.begin() == nullptr)
                       ; // logTrace() << "liteq: null type" << std::endl;
                     else if (!scopeCheckCallContext(*callexp, **typeCandidates.begin()))
                       decorateWithTypecast(*callexp, **typeCandidates.begin());
                   }
                 );
  }


  void resolveInheritedFunctionOverloads(SgGlobal& scope)
  {
    OverloadMap allrefs = collectAllFunctionRefExp(scope);
    WorkItems   workset = createWorksetFrom(allrefs);

    logInfo() << "resolveInheritedFunctionOverloads " << workset.size()
              << std::endl;

    while (!workset.empty())
    {
      OverloadMap::value_type& item      = *workset.next();
      OverloadSet&             overloads = item.second.ovlset();
      const std::size_t        numcands  = overloads.size();

      // nothing to be done ... go to next work item
      if (numcands < 2) continue;

      SgFunctionRefExp&         fnref  = SG_DEREF(item.first);
      const SgFunctionCallExp*  fncall = callNode(fnref);
      const SgExpressionPtrList args   = normalizedArguments(fncall, allrefs);

      if (numcands != 1)
        logInfo() << "resolve: " << fnref.get_parent()->unparseToString() << " " << numcands
                  //~ << "\n   in: " << fnref.get_parent()->get_parent()->unparseToString()
                  //~ << " : " << typeid(*fnref.get_parent()->get_parent()).name()
                  //~ << " - " << args.size()
                  << std::endl;

      {
        // ...
        // disambiguate based on arguments and argument types
        OverloadSet viables;
        auto isViable = [argTypes = argumentTypes(args, allrefs)]
                        (SgFunctionSymbol* fnsy)->bool
                        {
                          const std::vector<SgType*>& parmTypes = parameterTypes(fnsy);

                          return std::equal( argTypes.begin(),  argTypes.end(),
                                             parmTypes.begin(), parmTypes.end(),
                                             ArgParamTypeCompatibility{}
                                           );
                        };

        std::copy_if( overloads.begin(), overloads.end(),
                      std::back_inserter(viables),
                      isViable
                    );

        // put in place candidates
        if (viables.size())
          overloads.swap(viables);
        else
          logFlaw() << "0-viables (a) " << fnref.get_parent()->unparseToString()
                    << std::endl;
      }

      {
        if (overloads.size() != 1)
          logInfo() << "result-resolve: " << fnref.get_parent()->unparseToString() << " " << overloads.size()
                    << std::endl;

        // ...
        // disambiguate based on return types and context
        OverloadSet viables;
        auto isViableReturn = [expTypes = expectedTypes(fncall, allrefs)]
                              (SgFunctionSymbol* fnsy)->bool
                              {
                                if (expTypes.empty()) return true;

                                return ArgParamTypeCompatibility{}(functionReturnType(fnsy), expTypes);
                              };

        std::copy_if( overloads.begin(), overloads.end(),
                      std::back_inserter(viables),
                      isViableReturn
                    );

        // put in place candidates
        if (viables.size())
          overloads.swap(viables);
        else
          logFlaw() << "0-viables (r) " << fnref.get_parent()->unparseToString()
                    << std::endl;
      }

      // was there any progress (i.e., was the overloadset reduced) ?
      if (numcands == overloads.size()) continue;

      // after disambiguation, set the (new) symbol

      if (overloads.size() == 0)
      {
        logWarn() << "empty overload set " << fnref.unparseToString() << std::endl;
        fnref.set_symbol(item.second.orig_sym());
      }
      else
      {
        fnref.set_symbol(overloads.front());
      }

      // put parent and children call nodes in need of disambiguation back into the worklist
      auto appendWorkItem = [&allrefs, &workset]
                            (const SgCallExpression* call) -> void
                            {
                              if (call == nullptr) return;

                              WorkItem pos = allrefs.find(isSgFunctionRefExp(call->get_function()));

                              if (pos != allrefs.end())
                                workset.add(pos);
                            };

      // \todo recognize SgActualArguments
      for (const SgExpression* exp : args)
        appendWorkItem(isSgCallExpression(exp));

      appendWorkItem(parentCallNode(fncall));
    }

    // sanity check
    {
      logTrace() << "checking fun calls.." << std::endl;

      for (const OverloadMap::value_type& item : allrefs)
      {
        if (item.second.ovlset().size() != 1)
        {
          const SgExpression* exp = callNode(SG_DEREF(item.first));

          logFlaw() << "disambig: " << (exp ? exp->unparseToString() : std::string{"<null>"})
                                    << " " << item.second.ovlset().size()
                                    << std::endl;
        }
      }
    }

    typecastLiteralEquivalentFunctionsIfNeeded(allrefs);
  }

  bool hasValidFileInfo(const Sg_File_Info& fi)
  {
    bool basicValidity = (  fi.isCompilerGenerated()
                         || fi.isFrontendSpecific()
                         || fi.isTransformation()
                         || fi.isShared()
                         || fi.isSourcePositionUnavailableInFrontend()
                         );

    //~ logError() << "  " << basicValidity
               //~ << "  " << fi.get_filenameString().empty()
               //~ << "  " << fi.get_physical_file_id()
               //~ << std::endl;

    return ( basicValidity
           || ((!fi.get_filenameString().empty()) && (fi.get_physical_file_id() >= 0))
           );
  }

  bool hasValidFileInfo(const SgExpression* n)
  {
    return (n == nullptr) || hasValidFileInfo(SG_DEREF(n->get_operatorPosition()));
  }
} //end anonymous namespace

void convertLibadalangToROSE(std::vector<ada_base_entity*> roots, SgSourceFile* file, std::vector<std::string> file_paths)
{
  //ADA_ASSERT(file);

  logInfo() << "Building ROSE AST .." << std::endl;

  // the SageBuilder should not mess with source location information
  //   the mode is not well supported in ROSE
  auto defaultSourcePositionClassificationMode = sb::getSourcePositionClassificationMode();

  sb::setSourcePositionClassificationMode(sb::e_sourcePositionFrontendConstruction);

  //Unit_Struct_List_Struct*  adaUnit  = headNodes.Units;
  SgGlobal&                 astScope = SG_DEREF(file->get_globalScope());

  setSymbolTableCaseSensitivity(astScope);

  // sort all units topologically, so that all references can be resolved
  //   by a single translation pass.
  //std::vector<Unit_Struct*> units    = sortUnitsTopologically(adaUnit, AstContext{}.scope(astScope));

  // define the package standard
  //   as we are not able to read it out from Asis
  initializePkgStandard(astScope, roots.at(0));

  // translate all units
  for(long unsigned int i = 0; i < roots.size(); ++i){
    handleUnit(roots.at(i), AstContext{}.scope(astScope), file_paths.at(i));
  }
  // post processing
  replaceNullptrWithNullExpr();
  resolveInheritedFunctionOverloads(astScope);

  // free space that was allocated to store all translation mappings
  clearMappings();

  logInfo() << "Checking AST post-production" << std::endl;
  genFileInfo(file);
  //~ astSanityCheck(file);


  file->set_processedToIncludeCppDirectivesAndComments(false);

  // si::Ada::convertToOperatorRepresentation(&astScope);

  // undo changes to SageBuilder setup
  sb::setSourcePositionClassificationMode(defaultSourcePositionClassificationMode);
  logInfo() << "Building ROSE AST done" << std::endl;
}

bool startsWith(const std::string& s, const std::string& sub)
{
  return (s.rfind(sub, 0) == 0);
}

} //end Libadalang_ROSE_translation namespace

