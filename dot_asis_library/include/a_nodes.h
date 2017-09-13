#ifndef A_NODES_H
#define A_NODES_H

#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// NOTE: All of the enums below that duplicate ASIS enumerated types MUST match
// the ASIS types exactly, because we do unchecked conversions between them.
///////////////////////////////////////////////////////////////////////////////

enum Node_Kinds { // Not an ASIS type
  Not_A_Node,
  A_Context_Node,
  A_Unit_Node,
  An_Element_Node
};

typedef int Node_ID;

///////////////////////////////////////////////////////////////////////////////
// BEGIN context
///////////////////////////////////////////////////////////////////////////////

struct Context_Struct {
  char *name;
  char *parameters;
  char *debug_image;
};

///////////////////////////////////////////////////////////////////////////////
// END context
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN unit
///////////////////////////////////////////////////////////////////////////////
typedef Node_ID Unit_ID;

typedef Unit_ID *Unit_ID_Ptr;

// May take 2*4 bytes - 1 int, 1 ptr:
// _IDs_ points to the first of _length_ IDs:
struct Unit_ID_Array_Struct {
  int         Length;
  Unit_ID_Ptr IDs;
};
typedef struct Unit_ID_Array_Struct Unit_List;

enum Unit_Kinds {
  Not_A_Unit,

  A_Procedure,
  A_Function,
  A_Package,

  A_Generic_Procedure,
  A_Generic_Function,
  A_Generic_Package,

  A_Procedure_Instance,
  A_Function_Instance,
  A_Package_Instance,

  A_Procedure_Renaming,
  A_Function_Renaming,
  A_Package_Renaming,

  A_Generic_Procedure_Renaming,
  A_Generic_Function_Renaming,
  A_Generic_Package_Renaming,

  A_Procedure_Body,
  //  A unit interpreted only as the completion of a procedure, or a unit
  //  interpreted as both the declaration and body of a library
  //  procedure. Reference Manual 10.1.4(4)
  A_Function_Body,
  //  A unit interpreted only as the completion of a function, or a unit
  //  interpreted as both the declaration and body of a library
  //  function. Reference Manual 10.1.4(4)
  A_Package_Body,

  A_Procedure_Body_Subunit,
  A_Function_Body_Subunit,
  A_Package_Body_Subunit,
  A_Task_Body_Subunit,
  A_Protected_Body_Subunit,

  A_Nonexistent_Declaration,
  //  A unit that does not exist but is:
  //    1) mentioned in a with clause of another unit or,
  //    2) a required corresponding library_unit_declaration
  A_Nonexistent_Body,
  //  A unit that does not exist but is:
  //     1) known to be a corresponding subunit or,
  //     2) a required corresponding library_unit_body
  A_Configuration_Compilation,
  //  Corresponds to the whole content of a compilation with no
  //  compilation_unit, but possibly containing comments, configuration
  //  pragmas, or both. Any Context can have at most one unit of
  //  A_Configuration_Compilation kind. A unit of
  //  A_Configuration_Compilation does not have a name. This unit
  //  represents configuration pragmas that are "in effect".
  //
  //  GNAT-specific note: In case of GNAT the requirement to have at most
  //  one unit of A_Configuration_Compilation kind does not make sense: in
  //  GNAT compilation model configuration pragmas are contained in
  //  configuration files, and a compilation may use an arbitrary number
  //  of configuration files. That is, (Elements representing) different
  //  configuration pragmas may have different enclosing compilation units
  //  with different text names. So in the ASIS implementation for GNAT a
  //  Context may contain any number of units of
  //  A_Configuration_Compilation kind
  An_Unknown_Unit
};

enum Unit_Classes {
  Not_A_Class,
  //  A nil, nonexistent, unknown, or configuration compilation unit class.
  A_Public_Declaration,
  //  library_unit_declaration or library_unit_renaming_declaration.
  A_Public_Body,
  //  library_unit_body interpreted only as a completion. Its declaration
  //  is public.
  A_Public_Declaration_And_Body,
  //  subprogram_body interpreted as both a declaration and body of a
  //  library subprogram - Reference Manual 10.1.4(4).
  A_Private_Declaration,
  //  private library_unit_declaration or private
  //  library_unit_renaming_declaration.
  A_Private_Body,
  //  library_unit_body interpreted only as a completion. Its declaration
  //  is private.
  A_Separate_Body
  //  separate (parent_unit_name) proper_body.
};
  
enum Unit_Origins {
  Not_An_Origin,
  //  A nil or nonexistent unit origin. An_Unknown_Unit can be any origin
  A_Predefined_Unit,
  //  Ada predefined language environment units listed in Annex A(2).
  //  These include Standard and the three root library units: Ada,
  //  Interfaces, and System, and their descendants.  i.e., Ada.Text_Io,
  //  Ada.Calendar, Interfaces.C, etc.
  An_Implementation_Unit,
  //  Implementation specific library units, e.g., runtime support
  //  packages, utility libraries, etc. It is not required that any
  //  implementation supplied units have this origin. This is a suggestion.
  //  Implementations might provide, for example, precompiled versions of
  //  public domain software that could have An_Application_Unit origin.
  An_Application_Unit
  //  Neither A_Predefined_Unit or An_Implementation_Unit
};


// May take 21*4 (20*4 + 3) bytes - 5 ID, 3 enum, 2 Unit_List(2*4 ea), 8 char*, 3 bool:
struct Unit_Struct {
  Unit_ID           ID;
  enum Unit_Kinds   Unit_Kind;
  enum Unit_Classes Unit_Class;
  enum Unit_Origins Unit_Origin;
  // Enclosing_Context
  // Enclosing_Container
  Unit_List         Corresponding_Children;
  Unit_ID           Corresponding_Parent_Declaration;
  Unit_ID           Corresponding_Declaration;
  Unit_ID           Corresponding_Body;
  char             *Unit_Full_Name; // Ada name
  char             *Unique_Name; // file name etc.
  bool              Exists;
  bool              Can_Be_Main_Program;
  bool              Is_Body_Required;
  char             *Text_Name;
  char             *Text_Form;
  char             *Object_Name;
  char             *Object_Form;
  char             *Compilation_Command_Line_Options;
  Unit_List         Subunits;
  Unit_ID           Corresponding_Subunit_Parent_Body;
  char             *Debug_Image;
};
///////////////////////////////////////////////////////////////////////////////
// END unit 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN element 
///////////////////////////////////////////////////////////////////////////////

typedef Node_ID    Element_ID;
typedef Element_ID Declaration_ID;
typedef Element_ID Defining_Name_ID;
typedef Element_ID Discrete_Range_ID;
typedef Element_ID Discrete_Subtype_Definition_ID;
typedef Element_ID Expression_ID;
typedef Element_ID Name_ID;
typedef Element_ID Statement_ID;
typedef Element_ID Subtype_Indication_ID;
typedef Element_ID Type_Definition_ID;

// For this:
//   typedef Element_ID Element_ID_Array[];
//   typedef Element_ID_Array *Element_ID_Array_Ptr2;
// GNAT C-to-Ada translator produces (array is consrtained!):
//   type Element_ID_Array is array (size_t) of aliased Element_ID;
//   type Element_ID_Array_Ptr2 is access all Element_ID_Array;
//
// For this:
//   typedef Element_ID *Element_ID_Array_Ptr[];
// GNAT C-to-Ada translator produces:
//   type Element_ID_Array_Ptr is array (size_t) of access Element_ID;
//
// So, we are just going to use a pointer to an Element_ID and treat it as an 
// array:

typedef Element_ID *Element_ID_Ptr;

// May take 2*4 bytes - 1 int, 1 ptr:
// _IDs_ points to the first of _length_ IDs:
struct Element_ID_Array_Struct {
  int            Length;
  Element_ID_Ptr IDs;
};
typedef struct Element_ID_Array_Struct Element_List;
typedef Element_List Association_List;
typedef Element_List Component_Clause_List;
typedef Element_List Declaration_List;
typedef Element_List Declarative_Item_List;
typedef Element_List Defining_Name_List;
typedef Element_List Exception_Handler_List;
typedef Element_List Expression_List;
typedef Element_List Expression_Path_List;
typedef Element_List Name_List;
typedef Element_List Path_List;
typedef Element_List Parameter_Specification_List;
typedef Element_List Representation_Clause_List;
typedef Element_List Statement_List;

enum Element_Kinds {
  Not_An_Element,            // Nil_Element
  A_Pragma,                  // Asis.Elements
  A_Defining_Name,           // Asis.Declarations
  A_Declaration,             // Asis.Declarations
  A_Definition,              // Asis.Definitions
  An_Expression,             // Asis.Expressions
  An_Association,            // Asis.Expressions
  A_Statement,               // Asis.Statements
  A_Path,                    // Asis.Statements
  A_Clause,                  // Asis.Clauses
  An_Exception_Handler       // Asis.Statements
};

///////////////////////////////////////////////////////////////////////////////
// BEGIN element kind kinds
///////////////////////////////////////////////////////////////////////////////

enum Pragma_Kinds {
  Not_A_Pragma,                           // An unexpected element
  An_All_Calls_Remote_Pragma,             // E.2.3(5)
  An_Assert_Pragma,                       // 11.4.2, Ada 2005
  An_Assertion_Policy_Pragma,             // 11.4.2, Ada 2005
  An_Asynchronous_Pragma,                 // E.4.1(3)
  An_Atomic_Pragma,                       // C.6(3)
  An_Atomic_Components_Pragma,            // C.6(5)
  An_Attach_Handler_Pragma,               // C.3.1(4)
  A_Controlled_Pragma,                    // 13.11.3(3)
  A_Convention_Pragma,                    // B.1(7), M.1(5)
  A_CPU_Pragma,                           // J.15.9, Ada 2012
  A_Default_Storage_Pool_Pragma,          // 13.11.3, Ada 2012
  A_Detect_Blocking_Pragma,               // H.5, Ada 2005
  A_Discard_Names_Pragma,                 // C.5(3)
  A_Dispatching_Domain_Pragma,            // J.15.10 Ada 2012
  An_Elaborate_Pragma,                    // 10.2.1(20)
  An_Elaborate_All_Pragma,                // 10.2.1(21)
  An_Elaborate_Body_Pragma,               // 10.2.1(22)
  An_Export_Pragma,                       // B.1(5), M.1(5)
  An_Independent_Pragma,                  // J.15.8, Ada 2012
  A_Independent_Components_Pragma,        // J.15.8, Ada 2012
  An_Import_Pragma,                       // B.1(6), M.1(5)
  An_Inline_Pragma,                       // 6.3.2(3)
  An_Inspection_Point_Pragma,             // H.3.2(3)
  An_Interrupt_Handler_Pragma,            // C.3.1(2)
  An_Interrupt_Priority_Pragma,           // D.1(5)
  A_Linker_Options_Pragma,                // B.1(8)
  A_List_Pragma,                          // 2.8(21)
  A_Locking_Policy_Pragma,                // D.3(3)
  A_No_Return_Pragma,                     // J.15.2, Ada 2005
  A_Normalize_Scalars_Pragma,             // H.1(3)
  An_Optimize_Pragma,                     // 2.8(23)
  A_Pack_Pragma,                          // 13.2(3)
  A_Page_Pragma,                          // 2.8(22)
  A_Partition_Elaboration_Policy_Pragma,  // H.6, Ada 2005
  A_Preelaborable_Initialization_Pragma,  // 10.2.1, Ada 2005
  A_Preelaborate_Pragma,                  // 10.2.1(3)
  A_Priority_Pragma,                      // D.1(3)
  A_Priority_Specific_Dispatching_Pragma, // D.2.2, Ada 2005
  A_Profile_Pragma,                       //  13.12, Ada 2005
  A_Pure_Pragma,                          // 10.2.1(14)
  A_Queuing_Policy_Pragma,                // D.4(3)
  A_Relative_Deadline_Pragma,             // J.15.12, Ada 2005
  A_Remote_Call_Interface_Pragma,         // E.2.3(3)
  A_Remote_Types_Pragma,                  // E.2.2(3)
  A_Restrictions_Pragma,                  // 13.12(3)
  A_Reviewable_Pragma,                    // H.3.1(3)
  A_Shared_Passive_Pragma,                // E.2.1(3)
  A_Storage_Size_Pragma,                  // 13.3(63)
  A_Suppress_Pragma,                      // 11.5(4)
  A_Task_Dispatching_Policy_Pragma,       // D.2.2(2)
  An_Unchecked_Union_Pragma,              // J.15.6, Ada 2005
  An_Unsuppress_Pragma,                   // 11.5, Ada 2005
  A_Volatile_Pragma,                      // C.6(4)
  A_Volatile_Components_Pragma,           // C.6(6)

  An_Implementation_Defined_Pragma  // 2.8(14)
};

enum Defining_Name_Kinds {
  Not_A_Defining_Name,                       // An unexpected element
  A_Defining_Identifier,                     // 3.1(4)
  A_Defining_Character_Literal,              // 3.5.1(4)
  A_Defining_Enumeration_Literal,            // 3.5.1(3)
  A_Defining_Operator_Symbol,                // 6.1(9)
  A_Defining_Expanded_Name                   // 6.1(7)
};

enum Declaration_Kinds {
  Not_A_Declaration,                       // An unexpected element
  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  a full_type_declaration of the form:
  //  type defining_identifier [known_discriminant_part] is
  //     type_definition;

  A_Task_Type_Declaration,                  // 9.1(2)
  A_Protected_Type_Declaration,             // 9.4(2)
  An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
  //  //|A2005 start
  A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
  //  //|A2005 end
  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2) -> Trait_Kinds
  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3) -> Trait_Kinds

  A_Subtype_Declaration,                    // 3.2.2(2)

  A_Variable_Declaration,                   // 3.3.1(2) -> Trait_Kinds
  A_Constant_Declaration,                   // 3.3.1(4) -> Trait_Kinds
  A_Deferred_Constant_Declaration,          // 3.3.1(6),7.4(2) -> Trait_Kinds
  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)

  An_Integer_Number_Declaration,            // 3.3.2(2)
  A_Real_Number_Declaration,                // 3.5.6(2)

  An_Enumeration_Literal_Specification,     // 3.5.1(3)
  
  A_Discriminant_Specification,             // 3.7(5)   -> Trait_Kinds
  A_Component_Declaration,                  // 3.8(6)

  A_Loop_Parameter_Specification,           // 5.5(4)   -> Trait_Kinds
  //  //|A2012 start
  A_Generalized_Iterator_Specification,     // 5.5.2    -> Trait_Kinds
  An_Element_Iterator_Specification,        // 5.5.2    -> Trait_Kinds
  //  //|A2012 end

  A_Procedure_Declaration,                  // 6.1(4)   -> Trait_Kinds
  A_Function_Declaration,                   // 6.1(4)   -> Trait_Kinds

  A_Parameter_Specification,                // 6.1(15)  -> Trait_Kinds
  //                                                  -> Mode_Kinds
  A_Procedure_Body_Declaration,             // 6.3(2)
  A_Function_Body_Declaration,              // 6.3(2)

  //  //|A2005 start
  A_Return_Variable_Specification,          // 6.5
  A_Return_Constant_Specification,          // 6.5
  A_Null_Procedure_Declaration,             // 6.7
  //  //|A2005 end

  //  //|A2012 start
  An_Expression_Function_Declaration,       // 6.8
  //  //|A2012 end

  A_Package_Declaration,                    // 7.1(2)
  A_Package_Body_Declaration,               // 7.2(2)

  An_Object_Renaming_Declaration,           // 8.5.1(2)
  An_Exception_Renaming_Declaration,        // 8.5.2(2)
  A_Package_Renaming_Declaration,           // 8.5.3(2)
  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  A_Function_Renaming_Declaration,          // 8.5.4(2)
  A_Generic_Package_Renaming_Declaration,   // 8.5.5(2)
  A_Generic_Procedure_Renaming_Declaration, // 8.5.5(2)
  A_Generic_Function_Renaming_Declaration,  // 8.5.5(2)

  A_Task_Body_Declaration,                  // 9.1(6)
  A_Protected_Body_Declaration,             // 9.4(7)
  An_Entry_Declaration,                     // 9.5.2(2)
  An_Entry_Body_Declaration,                // 9.5.2(5)
  An_Entry_Index_Specification,             // 9.5.2(2)

  A_Procedure_Body_Stub,                    // 10.1.3(3)
  A_Function_Body_Stub,                     // 10.1.3(3)
  A_Package_Body_Stub,                      // 10.1.3(4)
  A_Task_Body_Stub,                         // 10.1.3(5)
  A_Protected_Body_Stub,                    // 10.1.3(6)

  An_Exception_Declaration,                 // 11.1(2)
  A_Choice_Parameter_Specification,         // 11.2(4)

  A_Generic_Procedure_Declaration,          // 12.1(2)
  A_Generic_Function_Declaration,           // 12.1(2)
  A_Generic_Package_Declaration,            // 12.1(2)

  A_Package_Instantiation,                  // 12.3(2)
  A_Procedure_Instantiation,                // 12.3(2)
  A_Function_Instantiation,                 // 12.3(2)

  A_Formal_Object_Declaration,              // 12.4(2)  -> Mode_Kinds
  A_Formal_Type_Declaration,                // 12.5(2)
  //  //|A2012 start
  A_Formal_Incomplete_Type_Declaration,
  //  //|A2012 end
  A_Formal_Procedure_Declaration,           // 12.6(2)
  //                                           -> Subprogram_Default_Kinds
  A_Formal_Function_Declaration,            // 12.6(2)
  //                                           -> Subprogram_Default_Kinds
  A_Formal_Package_Declaration,             // 12.7(2)
  A_Formal_Package_Declaration_With_Box     // 12.7(3)
};

enum Definition_Kinds {
  Not_A_Definition,                 // An unexpected element

  A_Type_Definition,                // 3.2.1(4)    -> Type_Kinds

  A_Subtype_Indication,             // 3.2.2(3)
  A_Constraint,                     // 3.2.2(5)    -> Constraint_Kinds

  A_Component_Definition,           // 3.6(7)      -> Trait_Kinds

  A_Discrete_Subtype_Definition,    // 3.6(6)      -> Discrete_Range_Kinds
  A_Discrete_Range,                 // 3.6.1(3)    -> Discrete_Range_Kinds

  An_Unknown_Discriminant_Part,     // 3.7(3)
  A_Known_Discriminant_Part,        // 3.7(2)

  A_Record_Definition,              // 3.8(3)
  A_Null_Record_Definition,         // 3.8(3)

  A_Null_Component,                 // 3.8(4)
  A_Variant_Part,                   // 3.8.1(2)
  A_Variant,                        // 3.8.1(3)

  An_Others_Choice,                 // 3.8.1(5), 4.3.1(5), 4.3.3(5), 11.2(5)

  //  //|A2005 start
  An_Access_Definition,             // 3.10(6/2)   -> Access_Definition_Kinds
  //  //|A2005 end

  A_Private_Type_Definition,        // 7.3(2)      -> Trait_Kinds
  A_Tagged_Private_Type_Definition, // 7.3(2)      -> Trait_Kinds
  A_Private_Extension_Definition,   // 7.3(3)      -> Trait_Kinds

  A_Task_Definition,                // 9.1(4)
  A_Protected_Definition,           // 9.4(4)

  A_Formal_Type_Definition,         // 12.5(3)     -> Formal_Type_Kinds

  //  //|A2012 start
  An_Aspect_Specification           // 13.3.1
  //  //|A2012 end  
};

///////////////////////////////////////////////////////////////////////////////
// BEGIN Definition types
///////////////////////////////////////////////////////////////////////////////
typedef Element_ID   Constraint_ID;
typedef Element_ID   Definition_ID;
typedef Element_List Definition_ID_List;
typedef Element_List Declarative_Item_ID_List;
typedef Element_List Record_Component_List;

enum Type_Kinds {
  Not_A_Type_Definition,                 // An unexpected element

  A_Derived_Type_Definition,             // 3.4(2)     -> Trait_Kinds
  A_Derived_Record_Extension_Definition, // 3.4(2)     -> Trait_Kinds

  An_Enumeration_Type_Definition,        // 3.5.1(2)

  A_Signed_Integer_Type_Definition,      // 3.5.4(3)
  A_Modular_Type_Definition,             // 3.5.4(4)

  A_Root_Type_Definition,                // 3.5.4(14), 3.5.6(3)
  //                                               -> Root_Type_Kinds
  A_Floating_Point_Definition,           // 3.5.7(2)

  An_Ordinary_Fixed_Point_Definition,    // 3.5.9(3)
  A_Decimal_Fixed_Point_Definition,      // 3.5.9(6)

  An_Unconstrained_Array_Definition,     // 3.6(2)
  A_Constrained_Array_Definition,        // 3.6(2)

  A_Record_Type_Definition,              // 3.8(2)     -> Trait_Kinds
  A_Tagged_Record_Type_Definition,       // 3.8(2)     -> Trait_Kinds

  //  //|A2005 start
  An_Interface_Type_Definition,          // 3.9.4      -> Interface_Kinds
  //  //|A2005 end
  An_Access_Type_Definition            // 3.10(2)    -> Access_Type_Kinds
};

enum Constraint_Kinds {
  Not_A_Constraint,                      // An unexpected element
  A_Range_Attribute_Reference,           // 3.5(2)
  A_Simple_Expression_Range,             // 3.2.2, 3.5(3)
  A_Digits_Constraint,                   // 3.2.2, 3.5.9
  A_Delta_Constraint,                    // 3.2.2, J.3
  An_Index_Constraint,                   // 3.2.2, 3.6.1
  A_Discriminant_Constraint              // 3.2.2
};
///////////////////////////////////////////////////////////////////////////////
// END Definition types
///////////////////////////////////////////////////////////////////////////////

enum Expression_Kinds {
  Not_An_Expression,                         // An unexpected element

  A_Box_Expression,                          //  Ada 2005
  //  4.3.1(4), 4.3.3(3,6)

  An_Integer_Literal,                        // 2.4
  A_Real_Literal,                            // 2.4.1
  A_String_Literal,                          // 2.6

  An_Identifier,                             // 4.1
  An_Operator_Symbol,                        // 4.1
  A_Character_Literal,                       // 4.1
  An_Enumeration_Literal,                    // 4.1
  An_Explicit_Dereference,                   // 4.1
  A_Function_Call,                           // 4.1

  An_Indexed_Component,                      // 4.1.1
  A_Slice,                                   // 4.1.2
  A_Selected_Component,                      // 4.1.3
  An_Attribute_Reference,                    // 4.1.4  -> Attribute_Kinds

  A_Record_Aggregate,                        // 4.3
  An_Extension_Aggregate,                    // 4.3
  A_Positional_Array_Aggregate,              // 4.3
  A_Named_Array_Aggregate,                   // 4.3

  An_And_Then_Short_Circuit,                 // 4.4
  An_Or_Else_Short_Circuit,                  // 4.4

  An_In_Membership_Test,                     // 4.4  Ada 2012
  A_Not_In_Membership_Test,                  // 4.4  Ada 2012

  A_Null_Literal,                            // 4.4
  A_Parenthesized_Expression,                // 4.4
  A_Raise_Expression,                        // 4.4 Ada 2012 (AI12-0022-1)

  A_Type_Conversion,                         // 4.6
  A_Qualified_Expression,                    // 4.7

  An_Allocation_From_Subtype,                // 4.8
  An_Allocation_From_Qualified_Expression,   // 4.8
  A_Case_Expression,                         // Ada 2012
  An_If_Expression,                          // Ada 2012
  A_For_All_Quantified_Expression,           // Ada 2012
  A_For_Some_Quantified_Expression           // Ada 2012
};

enum Association_Kinds {
  Not_An_Association,                    // An unexpected element

  A_Pragma_Argument_Association,         // 2.8
  A_Discriminant_Association,            // 3.7.1
  A_Record_Component_Association,        // 4.3.1
  An_Array_Component_Association,        // 4.3.3
  A_Parameter_Association,               // 6.4
  A_Generic_Association                  // 12.3
};

enum Statement_Kinds {
  Not_A_Statement,                     // An unexpected element

  A_Null_Statement,                    // 5.1
  An_Assignment_Statement,             // 5.2
  An_If_Statement,                     // 5.3
  A_Case_Statement,                    // 5.4

  A_Loop_Statement,                    // 5.5
  A_While_Loop_Statement,              // 5.5
  A_For_Loop_Statement,                // 5.5
  A_Block_Statement,                   // 5.6
  An_Exit_Statement,                   // 5.7
  A_Goto_Statement,                    // 5.8

  A_Procedure_Call_Statement,          // 6.4
  A_Return_Statement,                  // 6.5
  //  //|A2005 start
  An_Extended_Return_Statement,        // 6.5
  //  //|A2005 end

  An_Accept_Statement,                 // 9.5.2
  An_Entry_Call_Statement,             // 9.5.3

  A_Requeue_Statement,                 // 9.5.4
  A_Requeue_Statement_With_Abort,      // 9.5.4

  A_Delay_Until_Statement,             // 9.6
  A_Delay_Relative_Statement,          // 9.6

  A_Terminate_Alternative_Statement,   // 9.7.1
  A_Selective_Accept_Statement,        // 9.7.1
  A_Timed_Entry_Call_Statement,        // 9.7.2
  A_Conditional_Entry_Call_Statement,  // 9.7.3
  An_Asynchronous_Select_Statement,    // 9.7.4

  An_Abort_Statement,                  // 9.8
  A_Raise_Statement,                   // 11.3
  A_Code_Statement                     // 13.8
};

enum Path_Kinds {
  Not_A_Path,
  //  An unexpected element

  //  Statement paths:
  An_If_Path,
  //  5.3:
  //  if condition then
  //    sequence_of_statements

  An_Elsif_Path,
  //  5.3:
  //  elsif condition then
  //    sequence_of_statements

  An_Else_Path,
  //  5.3, 9.7.1, 9.7.3:
  //  else sequence_of_statements

  A_Case_Path,
  //  5.4:
  //  when discrete_choice_list =>
  //    sequence_of_statements

  A_Select_Path,
  //  9.7.1:
  //     select [guard] select_alternative
  //  9.7.2, 9.7.3:
  //     select entry_call_alternative
  //  9.7.4:
  //     select triggering_alternative

  An_Or_Path,
  //  9.7.1:
  //     or [guard] select_alternative
  //  9.7.2:
  //     or delay_alternative

  A_Then_Abort_Path,
  //  9.7.4
  //     then abort sequence_of_statements

  //  //|A2012 start
  //  Expression paths:
  A_Case_Expression_Path,
  //  ??? (RM 2012)
  //  when expression => expression

  An_If_Expression_Path,
  //  ??? (RM 2012)
  //  if condition then expression

  An_Elsif_Expression_Path,
  //  ??? (RM 2012)
  //  elsif condition then expression

  An_Else_Expression_Path
  //  ??? (RM 2012)
  //  else expression
};

enum Clause_Kinds {
  Not_A_Clause,                 // An unexpected element

  A_Use_Package_Clause,         // 8.4
  A_Use_Type_Clause,            // 8.4
  A_Use_All_Type_Clause,        // 8.4, Ada 2012

  A_With_Clause,                // 10.1.2

  A_Representation_Clause,      // 13.1     -> Representation_Clause_Kinds
  A_Component_Clause            // 13.5.1
};

// There is no enum Exception_Handler_Kinds because there is only one kind of 
// exception handler.

///////////////////////////////////////////////////////////////////////////////
// END element kind kinds
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN supporting kinds
///////////////////////////////////////////////////////////////////////////////

// For Defining_Operator_Symbol:
// For Expression:
  enum Operator_Kinds {
    Not_An_Operator,                   // An unexpected element

    An_And_Operator,                   // and
    An_Or_Operator,                    // or
    An_Xor_Operator,                   // xor
    An_Equal_Operator,                 // =
    A_Not_Equal_Operator,              // /=
    A_Less_Than_Operator,              // <
    A_Less_Than_Or_Equal_Operator,     // <=
    A_Greater_Than_Operator,           // >
    A_Greater_Than_Or_Equal_Operator,  // >=
    A_Plus_Operator,                   // +
    A_Minus_Operator,                  // -
    A_Concatenate_Operator,            // &
    A_Unary_Plus_Operator,             // +
    A_Unary_Minus_Operator,            // -
    A_Multiply_Operator,               // *
    A_Divide_Operator,                 // /
    A_Mod_Operator,                    // mod
    A_Rem_Operator,                    // rem
    An_Exponentiate_Operator,          // **
    An_Abs_Operator,                   // abs
    A_Not_Operator                     // not
};

// For Declaration:
enum Declaration_Origins {
  Not_A_Declaration_Origin,
  //  An unexpected element

  An_Explicit_Declaration,
  //  3.1(5) explicitly declared in the text of a program, or within
  //  an expanded generic template

  An_Implicit_Predefined_Declaration,
  //  3.1(5), 3.2.3(1), A.1(2)

  An_Implicit_Inherited_Declaration
  //  3.1(5), 3.4(6-35)
};

// For Declaration:
enum Mode_Kinds {
  Not_A_Mode,              // An unexpected element

  A_Default_In_Mode,       // procedure A(B :        C);
  An_In_Mode,              // procedure A(B : IN     C);
  An_Out_Mode,             // procedure A(B :    OUT C);
  An_In_Out_Mode           // procedure A(B : IN OUT C);
};
  
// For Declaration:
enum Subprogram_Default_Kinds {
      Not_A_Default,         // An unexpected element

      A_Name_Default,        // with subprogram_specification is default_name;
      A_Box_Default,         // with subprogram_specification is <>;
//  //|A2005 start
      A_Null_Default,        // with subprogram_specification is null;
//  //|A2005 end
      A_Nil_Default          // with subprogram_specification;
};

// For Declaration, Definition, Clause:
enum Trait_Kinds {
  Not_A_Trait,                         // An unexpected element

  An_Ordinary_Trait,
  //  The declaration or definition does not contain the reserved words
  //  "aliased", "reverse", "private", "limited", "abstract", or "access"
  //  in an access_definition

  An_Aliased_Trait,
  //  "aliased" is present

  An_Access_Definition_Trait,
  //  "access" in an access_definition is present
  //  //|A2005 start
  A_Null_Exclusion_Trait,
  //  "not null" is present
  //  //|A2005 end
  A_Reverse_Trait,
  //  "reverse" is present
  A_Private_Trait,
  //  Only "private" is present
  A_Limited_Trait,
  //  Only "limited" is present
  A_Limited_Private_Trait,
  //  "limited" and "private" are present
  An_Abstract_Trait,
  //  Only "abstract" is present
  An_Abstract_Private_Trait,
  //  "abstract" and "private" are present
  An_Abstract_Limited_Trait,
  //  "abstract" and "limited" are present
  An_Abstract_Limited_Private_Trait
  //  "abstract", "limited", and "private" are present

  //  //|D2005 start
  //  We need a note saying that An_Access_Definition_Trait is an obsolescent
  //  value kept only because of upward compatibility reasons. Now an
  //  access_definition that defines an anonymous access kind is represented as
  //  a first-class citizen in the ASIS Element classification hierarchy
  //  (An_Access_Definition value in Definition_Kinds and the subordinate
  //  Access_Definition_Kinds type).
  //  //|D2005 end
};

// For Expression:
enum Attribute_Kinds {
  Not_An_Attribute,              // An unexpected element

  An_Access_Attribute,           // 3.10.2(24), 3.10.2(32), K(2), K(4)
  An_Address_Attribute,          // 13.3(11), J.7.1(5), K(6)
  An_Adjacent_Attribute,         // A.5.3(48), K(8)
  An_Aft_Attribute,              // 3.5.10(5), K(12)
  An_Alignment_Attribute,        // 13.3(23), K(14)
  A_Base_Attribute,              // 3.5(15), K(17)
  A_Bit_Order_Attribute,         // 13.5.3(4), K(19)
  A_Body_Version_Attribute,      // E.3(4), K(21)
  A_Callable_Attribute,          // 9.9(2), K(23)
  A_Caller_Attribute,            // C.7.1(14), K(25)
  A_Ceiling_Attribute,           // A.5.3(33), K(27)
  A_Class_Attribute,             // 3.9(14), 7.3.1(9), K(31), K(34)
  A_Component_Size_Attribute,    // 13.3(69), K(36)
  A_Compose_Attribute,           // A.5.3(24), K(38)
  A_Constrained_Attribute,       // 3.7.2(3), J.4(2), K(42)
  A_Copy_Sign_Attribute,         // A.5.3(51), K(44)
  A_Count_Attribute,             // 9.9(5), K(48)
  A_Definite_Attribute,          // 12.5.1(23), K(50)
  A_Delta_Attribute,             // 3.5.10(3), K(52)
  A_Denorm_Attribute,            // A.5.3(9), K(54)
  A_Digits_Attribute,            // 3.5.8(2), 3.5.10(7), K(56), K(58)
  An_Exponent_Attribute,         // A.5.3(18), K(60)
  An_External_Tag_Attribute,     // 13.3(75), K(64)
  A_First_Attribute,             // 3.5(12), 3.6.2(3), K(68), K(70)
  A_First_Bit_Attribute,         // 13.5.2(3), K(72)
  A_Floor_Attribute,             // A.5.3(30), K(74)
  A_Fore_Attribute,              // 3.5.10(4), K(78)
  A_Fraction_Attribute,          // A.5.3(21), K(80)
  An_Identity_Attribute,         // 11.4.1(9), C.7.1(12), K(84), K(86)
  An_Image_Attribute,            // 3.5(35), K(88)
  An_Input_Attribute,            // 13.13.2(22), 13.13.2(32), K(92), K(96)
  A_Last_Attribute,              // 3.5(13), 3.6.2(5), K(102), K(104)
  A_Last_Bit_Attribute,          // 13.5.2(4), K(106)
  A_Leading_Part_Attribute,      // A.5.3(54), K(108)
  A_Length_Attribute,            // 3.6.2(9), K(117)
  A_Machine_Attribute,           // A.5.3(60), K(119)
  A_Machine_Emax_Attribute,      // A.5.3(8), K(123)
  A_Machine_Emin_Attribute,      // A.5.3(7), K(125)
  A_Machine_Mantissa_Attribute,  // A.5.3(6), K(127)
  A_Machine_Overflows_Attribute, // A.5.3(12), A.5.4(4), K(129), K(131)
  A_Machine_Radix_Attribute,     // A.5.3(2), A.5.4(2), K(133), K(135)
  A_Machine_Rounds_Attribute,    // A.5.3(11), A.5.4(3), K(137), K(139)
  A_Max_Attribute,               // 3.5(19), K(141)
  A_Max_Size_In_Storage_Elements_Attribute, //   13.11.1(3), K(145)
  A_Min_Attribute,               // 3.5(16), K(147)
  A_Model_Attribute,             // A.5.3(68), G.2.2(7), K(151)
  A_Model_Emin_Attribute,        // A.5.3(65), G.2.2(4), K(155)
  A_Model_Epsilon_Attribute,     // A.5.3(66), K(157)
  A_Model_Mantissa_Attribute,    // A.5.3(64), G.2.2(3), K(159)
  A_Model_Small_Attribute,       // A.5.3(67), K(161)
  A_Modulus_Attribute,           // 3.5.4(17), K(163)
  An_Output_Attribute,           // 13.13.2(19), 13.13.2(29), K(165), K(169)
  A_Partition_ID_Attribute,      // E.1(9), K(173)
  A_Pos_Attribute,               // 3.5.5(2), K(175)
  A_Position_Attribute,          // 13.5.2(2), K(179)
  A_Pred_Attribute,              // 3.5(25), K(181)
  A_Range_Attribute,             // 3.5(14), 3.6.2(7), K(187), ú(189)
  A_Read_Attribute,              // 13.13.2(6), 13.13.2(14), K(191), K(195)
  A_Remainder_Attribute,         // A.5.3(45), K(199)
  A_Round_Attribute,             // 3.5.10(12), K(203)
  A_Rounding_Attribute,          // A.5.3(36), K(207)
  A_Safe_First_Attribute,        // A.5.3(71), G.2.2(5), K(211)
  A_Safe_Last_Attribute,         // A.5.3(72), G.2.2(6), K(213)
  A_Scale_Attribute,             // 3.5.10(11), K(215)
  A_Scaling_Attribute,           // A.5.3(27), K(217)
  A_Signed_Zeros_Attribute,      // A.5.3(13), K(221)
  A_Size_Attribute,              // 13.3(40), 13.3(45), K(223), K(228)
  A_Small_Attribute,             // 3.5.10(2), K(230)
  A_Storage_Pool_Attribute,      // 13.11(13), K(232)
  A_Storage_Size_Attribute,      // 13.3(60), 13.11(14), J.9(2), K(234),
  //                                 K(236)
  A_Succ_Attribute,              // 3.5(22), K(238)
  A_Tag_Attribute,               // 3.9(16), 3.9(18), K(242), K(244)
  A_Terminated_Attribute,        // 9.9(3), K(246)
  A_Truncation_Attribute,        // A.5.3(42), K(248)
  An_Unbiased_Rounding_Attribute, // A.5.3(39), K(252)
  An_Unchecked_Access_Attribute,  // 13.10(3), H.4(18), K(256)
  A_Val_Attribute,                // 3.5.5(5), K(258)
  A_Valid_Attribute,              // 13.9.2(3), H(6), K(262)
  A_Value_Attribute,              // 3.5(52), K(264)
  A_Version_Attribute,            // E.3(3), K(268)
  A_Wide_Image_Attribute,         // 3.5(28), K(270)
  A_Wide_Value_Attribute,         // 3.5(40), K(274)
  A_Wide_Width_Attribute,         // 3.5(38), K(278)
  A_Width_Attribute,              // 3.5(39), K(280)
  A_Write_Attribute,              // 13.13.2(3), 13.13.2(11), K(282), K(286)

  //  |A2005 start
  //  New Ada 2005 attributes. To be alphabetically ordered later
  A_Machine_Rounding_Attribute,
  A_Mod_Attribute,
  A_Priority_Attribute,
  A_Stream_Size_Attribute,
  A_Wide_Wide_Image_Attribute,
  A_Wide_Wide_Value_Attribute,
  A_Wide_Wide_Width_Attribute,
  //  |A2005 end

  //  |A2012 start
  //  New Ada 2012 attributes. To be alphabetically ordered later
  A_Max_Alignment_For_Allocation_Attribute,
  An_Overlaps_Storage_Attribute,
  //  |A2012 end

  An_Implementation_Defined_Attribute,  // Reference Manual, Annex M
  An_Unknown_Attribute           // Unknown to ASIS
};

// For Element_Struct:
enum Enclosing_Kinds { // Not an ASIS type
  Not_Enclosing,
  Enclosing_Element,
  Enclosing_Unit 
};

///////////////////////////////////////////////////////////////////////////////
// END supporting kinds
///////////////////////////////////////////////////////////////////////////////

// May take ??*4 bytes (unfinished):
struct Pragma_Struct {
  enum Pragma_Kinds Pragma_Kind;
};

// May take 3*4 bytes - 2 enums, 1 char*:
struct Defining_Name_Struct {
  enum Defining_Name_Kinds  Defining_Name_Kind;
  char                     *Defining_Name_Image;
  
  // These fields are only valid for the kinds above them:
  // A_Defining_Character_Literal
  // A_Defining_Enumeration_Literal
  char                     *Position_Number_Image;
  char                     *Representation_Value_Image;
  // A_Defining_Expanded_Name 
  Name_ID                   Defining_Prefix;
  Defining_Name_ID          Defining_Selector;
  // When this is the name of a constant or a deferred constant:
  Declaration_ID            Corresponding_Constant_Declaration;
  // A_Defining_Operator_Symbol:
  enum Operator_Kinds       Operator_Kind;
  };

// May take 5*4 bytes - 5 enums:
struct Declaration_Struct {
  enum Declaration_Kinds   Declaration_Kind;
  enum Declaration_Origins Declaration_Origin;
  
  // These fields are only valid for the kinds above them:
  // A_Parameter_Specification |
  // A_Formal_Object_Declaration:
  enum Mode_Kinds          Mode_Kind;
  // A_Formal_Function_Declaration |
  // A_Formal_Procedure_Declaration:
  enum Subprogram_Default_Kinds 
                           Default_Kind;
  // A_Private_Type_Declaration |
  // A_Private_Extension_Declaration |
  // A_Variable_Declaration |
  // A_Constant_Declaration |
  // A_Deferred_Constant_Declaration |
  // A_Discriminant_Specification |
  // A_Loop_Parameter_Specification |
  // A_Generalized_Iterator_Specification |
  // An_Element_Iterator_Specification |
  // A_Procedure_Declaration |
  // A_Function_Declaration:
  enum Trait_Kinds         Trait_Kind;
  
  // TODO: add remaining valid kinds comments:
  Defining_Name_List             Names;
  Definition_ID                  Discriminant_Part;
  Definition_ID                  Type_Declaration_View;
  Definition_ID                  Object_Declaration_View;
  Element_List                   Aspect_Specifications;
  Expression_ID                  Initialization_Expression;
  Declaration_ID                 Corresponding_Type_Declaration;
  Declaration_ID                 Corresponding_Type_Completion;
  Declaration_ID                 Corresponding_Type_Partial_View;
  Declaration_ID                 Corresponding_First_Subtype;
  Declaration_ID                 Corresponding_Last_Constraint;
  Declaration_ID                 Corresponding_Last_Subtype;
  Representation_Clause_List     Corresponding_Representation_Clauses;
  Discrete_Subtype_Definition_ID Specification_Subtype_Definition;
  Element_ID                     Iteration_Scheme_Name;
  Element_ID                     Subtype_Indication;
  Parameter_Specification_List   Parameter_Profile;
  Element_ID                     Result_Profile;
  Expression_ID                  Result_Expression;
  bool                           Is_Overriding_Declaration;
  bool                           Is_Not_Overriding_Declaration;
  Element_List                   Body_Declarative_Items;
  Statement_List                 Body_Statements;
  Exception_Handler_List         Body_Exception_Handlers;
  Declaration_ID                 Body_Block_Statement;
  bool                           Is_Name_Repeated;
  Declaration_ID                 Corresponding_Declaration;
  Declaration_ID                 Corresponding_Body;
  Declaration_ID                 Corresponding_Subprogram_Derivation;
  Type_Definition_ID             Corresponding_Type;
  Declaration_ID                 Corresponding_Equality_Operator;
  Declarative_Item_List          Visible_Part_Declarative_Items;
  bool                           Is_Private_Present;
  Declarative_Item_List	         Private_Part_Declarative_Items;
  Expression_List                Declaration_Interface_List;
  Expression_ID                  Renamed_Entity;
  Expression_ID                  Corresponding_Base_Entity;
  Declaration_List               Protected_Operation_Items;
  Discrete_Subtype_Definition_ID Entry_Family_Definition;
  Declaration_ID                 Entry_Index_Specification;
  Expression_ID                  Entry_Barrier;
  Declaration_ID                 Corresponding_Subunit;
  bool                           Is_Subunit;
  Declaration_ID                 Corresponding_Body_Stub;
  Element_List                   Generic_Formal_Part;
  Expression_ID                  Generic_Unit_Name;
  Association_List               Generic_Actual_Part;
  Expression_ID                  Formal_Subprogram_Default;
  Defining_Name_ID               Corresponding_Generic_Element;
  bool                           Is_Dispatching_Operation;
};

// May take ??*4 bytes (unfinished):
struct Definition_Struct {
  enum Definition_Kinds Definition_Kind;
  
  // These fields are only valid for the kinds above them:
  // A_Component_Definition
  // A_Private_Type_Definition
  // A_Tagged_Private_Type_Definition
  // A_Private_Extension_Definition
  // A_Subtype_Indication
  // An_Access_Definition
  enum Trait_Kinds      Trait_Kind;
  // A_Type_Definition
  enum Type_Kinds       Type_Kind;
  // A_Derived_Type_Definition
  // A_Derived_Record_Extension_Definition
  Subtype_Indication_ID Parent_Subtype_Indication;
  // A_Derived_Record_Extension_Definition
  Definition_ID         Record_Definition;
  // A_Derived_Type_Definition
  // A_Derived_Record_Extension_Definition
  Definition_ID_List    Implicit_Inherited_Declarations;
  Definition_ID_List    Implicit_Inherited_Subprograms;
  Definition_ID         Corresponding_Parent_Subtype;
  Definition_ID         Corresponding_Root_Type;
  Definition_ID         Corresponding_Type_Structure;
  // A_Constraint
  enum Constraint_Kinds Constraint_Kind;
  // A_Simple_Expression_Range
  Expression_ID         Lower_Bound;
  Expression_ID         Upper_Bound;
  // A_Subtype_Indication
  // A_Discrete_Subtype_Definition (See Discrete_Range_Kinds)
  // A_Discrete_Range (See Discrete_Range_Kinds)
  Expression_ID         Subtype_Mark;
  Constraint_ID         Subtype_Constraint;
  // A_Component_Definition
  Subtype_Indication_ID Component_Subtype_Indication;
  Definition_ID         Component_Definition_View;
  
  // A_Record_Definition
  // A_Variant
  Record_Component_List Record_Components;
  Record_Component_List Implicit_Components;
  
  Declarative_Item_ID_List Visible_Part_Items;
  Declarative_Item_ID_List Private_Part_Items;
  bool                     Is_Private_Present;
  // TODO: not done yet
};

// May take 33*4 bytes - 19 IDs, 8 Lists, 1 bool, 3 enums, 2 char*:
struct Expression_Struct {
  enum Expression_Kinds Expression_Kind;
  Declaration_ID        Corresponding_Expression_Type;
  
  // These fields are only valid for the kinds above them:  
  // An_Integer_Literal,                        // 2.4
  // A_Real_Literal,                            // 2.4.1
  // A_String_Literal,                          // 2.6
  char                 *Value_Image;  
  // An_Identifier |                              // 4.1
  // An_Operator_Symbol |                         // 4.1
  // A_Character_Literal |                        // 4.1
  // An_Enumeration_Literal:
  char                 *Name_Image;
  Defining_Name_ID      Corresponding_Name_Definition;
  Defining_Name_List    Corresponding_Name_Definition_List; // Only >1 if the expression in a pragma is ambiguous
  Element_ID            Corresponding_Name_Declaration; // Decl or stmt
  // An_Operator_Symbol:
  enum Operator_Kinds   Operator_Kind;
  // An_Explicit_Dereference =>                   // 4.1
  // A_Function_Call =>                           // 4.1
  // An_Indexed_Component =>                      // 4.1.1
  // A_Slice =>                                   // 4.1.2
  // A_Selected_Component =>                      // 4.1.3
  // An_Attribute_Reference =>                    // 4.1.4
  Expression_ID         Prefix;
  // A_Function_Call =>                           // 4.1 
  // An_Indexed_Component (Is_Generalized_Indexing == true) //ASIS 2012 // 4.1.1
  Declaration_ID        Corresponding_Called_Function;
  // A_Function_Call =>                           // 4.1
  bool                  Is_Prefix_Call;
  Element_List          Function_Call_Parameters;
  // An_Indexed_Component =>                      // 4.1.1
  Expression_List       Index_Expressions;
  bool                  Is_Generalized_Indexing;
  // A_Slice =>                                   // 4.1.2
  Discrete_Range_ID     Slice_Range;
  // A_Selected_Component =>                      // 4.1.3
  Expression_ID         Selector;
  // An_Attribute_Reference :
  enum Attribute_Kinds  atribute_kind;
  Expression_ID         Attribute_Designator_Identifier;
  Expression_List       Attribute_Designator_Expressions;
  // A_Record_Aggregate =>                        // 4.3
  // An_Extension_Aggregate =>                    // 4.3
  Association_List      Record_Component_Associations;
  // An_Extension_Aggregate =>                    // 4.3
  Expression_ID         Extension_Aggregate_Expression;
  // A_Positional_Array_Aggregate |               // 4.3
  // A_Named_Array_Aggregate =>                   // 4.3  
  Association_List      Array_Component_Associations;
  // An_And_Then_Short_Circuit |                  // 4.4
  // An_Or_Else_Short_Circuit =>                  // 4.4
  Expression_ID         Short_Circuit_Operation_Left_Expression;
  Expression_ID         Short_Circuit_Operation_Right_Expression;
  // An_In_Membership_Test |                      // 4.4  Ada 2012
  // A_Not_In_Membership_Test =>                  // 4.4  Ada 2012
  Expression_ID         Membership_Test_Expression;
  Element_List          Membership_Test_Choices;
  // A_Parenthesized_Expression =>                // 4.4
  Expression_ID         Expression_Parenthesized;
  // A_Type_Conversion =>                         // 4.6
  // A_Qualified_Expression =>                    // 4.7
  Expression_ID         Converted_Or_Qualified_Subtype_Mark;
  Expression_ID         Converted_Or_Qualified_Expression;
  Expression_ID         Predicate;
  // An_Allocation_From_Subtype =>                // 4.8
  // An_Allocation_From_Qualified_Expression =>   // 4.8
  Expression_ID         Subpool_Name;
  // An_Allocation_From_Subtype =>                // 4.8
  Subtype_Indication_ID Allocator_Subtype_Indication;
  // An_Allocation_From_Qualified_Expression =>   // 4.8
  Expression_ID         Allocator_Qualified_Expression;
  // A_Case_Expression |                          // Ada 2012
  // An_If_Expression =>                          // Ada 2012
  Expression_Path_List  Expression_Paths;
  // A_For_All_Quantified_Expression |            // Ada 2012
  // A_For_Some_Quantified_Expression =>          // Ada 2012
  Declaration_ID        Iterator_Specification;
};

// May take ??*4 bytes (unfinished):
struct Association_Struct {
  enum Association_Kinds Association_Kind;
  // These fields are only valid for the kinds above them:  
  // An_Array_Component_Association,        // 4.3.3
  Expression_List        Array_Component_Choices;
  // A_Record_Component_Association,        // 4.3.1
  Expression_List        Record_Component_Choices;
  // An_Array_Component_Association,        // 4.3.3
  // A_Record_Component_Association,        // 4.3.1
  Expression_ID          Component_Expression;
  // A_Pragma_Argument_Association,         // 2.8
  // A_Parameter_Association,               // 6.4
  // A_Generic_Association                  // 12.3
  Expression_ID          Formal_Parameter;
  Expression_ID          Actual_Parameter;
  // A_Discriminant_Association,            // 3.7.1  
  Expression_List        Discriminant_Selector_Names;
  Expression_ID          Discriminant_Expression;
  // A_Discriminant_Association,            // 3.7.1
  // A_Record_Component_Association,        // 4.3.1
  // A_Parameter_Association,               // 6.4
  // A_Generic_Association                  // 12.3
  bool                   Is_Normalized;
  // A_Parameter_Association
  // A_Generic_Association
  //  //|A2005 start
  // A_Record_Component_Association
  //  //|A2005 end
  bool                   Is_Defaulted_Association;
};

// May take 37*4 bytes - 22 IDs, 12 Lists, 2 bools, and 1 enum:
struct Statement_Struct {
  enum Statement_Kinds   Statement_Kind;
  Defining_Name_List     Label_Names;
  
  // These fields are only valid for the kinds above them:  
  //   An_Assignment_Statement,             // 5.2
  Expression_ID          Assignment_Variable_Name;
  Expression_ID          Assignment_Expression;
  //   An_If_Statement,                     // 5.3
  //   A_Case_Statement,                    // 5.4
  //   A_Selective_Accept_Statement,        // 9.7.1
  //   A_Timed_Entry_Call_Statement,        // 9.7.2
  //   A_Conditional_Entry_Call_Statement,  // 9.7.3
  //   An_Asynchronous_Select_Statement,    // 9.7.4
  Path_List              Statement_Paths;
  //   A_Case_Statement,                    // 5.4
  Expression_ID          Case_Expression;
  //   A_Loop_Statement,                    // 5.5
  //   A_While_Loop_Statement,              // 5.5
  //   A_For_Loop_Statement,                // 5.5
  //   A_Block_Statement,                   // 5.6
  Defining_Name_ID       Statement_Identifier;
  //   A_Loop_Statement,                    // 5.5
  //   A_Block_Statement,                   // 5.6
  //   An_Accept_Statement,                 // 9.5.2
  bool                   Is_Name_Repeated;
  //   A_While_Loop_Statement,              // 5.5
  Expression_ID          While_Condition;
  //   A_For_Loop_Statement,                // 5.5
  Declaration_ID         For_Loop_Parameter_Specification;
  //   A_Loop_Statement,                    // 5.5
  //   A_While_Loop_Statement,              // 5.5
  //   A_For_Loop_Statement,                // 5.5
  Statement_List         Loop_Statements;
  //   A_Block_Statement,                   // 5.6
  bool                   Is_Declare_Block;
  Declarative_Item_List  Block_Declarative_Items;
  Statement_List         Block_Statements;
  Exception_Handler_List Block_Exception_Handlers;
  //   An_Exit_Statement,                   // 5.7
  Expression_ID          Exit_Loop_Name;
  Expression_ID          Exit_Condition;
  Expression_ID          Corresponding_Loop_Exited;
  //   A_Goto_Statement,                    // 5.8
  Expression_ID          Goto_Label;
  Statement_ID           Corresponding_Destination_Statement;
  //   A_Procedure_Call_Statement,          // 6.4
  //   An_Entry_Call_Statement,             // 9.5.3
  Expression_ID          Called_Name;
  Declaration_ID         Corresponding_Called_Entity;
  Association_List       Call_Statement_Parameters;
  //   A_Return_Statement,                  // 6.5
  Expression_ID          Return_Expression;
  //   //  //|A2005 start
  //   An_Extended_Return_Statement,        // 6.5
  Declaration_ID         Return_Object_Declaration;
  Statement_List         Extended_Return_Statements;
  Exception_Handler_List Extended_Return_Exception_Handlers;
  //   //  //|A2005 end
  //   An_Accept_Statement,                 // 9.5.2
  Expression_ID          Accept_Entry_Index;
  Name_ID                Accept_Entry_Direct_Name;
  Parameter_Specification_List 
                         Accept_Parameters;
  Statement_List         Accept_Body_Statements;
  Statement_List         Accept_Body_Exception_Handlers;
  Declaration_ID         Corresponding_Entry;
  //   A_Requeue_Statement,                 // 9.5.4
  //   A_Requeue_Statement_With_Abort,      // 9.5.4
  Name_ID                Requeue_Entry_Name;
  //   A_Delay_Until_Statement,             // 9.6
  //   A_Delay_Relative_Statement,          // 9.6
  Expression_ID          Delay_Expression;
  //   An_Abort_Statement,                  // 9.8
  Expression_List        Aborted_Tasks;
  //   A_Raise_Statement,                   // 11.3
  Expression_ID          Raised_Exception;
  Expression_ID          Associated_Message;
  //   A_Code_Statement                     // 13.8
  Expression_ID          Qualified_Expression;
};

// May take ??*4 bytes (unfinished):
struct Path_Struct {
  enum Path_Kinds Path_Kind;
  Statement_List Sequence_Of_Statements;
  Expression_ID  Condition_Expression;
  Element_List   Case_Path_Alternative_Choices;
  Expression_ID  Guard;
};

// May take 3*4 bytes - 1 enum, 1 List:
struct Clause_Struct {
  enum Clause_Kinds Clause_Kind;
  // These fields are only valid for the kinds above them:
  //   A_Use_Package_Clause
  //   A_Use_Type_Clause
  //   A_Use_All_Type_Clause
  //   A_With_Clause
  Name_List         Clause_Names;
  //   A_With_Clause
  enum Trait_Kinds  Trait_Kind;
  // TODO: Incomplete
};

// May take 5*4 bytes - 1 ID, 2 List:
struct Exception_Handler_Struct {
  Declaration_ID Choice_Parameter_Specification;
  Element_List   Exception_Choices;
  Statement_List Handler_Statements;
};

// May take 37*4 bytes (Statement_Struct, the largest component):
union Element_Union {
  int                             Dummy_Member; // For Ada default initialization
  struct Pragma_Struct            The_Pragma; // pragma is an Ada reserverd word
  struct Defining_Name_Struct     Defining_Name;
  struct Declaration_Struct       Declaration;
  struct Definition_Struct        Definition;
  struct Expression_Struct        Expression;
  struct Association_Struct       Association;
  struct Statement_Struct         Statement;
  struct Path_Struct              Path;
  struct Clause_Struct            Clause;
  struct Exception_Handler_Struct Exception_Handler;
};

// May take 5*4 bytes - 1 char*, 4 int:
struct Source_Location_Struct {
  char *Unit_Name;
  int   First_Line;
  int   First_Column;
  int   Last_Line;
  int   Last_Column;
};

// May take 46*4 bytes - 2 IDs, 2 enums, 5*4 struct, 37*4 union:
struct Element_Struct {
  Element_ID                    ID;
  enum Element_Kinds            Element_Kind;
  Node_ID                       Enclosing_Element_ID;
  enum Enclosing_Kinds          Enclosing_Kind;
  struct Source_Location_Struct Source_Location;
  union Element_Union           The_Union;
};

///////////////////////////////////////////////////////////////////////////////
// END element 
///////////////////////////////////////////////////////////////////////////////

// May take  44*4 bytes (Element_Struct, the largest component):
union Node_Union {
  int                   Dummy_Member; // For Ada default initialization
  struct Context_Struct Context;
  struct Unit_Struct    Unit;
  struct Element_Struct Element;
};

// May take 45*4 bytes - a 44*4 Node_Union, 1 enum:
struct Node_Struct {
  enum Node_Kinds  Node_Kind;
  union Node_Union The_Union;
};

// May take 47*4 bytes - 45*4 Node_Struct, 1 ptr, 1 int:
struct List_Node_Struct {
  struct Node_Struct       Node;
  struct List_Node_Struct *Next;
  // Number of nodes in next :
  int                      Next_Count;
};

typedef struct List_Node_Struct *Node_List_Ptr;

#endif //ifndef A_NODES_H

