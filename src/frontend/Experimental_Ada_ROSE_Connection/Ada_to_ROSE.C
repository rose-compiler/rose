#include "sage3basic.h"

#include <type_traits>

#include "Diagnostics.h"
#include "rose_config.h"
#include "sageGeneric.h"
#include "sageBuilder.h"
#include "Ada_to_ROSE_translation.h"
#include "Ada_to_ROSE.h"
#include "AdaMaker.h"
#include "AdaExpression.h"
#include "AdaStatement.h"
#include "AdaType.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Ada_ROSE_Translation
{

//
// logger

Sawyer::Message::Facility adalogger;

void logInit()
{
  adalogger = Sawyer::Message::Facility("ADA-Frontend", Rose::Diagnostics::destination);
}

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

  /// stores a mapping from an Element_ID to a loop statement
  /// \todo this should be localized in the AstContext class
  map_t<int, SgStatement*> asisLoopsMap;

  /// stores a mapping from string to builtin type nodes
  map_t<std::string, SgType*> adaTypesMap;
} // anonymous namespace

//~ map_t<int, SgDeclarationStatement*>& asisUnits() { return asisUnitsMap; }
map_t<int, SgInitializedName*>&      asisVars()  { return asisVarsMap;  }
map_t<int, SgInitializedName*>&      asisExcps() { return asisExcpsMap; }
map_t<int, SgDeclarationStatement*>& asisDecls() { return asisDeclsMap; }
map_t<int, SgDeclarationStatement*>& asisTypes() { return asisTypesMap; }
map_t<int, SgStatement*>&            asisLoops() { return asisLoopsMap; }
map_t<std::string, SgType*>&         adaTypes()  { return adaTypesMap;  }
ASIS_element_id_to_ASIS_MapType&     elemMap()   { return asisMap;      }
ASIS_element_id_to_ASIS_MapType&     unitMap()   { return asisMap;      }


//
// auxiliary classes and functions

LabelManager::~LabelManager()
{
  for (GotoContainer::value_type el : gotos)
    el.first->set_label(&lookupNode(labels, el.second));
}

void LabelManager::label(Element_ID id, SgLabelStatement& lblstmt)
{
  SgLabelStatement*& mapped = labels[id];

  ROSE_ASSERT(mapped == nullptr);
  mapped = &lblstmt;
}

void LabelManager::gotojmp(Element_ID id, SgGotoStatement& gotostmt)
{
  gotos.emplace_back(&gotostmt, id);
}

AstContext AstContext::scope_npc(SgScopeStatement& s) const
{
  AstContext tmp{*this};

  tmp.the_scope = &s;
  return tmp;
}

AstContext AstContext::scope(SgScopeStatement& s) const
{
  ROSE_ASSERT(s.get_parent());

  return scope_npc(s);
}

AstContext AstContext::labels(LabelManager& lm) const
{
  AstContext tmp{*this};

  tmp.all_labels = &lm;
  return tmp;
}

/// attaches the source location information from \ref elem to
///   the AST node \ref n.
void attachSourceLocation(SgLocatedNode& n, Element_Struct& elem)
{
  Source_Location_Struct& loc  = elem.Source_Location;
  std::string             unit{loc.Unit_Name};

  // \todo consider deleting existing source location information
  //~ delete n.get_file_info();
  //~ delete n.get_startOfConstruct();
  //~ delete n.get_endOfConstruct();

  n.set_file_info       (&mkFileInfo(unit, loc.First_Line, loc.First_Column));
  n.set_startOfConstruct(&mkFileInfo(unit, loc.First_Line, loc.First_Column));
  n.set_endOfConstruct  (&mkFileInfo(unit, loc.Last_Line,  loc.Last_Column));
}

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
    asisLoops().clear();
    adaTypes().clear();
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
          // currently only handles with clauses
          handleClause(elem, ctx);
          break;
        }

      case A_Defining_Name:           // Asis.Declarations
        {
          // handled by getName
          ROSE_ASSERT(false);
          break;
        }

      case A_Statement:               // Asis.Statements
        {
          // handled in StmtCreator
          ROSE_ASSERT(false);
          break;
        }

      case An_Expression:             // Asis.Expressions
        {
          // handled by getExpr
          ROSE_ASSERT(false);
          break;
        }

      case An_Exception_Handler:      // Asis.Statements
        {
          // handled by handleExceptionHandler
          ROSE_ASSERT(false);
          break;
        }

      case A_Definition:              // Asis.Definitions
        {
          // records (one of many definitions) are handled by getRecordBody
          Definition_Struct& def = elem.The_Union.Definition;

          logWarn() << "Unhandled element " << elem.Element_Kind
                    << "\n  definition kind: " << def.Definition_Kind
                    << std::endl;

          ROSE_ASSERT(false && !FAIL_ON_ERROR);
          break;
        }

      case Not_An_Element: /* break; */ // Nil_Element
      case A_Path:                    // Asis.Statements
      case A_Pragma:                  // Asis.Elements
      case An_Association:            // Asis.Expressions
      default:
        logWarn() << "Unhandled element " << elem.Element_Kind << std::endl;
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }
  }


  void handleElementID(Element_ID id, AstContext ctx)
  {
    handleElement(retrieveAs<Element_Struct>(elemMap(), id), ctx);
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

      void operator()(Unit_Struct& adaUnit)
      {
        handleUnit(adaUnit, ctx);
      }

      void operator()(Unit_Struct_List_Struct& adaUnit)
      {
        (*this)(adaUnit.Unit);
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

  void handleUnit(Unit_Struct& adaUnit, AstContext ctx)
  {
    // dispatch based on unit kind
    switch (adaUnit.Unit_Kind)
    {
      case A_Function:
      case A_Procedure:
        {
          logTrace() << "A " << (adaUnit.Unit_Kind == A_Function ? "function" : "procedure")
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
          /* break; */
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

          /* break; */
          break;
        }

      case A_Package:
        {
          logTrace() << "A package"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;

          ElemIdRange           elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange           unitRange = idRange(adaUnit.Corresponding_Children);

          if (elemRange.size() || unitRange.size())
          {
            logWarn() << "   elems# " << elemRange.size()
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

          ElemIdRange           elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange           unitRange = idRange(adaUnit.Corresponding_Children);

          if (elemRange.size() || unitRange.size())
          {
            logWarn() << "   elems# " << elemRange.size()
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

      case Not_A_Unit: /* break; */
      case A_Package_Instance:
      case A_Generic_Package:

      case A_Generic_Procedure:
      case A_Generic_Function:

      case A_Procedure_Instance:
      case A_Function_Instance:

      case A_Procedure_Renaming:
      case A_Function_Renaming:
      case A_Package_Renaming:

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
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }
  }
}


void ElemCreator::operator()(Element_Struct& elem)
{
  handleElement(elem, ctx, privateElems);
}

void secondConversion(Nodes_Struct& headNodes, SgSourceFile* file)
{
  ROSE_ASSERT(file);

  logInit();
  logInfo() << "Building ROSE AST .." << std::endl;

  Unit_Struct_List_Struct* adaLimit = 0;
  Unit_Struct_List_Struct* adaUnit  = headNodes.Units;
  SgGlobal&                astScope = SG_DEREF(file->get_globalScope());

  initializeAdaTypes(astScope);
  traverse(adaUnit, adaLimit, UnitCreator{AstContext{astScope}});
  clearMappings();

  logTrace() << "Generating DOT file: " << "adaTypedAst.dot" << std::endl;
  generateDOT(&astScope, "adaTypedAst");

  file->set_processedToIncludeCppDirectivesAndComments(false);
  logInfo() << "Building ROSE AST done" << std::endl;
}



}
