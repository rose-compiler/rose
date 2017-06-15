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

// For Declaration:
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

///////////////////////////////////////////////////////////////////////////////
// END supporting kinds
///////////////////////////////////////////////////////////////////////////////


struct Pragma_Struct {
  enum Pragma_Kinds kind;
};

struct Defining_Operator_Symbol_Struct {
  enum Operator_Kinds kind;
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
  enum Declaration_Kinds   kind;
  enum Declaration_Origins origin;
  
  // Only for kind ==
  // A_Parameter_Specification |
  // A_Formal_Object_Declaration:
  enum Mode_Kinds          mode;
  
  // Only for kind ==
  // A_Formal_Function_Declaration |
  // A_Formal_Procedure_Declaration:
  enum Subprogram_Default_Kinds subprogram_default;
  
  // Only for kind ==
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
  enum Trait_Kinds         trait;
};

struct Definition_Struct {
  enum Definition_Kinds kind;
};

struct Expression_Struct {
  enum Expression_Kinds kind;
  
  // Only for kind ==
  // An_Identifier |                              // 4.1
  // An_Operator_Symbol |                         // 4.1
  // A_Character_Literal |                        // 4.1
  // An_Enumeration_Literal:
  char *name_image;
  
  // Only for kind ==
  // An_Operator_Symbol:
  enum Operator_Kinds operator_kind;
  
  // Only for kind ==
  // An_Attribute_Reference :
  enum Attribute_Kinds atribute_kind;
  
  // TODO: INCOMPLETE! See asis_tool_2-element.adb.Process_Expression
    
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
  struct Pragma_Struct            the_pragma; // pragma is a reserverd word in Ada
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

struct Element_Struct {
  char                  *id;
  char                  *enclosing_id;
  char                  *source_location;
  enum  Element_Kinds    kind;
  struct Element_Struct *next;
  union Element_Union    element;
};

