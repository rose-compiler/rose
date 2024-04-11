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
//#include "AdaExpression.h"
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

//Function to turn an ada_text_type into a string
std::string dot_ada_text_type_to_string(ada_text_type input_text){
    ada_text value_text;
    value_text.length = input_text->n;
    value_text.chars = input_text->items;
    std::string return_string = ada_text_to_locale_string(&value_text);
    ada_destroy_text(&value_text);
    return return_string;
}

//Function to turn an ada_unbounded_text_type_array into a string
std::string dot_ada_unbounded_text_type_to_string(ada_unbounded_text_type_array input_text){
    ada_text value_text;
    std::string return_string = "";
    for(int i = 0; i < input_text->n; i++){
        ada_symbol_type current_symbol = input_text->items[i];
        ada_symbol_text(&current_symbol, &value_text);
        return_string += ada_text_to_locale_string(&value_text);
        ada_destroy_text(&value_text);
    }
    return return_string;
}

//Function to get the source location of an ada node as a string
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

//Function to hash a unique int from a node using the node's kind and location.
//The kind and location can be provided, but if not they will be determined in the function
int hash_node(ada_base_entity *node, int kind = -1, std::string full_sloc = ""){
    //Get the kind/sloc if they weren't provided
    if(kind == -1){
        kind = ada_node_kind(node);
    }
    if(full_sloc == ""){
        full_sloc = dot_ada_full_sloc(node);
    }

    std::string word_to_hash = full_sloc + std::to_string(kind);

    //Generate the hash
    int seed = 131; 
    unsigned int hash = 0;
    for(int i = 0; i < word_to_hash.length(); i++){
        hash = (hash * seed) + word_to_hash[i];
    }
    return hash;
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

void logKind(const char* kind, int elemID)
{
  if (!traceKind(kind)) return;

  logTrace() << kind;
  if (elemID > 0) logTrace() << ' ' << elemID;
  logTrace() << std::endl;
}

/*LabelAndLoopManager::~LabelAndLoopManager()
{
  for (GotoContainer::value_type el : gotos)
  {
    //~ el.first->set_label(&lookupNode(labels, el.second));
    ADA_ASSERT(el.first->get_label_expression() == nullptr);
    el.first->set_label_expression(&mkLabelRefExp(lookupNode(labels, el.second)));
  }
}

void LabelAndLoopManager::label(Element_ID id, SgLabelStatement& lblstmt)
{
  SgLabelStatement*& mapped = labels[id];

  ADA_ASSERT(mapped == nullptr);
  mapped = &lblstmt;
}

void LabelAndLoopManager::gotojmp(Element_ID id, SgGotoStatement& gotostmt)
{
  gotos.emplace_back(&gotostmt, id);
} */

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

/*AstContext
AstContext::labelsAndLoops(LabelAndLoopManager& lm) const
{
  AstContext tmp{*this};

  tmp.all_labels_loops = &lm;
  return tmp;
}*/

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

// static
void
AstContext::defaultStatementHandler(AstContext ctx, SgStatement& s)
{
  SgScopeStatement& scope = ctx.scope();

  scope.append_statement(&s);
  //ADA_ASSERT(s.get_parent() == &scope);
}

///

void handleElement(ada_base_entity* lal_element, AstContext ctx, bool isPrivate)
  {
    //Get the kind of this node
    ada_node_kind_enum element_kind = ada_node_kind(lal_element);

    ada_text kind_name;
    ada_kind_name(element_kind, &kind_name);
    std::string kind_name_string = ada_text_to_locale_string(&kind_name);
    logTrace()   << "handleElement called on a " << kind_name_string << std::endl;

    switch (element_kind)
    {
        case ada_subp_body:             // Asis.Declarations
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
        }

      case A_Definition:              // Asis.Definitions
        {
          handleDefinition(elem, ctx);
          break;
        }

      case A_Pragma:                  // Asis.Elements
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

void handleUnit(ada_base_entity* lal_unit, AstContext context)
  {

    //First, make sure this is a unit node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_unit);

    if(kind != ada_compilation_unit){
        logError() << "handleUnit provided incorrect node kind!\n";
    }


    bool        processUnit   = true;
    bool        logParentUnit = false;
    bool        logBodyUnit   = false;

    ada_text kind_name;
    ada_kind_name(kind, &kind_name);
    std::string kind_name_string = ada_text_to_locale_string(&kind_name);

    if (processUnit)
    {
      
      ada_unbounded_text_type_array p_syntactic_fully_qualified_name;
      int result = ada_compilation_unit_p_syntactic_fully_qualified_name(lal_unit, &p_syntactic_fully_qualified_name);

      std::string                             unitFile = dot_ada_unbounded_text_type_to_string(p_syntactic_fully_qualified_name);
      AstContext::PragmaContainer             pragmalist;
      AstContext::DeferredCompletionContainer compls;
      AstContext                              ctx = context.sourceFileName(unitFile)
                                                           .pragmas(pragmalist)
                                                           .deferredUnitCompletionContainer(compls);

      logTrace()   << "handleUnit called on a " << kind_name_string << std::endl;
                   //<< PrnUnitHeader(adaUnit);
      /*if (logParentUnit)
        logTrace() << "\n " << adaUnit.Corresponding_Parent_Declaration << " (Corresponding_Parent_Declaration)";

      if (logBodyUnit)
        logTrace() << "\n " << adaUnit.Corresponding_Body << " (Corresponding_Body)";

      logTrace()   << std::endl;*/ //TODO Add these features

      //ElemIdRange range = idRange(adaUnit.Context_Clause_Elements); //TODO ???
      bool        privateDecl = false;

      //Get the body node
      ada_base_entity unit_body;
      ada_compilation_unit_f_body(lal_unit, &unit_body);

      //This body node can be either ada_library_item or ada_subunit
      kind = ada_node_kind(&unit_body);

      ada_base_entity unit_declaration;
      //Fetch the unit declaration based on which kind we got
      if(kind == ada_library_item){
          ada_library_item_f_item(&unit_body, &unit_declaration);
          //Update the privateDecl field
          ada_base_entity ada_private_node;
          ada_bool p_as_bool;
          ada_library_item_f_has_private(&unit_body, &ada_private_node);
          ada_with_private_p_as_bool(&ada_private_node, &p_as_bool);
          privateDecl=(p_as_bool != 0);
      } else {
          ada_subunit_f_body(&unit_body, &unit_declaration);
      }
      
      //traverseIDs(range, elemMap(), ElemCreator{ctx}); handle the pragmas/prelude/with
      handleElement(&unit_declaration, ctx, privateDecl);

      //processAndPlacePragmas(adaUnit.Compilation_Pragmas, { &ctx.scope() }, ctx);

      for (AstContext::DeferredCompletion& c : compls) c();
    }
  }


namespace{
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
}

/// initialize translation settins
void initialize(const Rose::Cmdline::Ada::CmdlineSettings& settings)
{
  // settings.failhardAdb and fail_on_error are obsolete
  if (settings.failhardAdb) fail_on_error = true;
}

void computeSourceRangeFromChildren(SgLocatedNode& n)
{
  std::vector<SgNode*> successors = n.get_traversalSuccessorContainer();
  auto beg    = successors.begin();
  auto lim    = successors.end();
  auto first  = std::find_if(beg, lim, hasLocationInfo);
  auto rbeg   = successors.rbegin();
  auto rlim   = std::make_reverse_iterator(first);
  auto last   = std::find_if(rbeg, rlim, hasLocationInfo);

  if ((first == lim) || (last == rlim))
    return;

  cpyFileInfo( n,
               &SgLocatedNode::set_startOfConstruct, &SgLocatedNode::get_startOfConstruct,
               SG_DEREF(isSgLocatedNode(*first)) );

  cpyFileInfo( n,
               &SgLocatedNode::set_endOfConstruct,   &SgLocatedNode::get_endOfConstruct,
               SG_DEREF(isSgLocatedNode(*last)) );
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


    /*std::for_each( operatorExprs().begin(), operatorExprs().end(),
                   nullrepl
                 );*/

    logInfo() << "Replaced " << ctr << " nullptr with SgNullExpression." << std::endl;
}

void convertLibadalangToROSE(ada_base_entity* root, SgSourceFile* file)
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
  initializePkgStandard(astScope);

  // translate all units
  //std::for_each(units.begin(), units.end(), UnitCreator{AstContext{}.scope(astScope)});
  
  //This function just calls handleUnit
  //translate_libadalang(AstContext{}.scope(astScope), &root);
  handleUnit(root, AstContext{}.scope(astScope));

  // post processing
  replaceNullptrWithNullExpr();
  //resolveInheritedFunctionOverloads(astScope); //TODO enable

  // free space that was allocated to store all translation mappings
  //clearMappings();

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

