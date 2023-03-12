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
  constexpr bool PRINT_UNIT_DEPENDENCIES = false;

  using FunctionVector = std::vector<SgFunctionDeclaration*>;

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

  map_t<int, SgBasicBlock*> asisBlocksMap;

  /// stores a mapping from string to builtin type nodes
  map_t<AdaIdentifier, SgType*> adaTypesMap;

  /// stores a mapping from string to builtin exception nodes
  map_t<AdaIdentifier, SgInitializedName*> adaExcpsMap;

  /// stores a mapping from string to builtin exception nodes
  map_t<AdaIdentifier, SgAdaPackageSpecDecl*> adaPkgsMap;

  /// stores a mapping from string to builtin function declaration nodes
  //~ map_t<AdaIdentifier, FunctionVector> adaFuncsMap;

  /// stores variables defined in Standard or Ascii
  map_t<AdaIdentifier, SgInitializedName*> adaVarsMap;

  /// map of inherited symbols
  std::map<InheritedSymbolKey, SgAdaInheritedFunctionSymbol*> inheritedSymbolMap;

  /// maps generated operators
  map_t<OperatorKey, std::vector<OperatorDesc> > operatorSupportMap;
} // anonymous namespace

//~ map_t<int, SgDeclarationStatement*>&        asisUnits() { return asisUnitsMap; }
map_t<int, SgInitializedName*>&                           asisVars()         { return asisVarsMap;        }
map_t<int, SgInitializedName*>&                           asisExcps()        { return asisExcpsMap;       }
map_t<int, SgDeclarationStatement*>&                      asisDecls()        { return asisDeclsMap;       }
map_t<int, SgDeclarationStatement*>&                      asisTypes()        { return asisTypesMap;       }
map_t<int, SgBasicBlock*>&                                asisBlocks()       { return asisBlocksMap;      }
map_t<AdaIdentifier, SgType*>&                            adaTypes()         { return adaTypesMap;        }
map_t<AdaIdentifier, SgInitializedName*>&                 adaExcps()         { return adaExcpsMap;        }
map_t<AdaIdentifier, SgAdaPackageSpecDecl*>&              adaPkgs()          { return adaPkgsMap;         }
map_t<AdaIdentifier, SgInitializedName*>&                 adaVars()          { return adaVarsMap;         }
//~ map_t<AdaIdentifier, FunctionVector>&                     adaFuncs()         { return adaFuncsMap;        }
std::map<InheritedSymbolKey, SgAdaInheritedFunctionSymbol*>& inheritedSymbols() { return inheritedSymbolMap; }
map_t<OperatorKey, std::vector<OperatorDesc> >&           operatorSupport()  { return operatorSupportMap; }

//
// auxiliary classes and functions


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

AstContext
AstContext::scope_npc(SgScopeStatement& s) const
{
  // make sure that the installed handler handles SgScopeStatement
  // ADA_ASSERT(stmtHandler.target() == &defaultStatementHandler);

  AstContext tmp{*this};

  tmp.the_scope = &s;
  return tmp;
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

// static
void
AstContext::defaultStatementHandler(AstContext ctx, SgStatement& s)
{
  SgScopeStatement& scope = ctx.scope();

  scope.append_statement(&s);
  ADA_ASSERT(s.get_parent() == &scope);
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
    elemMap_update().clear();

    asisVars().clear();
    asisExcps().clear();
    asisDecls().clear();
    asisTypes().clear();
    asisBlocks().clear();
    adaTypes().clear();
    adaExcps().clear();
    adaPkgs().clear();
    adaVars().clear();
    //~ adaFuncs().clear();

    inheritedSymbols().clear();
    operatorSupport().clear();
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


  void handleElementID(Element_ID id, AstContext ctx, bool isPrivate = false)
  {
    handleElement(retrieveAs(elemMap(), id), ctx, isPrivate);
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
    bool        processUnit   = true;
    bool        logParentUnit = false;
    bool        logBodyUnit   = false;
    std::string kindName;

  //  A_Procedure,
  //  A_Function,
  //  A_Package,
  //  A_Generic_Procedure,
  //  A_Generic_Function,
  //  A_Generic_Package,
  //  A_Procedure_Instance,
  //  A_Function_Instance,
  //  A_Package_Instance,
  //  A_Procedure_Renaming,
  //  A_Function_Renaming,
  //  A_Package_Renaming,
  //  A_Generic_Procedure_Renaming,
  //  A_Generic_Function_Renaming,
  //  A_Generic_Package_Renaming,
  //  A_Procedure_Body,
  //  A_Function_Body,
  //  A_Package_Body,

  //  A_Procedure,
  //  A_Function,
  //  A_Package,
  //  A_Generic_Procedure,
  //  A_Generic_Function,
  //  A_Generic_Package,

    // dispatch based on unit kind
    switch (adaUnit.Unit_Kind)
    {
      case A_Generic_Procedure:
        kindName = "A_Generic_Procedure"; logParentUnit = true; logBodyUnit = true;
        break;

      case A_Generic_Function:
        kindName = "A_Generic_Function"; logParentUnit = true; logBodyUnit = true;
        break;

      case A_Function:
        kindName = "A_Function"; logParentUnit = true; logBodyUnit = true;
        break;

      case A_Procedure:
        kindName = "A_Procedure"; logParentUnit = true; logBodyUnit = true;
        break;

      case A_Generic_Package:
        kindName = "A_Generic_Package"; logParentUnit = true; logBodyUnit = true;
        break;

      case A_Package:
        kindName = "A_Package"; logParentUnit = true; logBodyUnit = true;
        break;

      case A_Procedure_Body_Subunit:     kindName = "A_Procedure_Body_Subunit"; break;
      case A_Function_Body_Subunit:      kindName = "A_Function_Body_Subunit"; break;
      case A_Package_Body_Subunit:       kindName = "A_Package_Body_Subunit"; break;
      case A_Function_Body:              kindName = "A_Function_Body"; break;
      case A_Procedure_Body:             kindName = "A_Procedure_Body"; break;
      case A_Package_Body:               kindName = "A_Package_Body"; break;
      case A_Procedure_Instance:         kindName = "A_Procedure_Instance"; logParentUnit = true; break;
      case A_Function_Instance:          kindName = "A_Function_Instance"; logParentUnit = true; break;
      case A_Package_Instance:           kindName = "A_Package_Instance"; logParentUnit = true; break;
      case A_Task_Body_Subunit:          kindName = "A_Task_Body_Subunit"; break;
      case A_Protected_Body_Subunit:     kindName = "A_Protected_Body_Subunit"; break;
      case A_Procedure_Renaming:         kindName = "A_Procedure_Renaming"; logParentUnit = true; break;
      case A_Generic_Package_Renaming:   kindName = "A_Generic_Package_Renaming"; logParentUnit = true; break;
      case A_Package_Renaming:           kindName = "A_Package_Renaming"; logParentUnit = true; break;
      case A_Function_Renaming:          kindName = "A_Function_Renaming"; logParentUnit = true; break;
      case A_Generic_Procedure_Renaming: kindName = "A_Generic_Procedure_Renaming"; logParentUnit = true; break;
      case A_Generic_Function_Renaming:  kindName = "A_Generic_Function_Renaming"; logParentUnit = true; break;


      case Not_A_Unit:

      //  A unit interpreted only as the completion of a function: or a unit
      //  interpreted as both the declaration and body of a library
      //  function. Reference Manual 10.1.4(4)

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
        processUnit = false;
        logWarn() << "unit kind unhandled: " << adaUnit.Unit_Kind << std::endl;
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    if (processUnit)
    {
      logTrace()   << "A " << kindName
                   << PrnUnitHeader(adaUnit);
      if (logParentUnit)
        logTrace() << "\n " << adaUnit.Corresponding_Parent_Declaration << " (Corresponding_Parent_Declaration)";

      if (logBodyUnit)
        logTrace() << "\n " << adaUnit.Corresponding_Body << " (Corresponding_Body)";

      logTrace()   << std::endl;

      ElemIdRange range = idRange(adaUnit.Context_Clause_Elements);
      bool        privateDecl = (  (adaUnit.Unit_Class == A_Private_Declaration)
                                || (adaUnit.Unit_Class == A_Private_Body)
                                );

      traverseIDs(range, elemMap(), ElemCreator{ctx});
      handleElementID(adaUnit.Unit_Declaration, ctx, privateDecl);
    }
  }

/*
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
*/

  struct UnitEntry
  {
    Unit_Struct*         unit;
    //~ std::vector<AdaIdentifier> dependencies;
    std::vector<Unit_ID> dependencies;
    bool                 marked;
  };

  struct UniqueUnitId
  {
    bool          isbody;
    AdaIdentifier name;
  };

  inline
  std::ostream& operator<<(std::ostream& os, const UniqueUnitId& id)
  {
    return os << id.name << (id.isbody ? " (body)" : "");
  }

  bool startsWith(const std::string& s, const std::string& sub)
  {
    return (s.rfind(sub, 0) == 0);
  }

  bool isSystemPackage(Unit_Struct* unit)
  {
    ADA_ASSERT(unit);
    AdaIdentifier name(unit->Unit_Full_Name);

    return startsWith(name, "SYSTEM");
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

  using UnitNameUnitIDMap = std::map<UniqueUnitId, Unit_ID>;

  struct DependencyExtractor
  {
    explicit
    DependencyExtractor(std::vector<Unit_ID>& vec, const UnitNameUnitIDMap& nm2id, AstContext ctx)
    : deps(vec), unitName2Id(nm2id), astctx(ctx)
    {}

    void operator()(Element_Struct& elem)
    {
      ADA_ASSERT(elem.Element_Kind == A_Clause);

      Clause_Struct& clause = elem.The_Union.Clause;

      if (clause.Clause_Kind != A_With_Clause)
        return;

      std::vector<Unit_ID>& res = deps;
      const UnitNameUnitIDMap& nm2id = unitName2Id;
      AstContext            ctx{astctx};

      traverseIDs( idRange(clause.Clause_Names), elemMap(),
                   [&res, &ctx, &nm2id](Element_Struct& el) -> void
                   {
                     ADA_ASSERT (el.Element_Kind == An_Expression);
                     NameData imported = getName(el, ctx);
                     std::string unitName = imported.fullName;

                     // try spec first
                     auto pos = nm2id.find(UniqueUnitId{false, imported.fullName});

                     // functional and procedural units may have bodies
                     if (pos == nm2id.end())
                       pos = nm2id.find(UniqueUnitId{true, imported.fullName});

                     if (pos == nm2id.end())
                     {
                       logError() << "unknown unit: " << imported.fullName << std::endl;
                       return;
                     }

                     res.emplace_back(pos->second);
                   }
                 );
    }

    std::vector<Unit_ID>&    deps;
    const UnitNameUnitIDMap& unitName2Id;
    AstContext               astctx;
  };


  void addWithClausDependencies( Unit_Struct& unit,
                                 std::vector<Unit_ID>& res,
                                 const UnitNameUnitIDMap& unitName2Id,
                                 AstContext ctx
                               )
  {
    ElemIdRange range = idRange(unit.Context_Clause_Elements);

    traverseIDs(range, elemMap(), DependencyExtractor{res, unitName2Id, ctx});
  }


  void dfs( std::map<Unit_ID, UnitEntry>& m,
            std::map<Unit_ID, UnitEntry>::value_type& el,
            std::vector<Unit_Struct*>& res
          )
  {
    if (el.second.marked) return;

    el.second.marked = true;

    // handle dependencies first
    for (Unit_ID depID : el.second.dependencies)
    {
      auto pos = m.find(depID);
      ADA_ASSERT(pos != m.end());

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

  size_t getUnitIDofParent(const Unit_Struct& unit)
  {
    size_t res = 0;

    switch (unit.Unit_Kind)
    {
      case A_Procedure:
      case A_Function:
      case A_Package:
      case A_Generic_Procedure:
      case A_Generic_Function:
      case A_Generic_Package:
      case A_Procedure_Instance:
      case A_Function_Instance:
      case A_Package_Instance:
      case A_Procedure_Renaming:
      case A_Function_Renaming:
      case A_Package_Renaming:
      case A_Generic_Procedure_Renaming:
      case A_Generic_Function_Renaming:
      case A_Generic_Package_Renaming:
      case A_Procedure_Body:
      case A_Function_Body:
      case A_Package_Body:
        {
          res = unit.Corresponding_Parent_Declaration;
          break;
        }

      case A_Procedure_Body_Subunit:
      case A_Function_Body_Subunit:
      case A_Package_Body_Subunit:
      case A_Task_Body_Subunit:
      case A_Protected_Body_Subunit:
        {
          res = unit.Corresponding_Subunit_Parent_Body;
          break;
        }

      default: ;
    }

    return res;
  }

  Declaration_ID getCorrespondingDeclaration(const Unit_Struct& unit)
  {
    const bool hasCorrespDecl = (  (unit.Unit_Kind == A_Procedure_Body)
                                || (unit.Unit_Kind != A_Function_Body)
                                || (unit.Unit_Kind != A_Package_Body)
                                || (unit.Unit_Kind != An_Unknown_Unit)
                                );

    return hasCorrespDecl ? unit.Corresponding_Declaration : 0;
  }

  std::vector<Unit_Struct*>
  sortUnitsTopologically(Unit_Struct_List_Struct* adaUnit, AstContext ctx)
  {
    using DependencyMap     = std::map<Unit_ID, UnitEntry> ;
    using UnitVector        = std::vector<Unit_Struct_List_Struct*> ;

    UnitVector        allUnits;

    // build maps for all units
    for (Unit_Struct_List_Struct* unit = adaUnit; unit != nullptr; unit = unit->Next)
      allUnits.push_back(unit);

    UnitNameUnitIDMap nameIdMap;
    DependencyMap     deps;

    for (Unit_Struct_List_Struct* unit : allUnits)
    {
      ADA_ASSERT(unit);

      auto nmidRes = nameIdMap.emplace(uniqueUnitName(unit->Unit), unit->Unit.ID);
      ADA_ASSERT(nmidRes.second);

      // map specifications to their name
      auto depsRes = deps.emplace(unit->Unit.ID, UnitEntry{&(unit->Unit), std::vector<int>{}, false});
      ADA_ASSERT(depsRes.second);
    }

    // link the units
    for (Unit_Struct_List_Struct* unit : allUnits)
    {
      DependencyMap::iterator depPos = deps.find(unit->Unit.ID);
      ADA_ASSERT(depPos != deps.end());

      UnitEntry&              unitEntry = depPos->second;
      const size_t            parentID  = getUnitIDofParent(unit->Unit);

      // add the parent unit, if present
      if (deps.find(parentID) != deps.end())
      {
        unitEntry.dependencies.emplace_back(parentID);
      }
      else if (parentID > 0)
      {
        // parentID == 1.. 1 refers to the package standard (currently not extracted from Asis)
        UniqueUnitId uid = uniqueUnitName(unit->Unit);

        logWarn() << "unknown unit dependency: "
                  << uid << " #" << unit->Unit.ID
                  << " -> #" << parentID
                  << std::endl;
      }

      // add the declaration unit (SPEC) if present
      if (Declaration_ID correspUnit = getCorrespondingDeclaration(unit->Unit))
      {
        if (!isInvalidId(correspUnit))
        {
          ADA_ASSERT (deps.end() != deps.find(correspUnit));

          unitEntry.dependencies.emplace_back(correspUnit);
        }
      }

      addWithClausDependencies(unit->Unit, unitEntry.dependencies, nameIdMap, ctx);
    }

    std::vector<Unit_Struct*> res;

    // topo sort

    // sort system packages first
    for (DependencyMap::value_type& el : deps)
      if (isSystemPackage(el.second.unit))
        dfs(deps, el, res);

    // sort language and user defined packages
    for (DependencyMap::value_type& el : boost::adaptors::reverse(deps))
      dfs(deps, el, res);

    if (PRINT_UNIT_DEPENDENCIES)
    {
      // print all module dependencies
      for (DependencyMap::value_type& el : deps)
      {
        logWarn() << el.second.unit->Unit_Full_Name << " (" << el.first << "): ";
        for (int n : el.second.dependencies)
          logWarn() << deps.at(n).unit->Unit_Full_Name << " (" << n << "), ";

        logWarn() << std::endl << std::endl;
      }
    }

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

    //~ void logVarRefExp(SgVarRefExp* n)
    //~ {
      //~ if (!n) return;

      //~ logWarn() << "verref = " << n->unparseToString() << std::endl;
    //~ }

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
      //~ checkDecl(isSgDeclarationStatement(n));
      //~ checkScope(isSgScopeStatement(n));
      //~ logVarRefExp(isSgVarRefExp(n));
    }
  };


  /// Implements a quick check that the AST is properly constructed.
  ///   While some issues, such as parent pointers will be fixed at the
  ///   post processing stage, it may be good to point inconsistencies
  ///   out anyway.
  inline
  void astSanityCheck(SgSourceFile* file)
  {
    AstSanityCheck checker;

    checker.traverse(file, preorder);
  }

  // when set to true inference becomes more permissive, b/c it may stop early.
  constexpr bool INFERENCE_SHORTCUT = false;

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

  struct OverloadInfo : std::tuple<SgFunctionSymbol*, OverloadSet>
  {
    using base = std::tuple<SgFunctionSymbol*, OverloadSet>;
    using base::base;

    SgFunctionSymbol*  orig_sym() const { return std::get<0>(*this); }
          OverloadSet& ovlset()         { return std::get<1>(*this); }
    const OverloadSet& ovlset() const   { return std::get<1>(*this); }
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
        //~ if (beg == lim) return;

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

        m.emplace(fnref, OverloadInfo{fnsym, std::move(overloads)});
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

    extractor.traverse(&scope, preorder);

    logInfo() << "Calls requiring disambiguation: " << extractor.reqdisambig << std::endl;
    return std::move(extractor);
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
      if (prmRoot.polymorphic()) return true;

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

  bool
  isAdaLiteralList(const SgExpression* arg)
  {
    return isSgAggregateInitializer(arg);
    //~ const SgExprListExp* lst = isSgExprListExp(arg);
    //~ if (!lst) return false;

    //~ const SgExpressionPtrList& exprs = lst->get_expressions();

    //~ return std::all_of( exprs.begin(), exprs.end(),
                        //~ [](const SgExpression* e) { return isSgValueExp(e); }
                      //~ );
  }

  std::set<const SgType*>
  simpleExpressionType(const SgExpression& arg)
  {
    // literals are convertible to derived types
    //    without further type resolution, it is unclear what type this
    //    literals have. Thus, exclude literals from participating in
    //    overload resolution for now.
    if (isSgValueExp(&arg) || isAdaLiteralList(&arg))
      return {};

    //~ return { si::Ada::typeOfExpr(arg) };
    return { arg.get_type() };
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

    if (pos == allrefs.end())
      return { arg.get_type() };

    ResultType res;

    for (SgFunctionSymbol* sym : pos->second.ovlset())
    {
      ASSERT_not_null(sym);

      const SgFunctionDeclaration&        fndcl  = SG_DEREF(sym->get_declaration());
      const SgAdaInheritedFunctionSymbol* inhsym = isSgAdaInheritedFunctionSymbol(sym);
      const SgFunctionType*               fnty   = inhsym ? inhsym->get_derivedFunctionType()
                                                          : fndcl.get_type();
      ASSERT_not_null(fnty);

      res.insert(fnty->get_return_type());
    }

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

  SgExpressionPtrList
  normalizedArguments(const SgFunctionCallExp* fncall)
  {
    try
    {
      if (fncall != nullptr)
        return si::Ada::normalizedCallArguments(*fncall);
    }
    catch (const std::logic_error& e)
    {
      logError() << e.what() << " in call "
                 << (fncall ? fncall->unparseToString() : "<nullptr>")
                 << std::endl;
    }

    return {};
  }

  std::set<const SgType*>
  typesFromCallContext( const SgFunctionCallExp& parentCall,
                        const SgFunctionCallExp& childCall,
                        const OverloadMap& allrefs
                      );


  std::set<const SgType*>
  callTypeEqualityConstraint( const SgFunctionCallExp& parentCall,
                              std::size_t pos,
                              const OverloadMap& allrefs
                            )
  {
    const SgExpression* arg = normalizedArguments(&parentCall).at(pos);

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
    {
      //~ logTrace() << "typesFromCallContext: ret {} @: "
                 //~ << (ovpos != allrefs.end()) << " / " << (fnref == nullptr)
                 //~ << parentCall.get_function()
                 //~ << std::endl;
      return {};
    }

    ADA_ASSERT(fnref != nullptr);
    const SgFunctionDeclaration* fndcl = fnref->getAssociatedFunctionDeclaration();

    // do not trust arguments of compiler generated functions
    if (fndcl == nullptr)
    {
      logTrace() << "typesFromCallContext: ret {} @ 2" << std::endl;
      return {};
    }

    const bool compilerGenerated     = testProperty(*fndcl, &Sg_File_Info::isCompilerGenerated);
    const bool compilerGenComparison = compilerGenerated && isComparisonOperator(*fndcl);

    if (compilerGenerated && !compilerGenComparison)
    {
      logTrace() << "typesFromCallContext: ret {} @ 3" << std::endl;
      return {};
    }

    std::set<const SgType*> res;

    try
    {
      // \todo instead of computing the normalizedArgumentPosition every time,
      //       the information could memoized or stored in the OverloadMap.
      std::size_t argpos = si::Ada::normalizedArgumentPosition(parentCall, childCall);

      if (!compilerGenComparison)
      {
        logTrace() << "typesFromCallContext: normal" << std::endl;

        for (SgFunctionSymbol* fnsym : ovpos->second.ovlset())
        {
          const SgAdaInheritedFunctionSymbol* inhsym = isSgAdaInheritedFunctionSymbol(fnsym);
          const SgFunctionType*               fnty   = inhsym ? inhsym->get_derivedFunctionType()
                                                              : isSgFunctionType(fnsym->get_type());

          res.insert(fnty->get_arguments().at(argpos));
        }
      }
      else
      {
        logTrace() << "typesFromCallContext: compgen" << std::endl;
        ADA_ASSERT(ovpos->second.ovlset().size() < 2);
        ADA_ASSERT(argpos == 0 || argpos == 1);

        res = callTypeEqualityConstraint(parentCall, 1 - argpos, allrefs);
      }
    }
    catch (const std::logic_error&)
    {
      /* catches exceptions from normalizedArgumentPosition. */
      logTrace() << "typesFromCallContext: ex " << std::endl;
      ADA_ASSERT(res.empty());
    }

    return res;
  }

  std::set<const SgType*>
  typesFromAssignContext( const SgAssignOp& parentAssign,
                          const SgFunctionCallExp& childCall,
                          const OverloadMap& /* allrefs */
                        )
  {
    ADA_ASSERT (parentAssign.get_rhs_operand() == &childCall);

    const SgExpression& lhs = SG_DEREF(parentAssign.get_lhs_operand());

    return { lhs.get_type() };
  }

  std::set<const SgType*>
  typesFromAssignInitializer( const SgAssignInitializer& assignIni,
                              const SgFunctionCallExp& /* childCall */,
                              const OverloadMap& /* allrefs */
                            )
  {
    if (const SgInitializedName* var = isSgInitializedName(assignIni.get_parent()))
      return { var->get_type() };

    return { };
  }



  // computes the types as expected from the call context
  //   if the type should not be used, or the call is to a procedure
  //   return the empty set.
  std::set<const SgType*>
  expectedTypes(const SgFunctionCallExp* call, const OverloadMap& allrefs)
  {
    ASSERT_not_null(call);

    const SgNode* parentNode = call->get_parent();

    if (const SgActualArgumentExpression* parentNamed = isSgActualArgumentExpression(parentNode))
      parentNode = parentNamed->get_parent();

    if (const SgFunctionCallExp* parentCall = parentCallNode(isSgExprListExp(parentNode)))
      return typesFromCallContext(*parentCall, *call, allrefs);

    if (const SgAssignOp* assignExp = isSgAssignOp(parentNode))
      return typesFromAssignContext(*assignExp, *call, allrefs);

    if (/*const SgCastExp* castExp =*/ isSgCastExp(parentNode))
      return { }; // we cannot assume anything about the operand type

    if (const SgAssignInitializer* assignIni = isSgAssignInitializer(parentNode))
      return typesFromAssignInitializer(*assignIni, *call, allrefs);

    if (isSgOrOp(parentNode) || isSgAndOp(parentNode))
      return { sb::buildBoolType() };

    if (const SgReturnStmt* retstmt = isSgReturnStmt(parentNode))
    {
      const SgFunctionDeclaration& fndcl = sg::ancestor<SgFunctionDeclaration>(*retstmt);
      const SgFunctionType&        fnty  = SG_DEREF(fndcl.get_type());

      return { fnty.get_return_type() };
    }

    if (isSgExprStatement(parentNode)) // procedure call
      return {};

    ASSERT_not_null(parentNode);
    logFlaw() << "unrecognizedCall context: " << typeid(*parentNode).name()
              << std::endl;
    return {};
  }

  const SgType*
  functionReturnType(const SgFunctionSymbol* fnsy)
  {
    const SgFunctionType* fnty = isSgFunctionType(fnsy->get_type());

    return fnty ? fnty->get_return_type() : nullptr;
  }

  const std::vector<SgType*>&
  parameterTypes(const SgFunctionSymbol* sym)
  {
    ASSERT_not_null(sym);

    if (const SgAdaInheritedFunctionSymbol* inhsym = isSgAdaInheritedFunctionSymbol(sym))
      return SG_DEREF(inhsym->get_derivedFunctionType()).get_arguments();

    const SgFunctionDeclaration& fndcl = SG_DEREF(sym->get_declaration());

    return SG_DEREF(fndcl.get_type()).get_arguments();
  }

  void resolveInheritedFunctionOverloads(SgGlobal& scope)
  {
    OverloadMap allrefs = collectAllFunctionRefExp(scope);
    WorkItems   workset = createWorksetFrom(allrefs);

    logInfo() << "resolveInheritedFunctionOverloads " << workset.size() << std::endl;

    while (!workset.empty())
    {
      OverloadMap::value_type& item      = *workset.next();
      OverloadSet&             overloads = item.second.ovlset();
      const std::size_t        numcands  = overloads.size();

      // nothing to be done ... go to next work item
      if (INFERENCE_SHORTCUT && (numcands < 2)) continue;

      SgFunctionRefExp&         fnref  = SG_DEREF(item.first);
      const SgFunctionCallExp*  fncall = callNode(fnref);
      const SgExpressionPtrList args   = normalizedArguments(fncall);

      if (numcands != 1)
        logInfo() << "resolve: " << fnref.get_parent()->unparseToString() << " " << numcands
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
        if (!INFERENCE_SHORTCUT || viables.size())
          overloads.swap(viables);
      }

      {
        //~ if (overloads.size() != 1)
          logInfo() << "result-resolve: " << fnref.get_parent()->unparseToString() << " " << overloads.size()
                    << std::endl;

        // ...
        // disambiguate based on return types and context
        OverloadSet viables;
        auto isViableReturn = [expTypes = expectedTypes(fncall, allrefs)]
                              (SgFunctionSymbol* fnsy)->bool
                              {
                                logInfo() << "result-size: " << expTypes.size() << std::endl;
                                if (expTypes.empty()) return true;

                                return ArgParamTypeCompatibility{}(functionReturnType(fnsy), expTypes);
                              };

        std::copy_if( overloads.begin(), overloads.end(),
                      std::back_inserter(viables),
                      isViableReturn
                    );

        // put in place candidates
        if (!INFERENCE_SHORTCUT || viables.size())
          overloads.swap(viables);
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
    if (!INFERENCE_SHORTCUT)
    {
      logTrace() << "checking fun calls.." << std::endl;

      for (const OverloadMap::value_type& item : allrefs)
      {
        const SgExpression* exp = callNode(SG_DEREF(item.first));

        if (item.second.ovlset().size() != 1)
        {
          logFlaw() << "disambig: " << (exp ? exp->unparseToString() : std::string{"<null>"})
                                    << " " << item.second.ovlset().size()
                                    << std::endl;
        }
      }
    }
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

  resolveInheritedFunctionOverloads(astScope);
  clearMappings();

  //~ std::string astDotFile = astDotFileName(*file);
  //~ logTrace() << "Generating DOT file for ROSE AST: " << astDotFile << std::endl;
  //~ generateDOT(&astScope, astDotFile);

  logInfo() << "Checking AST post-production" << std::endl;
  inheritFileInfo(file);
  //~ astSanityCheck(file);

  file->set_processedToIncludeCppDirectivesAndComments(false);
  logInfo() << "Building ROSE AST done" << std::endl;

  //~ si::Ada::convertToOperatorRepresentation(&astScope);
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
