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
  enum Operator_Kind {
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
// END supporting kinds
///////////////////////////////////////////////////////////////////////////////


struct Pragma_Struct {
  enum Pragma_Kinds kind;
};

struct Defining_Operator_Symbol_Struct {
  enum Operator_Kind kind;
};

union Defining_Name_Union {
  struct Defining_Operator_Symbol_Struct defining_operator_symbol;
};

struct Defining_Name_Struct {
  enum Defining_Name_Kinds  kind;
  char                     *name;
  union Defining_Name_Union defining_name;
};

struct Declaration_Struct {
  enum Declaration_Kinds kind;
};

struct Definition_Struct {
  enum Definition_Kinds kind;
};

struct Expression_Struct {
  enum Expression_Kinds kind;
};

struct Association_Struct {
  enum Association_Kinds kind;
};

struct Statement_Struct {
  enum Statement_Kinds kind;
};

struct Path_Struct {
  enum Path_Kinds kind;
};

struct Clause_Struct {
  enum Clause_Kinds kind;
};

struct Exception_Handler_Struct {
};

union Element_Union {
  struct Pragma_Struct            pragma;
  struct Defining_Name_Struct     defining_name;
  struct Declaration_Struct       declaration;
  struct Definition_Struct        definition;
  struct Expression_Struct        expression;
  struct Association_Struct       association;
  struct Statement_Struct         statement;
  struct Path_Struct              path;
  struct Clause_Struct            clause;
  struct Exception_Handler_Struct exception_handler;
};

struct Element_Struct;

// This turns into System.Address:
typedef struct Element_Struct *Element_Struct_Pointer;

struct Element_Struct {
  char                  *id;
  char                  *source_location;
  enum  Element_Kinds    kind;
  Element_Struct_Pointer next;
  union Element_Union    element;
};

// This turns into access all Element_Struct:
typedef struct Element_Struct *Element_Struct_Access;


