#include "sage3basic.h"

#include <type_traits>
#include <algorithm>

#include "Rose/Diagnostics.h"
#include "rose_config.h"
#include "sageGeneric.h"
#include "sageBuilder.h"
#include "Ada_to_ROSE_translation.h"
#include "Ada_to_ROSE.h"
#include "AdaMaker.h"
#include "AdaExpression.h"
#include "AdaStatement.h"
#include "AdaType.h"

#include "sageInterfaceAda.h"


// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Ada_ROSE_Translation
{

//
// logger

extern Sawyer::Message::Facility mlog;

static bool fail_on_error = false;


//
// declaration store and retrieval

namespace
{
  /// stores a mapping from Unit_ID to constructed root node in AST
  //~ map_t<int, SgDeclarationStatement*> asisUnitsMap;

  /// stores a mapping from Element_ID to SgInitializedName
  map_t<int, SgInitializedName*> asisVarsMap;

  /// stores a mapping from Element_ID to Exception declaration
  map_t<int, SgInitializedName*> asisExcpsMap;

  /// stores a mapping from Declaration_ID to SgDeclarationStatement
  map_t<int, SgDeclarationStatement*> asisDeclsMap;

  /// stores a mapping from Element_ID to ROSE type declaration
  map_t<int, SgDeclarationStatement*> asisTypesMap;

  /// stores a mapping from string to builtin type nodes
  map_t<AdaIdentifier, SgType*> adaTypesMap;

  /// stores a mapping from string to builtin exception nodes
  map_t<AdaIdentifier, SgInitializedName*> adaExcpsMap;

  /// stores a mapping from string to builtin exception nodes
  map_t<AdaIdentifier, SgAdaPackageSpecDecl*> adaPkgsMap;

  map_t<std::pair<const SgFunctionDeclaration*, const SgTypedefType*>, SgAdaInheritedFunctionSymbol*> inheritedSymbolMap;
} // anonymous namespace

//~ map_t<int, SgDeclarationStatement*>&        asisUnits() { return asisUnitsMap; }
map_t<int, SgInitializedName*>&              asisVars()  { return asisVarsMap;  }
map_t<int, SgInitializedName*>&              asisExcps() { return asisExcpsMap; }
map_t<int, SgDeclarationStatement*>&         asisDecls() { return asisDeclsMap; }
map_t<int, SgDeclarationStatement*>&         asisTypes() { return asisTypesMap; }
map_t<AdaIdentifier, SgType*>&               adaTypes()  { return adaTypesMap;  }
map_t<AdaIdentifier, SgInitializedName*>&    adaExcps()  { return adaExcpsMap;  }
map_t<AdaIdentifier, SgAdaPackageSpecDecl*>& adaPkgs()   { return adaPkgsMap;   }

map_t<std::pair<const SgFunctionDeclaration*, const SgTypedefType*>, SgAdaInheritedFunctionSymbol*>&
inheritedSymbols()
{
  return inheritedSymbolMap;
}

ASIS_element_id_to_ASIS_MapType&     elemMap()   { return asisMap;      }
ASIS_element_id_to_ASIS_MapType&     unitMap()   { return asisMap;      }


//
// auxiliary classes and functions


/// returns true of the kind is of interest
static inline
bool traceKind(const char* /* kind */)
{
  return true;
}

void logKind(const char* kind, bool /* unused */)
{
  if (!traceKind(kind)) return;

  logTrace() << kind << std::endl;
}

LabelAndLoopManager::~LabelAndLoopManager()
{
  for (GotoContainer::value_type el : gotos)
    el.first->set_label(&lookupNode(labels, el.second));
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
}

AstContext::AstContext()
: enum_builder(mkEnumeratorRef) /* all other members are default initialzied */
{}

AstContext
AstContext::scope_npc(SgScopeStatement& s) const
{
  AstContext tmp{*this};

  tmp.the_scope = &s;
  return tmp;
}

AstContext
AstContext::scope(SgScopeStatement& s) const
{
  ADA_ASSERT(s.get_parent());

  return scope_npc(s);
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
AstContext::variantName(Name name) const
{
  AstContext tmp{*this};

  tmp.active_variant_names.push_back(name);
  return tmp;
}

AstContext
AstContext::variantChoice(Element_ID_List choice) const
{
  AstContext tmp{*this};

  tmp.active_variant_choices.push_back(choice);
  return tmp;
}

const std::vector<Name>&
AstContext::variantNames() const
{
  return active_variant_names;
}

const std::vector<Element_ID_List>&
AstContext::variantChoices() const
{
  return active_variant_choices;
}

AstContext
AstContext::enumBuilder(EnumeratorBuilder bld) const
{
  AstContext cpy{*this};

  cpy.enum_builder = std::move(bld);
  return cpy;
}

AstContext::EnumeratorBuilder
AstContext::enumBuilder() const
{
  return enum_builder;
}


void updFileInfo(Sg_File_Info* n, const Sg_File_Info* orig)
{
  ADA_ASSERT(n && orig);

  n->unsetCompilerGenerated();
  n->unsetTransformation();
  n->set_physical_filename(orig->get_physical_filename());
  n->set_filenameString(orig->get_filenameString());
  n->set_line(orig->get_line());
  n->set_col(orig->get_line());

  n->setOutputInCodeGeneration();
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
  if (Sg_File_Info* info = (n.*getter)())
  {
    info->unsetCompilerGenerated();
    info->unsetTransformation();
    info->set_physical_filename(filename);
    info->set_filenameString(filename);
    info->set_line(line);
    info->set_physical_line(line);
    info->set_col(col);

    info->setOutputInCodeGeneration();
    return;
  }

  (n.*setter)(&mkFileInfo(filename, line, col));
}


///


namespace
{
  /// \private
  template <class SageNode>
  void attachSourceLocation_internal(SageNode& n, Element_Struct& elem, AstContext ctx)
  {
    const std::string&      unit = ctx.sourceFileName();
    Source_Location_Struct& loc  = elem.Source_Location;

    setFileInfo( n,
                 &SageNode::set_startOfConstruct, &SageNode::get_startOfConstruct,
                 unit, loc.First_Line, loc.First_Column );

    setFileInfo( n,
                 &SageNode::set_endOfConstruct,   &SageNode::get_endOfConstruct,
                 unit, loc.Last_Line,  loc.Last_Column );
  }
}

/// attaches the source location information from \ref elem to
///   the AST node \ref n.
/// \note to avoid useless instantiations, the template function has two
///       front functions for Sage nodes with location information.
/// \note If an expression has decayed to a located node, the operator position will not be set.
/// \{
void attachSourceLocation(SgLocatedNode& n, Element_Struct& elem, AstContext ctx)
{
  attachSourceLocation_internal(n, elem, ctx);
}

void attachSourceLocation(SgExpression& n, Element_Struct& elem, AstContext ctx)
{
  const std::string&      unit = ctx.sourceFileName();
  Source_Location_Struct& loc  = elem.Source_Location;

  setFileInfo( n,
               &SgExpression::set_operatorPosition, &SgExpression::get_operatorPosition,
               unit, loc.First_Line, loc.First_Column );

  attachSourceLocation(static_cast<SgLocatedNode&>(n), elem, ctx);
}

void attachSourceLocation(SgPragma& n, Element_Struct& elem, AstContext ctx)
{
  attachSourceLocation_internal(n, elem, ctx);
}
/// \}

namespace
{
  /// clears all mappings created during translation
  void clearMappings()
  {
    //~ asisUnits().clear();
    elemMap().clear();
    unitMap().clear();

    asisVars().clear();
    asisExcps().clear();
    asisDecls().clear();
    asisTypes().clear();
    adaTypes().clear();
    adaExcps().clear();
    adaPkgs().clear();
    inheritedSymbols().clear();
  }

  //
  // elements

  /// converts the element \ref elm to a ROSE representation
  /// \todo since most element-kinds are handled in specialized conversion
  ///       functions, consider renaming this function to handleDecl
  ///       and moving it into AdaStatement.C
  void handleElement(Element_Struct& elem, AstContext ctx, bool isPrivate = false)
  {
    switch (elem.Element_Kind)
    {
        case A_Declaration:             // Asis.Declarations
        {
          handleDeclaration(elem, ctx, isPrivate);
          break;
        }

      case A_Clause:                  // Asis.Clauses
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

      case Not_An_Element:  // Nil_Element
      case A_Path:                    // Asis.Statements
      case An_Association:            // Asis.Expressions
      default:
        logWarn() << "Unhandled element " << elem.Element_Kind << std::endl;
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }
  }


  void handleElementID(Element_ID id, AstContext ctx)
  {
    handleElement(retrieveAs(elemMap(), id), ctx);
  }

  //
  // Units

  void handleUnit(Unit_Struct& adaUnit, AstContext ctx);

  struct UnitCreator
  {
      explicit
      UnitCreator(AstContext astctx)
      : ctx(astctx)
      {}
/*
      void operator()(Unit_Struct& adaUnit)
      {
        handleUnit(adaUnit, ctx);
      }

      void operator()(Unit_Struct_List_Struct& adaUnit)
      {
        (*this)(adaUnit.Unit);
      }
*/

      void operator()(Unit_Struct* adaUnit)
      {
        handleUnit(SG_DEREF(adaUnit), ctx);
      }

      AstContext ctx;
  };

  struct PrnUnitHeader
  {
    explicit
    PrnUnitHeader(Unit_Struct& adaUnit)
    : unit(adaUnit)
    {}

    Unit_Struct& unit;
  };

  std::ostream& operator<<(std::ostream& os, PrnUnitHeader obj)
  {
    os << "\n " << obj.unit.ID                  << " (ID)"
       << "\n " << obj.unit.Unit_Full_Name      << " (Unit_Full_Name)"
       << "\n " << obj.unit.Unique_Name         << " (Unique_Name)"
       << "\n " << obj.unit.Text_Name           << " (Text_Name)"
       << "\n " << obj.unit.Object_Name         << " (Object_Name)"
       << "\n " << obj.unit.Exists              << " (Exists)"
       << "\n " << obj.unit.Can_Be_Main_Program << " (Can_Be_Main_Program)"
       << "\n " << obj.unit.Is_Body_Required    << " (Is_Body_Required)"
       << "\n " << obj.unit.Unit_Declaration    << " (Unit_Declaration)"
       ;

    return os;
  }

  void handleUnit(Unit_Struct& adaUnit, AstContext context)
  {
    std::string unitFile{adaUnit.Text_Name};
    AstContext  ctx = context.sourceFileName(unitFile);

    // dispatch based on unit kind
    switch (adaUnit.Unit_Kind)
    {
      case A_Generic_Procedure:
      case A_Generic_Function:
      case A_Function:
      case A_Procedure:
        {
          std::string kindName;

               if (adaUnit.Unit_Kind == A_Function)          kindName = "function";
          else if (adaUnit.Unit_Kind == A_Procedure)         kindName = "procedure";
          else if (adaUnit.Unit_Kind == A_Generic_Function)  kindName = "generic function";
          else if (adaUnit.Unit_Kind == A_Generic_Procedure) kindName = "generic procedure";

          logTrace() << "A " << kindName
                     << PrnUnitHeader(adaUnit)
                     << "\n " << adaUnit.Corresponding_Parent_Declaration << " (Corresponding_Parent_Declaration)"
                     << "\n " << adaUnit.Corresponding_Body << " (Corresponding_Body)"
                     << std::endl;

          ElemIdRange range = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(range, elemMap(), ElemCreator{ctx});
          handleElementID(adaUnit.Unit_Declaration, ctx);

          /* unused optional elems:
               Unit_ID             Corresponding_Parent_Declaration
               Unit_ID             Corresponding_Declaration;
               Unit_ID             Corresponding_Body;
               Unit_List           Subunits;
          */

          break;
        }

      case A_Function_Body:
      case A_Procedure_Body:
        {
          logTrace() << "A " << (adaUnit.Unit_Kind == A_Function_Body ? "function" : "procedure") << " body"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;
          ElemIdRange range = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(range, elemMap(), ElemCreator{ctx});
          handleElementID(adaUnit.Unit_Declaration, ctx);

          /* unused optional elems:
               Unit_ID             Corresponding_Parent_Declaration;
               Unit_ID             Corresponding_Declaration;
               Unit_List           Subunits;
          */


          break;
        }

      case A_Generic_Package:
      case A_Package:
        {
          logTrace() << "A package"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;

          ElemIdRange elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange unitRange = idRange(adaUnit.Corresponding_Children);

          if (elemRange.size() || unitRange.size())
          {
            (unitRange.size() ? logWarn() : logInfo())
               << "   elems# " << elemRange.size()
               << "\n    subs# " << unitRange.size()
               << std::endl;

            traverseIDs(elemRange, elemMap(), ElemCreator{ctx});

            // units seem to be included in the elements
            // \todo double check this
            //~ traverseIDs(unitRange, unitMap(), UnitCreator(ctx));
          }

          handleElementID(adaUnit.Unit_Declaration, ctx);

          /* unused fields:
          */
          break;
        }

      case A_Package_Body:
        {
          logTrace() << "A package body"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;

          ElemIdRange elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange unitRange = idRange(adaUnit.Corresponding_Children);

          if (elemRange.size() || unitRange.size())
          {
            (unitRange.size() ? logWarn() : logInfo())
                 << "   elems# " << elemRange.size()
                 << "\n    subs# " << unitRange.size()
                 << std::endl;

            traverseIDs(elemRange, elemMap(), ElemCreator{ctx});
            // units seem to be included in the elements
            // \todo double check this
            //~ traverseIDs(unitRange, unitMap(), UnitCreator(ctx));
          }

          handleElementID(adaUnit.Unit_Declaration, ctx);
          break;
        }

      case A_Package_Renaming:
        {
          logTrace() << "A package renaming"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;

          ElemIdRange range = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(range, elemMap(), ElemCreator{ctx});
          handleElementID(adaUnit.Unit_Declaration, ctx);
          break;
        }

      case Not_A_Unit:
      case A_Package_Instance:

      case A_Procedure_Instance:
      case A_Function_Instance:

      case A_Procedure_Renaming:
      case A_Function_Renaming:

      case A_Generic_Procedure_Renaming:
      case A_Generic_Function_Renaming:
      case A_Generic_Package_Renaming:

      //  A unit interpreted only as the completion of a function: or a unit
      //  interpreted as both the declaration and body of a library
      //  function. Reference Manual 10.1.4(4)

      case A_Procedure_Body_Subunit:
      case A_Function_Body_Subunit:
      case A_Package_Body_Subunit:
      case A_Task_Body_Subunit:
      case A_Protected_Body_Subunit:

      case A_Nonexistent_Declaration:
      //  A unit that does not exist but is:
      //    1) mentioned in a with clause of another unit or:
      //    2) a required corresponding library_unit_declaration
      case A_Nonexistent_Body:
      //  A unit that does not exist but is:
      //     1) known to be a corresponding subunit or:
      //     2) a required corresponding library_unit_body
      case A_Configuration_Compilation:
      //  Corresponds to the whole content of a compilation with no
      //  compilation_unit, but possibly containing comments, configuration
      //  pragmas, or both. Any Context can have at most one unit of
      //  case A_Configuration_Compilation kind. A unit of
      //  case A_Configuration_Compilation does not have a name. This unit
      //  represents configuration pragmas that are "in effect".
      //
      //  GNAT-specific note: In case of GNAT the requirement to have at most
      //  one unit of case A_Configuration_Compilation kind does not make sense: in
      //  GNAT compilation model configuration pragmas are contained in
      //  configuration files, and a compilation may use an arbitrary number
      //  of configuration files. That is, (Elements representing) different
      //  configuration pragmas may have different enclosing compilation units
      //  with different text names. So in the ASIS implementation for GNAT a
      //  Context may contain any number of units of
      //  case A_Configuration_Compilation kind
      case An_Unknown_Unit:

      default:
        logWarn() << "unit kind unhandled: " << adaUnit.Unit_Kind << std::endl;
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }
  }

  std::string
  astDotFileName(const SgSourceFile& file)
  {
    std::string            res = file.generateOutputFileName();
    std::string::size_type pos = res.find('.');

    if (pos != std::string::npos)
      res = res.substr(0, pos);

    res += "_rose";
    return res;
  }

  struct UnitEntry
  {
    Unit_Struct*               unit;
    std::vector<AdaIdentifier> dependencies;
    bool                       marked;
  };

  struct UniqueUnitId
  {
    bool          isbody;
    AdaIdentifier name;
  };

  bool startsWith(const std::string& s, const std::string& sub)
  {
    return (s.rfind(sub, 0) == 0);
  }

  // sort specifications before bodies
  bool operator<(const UniqueUnitId& lhs, const UniqueUnitId& rhs)
  {
    // System inclusion is implied, even if it is not referenced.
    //   Ordering it first is semantically consistent.
    if ((startsWith(lhs.name, "SYSTEM")) && (!startsWith(rhs.name, "SYSTEM")))
      return true;

    if ((!startsWith(lhs.name, "SYSTEM")) && (startsWith(rhs.name, "SYSTEM")))
      return false;

    if ((lhs.isbody == false) && (rhs.isbody == true))
      return true;

    if (lhs.isbody != rhs.isbody)
      return false;

    return lhs.name < rhs.name;
  }

  struct DependencyExtractor
  {
    explicit
    DependencyExtractor(std::vector<AdaIdentifier>& vec, AstContext ctx)
    : deps(vec), astctx(ctx)
    {}

    void operator()(Element_Struct& elem)
    {
      ADA_ASSERT(elem.Element_Kind == A_Clause);

      Clause_Struct& clause = elem.The_Union.Clause;

      if (clause.Clause_Kind != A_With_Clause)
        return;

      std::vector<AdaIdentifier>& res = deps;
      AstContext                  ctx{astctx};

      traverseIDs( idRange(clause.Clause_Names), elemMap(),
                   [&res, &ctx](Element_Struct& el) -> void
                   {
                     ADA_ASSERT (el.Element_Kind == An_Expression);
                     NameData imported = getName(el, ctx);

                     res.emplace_back(imported.fullName);
                   }
                 );
    }

    std::vector<AdaIdentifier>& deps;
    AstContext                  astctx;
  };


  void addWithClausDependencies(Unit_Struct& unit, std::vector<AdaIdentifier>& res, AstContext ctx)
  {
    ElemIdRange range = idRange(unit.Context_Clause_Elements);

    traverseIDs(range, elemMap(), DependencyExtractor{res, ctx});
  }


  void dfs( std::map<UniqueUnitId, UnitEntry>& m,
            std::map<UniqueUnitId, UnitEntry>::value_type& el,
            std::vector<Unit_Struct*>& res
          )
  {
    if (el.second.marked) return;

    el.second.marked = true;

    for (const AdaIdentifier& depname : el.second.dependencies)
    {
      auto pos = m.find(UniqueUnitId{false, depname});

      // functional and procedural units may have bodies
      if (pos == m.end())
        pos = m.find(UniqueUnitId{true, depname});

      if (pos == m.end())
      {
        logError() << "unknown unit: " << depname << std::endl;
        continue;
      }

      dfs(m, *pos, res);
    }

    res.push_back(el.second.unit);
  }

  UniqueUnitId uniqueUnitName(Unit_Struct& unit)
  {
    const bool  isBody = (  (unit.Unit_Kind == A_Package_Body)
                         || (unit.Unit_Kind == A_Function_Body)
                         || (unit.Unit_Kind == A_Procedure_Body)
                         );

    return UniqueUnitId{isBody, AdaIdentifier{unit.Unit_Full_Name}};
  }

  std::vector<Unit_Struct*>
  sortUnitsTopologically(Unit_Struct_List_Struct* adaUnit, AstContext ctx)
  {
    using DependencyMap = std::map<UniqueUnitId, UnitEntry>;
    using IdEntryMap    = std::map<Unit_ID, DependencyMap::iterator> ;

    DependencyMap deps;
    IdEntryMap    idmap;

    // build maps for all units
    for (Unit_Struct_List_Struct* unit = adaUnit; unit != nullptr; unit = unit->Next)
    {
      ADA_ASSERT(unit);

      UniqueUnitId uniqueId = uniqueUnitName(unit->Unit);

      auto depres = deps.emplace( uniqueId,
                                  UnitEntry{&(unit->Unit), std::vector<AdaIdentifier>{}, false}
                                );
      ADA_ASSERT(depres.second);

      // map specifications to their name
      auto idmres = idmap.emplace(unit->Unit.ID, depres.first);
      ADA_ASSERT(idmres.second);
    }

    // link the units
    for (Unit_Struct_List_Struct* unit = adaUnit; unit != nullptr; unit = unit->Next)
    {
      IdEntryMap::iterator    uit = idmap.find(unit->Unit.ID);
      ADA_ASSERT(uit != idmap.end());

      DependencyMap::iterator pos = uit->second;
      const size_t            parentID = unit->Unit.Corresponding_Parent_Declaration;
      IdEntryMap::iterator    idpos    = idmap.find(parentID);

      if (idpos != idmap.end())
      {
        pos->second.dependencies.emplace_back(idpos->second->first.name);
      }
      else if (parentID > 0)
      {
        // parentID == 1.. 1 refers to the package standard (currently not extracted from Asis)
        logWarn() << "unknown unit dependency: "
                  << pos->first.name << ' '
                  << (pos->first.isbody ? "[body]" : "[spec]")
                  << "#" << pos->second.unit->ID
                  << " -> #" << parentID
                  << std::endl;
      }

      addWithClausDependencies(unit->Unit, pos->second.dependencies, ctx);
    }

    std::vector<Unit_Struct*> res;

    // topo sort
    for (DependencyMap::value_type& el : deps)
      dfs(deps, el, res);

  /*
    // print all module dependencies
    for (const DependencyMap::value_type& el : deps)
    {
      logWarn() << el.first << "\n  ";

      for (const std::string& n : el.second.dependencies)
        logWarn() << n << ", ";

      logWarn() << std::endl << std::endl;
    }
  */

    logTrace() << "\nTopologically sorted module processing order"
               << std::endl;

    for (const Unit_Struct* uptr : res)
      logTrace() << uptr->Unit_Full_Name
                 << "(" << uptr->ID << "), ";

    logTrace() << std::endl;
    return res;
  }

  struct InheritFileInfo : AstSimpleProcessing
  {
    void visit(SgNode* sageNode) override
    {
      SgLocatedNode* n = isSgLocatedNode(sageNode);

      if (n == nullptr || !n->isTransformation()) return;

      SgLocatedNode* parentNode = isSgLocatedNode(n->get_parent());
      ADA_ASSERT(parentNode && !parentNode->isTransformation());

      updFileInfo(n->get_file_info(),        parentNode->get_file_info());
      updFileInfo(n->get_startOfConstruct(), parentNode->get_startOfConstruct());
      updFileInfo(n->get_endOfConstruct(),   parentNode->get_endOfConstruct());

      ADA_ASSERT(!n->isTransformation());
    }
  };

  /// Implements a quick check that the AST is properly constructed.
  ///   While some issues, such as parent pointers will be fixed at the
  ///   post processing stage, it may be good to point inconsistencies
  ///   out anyway.
  void inheritFileInfo(SgSourceFile* file)
  {
    InheritFileInfo fixer;

    fixer.traverse(file, preorder);
  }


  struct AstSanityCheck : AstSimpleProcessing
  {
    void checkDecl(SgDeclarationStatement* dcl)
    {
      if (!dcl) return;

      std::string s = si::get_name(dcl);
    }

    void checkScope(SgScopeStatement* scope)
    {
      if (!scope) return;

      std::string s = si::get_name(scope);
    }

    void visit(SgNode* sageNode) override
    {
      SgLocatedNode* n = isSgLocatedNode(sageNode);

      if (n == nullptr) return;

      const bool hasParent    = (n->get_parent() != nullptr);
      const bool hasFileInfo  = (n->get_file_info() != nullptr);
      const bool hasStartInfo = (n->get_startOfConstruct() != nullptr);
      const bool hasEndInfo   = (n->get_endOfConstruct() != nullptr);
      const bool hasNoTransf  = (!n->isTransformation());
      const bool hasNoCompgen = (!n->isCompilerGenerated());
      const bool printOutput  = (!hasFileInfo || !hasStartInfo || !hasEndInfo || !hasNoTransf || !hasNoCompgen);

      if (!hasParent)
        logWarn() << typeid(*n).name() << ": get_parent is NULL" << std::endl;

      if (!hasFileInfo)
        logWarn() << typeid(*n).name() << ": get_file_info is NULL" << std::endl;

      if (!hasStartInfo)
        logWarn() << typeid(*n).name() << ": get_startOfConstruct is NULL" << std::endl;

      if (!hasEndInfo)
        logWarn() << typeid(*n).name() << ": get_endOfConstruct is NULL" << std::endl;

      if (!hasNoTransf)
        logWarn() << typeid(*n).name() << ": isTransformation is set" << std::endl;

      if (!hasNoCompgen)
        logWarn() << typeid(*n).name() << ": isCompilerGenerated is set" << std::endl;

      if (hasParent && printOutput)
        logWarn() << "        parent is " << typeid(*n->get_parent()).name()
                  << std::endl;

      if (!hasParent || printOutput)
        logWarn() << "        unparsed: " << n->unparseToString()
                  << std::endl;

      //~ checkType(isSgExpression(n));
      //~ checkType(isSgInitializedName(n));
      //~ checkExpr(isSgAdaAttributeExp(n));
      checkDecl(isSgDeclarationStatement(n));
      checkScope(isSgScopeStatement(n));
    }
  };


  /// Implements a quick check that the AST is properly constructed.
  ///   While some issues, such as parent pointers will be fixed at the
  ///   post processing stage, it may be good to point inconsistencies
  ///   out anyway.
  void astSanityCheck(SgSourceFile* file)
  {
    AstSanityCheck checker;

    checker.traverse(file, preorder);
  }
} // anonymous


void ElemCreator::operator()(Element_Struct& elem)
{
  handleElement(elem, ctx, privateElems);
}

void convertAsisToROSE(Nodes_Struct& headNodes, SgSourceFile* file)
{
  ADA_ASSERT(file);

  logInfo() << "Building ROSE AST .." << std::endl;

  Unit_Struct_List_Struct*  adaUnit  = headNodes.Units;
  SgGlobal&                 astScope = SG_DEREF(file->get_globalScope());

  setSymbolTableCaseSensitivity(astScope);

  std::vector<Unit_Struct*> units    = sortUnitsTopologically(adaUnit, AstContext{}.scope(astScope));

  initializePkgStandard(astScope);
  std::for_each(units.begin(), units.end(), UnitCreator{AstContext{}.scope(astScope)});
  clearMappings();

  std::string astDotFile = astDotFileName(*file);
  logTrace() << "Generating DOT file for ROSE AST: " << astDotFile << std::endl;
  generateDOT(&astScope, astDotFile);

  logInfo() << "Checking AST post-production" << std::endl;
  inheritFileInfo(file);
  astSanityCheck(file);

  file->set_processedToIncludeCppDirectivesAndComments(false);
  logInfo() << "Building ROSE AST done" << std::endl;
}

bool FAIL_ON_ERROR(AstContext ctx)
{
  static const char* failSuffix = ".adb";

  if (!fail_on_error)
    return fail_on_error;

  const std::string& filename = ctx.sourceFileName();

  return (  filename.size() > 3
         && std::equal(filename.end()-4, filename.end(), failSuffix)
         );
}

/// initialize translation settins
void initialize(const Rose::Cmdline::Ada::CmdlineSettings& settings)
{
  if (settings.failhardAdb) fail_on_error = true;
}


}
