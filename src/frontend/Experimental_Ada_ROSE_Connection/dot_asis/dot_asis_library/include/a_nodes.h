#ifndef A_NODES_H
#define A_NODES_H

#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// NOTE: All of the enums below that duplicate ASIS enumerated types MUST match
// the ASIS types exactly, because we do unchecked conversions between them.
///////////////////////////////////////////////////////////////////////////////

typedef char* Program_Text;
typedef int   ASIS_Integer;

///////////////////////////////////////////////////////////////////////////////
// BEGIN element 
///////////////////////////////////////////////////////////////////////////////

// This is NOT a subset of Node_ID:
typedef int Element_ID;

// For this:
//   typedef Element_ID Element_ID_Array[];
//   typedef Element_ID_Array *Element_ID_Array_Ptr2;
// GNAT C-to-Ada translator produces (array is constrained!):
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
typedef struct _Element_ID_Array_Struct {
  int            Length;
  Element_ID_Ptr IDs;
} Element_ID_Array_Struct;

typedef Element_ID_Array_Struct Element_ID_List;

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
// BEGIN supporting kinds
///////////////////////////////////////////////////////////////////////////////

// These are needed in multiple structs below, so they are all 
// defined here:


typedef Element_ID      Access_Type_Definition;      
typedef Element_ID      Association;
typedef Element_ID_List Association_List;
typedef Element_ID      Case_Statement_Alternative;
typedef Element_ID      Clause;
typedef Element_ID      Component_Clause;
typedef Element_ID_List Component_Clause_List;
typedef Element_ID      Component_Declaration;
typedef Element_ID      Component_Definition;
typedef Element_ID      Constraint_ID;
typedef Element_ID      Constraint;
typedef Element_ID      Context_Clause;
typedef Element_ID_List Context_Clause_List;
typedef Element_ID      Declaration;
typedef Element_ID      Declaration_ID;
typedef Element_ID_List Declaration_List;
typedef Element_ID_List Declarative_Item_List;
typedef Element_ID      Defining_Name_ID;
typedef Element_ID      Definition;
typedef Element_ID      Definition_ID;
typedef Element_ID_List Definition_List;
typedef Element_ID      Discrete_Range;
typedef Element_ID      Discrete_Range_ID;
typedef Element_ID_List Discrete_Range_List;
typedef Element_ID      Discrete_Subtype_Definition;
typedef Element_ID      Discrete_Subtype_Definition_ID;
typedef Element_ID      Discriminant_Association;
typedef Element_ID_List Discriminant_Association_List;
typedef Element_ID_List Discriminant_Specification_List;
typedef Element_ID      Defining_Name;
typedef Element_ID_List Defining_Name_List;
typedef Element_ID      Exception_Handler;
typedef Element_ID_List Exception_Handler_List;
typedef Element_ID      Expression;
typedef Element_ID      Expression_ID;
typedef Element_ID_List Expression_List;
typedef Element_ID_List Expression_Path_List;
typedef Element_ID      Formal_Type_Definition;
typedef Element_ID      Generic_Formal_Parameter;
typedef Element_ID_List Generic_Formal_Parameter_List;
typedef Element_ID      Identifier;
typedef Element_ID_List Identifier_List;
typedef Element_ID      Name;
typedef Element_ID      Name_ID;
typedef Element_ID_List Name_List;
typedef Element_ID      Parameter_Specification;
typedef Element_ID_List Parameter_Specification_List;
typedef Element_ID      Path;
typedef Element_ID_List Path_List;
typedef Element_ID      Pragma_Element;
typedef Element_ID_List Pragma_Element_ID_List;
typedef Element_ID      Range_Constraint;
typedef Element_ID      Record_Component;
typedef Element_ID_List Record_Component_List;
typedef Element_ID      Record_Definition;
typedef Element_ID      Representation_Clause;
typedef Element_ID_List Representation_Clause_List;
typedef Element_ID      Root_Type_Definition;
typedef Element_ID      Select_Alternative;
typedef Element_ID      Statement;
typedef Element_ID      Statement_ID;
typedef Element_ID_List Statement_List;
typedef Element_ID      Subtype_Indication;
typedef Element_ID      Subtype_Indication_ID;
typedef Element_ID      Subtype_Mark;
typedef Element_ID      Type_Definition;
typedef Element_ID      Type_Definition_ID;
typedef Element_ID      Variant;
typedef Element_ID_List Variant_Component_List;
typedef Element_ID_List Variant_List;

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

///////////////////////////////////////////////////////////////////////////////
// BEGIN Pragma
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
  An_Implementation_Defined_Pragma        // 2.8(14)
};

// May take ??*4 bytes:
struct Pragma_Struct {
  enum Pragma_Kinds   Pragma_Kind;
  Pragma_Element_ID_List Pragmas;
  Program_Text        Pragma_Name_Image;
  Association_List    Pragma_Argument_Associations;
};

///////////////////////////////////////////////////////////////////////////////
// END Pragma
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Defining_Name
///////////////////////////////////////////////////////////////////////////////

enum Defining_Name_Kinds {
  Not_A_Defining_Name,                       // An unexpected element
  A_Defining_Identifier,                     // 3.1(4)
  A_Defining_Character_Literal,              // 3.5.1(4)
  A_Defining_Enumeration_Literal,            // 3.5.1(3)
  A_Defining_Operator_Symbol,                // 6.1(9)
  A_Defining_Expanded_Name                   // 6.1(7)
};

// May take :
struct Defining_Name_Struct {
  enum Defining_Name_Kinds  Defining_Name_Kind;
  char                     *Defining_Name_Image;
  Name_List                 References;
  bool                      Is_Referenced;
  
  // These fields are only valid for the kinds above them:
  // A_Defining_Operator_Symbol:
  enum Operator_Kinds       Operator_Kind;
  // A_Defining_Character_Literal
  // A_Defining_Enumeration_Literal
  char                     *Position_Number_Image;
  char                     *Representation_Value_Image;
  // A_Defining_Expanded_Name 
  Name_ID                   Defining_Prefix;
  Defining_Name_ID          Defining_Selector;
  // When this is the name of a constant or a deferred constant:
  Declaration_ID            Corresponding_Constant_Declaration;
  // The defining name of an entity declared within the 
  // implicit specification of a generic instantiation:
  Defining_Name_ID          Corresponding_Generic_Element;
  };

///////////////////////////////////////////////////////////////////////////////
// END Defining_Name
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Declaration
///////////////////////////////////////////////////////////////////////////////

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

// May take :
struct Declaration_Struct {
  enum Declaration_Kinds         Declaration_Kind;
  enum Declaration_Origins       Declaration_Origin;
  Pragma_Element_ID_List            Corresponding_Pragmas;
  Defining_Name_List             Names;  
  Element_ID_List                   Aspect_Specifications;
  Representation_Clause_List     Corresponding_Representation_Clauses;
  
  // These fields are only valid for the kinds above them:
  //     A_Formal_Procedure_Declaration
  //     A_Formal_Function_Declaration
  //     A_Function_Declaration
  //     An_Ordinary_Type_Declaration
  //     A_Private_Type_Declaration
  //     A_Private_Extension_Declaration
  //     A_Procedure_Declaration
  bool                           Has_Abstract;
  //     A_Constant_Declaration
  //     A_Deferred_Constant_Declaration
  //     A_Return_Object_Declaration
  //     A_Variable_Declaration
  //     A_Component_Declaration
  //     A_Parameter_Specification  //  2012
  bool                           Has_Aliased;
  //     An_Ordinary_Type_Declaration
  //     A_Private_Type_Declaration
  //     A_Private_Extension_Declaration
  bool                           Has_Limited;
  //     A_Private_Extension_Declaration
  //     A_Private_Type_Declaration
  bool                           Has_Private;
  //     A_Protected_Body_Declaration
  //     A_Protected_Type_Declaration
  //     A_Single_Protected_Declaration
  //     A_Protected_Body_Stub
  bool                           Has_Protected;
  //     A_Loop_Parameter_Specification
  //     A_Generalized_Iterator_Specification
  //     An_Element_Iterator_Specification
  bool                           Has_Reverse;
  //     A_Task_Type_Declaration
  //     A_Single_Task_Declaration
  //     A_Task_Body_Declaration
  //     A_Task_Body_Stub
  bool                           Has_Task;
  //     A_Type_Definition
  //     An_Access_Definition
  //     A_Subtype_Indication
  bool                           Has_Null_Exclusion;
  //       A_Function_Declaration
  //       A_Function_Body_Declaration
  //       An_Expression_Function_Declaration
  //       A_Function_Renaming_Declaration
  //       A_Function_Body_Stub
  //       A_Generic_Function_Declaration
  //       A_Formal_Function_Declaration
  bool                           Is_Not_Null_Return;
  // A_Parameter_Specification |
  // A_Formal_Object_Declaration:
  enum Mode_Kinds          Mode_Kind;
  // A_Formal_Function_Declaration |
  // A_Formal_Procedure_Declaration:
  enum Subprogram_Default_Kinds  Default_Kind;
  //        A_Procedure_Body_Declaration    (pragmas from declarative region +
  //                                                      statements)
  //        A_Function_Body_Declaration     (pragmas from declarative region +
  //                                                      statements)
  //        A_Package_Declaration           (pragmas from visible + private
  //                                                      declarative regions)
  //        A_Package_Body_Declaration      (pragmas from declarative region +
  //                                                      statements)
  //        A_Task_Body_Declaration         (pragmas from declarative region +
  //                                                      statements)
  //        A_Protected_Body_Declaration    (pragmas from declarative region)
  //        An_Entry_Body_Declaration       (pragmas from declarative region +
  //                                                      statements)
  //        A_Generic_Procedure_Declaration (pragmas from formal declarative
  //                                                      region)
  //        A_Generic_Function_Declaration  (pragmas from formal declarative
  //                                                      region)
  //        A_Generic_Package_Declaration   (pragmas from formal + visible +
  //                                           private declarative regions)
  Pragma_Element_ID_List            Pragmas;
  //       A_Package_Declaration
  //       A_Package_Body_Declaration
  //       A_Procedure_Body_Declaration
  //       A_Function_Body_Declaration
  //       A_Generic_Package_Declaration
  //       A_Task_Type_Declaration
  //       A_Single_Task_Declaration
  //       A_Task_Body_Declaration
  //       A_Protected_Type_Declaration
  //       A_Single_Protected_Declaration
  //       A_Protected_Body_Declaration
  //       An_Entry_Body_Declaration
  Element_ID                     Corresponding_End_Name;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
  //  A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  //  A_Formal_Type_Declaration,                // 12.5(2)
  Definition_ID                  Discriminant_Part;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  //  A_Formal_Type_Declaration,                // 12.5(2)
  //  An_Incomplete_Type_Declaration            // asis-declarations.ads 313
  Definition_ID                  Type_Declaration_View;
  //  A_Variable_Declaration,                   // 3.3.1(2)
  //  A_Constant_Declaration,                   // 3.3.1(4)
  //  A_Deferred_Constant_Declaration,          // 3.3.1(6),7.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  //  A_Discriminant_Specification,             // 3.7(5)  
  //  A_Component_Declaration,                  // 3.8(6)
  //  A_Parameter_Specification,                // 6.1(15) 
  //  A_Return_Variable_Specification,          // 6.5
  //  An_Object_Renaming_Declaration,           // 8.5.1(2)
  //  A_Formal_Object_Declaration,              // 12.4(2)
  Definition_ID                  Object_Declaration_View;
  //  A_Variable_Declaration,                   // 3.3.1(2)
  //  A_Constant_Declaration,                   // 3.3.1(4)
  //  An_Integer_Number_Declaration,            // 3.3.2(2)
  //  A_Real_Number_Declaration,                // 3.5.6(2)
  //  A_Discriminant_Specification,             // 3.7(5)  
  //  A_Component_Declaration,                  // 3.8(6)
  //  A_Parameter_Specification,                // 6.1(15) 
  //  A_Return_Variable_Specification,          // 6.5
  //  A_Formal_Object_Declaration,              // 12.4(2)
  Expression_ID                  Initialization_Expression;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
  //  A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  Declaration_ID                 Corresponding_Type_Declaration;
  //  An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
  //  A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3
  //  An_Incomplete_Type_Declaration            // asis-declarations.ads 689
  Declaration_ID                 Corresponding_Type_Completion;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  //  An_Incomplete_Type_Declaration            // asis-declarations.ads 748
  Declaration_ID                 Corresponding_Type_Partial_View;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  //  A_Subtype_Declaration,                    // 3.2.2(2)
  //  A_Formal_Type_Declaration,                // 12.5(2)
  Declaration_ID                 Corresponding_First_Subtype;
  Declaration_ID                 Corresponding_Last_Constraint;
  Declaration_ID                 Corresponding_Last_Subtype;
  //  A_Loop_Parameter_Specification,           // 5.5(4)  
  //  An_Entry_Index_Specification,             // 9.5.2(2)
  Discrete_Subtype_Definition_ID Specification_Subtype_Definition;
  //  A_Generalized_Iterator_Specification,     // 5.5.2   
  //  An_Element_Iterator_Specification,        // 5.5.2   
  Element_ID                     Iteration_Scheme_Name;
  //  An_Element_Iterator_Specification,        // 5.5.2   
  Element_ID                     Subtype_Indication;
  //  A_Procedure_Declaration,                  // 6.1(4)  
  //  A_Function_Declaration,                   // 6.1(4)  
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Null_Procedure_Declaration,             // 6.7
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  An_Entry_Declaration,                     // 9.5.2(2)
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Formal_Procedure_Declaration,           // 12.6(2)
  //  A_Formal_Function_Declaration,            // 12.6(2)
  Parameter_Specification_List   Parameter_Profile;
  //  A_Function_Declaration,                   // 6.1(4)  
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Formal_Function_Declaration,            // 12.6(2)
  Element_ID                     Result_Profile;
  //  An_Expression_Function_Declaration,       // 6.8
  Expression_ID                  Result_Expression;
  //  A_Procedure_Declaration,                  // 6.1(4)  
  //  A_Function_Declaration,                   // 6.1(4)  
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Null_Procedure_Declaration,             // 6.7
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  An_Entry_Declaration,                     // 9.5.2(2)
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Formal_Procedure_Declaration,           // 12.6(2)
  //  A_Formal_Function_Declaration,            // 12.6(2)
  bool                           Is_Overriding_Declaration;
  bool                           Is_Not_Overriding_Declaration;
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Package_Body_Declaration,               // 7.2(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  Element_ID_List                   Body_Declarative_Items;
  Statement_List                 Body_Statements;
  Exception_Handler_List         Body_Exception_Handlers;
  Declaration_ID                 Body_Block_Statement;
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Package_Declaration,                    // 7.1(2)
  //  A_Package_Body_Declaration,               // 7.2(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  A_Protected_Body_Declaration,             // 9.4(7)
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  bool                           Is_Name_Repeated;
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  //  A_Procedure_Declaration,                  // 6.1(4)  
  //  A_Function_Declaration,                   // 6.1(4)  
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Null_Procedure_Declaration,             // 6.7
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Package_Declaration,                    // 7.1(2)
  //  A_Package_Body_Declaration,               // 7.2(2)
  //  A_Package_Renaming_Declaration,           // 8.5.3(2)
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  A_Generic_Package_Renaming_Declaration,   // 8.5.5(2)
  //  A_Generic_Procedure_Renaming_Declaration, // 8.5.5(2)
  //  A_Generic_Function_Renaming_Declaration,  // 8.5.5(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  A_Protected_Body_Declaration,             // 9.4(7)
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  //  An_Entry_Index_Specification,             // 9.5.2(2)
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Package_Body_Stub,                      // 10.1.3(4)
  //  A_Task_Body_Stub,                         // 10.1.3(5)
  //  A_Protected_Body_Stub,                    // 10.1.3(6)
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  //  A_Package_Instantiation,                  // 12.3(2)
  //  A_Procedure_Instantiation,                // 12.3(2)
  //  A_Function_Instantiation,                 // 12.3(2)
  //  A_Formal_Package_Declaration,             // 12.7(2)
  //  A_Formal_Package_Declaration_With_Box     // 12.7(3)
  Declaration_ID                 Corresponding_Declaration;
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  //  A_Procedure_Declaration,                  // 6.1(4)  
  //  A_Function_Declaration,                   // 6.1(4)  
  //  An_Entry_Declaration,                     // 9.5.2(2)
  //  An_Entry_Index_Specification,             // 9.5.2(2)
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  //  A_Package_Instantiation,                  // 12.3(2)
  //  A_Procedure_Instantiation,                // 12.3(2)
  //  A_Function_Instantiation,                 // 12.3(2)
  //  A_Formal_Package_Declaration,             // 12.7(2)
  Declaration_ID                 Corresponding_Body;
  //  A_Procedure_Declaration,                  // 6.1(4)  
  //  A_Function_Declaration,                   // 6.1(4)  
  Declaration_ID                 Corresponding_Subprogram_Derivation;
  //  A_Procedure_Declaration,                  // 6.1(4)  
  //  A_Function_Declaration,                   // 6.1(4)  
  //  An_Expression_Function_Declaration,       // 6.8
  Type_Definition_ID             Corresponding_Type;
  //  A_Function_Declaration,                   // 6.1(4)  
  Declaration_ID                 Corresponding_Equality_Operator;
  //  A_Package_Declaration,                    // 7.1(2)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  Declarative_Item_List          Visible_Part_Declarative_Items;
  bool                           Is_Private_Present;
  Declarative_Item_List          Private_Part_Declarative_Items;
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  Expression_List                Declaration_Interface_List;
  //  An_Object_Renaming_Declaration,           // 8.5.1(2)
  //  An_Exception_Renaming_Declaration,        // 8.5.2(2)
  //  A_Package_Renaming_Declaration,           // 8.5.3(2)
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  A_Generic_Package_Renaming_Declaration,   // 8.5.5(2)
  //  A_Generic_Procedure_Renaming_Declaration, // 8.5.5(2)
  //  A_Generic_Function_Renaming_Declaration,  // 8.5.5(2)
  Expression_ID                  Renamed_Entity;
  Expression_ID                  Corresponding_Base_Entity;
  //  A_Protected_Body_Declaration,             // 9.4(7)
  Declaration_List               Protected_Operation_Items;
  //  An_Entry_Declaration,                     // 9.5.2(2)
  Discrete_Subtype_Definition_ID Entry_Family_Definition;
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  Declaration_ID                 Entry_Index_Specification;
  Expression_ID                  Entry_Barrier;
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Package_Body_Stub,                      // 10.1.3(4)
  //  A_Task_Body_Stub,                         // 10.1.3(5)
  //  A_Protected_Body_Stub,                    // 10.1.3(6)
  Declaration_ID                 Corresponding_Subunit;
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Package_Body_Declaration,               // 7.2(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  A_Protected_Body_Declaration,             // 9.4(7)
  bool                           Is_Subunit;
  //  ONLY present if Is_Subunit is true:
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Package_Body_Declaration,               // 7.2(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  A_Protected_Body_Declaration,             // 9.4(7)
  Declaration_ID                 Corresponding_Body_Stub;
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  Element_ID_List                   Generic_Formal_Part;
  //  A_Package_Instantiation,                  // 12.3(2)
  //  A_Procedure_Instantiation,                // 12.3(2)
  //  A_Function_Instantiation,                 // 12.3(2)
  //  A_Formal_Package_Declaration,             // 12.7(2)
  //  A_Formal_Package_Declaration_With_Box     // 12.7(3)
  Expression_ID                  Generic_Unit_Name;
  Association_List               Generic_Actual_Part;
  //  A_Formal_Procedure_Declaration,           // 12.6(2)
  //  A_Formal_Function_Declaration,            // 12.6(2)
  Expression_ID                  Formal_Subprogram_Default;
  //  A_Procedure_Declaration,                  // 6.1(4)  
  //  A_Function_Declaration,                   // 6.1(4)  
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Null_Procedure_Declaration,             // 6.7
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  bool                           Is_Dispatching_Operation;
};

///////////////////////////////////////////////////////////////////////////////
// END Declaration
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Definition
///////////////////////////////////////////////////////////////////////////////

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

typedef enum _Type_Kinds {
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
} Type_Kinds;

typedef enum _Constraint_Kinds {
  Not_A_Constraint,                      // An unexpected element
  A_Range_Attribute_Reference,           // 3.5(2)
  A_Simple_Expression_Range,             // 3.2.2, 3.5(3)
  A_Digits_Constraint,                   // 3.2.2, 3.5.9
  A_Delta_Constraint,                    // 3.2.2, J.3
  An_Index_Constraint,                   // 3.2.2, 3.6.1
  A_Discriminant_Constraint              // 3.2.2
} Constraint_Kinds;

typedef enum _Interface_Kinds {  // 3.9.4
  Not_An_Interface,                 // An unexpected element
  An_Ordinary_Interface,            // interface ...
  A_Limited_Interface,              // limited interface ...
  A_Task_Interface,                 // task interface ...
  A_Protected_Interface,            // protected interface ...
  A_Synchronized_Interface        // synchronized interface ...
} Interface_Kinds; 

typedef enum _Root_Type_Kinds {
  Not_A_Root_Type_Definition,            // An unexpected element
  A_Root_Integer_Definition,             // 3.4.1(8)
  A_Root_Real_Definition,                // 3.4.1(8)
  A_Universal_Integer_Definition,        // 3.4.1(6)
  A_Universal_Real_Definition,           // 3.4.1(6)
  A_Universal_Fixed_Definition         // 3.4.1(6)
 } Root_Type_Kinds;

typedef enum _Discrete_Range_Kinds {
  Not_A_Discrete_Range,                  // An unexpected element
  A_Discrete_Subtype_Indication,         // 3.6.1(6), 3.2.2
  A_Discrete_Range_Attribute_Reference,  // 3.6.1, 3.5
  A_Discrete_Simple_Expression_Range   // 3.6.1, 3.5
} Discrete_Range_Kinds;

typedef enum _Formal_Type_Kinds {
  Not_A_Formal_Type_Definition,             // An unexpected element
  A_Formal_Private_Type_Definition,         // 12.5.1(2)   -> Trait_Kinds
  A_Formal_Tagged_Private_Type_Definition,  // 12.5.1(2)   -> Trait_Kinds
  A_Formal_Derived_Type_Definition,         // 12.5.1(3)   -> Trait_Kinds
  A_Formal_Discrete_Type_Definition,        // 12.5.2(2)
  A_Formal_Signed_Integer_Type_Definition,  // 12.5.2(3)
  A_Formal_Modular_Type_Definition,         // 12.5.2(4)
  A_Formal_Floating_Point_Definition,       // 12.5.2(5)
  A_Formal_Ordinary_Fixed_Point_Definition, // 12.5.2(6)
  A_Formal_Decimal_Fixed_Point_Definition,  // 12.5.2(7)

  //|A2005 start
  A_Formal_Interface_Type_Definition,       // 12.5.5(2) -> Interface_Kinds
  //|A2005 end

  A_Formal_Unconstrained_Array_Definition,  // 3.6(3)
  A_Formal_Constrained_Array_Definition,    // 3.6(5)
  A_Formal_Access_Type_Definition           // 3.10(3),3.10(5)
  //                                                 -> Access_Type_Kinds
} Formal_Type_Kinds;

typedef enum _Access_Type_Kinds {
  Not_An_Access_Type_Definition,       // An unexpected element
  A_Pool_Specific_Access_To_Variable,  // access subtype_indication
  An_Access_To_Variable,               // access all subtype_indication
  An_Access_To_Constant,               // access constant subtype_indication
  An_Access_To_Procedure,              // access procedure
  An_Access_To_Protected_Procedure,    // access protected procedure
  An_Access_To_Function,               // access function
  An_Access_To_Protected_Function      // access protected function
} Access_Type_Kinds;

typedef enum _Access_Definition_Kinds {
  Not_An_Access_Definition,       // An unexpected element
  An_Anonymous_Access_To_Variable,  // [...] access subtype_mark
  An_Anonymous_Access_To_Constant,  // [...] access constant subtype_mark
  An_Anonymous_Access_To_Procedure,           // access procedure
  An_Anonymous_Access_To_Protected_Procedure, // access protected procedure
  An_Anonymous_Access_To_Function,            // access function
  An_Anonymous_Access_To_Protected_Function   // access protected function
} Access_Definition_Kinds;

typedef struct _Access_Type_Struct {
  Access_Type_Kinds            Access_Type_Kind;  
  bool                         Has_Null_Exclusion;
  // These fields are only valid for the kinds above them:
  // An_Access_To_Function
  // An__Access_To_Protected_Function
  bool                         Is_Not_Null_Return;
  // A_Pool_Specific_Access_To_Variable
  // An_Access_To_Variable
  // An_Access_To_Constant
  Subtype_Indication           Access_To_Object_Definition;
  // An_Access_To_Procedure
  // An_Access_To_Protected_Procedure
  // An_Access_To_Function
  // An_Access_To_Protected_Function
  Parameter_Specification_List Access_To_Subprogram_Parameter_Profile; 
  // An_Access_To_Function
  // An_Access_To_Protected_Function
  Element_ID                   Access_To_Function_Result_Profile;
} Access_Type_Struct;

typedef struct _Type_Definition_Struct {
  Type_Kinds           Type_Kind;
  bool                 Has_Abstract;
  bool                 Has_Limited;
  bool                 Has_Private;
  Declaration_List     Corresponding_Type_Operators;
  // These fields are only valid for the kinds above them:
  //  An_Interface_Type_Definition
  bool                 Has_Protected;
  bool                 Has_Synchronized;
  // A_Tagged_Record_Type_Definition
  bool                 Has_Tagged;
  // A_Task_Definition
  bool                 Has_Task;
  // A_Discriminant_Specification
  // A_Parameter_Specification
  // A_Formal_Object_Declaration
  // An_Object_Renaming_Declaration
  bool                 Has_Null_Exclusion;
  // An_Interface_Type_Definition
  Interface_Kinds      Interface_Kind;
  // A_Root_Type_Definition
  Root_Type_Kinds      Root_Type_Kind;
  // A_Derived_Type_Definition
  // A_Derived_Record_Extension_Definition
  Subtype_Indication   Parent_Subtype_Indication;  
  // A_Derived_Record_Extension_Definition
  // A_Record_Type_Definition
  // A_Tagged_Record_Type_Definition
  Definition           Record_Definition;
  // A_Derived_Type_Definition
  // A_Derived_Record_Extension_Definition
  Declaration_List     Implicit_Inherited_Declarations;
  Declaration_List     Implicit_Inherited_Subprograms;
  Declaration          Corresponding_Parent_Subtype;
  Declaration          Corresponding_Root_Type;
  Declaration          Corresponding_Type_Structure;
  // An_Enumeration_Type_Definition
  Declaration_List     Enumeration_Literal_Declarations;
  // A_Signed_Integer_Type_Definition
  Range_Constraint     Integer_Constraint;
  // A_Modular_Type_Definition
  Expression           Mod_Static_Expression;
  // A_Floating_Point_Definition
  // A_Decimal_Fixed_Point_Definition
  Expression           Digits_Expression;
  // An_Ordinary_Fixed_Point_Definition
  // A_Decimal_Fixed_Point_Definition
  Expression           Delta_Expression;
  // A_Floating_Point_Definition
  // An_Ordinary_Fixed_Point_Definition
  // A_Decimal_Fixed_Point_Definition
  Range_Constraint     Real_Range_Constraint;
  // An_Unconstrained_Array_Definition
  Expression_List      Index_Subtype_Definitions;
  // A_Constrained_Array_Definition
  Expression_List      Discrete_Subtype_Definitions;
  // An_Unconstrained_Array_Definition
  // A_Constrained_Array_Definition
  Component_Definition Array_Component_Definition;
  // A_Derived_Record_Extension_Definition
  // An_Interface_Type_Definition
  Expression_List      Definition_Interface_List;
  // An_Access_Type_Definition
  Access_Type_Struct   Access_Type;
} Type_Definition_Struct;

typedef struct _Subtype_Indication_Struct {
  bool       Has_Null_Exclusion;
  Expression Subtype_Mark;
  Constraint Subtype_Constraint;
} Subtype_Indication_Struct;

typedef struct _Constraint_Struct {
  Constraint_Kinds              Constraint_Kind;
  // These fields are only valid for the kinds above them:
  // A_Digits_Constraint
  Expression                    Digits_Expression;
  // A_Delta_Constraint
  Expression                    Delta_Expression;
  // A_Digits_Constraint
  // A_Delta_Constraint
  Range_Constraint              Real_Range_Constraint;
  // A_Simple_Expression_Range
  Expression                    Lower_Bound;
  Expression                    Upper_Bound;
  // A_Range_Attribute_Reference
  Expression                    Range_Attribute;
  // An_Index_Constraint
  Discrete_Range_List           Discrete_Ranges;
  // A_Discriminant_Constraint
  Discriminant_Association_List Discriminant_Associations;
} Constraint_Struct;

typedef struct _Component_Definition_Struct {
  Definition Component_Definition_View;
} Component_Definition_Struct;

typedef struct _Discrete_Subtype_Definition_Struct {
  Discrete_Range_Kinds Discrete_Range_Kind;
  // These fields are only valid for the kinds above them:
  // A_Discrete_Subtype_Indication
  Expression           Subtype_Mark;
  Constraint           Subtype_Constraint;
} Discrete_Subtype_Definition_Struct;

typedef struct _Discrete_Range_Struct {
  Discrete_Range_Kinds Discrete_Range_Kind;
  // These fields are only valid for the kinds above them:
  // A_Discrete_Subtype_Indication
  Expression           Subtype_Mark;
  Constraint           Subtype_Constraint;
  // A_Discrete_Simple_Expression_Range
  Expression           Lower_Bound;
  Expression           Upper_Bound;
  // A_Discrete_Range_Attribute_Reference
  Expression           Range_Attribute;
} Discrete_Range_Struct;

typedef struct _Known_Discriminant_Part_Struct {
  Discriminant_Specification_List Discriminants;
} Known_Discriminant_Part_Struct;

typedef struct _Record_Definition_Struct {
  Record_Component_List Record_Components;
  Record_Component_List Implicit_Components;
} Record_Definition_Struct;

typedef struct _Variant_Part_Struct {
  Name         Discriminant_Direct_Name;
  Variant_List Variants;
} Variant_Part_Struct;

typedef struct _Variant_Struct {
  Record_Component_List Record_Components;
  Record_Component_List Implicit_Components;
  Element_ID_List          Variant_Choices;
} Variant_Struct;

typedef struct _Access_Definition_Struct {
  Access_Definition_Kinds      Access_Definition_Kind;
  bool                         Has_Null_Exclusion;
  // These fields are only valid for the kinds above them:
  // An_Anonymous_Access_To_Function
  // An_Anonymous_Access_To_Protected_Function
  bool                         Is_Not_Null_Return;
  // An_Anonymous_Access_To_Variable
  // An_Anonymous_Access_To_Constant
  Expression                   Anonymous_Access_To_Object_Subtype_Mark;
  // An_Anonymous_Access_To_Procedure
  // An_Anonymous_Access_To_Protected_Procedure
  // An_Anonymous_Access_To_Function
  // An_Anonymous_Access_To_Protected_Function
  Parameter_Specification_List Access_To_Subprogram_Parameter_Profile; 
  // An_Anonymous_Access_To_Function
  // An_Anonymous_Access_To_Protected_Function
  Element_ID                   Access_To_Function_Result_Profile;
} Access_Definition_Struct;

typedef struct _Private_Type_Definition_Struct {
  bool             Has_Abstract;
  bool             Has_Limited;
  bool             Has_Private;
} Private_Type_Definition_Struct;

typedef struct _Tagged_Private_Type_Definition_Struct {
  bool             Has_Abstract;
  bool             Has_Limited;
  bool             Has_Private;
  bool             Has_Tagged;
} Tagged_Private_Type_Definition_Struct;

typedef struct _Private_Extension_Definition_Struct {
  bool               Has_Abstract;
  bool               Has_Limited;
  bool               Has_Private;
  bool               Has_Synchronized;
  Declaration_List   Implicit_Inherited_Declarations;
  Declaration_List   Implicit_Inherited_Subprograms;
  Expression_List    Definition_Interface_List;
  Subtype_Indication Ancestor_Subtype_Indication;
} Private_Extension_Definition_Struct;

typedef struct _Task_Definition_Struct {
  bool                  Has_Task;
  Declarative_Item_List Visible_Part_Items;
  Declarative_Item_List Private_Part_Items;
  bool                  Is_Private_Present;
} Task_Definition_Struct;

typedef struct _Protected_Definition_Struct {
  bool                  Has_Protected;
  Declarative_Item_List Visible_Part_Items;
  Declarative_Item_List Private_Part_Items;
  bool                  Is_Private_Present;
} Protected_Definition_Struct;

typedef struct _Formal_Type_Definition_Struct {
  Formal_Type_Kinds    Formal_Type_Kind;
  Declaration_List     Corresponding_Type_Operators;
  // These fields are only valid for the kinds above them:
  // A_Formal_Private_Type_Definition
  // A_Formal_Tagged_Private_Type_Definition
  // A_Formal_Derived_Type_Definition
  bool                 Has_Abstract;
  bool                 Has_Limited;
  // A_Formal_Private_Type_Definition
  // A_Formal_Tagged_Private_Type_Definition
  bool                 Has_Private;
  // A_Formal_Derived_Type_Definition
  bool                 Has_Synchronized;
  // A_Formal_Tagged_Private_Type_Definition
  bool                 Has_Tagged;
  // A_Formal_Interface_Type_Definition
  Interface_Kinds      Interface_Kind;
  // A_Formal_Derived_Type_Definition
  Declaration_List     Implicit_Inherited_Declarations;
  Declaration_List     Implicit_Inherited_Subprograms;
  // A_Formal_Unconstrained_Array_Definition
  Expression_List      Index_Subtype_Definitions;
  // A_Formal_Constrained_Array_Definition
  Expression_List      Discrete_Subtype_Definitions;
  // A_Formal_Unconstrained_Array_Definition
  // A_Formal_Constrained_Array_Definition
  Component_Definition Array_Component_Definition;
  // A_Formal_Derived_Type_Definition
  Expression           Subtype_Mark;
  // A_Formal_Derived_Type_Definition
  // A_Formal_Interface_Type_Definition
  Expression_List      Definition_Interface_List;
  // A_Formal_Access_Type_Definition
  Access_Type_Struct   Access_Type;
} Formal_Type_Definition_Struct;

typedef struct _Aspect_Specification_Struct {
  Element_ID Aspect_Mark;
  Element_ID Aspect_Definition;
} Aspect_Specification_Struct;

typedef int       No_Struct;
typedef No_Struct Unknown_Discriminant_Part_Struct;
typedef No_Struct Null_Record_Definition_Struct;
typedef No_Struct Null_Component_Struct;
typedef No_Struct Others_Choice_Struct;

typedef union _Definition_Union {
  int                                   Dummy_Member; // For Ada default initialization
  Type_Definition_Struct                The_Type_Definition;
  Subtype_Indication_Struct             The_Subtype_Indication;
  Constraint_Struct                     The_Constraint;
  Component_Definition_Struct           The_Component_Definition;
  Discrete_Subtype_Definition_Struct    The_Discrete_Subtype_Definition;
  Discrete_Range_Struct                 The_Discrete_Range;
  Unknown_Discriminant_Part_Struct      The_Unknown_Discriminant_Part;
  Known_Discriminant_Part_Struct        The_Known_Discriminant_Part;
  Record_Definition_Struct              The_Record_Definition;
  Null_Record_Definition_Struct         The_Null_Record_Definition;
  Null_Component_Struct                 The_Null_Component;
  Variant_Part_Struct                   The_Variant_Part;
  Variant_Struct                        The_Variant;
  Others_Choice_Struct                  The_Others_Choice;
  Access_Definition_Struct              The_Access_Definition;
  Private_Type_Definition_Struct        The_Private_Type_Definition;
  Tagged_Private_Type_Definition_Struct The_Tagged_Private_Type_Definition;
  Private_Extension_Definition_Struct   The_Private_Extension_Definition;
  Task_Definition_Struct                The_Task_Definition;
  Protected_Definition_Struct           The_Protected_Definition;
  Formal_Type_Definition_Struct         The_Formal_Type_Definition;
  Aspect_Specification_Struct           The_Aspect_Specification;
} Definition_Union;  

// May take ??*4 bytes:
struct Definition_Struct {
  enum Definition_Kinds Definition_Kind;
  Definition_Union      The_Union;
};

///////////////////////////////////////////////////////////////////////////////
// END Definition
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Expression
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

// May take 33*4 bytes - 19 IDs, 8 Lists, 1 bool, 3 enums, 2 char*:
struct Expression_Struct {
  enum Expression_Kinds Expression_Kind;
  bool                  Is_Prefix_Notation;
  Declaration_ID        Corresponding_Expression_Type;
  Element_ID            Corresponding_Expression_Type_Definition;
  
  // These fields are only valid for the kinds above them:  
  // An_Operator_Symbol:
  enum Operator_Kinds   Operator_Kind;
  // An_Integer_Literal,                        // 2.4
  // A_Real_Literal,                            // 2.4.1
  // A_String_Literal,                          // 2.6
  // An_Attribute_Reference :
  enum Attribute_Kinds  Attribute_Kind;
  char                 *Value_Image;  
  // An_Identifier |                              // 4.1
  // An_Operator_Symbol |                         // 4.1
  // A_Character_Literal |                        // 4.1
  // An_Enumeration_Literal:
  char                 *Name_Image;
  Defining_Name_ID      Corresponding_Name_Definition;
  Defining_Name_List    Corresponding_Name_Definition_List; // Only >1 if the expression in a pragma is ambiguous
  Element_ID            Corresponding_Name_Declaration; // Decl or stmt
  // An_Explicit_Dereference =>                   // 4.1
  // A_Function_Call =>                           // 4.1
  // An_Indexed_Component =>                      // 4.1.1
  // A_Slice =>                                   // 4.1.2
  // A_Selected_Component =>                      // 4.1.3
  // An_Attribute_Reference =>                    // 4.1.4
  Expression_ID         Prefix;
  // An_Indexed_Component =>                      // 4.1.1
  Expression_List       Index_Expressions;
  // A_Slice =>                                   // 4.1.2
  Discrete_Range_ID     Slice_Range;
  // A_Selected_Component =>                      // 4.1.3
  Expression_ID         Selector;
  // An_Attribute_Reference :
  Expression_ID         Attribute_Designator_Identifier;
  //       An_Attribute_Reference
  //           Appropriate Attribute_Kinds:
  //                A_First_Attribute
  //                A_Last_Attribute
  //                A_Length_Attribute
  //                A_Range_Attribute
  //                An_Implementation_Defined_Attribute
  //                An_Unknown_Attribute
  Expression_List       Attribute_Designator_Expressions;
  // A_Record_Aggregate =>                        // 4.3
  // An_Extension_Aggregate =>                    // 4.3
  Association_List      Record_Component_Associations;
  // An_Extension_Aggregate =>                    // 4.3
  Expression_ID         Extension_Aggregate_Expression;
  // A_Positional_Array_Aggregate |               // 4.3
  // A_Named_Array_Aggregate =>                   // 4.3  
  Association_List      Array_Component_Associations;
  // A_Parenthesized_Expression =>                // 4.4
  Expression_ID         Expression_Parenthesized;
  // A_Function_Call =>                           // 4.1
  bool                  Is_Prefix_Call;
  // A_Function_Call =>                           // 4.1 
  // An_Indexed_Component (Is_Generalized_Indexing == true) //ASIS 2012 // 4.1.1
  Declaration_ID        Corresponding_Called_Function;
  // A_Function_Call =>                           // 4.1
  Element_ID_List          Function_Call_Parameters;
  // An_And_Then_Short_Circuit |                  // 4.4
  // An_Or_Else_Short_Circuit =>                  // 4.4
  Expression_ID         Short_Circuit_Operation_Left_Expression;
  Expression_ID         Short_Circuit_Operation_Right_Expression;
  // An_In_Membership_Test |                      // 4.4  Ada 2012
  // A_Not_In_Membership_Test =>                  // 4.4  Ada 2012
  Expression_ID         Membership_Test_Expression;
  Element_ID_List          Membership_Test_Choices;
  // A_Type_Conversion =>                         // 4.6
  // A_Qualified_Expression =>                    // 4.7
  Expression_ID         Converted_Or_Qualified_Subtype_Mark;
  Expression_ID         Converted_Or_Qualified_Expression;
  // An_Allocation_From_Subtype =>                // 4.8
  Subtype_Indication_ID Allocator_Subtype_Indication;
  // An_Allocation_From_Qualified_Expression =>   // 4.8
  Expression_ID         Allocator_Qualified_Expression;
  // A_Case_Expression |                          // Ada 2012
  // An_If_Expression =>                          // Ada 2012
  Expression_Path_List  Expression_Paths;
  // An_Indexed_Component =>                      // 4.1.1
  bool                  Is_Generalized_Indexing;
  // A_Function_Call
  bool                  Is_Generalized_Reference;
  // A_For_All_Quantified_Expression |            // Ada 2012
  // A_For_Some_Quantified_Expression =>          // Ada 2012
  Declaration_ID        Iterator_Specification;
  // A_Type_Conversion =>                         // 4.6
  // A_Qualified_Expression =>                    // 4.7
  Expression_ID         Predicate;
  // An_Allocation_From_Subtype =>                // 4.8
  // An_Allocation_From_Qualified_Expression =>   // 4.8
  Expression_ID         Subpool_Name;
  // An_Identifier
  // An_Operator_Symbol
  // A_Character_Literal
  // An_Enumeration_Literal
  Defining_Name_ID      Corresponding_Generic_Element;
  // A_Function_Call
  bool                  Is_Dispatching_Call;
  bool                  Is_Call_On_Dispatching_Operation;
};

///////////////////////////////////////////////////////////////////////////////
// END Expression
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Association
///////////////////////////////////////////////////////////////////////////////

enum Association_Kinds {
  Not_An_Association,                    // An unexpected element

  A_Pragma_Argument_Association,         // 2.8
  A_Discriminant_Association,            // 3.7.1
  A_Record_Component_Association,        // 4.3.1
  An_Array_Component_Association,        // 4.3.3
  A_Parameter_Association,               // 6.4
  A_Generic_Association                  // 12.3
};

// May take 10*4 bytes - 1*enum, 3*List, 4*ID, 2*bool:
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
  // A_Record_Component_Association
  bool                   Is_Defaulted_Association;
};

///////////////////////////////////////////////////////////////////////////////
// END Association
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Statement
///////////////////////////////////////////////////////////////////////////////

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

// May take 37*4 bytes - 22 IDs, 12 Lists, 2 bools, and 1 enum:
struct Statement_Struct {
  enum Statement_Kinds   Statement_Kind;
  Pragma_Element_ID_List    Corresponding_Pragmas;
  Defining_Name_List     Label_Names;
  
  // These fields are only valid for the kinds above them:  
  //   A_Procedure_Call_Statement
  bool                   Is_Prefix_Notation;
  //   A_Loop_Statement                (pragmas from statement list)
  //   A_While_Loop_Statement          (pragmas from statement list)
  //   A_For_Loop_Statement            (pragmas from statement list)
  //   A_Block_Statement               (pragmas from declarative region +
  //                                                 statements)
  //   An_Accept_Statement             (pragmas from statement list +
  Pragma_Element_ID_List    Pragmas;
  //   A_Loop_Statement
  //   A_While_Loop_Statement
  //   A_For_Loop_Statement
  //   A_Block_Statement
  //   An_Accept_Statement
  Element_ID             Corresponding_End_Name;
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
  //   A_Return_Statement,                  // 6.5
  Expression_ID          Return_Expression;
  //   //  //|A2005 start
  //   An_Extended_Return_Statement,        // 6.5
  Declaration_ID         Return_Object_Declaration;
  Statement_List         Extended_Return_Statements;
  Exception_Handler_List Extended_Return_Exception_Handlers;
  //   A_Goto_Statement,                    // 5.8
  Expression_ID          Goto_Label;
  Statement_ID           Corresponding_Destination_Statement;
  //   A_Procedure_Call_Statement,          // 6.4
  //   An_Entry_Call_Statement,             // 9.5.3
  Expression_ID          Called_Name;
  Declaration_ID         Corresponding_Called_Entity;
  Association_List       Call_Statement_Parameters;
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
  //   A_Procedure_Call_Statement
  bool                   Is_Dispatching_Call;
  bool                   Is_Call_On_Dispatching_Operation;
  //   A_Procedure_Call_Statement
  //   An_Entry_Call_Statement
  Declaration            Corresponding_Called_Entity_Unwound;
};

///////////////////////////////////////////////////////////////////////////////
// END Statement
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Path
///////////////////////////////////////////////////////////////////////////////

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

// May take 5*4 bytes - 1*enum, 2*List, 2*ID:
struct Path_Struct {
  enum Path_Kinds Path_Kind;
  Statement_List Sequence_Of_Statements;
  Expression     Dependent_Expression;

  // These fields are only valid for the kinds above them:  
  // An_If_Path,
  // An_Elsif_Path,
  Expression_ID  Condition_Expression;
  // A_Case_Path,
  // A_Case_Expression_Path,
  Element_ID_List   Case_Path_Alternative_Choices;
  // A_Select_Path,
  // An_Or_Path,
  Expression_ID  Guard;
};

///////////////////////////////////////////////////////////////////////////////
// END Path
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Clause
///////////////////////////////////////////////////////////////////////////////

enum Clause_Kinds {
  Not_A_Clause,                 // An unexpected element
  A_Use_Package_Clause,         // 8.4
  A_Use_Type_Clause,            // 8.4
  
  A_Use_All_Type_Clause,        // 8.4, Ada 2012

  A_With_Clause,                // 10.1.2
  A_Representation_Clause,      // 13.1     -> Representation_Clause_Kinds
  A_Component_Clause            // 13.5.1
};

typedef enum _Representation_Clause_Kinds {
      Not_A_Representation_Clause,              // An unexpected element
      An_Attribute_Definition_Clause,           // 13.3
      An_Enumeration_Representation_Clause,     // 13.4
      A_Record_Representation_Clause,           // 13.5.1
      An_At_Clause                              // J.7
  } Representation_Clause_Kinds;

typedef struct _Representation_Clause_Struct {
  Representation_Clause_Kinds Representation_Clause_Kind;
  Name                        Representation_Clause_Name;

  // These fields are only valid for the kinds above them:
  // A_Record_Representation_Clause
  Pragma_Element_ID_List         Pragmas;
  // An_Attribute_Definition_Clause
  // An_Enumeration_Representation_Clause
  // An_At_Clause
  Expression                  Representation_Clause_Expression;
  // A_Record_Representation_Clause
  Expression                  Mod_Clause_Expression;  
  Component_Clause_List       Component_Clauses; 
} Representation_Clause_Struct;

// May take 9*4 bytes - 2*enum, 2*List, 5*ID:
struct Clause_Struct {
  enum Clause_Kinds Clause_Kind;
  // These fields are only valid for the kinds above them:
  //   A_With_Clause
  bool              Has_Limited;
  //   A_Use_Package_Clause
  //   A_Use_Type_Clause
  //   A_Use_All_Type_Clause
  //   A_With_Clause
  Name_List         Clause_Names;
  //   A_Component_Clause  
  Name_ID           Representation_Clause_Name;
  Expression_ID     Representation_Clause_Expression;
  Expression_ID     Mod_Clause_Expression;
  Element_ID_List      Component_Clauses;
  Expression_ID     Component_Clause_Position;
  Element_ID        Component_Clause_Range;
  //   A_Representation_Clause
  Representation_Clause_Struct  
                    Representation_Clause;
};

///////////////////////////////////////////////////////////////////////////////
// END Clause
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN Exception_Handler
///////////////////////////////////////////////////////////////////////////////

// There is no enum Exception_Handler_Kinds because there is only one kind of 
// exception handler.

// May take 7*4 bytes - 1 ID, 3 List:
struct Exception_Handler_Struct {
  Pragma_Element_ID_List Pragmas;
  Declaration_ID      Choice_Parameter_Specification;
  Element_ID_List        Exception_Choices;
  Statement_List      Handler_Statements;
};

///////////////////////////////////////////////////////////////////////////////
// END Exception_Handler
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// END supporting kinds
///////////////////////////////////////////////////////////////////////////////

// May take ?? bytes (??, the largest component):
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

enum Enclosing_Kinds { // Not an ASIS type
  Not_Enclosing,
  Enclosing_Element,
  Enclosing_Unit 
};

// May take 5*4 bytes - 1*char*, 4*int:
struct Source_Location_Struct {
  char *Unit_Name;
  int   First_Line;
  int   First_Column;
  int   Last_Line;
  int   Last_Column;
};

// Declared here because used in Element_Struct:
typedef int Unit_ID;

// May take ?? bytes - 2*ID, 2*enum, 1*5*4 struct, 1*?? union:
typedef struct _Element_Struct {
  Element_ID                    ID;
  enum Element_Kinds            Element_Kind;
  Unit_ID                       Enclosing_Compilation_Unit;
  bool                          Is_Part_Of_Implicit;
  bool                          Is_Part_Of_Inherited;
  bool                          Is_Part_Of_Instance;
  ASIS_Integer                  Hash;
  Element_ID                    Enclosing_Element_ID;
  enum Enclosing_Kinds          Enclosing_Kind;
  struct Source_Location_Struct Source_Location;
  char*                         Debug_Image;
  union Element_Union           The_Union;
} Element_Struct;

///////////////////////////////////////////////////////////////////////////////
// END element 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN unit
///////////////////////////////////////////////////////////////////////////////

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

// May take 26*4 (20*4 + 3) bytes - 6*ID, 3*enum, 4*List(2*4 ea), 8*char*, 3*bool:
typedef struct _Unit_Struct {
  Unit_ID             ID;
  enum Unit_Kinds     Unit_Kind;
  enum Unit_Classes   Unit_Class;
  enum Unit_Origins   Unit_Origin;
  char               *Unit_Full_Name; // Ada name
  char               *Unique_Name; // file name etc.
  bool                Exists;
  bool                Can_Be_Main_Program;
  bool                Is_Body_Required;
  char               *Text_Name;
  char               *Text_Form;
  char               *Object_Name;
  char               *Object_Form;
  char               *Compilation_Command_Line_Options;
  char               *Debug_Image;
  Declaration_ID      Unit_Declaration;
  Context_Clause_List Context_Clause_Elements;
  Pragma_Element_ID_List Compilation_Pragmas;
  bool                Is_Standard;
  
  // The fields below are only applicable to the kinds above them:
  //  A_Package,
  //  A_Generic_Package,
  //  A_Package_Instance,
  Unit_List           Corresponding_Children;
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
  Unit_ID             Corresponding_Parent_Declaration;
  //  A_Procedure_Body,
  //  A_Function_Body,
  //  A_Package_Body,
  //  An_Unknown_Unit
  Unit_ID             Corresponding_Declaration;
  //  A_Procedure,
  //  A_Function,
  //  A_Package,
  //  A_Generic_Procedure,
  //  A_Generic_Function,
  //  A_Generic_Package,
  //  An_Unknown_Unit
  Unit_ID             Corresponding_Body;
  //  A_Procedure_Body,
  //  A_Function_Body,
  //  A_Package_Body,
  //  A_Procedure_Body_Subunit,
  //  A_Function_Body_Subunit,
  //  A_Package_Body_Subunit,
  //  A_Task_Body_Subunit,
  //  A_Protected_Body_Subunit,
  Unit_List           Subunits;
  //  A_Procedure_Body_Subunit,
  //  A_Function_Body_Subunit,
  //  A_Package_Body_Subunit,
  //  A_Task_Body_Subunit,
  //  A_Protected_Body_Subunit,
  Unit_ID             Corresponding_Subunit_Parent_Body;
} Unit_Struct;
///////////////////////////////////////////////////////////////////////////////
// END unit 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BEGIN context
///////////////////////////////////////////////////////////////////////////////

typedef struct _Context_Struct {
  char *name;
  char *parameters;
  char *debug_image;
} Context_Struct;

///////////////////////////////////////////////////////////////////////////////
// END context
///////////////////////////////////////////////////////////////////////////////

typedef struct _Unit_Struct_List_Struct {
  Unit_Struct                      Unit;
  struct _Unit_Struct_List_Struct *Next;
  int                              Next_Count;
} Unit_Struct_List_Struct;

typedef Unit_Struct_List_Struct *Unit_Structs_Ptr;

typedef struct _Element_Struct_List_Struct {
  Element_Struct                      Element;
  struct _Element_Struct_List_Struct *Next;
  int                                 Next_Count;
} Element_Struct_List_Struct;

typedef Element_Struct_List_Struct *Element_Structs_Ptr;

typedef struct _Nodes_Struct {
  Context_Struct      Context;
  Unit_Structs_Ptr    Units;
  Element_Structs_Ptr Elements;
} Nodes_Struct;

#endif //ifndef A_NODES_H

