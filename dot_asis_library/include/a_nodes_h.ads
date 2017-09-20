pragma Ada_2005;
pragma Style_Checks (Off);

with Interfaces.C; use Interfaces.C;
with Interfaces.C.Strings;
with Interfaces.C.Extensions;

package a_nodes_h is

   type Node_Kinds is 
     (Not_A_Node,
      A_Context_Node,
      A_Unit_Node,
      An_Element_Node);
   pragma Convention (C, Node_Kinds);  -- a_nodes.h:11

   subtype Node_ID is int;  -- a_nodes.h:18

   type Context_Struct is record
      name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:25
      parameters : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:26
      debug_image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:27
   end record;
   pragma Convention (C_Pass_By_Copy, Context_Struct);  -- a_nodes.h:24

   subtype Unit_ID is Node_ID;  -- a_nodes.h:37

   type Unit_ID_Ptr is access all Unit_ID;  -- a_nodes.h:39

   type Unit_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:44
      IDs : Unit_ID_Ptr;  -- a_nodes.h:45
   end record;
   pragma Convention (C_Pass_By_Copy, Unit_ID_Array_Struct);  -- a_nodes.h:43

   subtype Unit_List is Unit_ID_Array_Struct;  -- a_nodes.h:47

   type Unit_Kinds is 
     (Not_A_Unit,
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
      A_Function_Body,
      A_Package_Body,
      A_Procedure_Body_Subunit,
      A_Function_Body_Subunit,
      A_Package_Body_Subunit,
      A_Task_Body_Subunit,
      A_Protected_Body_Subunit,
      A_Nonexistent_Declaration,
      A_Nonexistent_Body,
      A_Configuration_Compilation,
      An_Unknown_Unit);
   pragma Convention (C, Unit_Kinds);  -- a_nodes.h:49

   type Unit_Classes is 
     (Not_A_Class,
      A_Public_Declaration,
      A_Public_Body,
      A_Public_Declaration_And_Body,
      A_Private_Declaration,
      A_Private_Body,
      A_Separate_Body);
   pragma Convention (C, Unit_Classes);  -- a_nodes.h:116

   type Unit_Origins is 
     (Not_An_Origin,
      A_Predefined_Unit,
      An_Implementation_Unit,
      An_Application_Unit);
   pragma Convention (C, Unit_Origins);  -- a_nodes.h:137

   type Unit_Struct is record
      ID : aliased Unit_ID;  -- a_nodes.h:160
      Unit_Kind : aliased Unit_Kinds;  -- a_nodes.h:161
      Unit_Class : aliased Unit_Classes;  -- a_nodes.h:162
      Unit_Origin : aliased Unit_Origins;  -- a_nodes.h:163
      Corresponding_Children : aliased Unit_List;  -- a_nodes.h:167
      Corresponding_Parent_Declaration : aliased Unit_ID;  -- a_nodes.h:186
      Corresponding_Declaration : aliased Unit_ID;  -- a_nodes.h:191
      Corresponding_Body : aliased Unit_ID;  -- a_nodes.h:199
      Unit_Full_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:201
      Unique_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:202
      Exists : aliased Extensions.bool;  -- a_nodes.h:203
      Can_Be_Main_Program : aliased Extensions.bool;  -- a_nodes.h:204
      Is_Body_Required : aliased Extensions.bool;  -- a_nodes.h:205
      Text_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:206
      Text_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:207
      Object_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:208
      Object_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:209
      Compilation_Command_Line_Options : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:210
      Subunits : aliased Unit_List;  -- a_nodes.h:219
      Corresponding_Subunit_Parent_Body : aliased Unit_ID;  -- a_nodes.h:225
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:227
   end record;
   pragma Convention (C_Pass_By_Copy, Unit_Struct);  -- a_nodes.h:157

   subtype Element_ID is Node_ID;  -- a_nodes.h:237

   subtype Declaration_ID is Element_ID;  -- a_nodes.h:238

   subtype Defining_Name_ID is Element_ID;  -- a_nodes.h:239

   subtype Discrete_Range_ID is Element_ID;  -- a_nodes.h:240

   subtype Discrete_Subtype_Definition_ID is Element_ID;  -- a_nodes.h:241

   subtype Expression_ID is Element_ID;  -- a_nodes.h:242

   subtype Name_ID is Element_ID;  -- a_nodes.h:243

   subtype Statement_ID is Element_ID;  -- a_nodes.h:244

   subtype Subtype_Indication_ID is Element_ID;  -- a_nodes.h:245

   subtype Type_Definition_ID is Element_ID;  -- a_nodes.h:246

   type Element_ID_Ptr is access all Element_ID;  -- a_nodes.h:263

   type Element_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:268
      IDs : Element_ID_Ptr;  -- a_nodes.h:269
   end record;
   pragma Convention (C_Pass_By_Copy, Element_ID_Array_Struct);  -- a_nodes.h:267

   subtype Element_List is Element_ID_Array_Struct;  -- a_nodes.h:271

   subtype Association_List is Element_ID_Array_Struct;  -- a_nodes.h:272

   subtype Component_Clause_List is Element_ID_Array_Struct;  -- a_nodes.h:273

   subtype Declaration_List is Element_ID_Array_Struct;  -- a_nodes.h:274

   subtype Declarative_Item_List is Element_ID_Array_Struct;  -- a_nodes.h:275

   subtype Defining_Name_List is Element_ID_Array_Struct;  -- a_nodes.h:276

   subtype Exception_Handler_List is Element_ID_Array_Struct;  -- a_nodes.h:277

   subtype Expression_List is Element_ID_Array_Struct;  -- a_nodes.h:278

   subtype Expression_Path_List is Element_ID_Array_Struct;  -- a_nodes.h:279

   subtype Name_List is Element_ID_Array_Struct;  -- a_nodes.h:280

   subtype Path_List is Element_ID_Array_Struct;  -- a_nodes.h:281

   subtype Parameter_Specification_List is Element_ID_Array_Struct;  -- a_nodes.h:282

   subtype Representation_Clause_List is Element_ID_Array_Struct;  -- a_nodes.h:283

   subtype Statement_List is Element_ID_Array_Struct;  -- a_nodes.h:284

   type Element_Kinds is 
     (Not_An_Element,
      A_Pragma,
      A_Defining_Name,
      A_Declaration,
      A_Definition,
      An_Expression,
      An_Association,
      A_Statement,
      A_Path,
      A_Clause,
      An_Exception_Handler);
   pragma Convention (C, Element_Kinds);  -- a_nodes.h:286

   type Pragma_Kinds is 
     (Not_A_Pragma,
      An_All_Calls_Remote_Pragma,
      An_Assert_Pragma,
      An_Assertion_Policy_Pragma,
      An_Asynchronous_Pragma,
      An_Atomic_Pragma,
      An_Atomic_Components_Pragma,
      An_Attach_Handler_Pragma,
      A_Controlled_Pragma,
      A_Convention_Pragma,
      A_CPU_Pragma,
      A_Default_Storage_Pool_Pragma,
      A_Detect_Blocking_Pragma,
      A_Discard_Names_Pragma,
      A_Dispatching_Domain_Pragma,
      An_Elaborate_Pragma,
      An_Elaborate_All_Pragma,
      An_Elaborate_Body_Pragma,
      An_Export_Pragma,
      An_Independent_Pragma,
      A_Independent_Components_Pragma,
      An_Import_Pragma,
      An_Inline_Pragma,
      An_Inspection_Point_Pragma,
      An_Interrupt_Handler_Pragma,
      An_Interrupt_Priority_Pragma,
      A_Linker_Options_Pragma,
      A_List_Pragma,
      A_Locking_Policy_Pragma,
      A_No_Return_Pragma,
      A_Normalize_Scalars_Pragma,
      An_Optimize_Pragma,
      A_Pack_Pragma,
      A_Page_Pragma,
      A_Partition_Elaboration_Policy_Pragma,
      A_Preelaborable_Initialization_Pragma,
      A_Preelaborate_Pragma,
      A_Priority_Pragma,
      A_Priority_Specific_Dispatching_Pragma,
      A_Profile_Pragma,
      A_Pure_Pragma,
      A_Queuing_Policy_Pragma,
      A_Relative_Deadline_Pragma,
      A_Remote_Call_Interface_Pragma,
      A_Remote_Types_Pragma,
      A_Restrictions_Pragma,
      A_Reviewable_Pragma,
      A_Shared_Passive_Pragma,
      A_Storage_Size_Pragma,
      A_Suppress_Pragma,
      A_Task_Dispatching_Policy_Pragma,
      An_Unchecked_Union_Pragma,
      An_Unsuppress_Pragma,
      A_Volatile_Pragma,
      A_Volatile_Components_Pragma,
      An_Implementation_Defined_Pragma);
   pragma Convention (C, Pragma_Kinds);  -- a_nodes.h:304

   type Defining_Name_Kinds is 
     (Not_A_Defining_Name,
      A_Defining_Identifier,
      A_Defining_Character_Literal,
      A_Defining_Enumeration_Literal,
      A_Defining_Operator_Symbol,
      A_Defining_Expanded_Name);
   pragma Convention (C, Defining_Name_Kinds);  -- a_nodes.h:364

   type Declaration_Kinds is 
     (Not_A_Declaration,
      An_Ordinary_Type_Declaration,
      A_Task_Type_Declaration,
      A_Protected_Type_Declaration,
      An_Incomplete_Type_Declaration,
      A_Tagged_Incomplete_Type_Declaration,
      A_Private_Type_Declaration,
      A_Private_Extension_Declaration,
      A_Subtype_Declaration,
      A_Variable_Declaration,
      A_Constant_Declaration,
      A_Deferred_Constant_Declaration,
      A_Single_Task_Declaration,
      A_Single_Protected_Declaration,
      An_Integer_Number_Declaration,
      A_Real_Number_Declaration,
      An_Enumeration_Literal_Specification,
      A_Discriminant_Specification,
      A_Component_Declaration,
      A_Loop_Parameter_Specification,
      A_Generalized_Iterator_Specification,
      An_Element_Iterator_Specification,
      A_Procedure_Declaration,
      A_Function_Declaration,
      A_Parameter_Specification,
      A_Procedure_Body_Declaration,
      A_Function_Body_Declaration,
      A_Return_Variable_Specification,
      A_Return_Constant_Specification,
      A_Null_Procedure_Declaration,
      An_Expression_Function_Declaration,
      A_Package_Declaration,
      A_Package_Body_Declaration,
      An_Object_Renaming_Declaration,
      An_Exception_Renaming_Declaration,
      A_Package_Renaming_Declaration,
      A_Procedure_Renaming_Declaration,
      A_Function_Renaming_Declaration,
      A_Generic_Package_Renaming_Declaration,
      A_Generic_Procedure_Renaming_Declaration,
      A_Generic_Function_Renaming_Declaration,
      A_Task_Body_Declaration,
      A_Protected_Body_Declaration,
      An_Entry_Declaration,
      An_Entry_Body_Declaration,
      An_Entry_Index_Specification,
      A_Procedure_Body_Stub,
      A_Function_Body_Stub,
      A_Package_Body_Stub,
      A_Task_Body_Stub,
      A_Protected_Body_Stub,
      An_Exception_Declaration,
      A_Choice_Parameter_Specification,
      A_Generic_Procedure_Declaration,
      A_Generic_Function_Declaration,
      A_Generic_Package_Declaration,
      A_Package_Instantiation,
      A_Procedure_Instantiation,
      A_Function_Instantiation,
      A_Formal_Object_Declaration,
      A_Formal_Type_Declaration,
      A_Formal_Incomplete_Type_Declaration,
      A_Formal_Procedure_Declaration,
      A_Formal_Function_Declaration,
      A_Formal_Package_Declaration,
      A_Formal_Package_Declaration_With_Box);
   pragma Convention (C, Declaration_Kinds);  -- a_nodes.h:373

   type Definition_Kinds is 
     (Not_A_Definition,
      A_Type_Definition,
      A_Subtype_Indication,
      A_Constraint,
      A_Component_Definition,
      A_Discrete_Subtype_Definition,
      A_Discrete_Range,
      An_Unknown_Discriminant_Part,
      A_Known_Discriminant_Part,
      A_Record_Definition,
      A_Null_Record_Definition,
      A_Null_Component,
      A_Variant_Part,
      A_Variant,
      An_Others_Choice,
      An_Access_Definition,
      A_Private_Type_Definition,
      A_Tagged_Private_Type_Definition,
      A_Private_Extension_Definition,
      A_Task_Definition,
      A_Protected_Definition,
      A_Formal_Type_Definition,
      An_Aspect_Specification);
   pragma Convention (C, Definition_Kinds);  -- a_nodes.h:477

   subtype Constraint_ID is Element_ID;  -- a_nodes.h:523

   subtype Definition_ID is Element_ID;  -- a_nodes.h:524

   subtype Definition_ID_List is Element_ID_Array_Struct;  -- a_nodes.h:525

   subtype Declarative_Item_ID_List is Element_ID_Array_Struct;  -- a_nodes.h:526

   subtype Record_Component_List is Element_ID_Array_Struct;  -- a_nodes.h:527

   type Type_Kinds is 
     (Not_A_Type_Definition,
      A_Derived_Type_Definition,
      A_Derived_Record_Extension_Definition,
      An_Enumeration_Type_Definition,
      A_Signed_Integer_Type_Definition,
      A_Modular_Type_Definition,
      A_Root_Type_Definition,
      A_Floating_Point_Definition,
      An_Ordinary_Fixed_Point_Definition,
      A_Decimal_Fixed_Point_Definition,
      An_Unconstrained_Array_Definition,
      A_Constrained_Array_Definition,
      A_Record_Type_Definition,
      A_Tagged_Record_Type_Definition,
      An_Interface_Type_Definition,
      An_Access_Type_Definition);
   pragma Convention (C, Type_Kinds);  -- a_nodes.h:529

   type Constraint_Kinds is 
     (Not_A_Constraint,
      A_Range_Attribute_Reference,
      A_Simple_Expression_Range,
      A_Digits_Constraint,
      A_Delta_Constraint,
      An_Index_Constraint,
      A_Discriminant_Constraint);
   pragma Convention (C, Constraint_Kinds);  -- a_nodes.h:559

   type Expression_Kinds is 
     (Not_An_Expression,
      A_Box_Expression,
      An_Integer_Literal,
      A_Real_Literal,
      A_String_Literal,
      An_Identifier,
      An_Operator_Symbol,
      A_Character_Literal,
      An_Enumeration_Literal,
      An_Explicit_Dereference,
      A_Function_Call,
      An_Indexed_Component,
      A_Slice,
      A_Selected_Component,
      An_Attribute_Reference,
      A_Record_Aggregate,
      An_Extension_Aggregate,
      A_Positional_Array_Aggregate,
      A_Named_Array_Aggregate,
      An_And_Then_Short_Circuit,
      An_Or_Else_Short_Circuit,
      An_In_Membership_Test,
      A_Not_In_Membership_Test,
      A_Null_Literal,
      A_Parenthesized_Expression,
      A_Raise_Expression,
      A_Type_Conversion,
      A_Qualified_Expression,
      An_Allocation_From_Subtype,
      An_Allocation_From_Qualified_Expression,
      A_Case_Expression,
      An_If_Expression,
      A_For_All_Quantified_Expression,
      A_For_Some_Quantified_Expression);
   pragma Convention (C, Expression_Kinds);  -- a_nodes.h:572

   type Association_Kinds is 
     (Not_An_Association,
      A_Pragma_Argument_Association,
      A_Discriminant_Association,
      A_Record_Component_Association,
      An_Array_Component_Association,
      A_Parameter_Association,
      A_Generic_Association);
   pragma Convention (C, Association_Kinds);  -- a_nodes.h:620

   type Statement_Kinds is 
     (Not_A_Statement,
      A_Null_Statement,
      An_Assignment_Statement,
      An_If_Statement,
      A_Case_Statement,
      A_Loop_Statement,
      A_While_Loop_Statement,
      A_For_Loop_Statement,
      A_Block_Statement,
      An_Exit_Statement,
      A_Goto_Statement,
      A_Procedure_Call_Statement,
      A_Return_Statement,
      An_Extended_Return_Statement,
      An_Accept_Statement,
      An_Entry_Call_Statement,
      A_Requeue_Statement,
      A_Requeue_Statement_With_Abort,
      A_Delay_Until_Statement,
      A_Delay_Relative_Statement,
      A_Terminate_Alternative_Statement,
      A_Selective_Accept_Statement,
      A_Timed_Entry_Call_Statement,
      A_Conditional_Entry_Call_Statement,
      An_Asynchronous_Select_Statement,
      An_Abort_Statement,
      A_Raise_Statement,
      A_Code_Statement);
   pragma Convention (C, Statement_Kinds);  -- a_nodes.h:631

   type Path_Kinds is 
     (Not_A_Path,
      An_If_Path,
      An_Elsif_Path,
      An_Else_Path,
      A_Case_Path,
      A_Select_Path,
      An_Or_Path,
      A_Then_Abort_Path,
      A_Case_Expression_Path,
      An_If_Expression_Path,
      An_Elsif_Expression_Path,
      An_Else_Expression_Path);
   pragma Convention (C, Path_Kinds);  -- a_nodes.h:672

   type Clause_Kinds is 
     (Not_A_Clause,
      A_Use_Package_Clause,
      A_Use_Type_Clause,
      A_Use_All_Type_Clause,
      A_With_Clause,
      A_Representation_Clause,
      A_Component_Clause);
   pragma Convention (C, Clause_Kinds);  -- a_nodes.h:733

   type Operator_Kinds is 
     (Not_An_Operator,
      An_And_Operator,
      An_Or_Operator,
      An_Xor_Operator,
      An_Equal_Operator,
      A_Not_Equal_Operator,
      A_Less_Than_Operator,
      A_Less_Than_Or_Equal_Operator,
      A_Greater_Than_Operator,
      A_Greater_Than_Or_Equal_Operator,
      A_Plus_Operator,
      A_Minus_Operator,
      A_Concatenate_Operator,
      A_Unary_Plus_Operator,
      A_Unary_Minus_Operator,
      A_Multiply_Operator,
      A_Divide_Operator,
      A_Mod_Operator,
      A_Rem_Operator,
      An_Exponentiate_Operator,
      An_Abs_Operator,
      A_Not_Operator);
   pragma Convention (C, Operator_Kinds);  -- a_nodes.h:759

   type Declaration_Origins is 
     (Not_A_Declaration_Origin,
      An_Explicit_Declaration,
      An_Implicit_Predefined_Declaration,
      An_Implicit_Inherited_Declaration);
   pragma Convention (C, Declaration_Origins);  -- a_nodes.h:786

   type Mode_Kinds is 
     (Not_A_Mode,
      A_Default_In_Mode,
      An_In_Mode,
      An_Out_Mode,
      An_In_Out_Mode);
   pragma Convention (C, Mode_Kinds);  -- a_nodes.h:802

   type Subprogram_Default_Kinds is 
     (Not_A_Default,
      A_Name_Default,
      A_Box_Default,
      A_Null_Default,
      A_Nil_Default);
   pragma Convention (C, Subprogram_Default_Kinds);  -- a_nodes.h:812

   type Trait_Kinds is 
     (Not_A_Trait,
      An_Ordinary_Trait,
      An_Aliased_Trait,
      An_Access_Definition_Trait,
      A_Null_Exclusion_Trait,
      A_Reverse_Trait,
      A_Private_Trait,
      A_Limited_Trait,
      A_Limited_Private_Trait,
      An_Abstract_Trait,
      An_Abstract_Private_Trait,
      An_Abstract_Limited_Trait,
      An_Abstract_Limited_Private_Trait);
   pragma Convention (C, Trait_Kinds);  -- a_nodes.h:824

   type Attribute_Kinds is 
     (Not_An_Attribute,
      An_Access_Attribute,
      An_Address_Attribute,
      An_Adjacent_Attribute,
      An_Aft_Attribute,
      An_Alignment_Attribute,
      A_Base_Attribute,
      A_Bit_Order_Attribute,
      A_Body_Version_Attribute,
      A_Callable_Attribute,
      A_Caller_Attribute,
      A_Ceiling_Attribute,
      A_Class_Attribute,
      A_Component_Size_Attribute,
      A_Compose_Attribute,
      A_Constrained_Attribute,
      A_Copy_Sign_Attribute,
      A_Count_Attribute,
      A_Definite_Attribute,
      A_Delta_Attribute,
      A_Denorm_Attribute,
      A_Digits_Attribute,
      An_Exponent_Attribute,
      An_External_Tag_Attribute,
      A_First_Attribute,
      A_First_Bit_Attribute,
      A_Floor_Attribute,
      A_Fore_Attribute,
      A_Fraction_Attribute,
      An_Identity_Attribute,
      An_Image_Attribute,
      An_Input_Attribute,
      A_Last_Attribute,
      A_Last_Bit_Attribute,
      A_Leading_Part_Attribute,
      A_Length_Attribute,
      A_Machine_Attribute,
      A_Machine_Emax_Attribute,
      A_Machine_Emin_Attribute,
      A_Machine_Mantissa_Attribute,
      A_Machine_Overflows_Attribute,
      A_Machine_Radix_Attribute,
      A_Machine_Rounds_Attribute,
      A_Max_Attribute,
      A_Max_Size_In_Storage_Elements_Attribute,
      A_Min_Attribute,
      A_Model_Attribute,
      A_Model_Emin_Attribute,
      A_Model_Epsilon_Attribute,
      A_Model_Mantissa_Attribute,
      A_Model_Small_Attribute,
      A_Modulus_Attribute,
      An_Output_Attribute,
      A_Partition_ID_Attribute,
      A_Pos_Attribute,
      A_Position_Attribute,
      A_Pred_Attribute,
      A_Range_Attribute,
      A_Read_Attribute,
      A_Remainder_Attribute,
      A_Round_Attribute,
      A_Rounding_Attribute,
      A_Safe_First_Attribute,
      A_Safe_Last_Attribute,
      A_Scale_Attribute,
      A_Scaling_Attribute,
      A_Signed_Zeros_Attribute,
      A_Size_Attribute,
      A_Small_Attribute,
      A_Storage_Pool_Attribute,
      A_Storage_Size_Attribute,
      A_Succ_Attribute,
      A_Tag_Attribute,
      A_Terminated_Attribute,
      A_Truncation_Attribute,
      An_Unbiased_Rounding_Attribute,
      An_Unchecked_Access_Attribute,
      A_Val_Attribute,
      A_Valid_Attribute,
      A_Value_Attribute,
      A_Version_Attribute,
      A_Wide_Image_Attribute,
      A_Wide_Value_Attribute,
      A_Wide_Width_Attribute,
      A_Width_Attribute,
      A_Write_Attribute,
      A_Machine_Rounding_Attribute,
      A_Mod_Attribute,
      A_Priority_Attribute,
      A_Stream_Size_Attribute,
      A_Wide_Wide_Image_Attribute,
      A_Wide_Wide_Value_Attribute,
      A_Wide_Wide_Width_Attribute,
      A_Max_Alignment_For_Allocation_Attribute,
      An_Overlaps_Storage_Attribute,
      An_Implementation_Defined_Attribute,
      An_Unknown_Attribute);
   pragma Convention (C, Attribute_Kinds);  -- a_nodes.h:869

   type Enclosing_Kinds is 
     (Not_Enclosing,
      Enclosing_Element,
      Enclosing_Unit);
   pragma Convention (C, Enclosing_Kinds);  -- a_nodes.h:981

   type Pragma_Struct is record
      Pragma_Kind : aliased Pragma_Kinds;  -- a_nodes.h:993
   end record;
   pragma Convention (C_Pass_By_Copy, Pragma_Struct);  -- a_nodes.h:992

   type Defining_Name_Struct is record
      Defining_Name_Kind : aliased Defining_Name_Kinds;  -- a_nodes.h:998
      Defining_Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:999
      Position_Number_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1004
      Representation_Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1005
      Defining_Prefix : aliased Name_ID;  -- a_nodes.h:1007
      Defining_Selector : aliased Defining_Name_ID;  -- a_nodes.h:1008
      Corresponding_Constant_Declaration : aliased Declaration_ID;  -- a_nodes.h:1010
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:1012
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:1015
   end record;
   pragma Convention (C_Pass_By_Copy, Defining_Name_Struct);  -- a_nodes.h:997

   type Declaration_Struct is record
      Declaration_Kind : aliased Declaration_Kinds;  -- a_nodes.h:1020
      Declaration_Origin : aliased Declaration_Origins;  -- a_nodes.h:1021
      Mode_Kind : aliased Mode_Kinds;  -- a_nodes.h:1026
      Default_Kind : aliased Subprogram_Default_Kinds;  -- a_nodes.h:1030
      Trait_Kind : aliased Trait_Kinds;  -- a_nodes.h:1042
      Names : aliased Defining_Name_List;  -- a_nodes.h:1046
      Discriminant_Part : aliased Definition_ID;  -- a_nodes.h:1055
      Type_Declaration_View : aliased Definition_ID;  -- a_nodes.h:1062
      Object_Declaration_View : aliased Definition_ID;  -- a_nodes.h:1074
      Aspect_Specifications : aliased Element_List;  -- a_nodes.h:1076
      Initialization_Expression : aliased Expression_ID;  -- a_nodes.h:1086
      Corresponding_Type_Declaration : aliased Declaration_ID;  -- a_nodes.h:1094
      Corresponding_Type_Completion : aliased Declaration_ID;  -- a_nodes.h:1099
      Corresponding_Type_Partial_View : aliased Declaration_ID;  -- a_nodes.h:1105
      Corresponding_First_Subtype : aliased Declaration_ID;  -- a_nodes.h:1113
      Corresponding_Last_Constraint : aliased Declaration_ID;  -- a_nodes.h:1114
      Corresponding_Last_Subtype : aliased Declaration_ID;  -- a_nodes.h:1115
      Corresponding_Representation_Clauses : aliased Representation_Clause_List;  -- a_nodes.h:1117
      Specification_Subtype_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:1120
      Iteration_Scheme_Name : aliased Element_ID;  -- a_nodes.h:1123
      Subtype_Indication : aliased Element_ID;  -- a_nodes.h:1125
      Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:1142
      Result_Profile : aliased Element_ID;  -- a_nodes.h:1150
      Result_Expression : aliased Expression_ID;  -- a_nodes.h:1152
      Is_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:1168
      Is_Not_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:1169
      Body_Declarative_Items : aliased Element_List;  -- a_nodes.h:1175
      Body_Statements : aliased Statement_List;  -- a_nodes.h:1176
      Body_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1177
      Body_Block_Statement : aliased Declaration_ID;  -- a_nodes.h:1178
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:1191
      Corresponding_Declaration : aliased Declaration_ID;  -- a_nodes.h:1227
      Corresponding_Body : aliased Declaration_ID;  -- a_nodes.h:1243
      Corresponding_Subprogram_Derivation : aliased Declaration_ID;  -- a_nodes.h:1246
      Corresponding_Type : aliased Type_Definition_ID;  -- a_nodes.h:1250
      Corresponding_Equality_Operator : aliased Declaration_ID;  -- a_nodes.h:1252
      Visible_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1255
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1256
      Private_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1257
      Declaration_Interface_List : aliased Expression_List;  -- a_nodes.h:1262
      Renamed_Entity : aliased Expression_ID;  -- a_nodes.h:1271
      Corresponding_Base_Entity : aliased Expression_ID;  -- a_nodes.h:1272
      Protected_Operation_Items : aliased Declaration_List;  -- a_nodes.h:1274
      Entry_Family_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:1276
      Entry_Index_Specification : aliased Declaration_ID;  -- a_nodes.h:1278
      Entry_Barrier : aliased Expression_ID;  -- a_nodes.h:1279
      Corresponding_Subunit : aliased Declaration_ID;  -- a_nodes.h:1285
      Is_Subunit : aliased Extensions.bool;  -- a_nodes.h:1291
      Corresponding_Body_Stub : aliased Declaration_ID;  -- a_nodes.h:1292
      Generic_Formal_Part : aliased Element_List;  -- a_nodes.h:1296
      Generic_Unit_Name : aliased Expression_ID;  -- a_nodes.h:1302
      Generic_Actual_Part : aliased Association_List;  -- a_nodes.h:1303
      Formal_Subprogram_Default : aliased Expression_ID;  -- a_nodes.h:1306
      Is_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1317
   end record;
   pragma Convention (C_Pass_By_Copy, Declaration_Struct);  -- a_nodes.h:1019

   type Definition_Struct is record
      Definition_Kind : aliased Definition_Kinds;  -- a_nodes.h:1322
      Trait_Kind : aliased Trait_Kinds;  -- a_nodes.h:1331
      Type_Kind : aliased Type_Kinds;  -- a_nodes.h:1333
      Parent_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1336
      Record_Definition : aliased Definition_ID;  -- a_nodes.h:1338
      Implicit_Inherited_Declarations : aliased Definition_ID_List;  -- a_nodes.h:1341
      Implicit_Inherited_Subprograms : aliased Definition_ID_List;  -- a_nodes.h:1342
      Corresponding_Parent_Subtype : aliased Definition_ID;  -- a_nodes.h:1343
      Corresponding_Root_Type : aliased Definition_ID;  -- a_nodes.h:1344
      Corresponding_Type_Structure : aliased Definition_ID;  -- a_nodes.h:1345
      Constraint_Kind : aliased Constraint_Kinds;  -- a_nodes.h:1347
      Lower_Bound : aliased Expression_ID;  -- a_nodes.h:1349
      Upper_Bound : aliased Expression_ID;  -- a_nodes.h:1350
      Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:1354
      Subtype_Constraint : aliased Constraint_ID;  -- a_nodes.h:1355
      Component_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1357
      Component_Definition_View : aliased Definition_ID;  -- a_nodes.h:1358
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1362
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1363
      Visible_Part_Items : aliased Declarative_Item_ID_List;  -- a_nodes.h:1365
      Private_Part_Items : aliased Declarative_Item_ID_List;  -- a_nodes.h:1366
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1367
   end record;
   pragma Convention (C_Pass_By_Copy, Definition_Struct);  -- a_nodes.h:1321

   type Expression_Struct is record
      Expression_Kind : aliased Expression_Kinds;  -- a_nodes.h:1373
      Corresponding_Expression_Type : aliased Declaration_ID;  -- a_nodes.h:1374
      Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1380
      Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1385
      Corresponding_Name_Definition : aliased Defining_Name_ID;  -- a_nodes.h:1386
      Corresponding_Name_Definition_List : aliased Defining_Name_List;  -- a_nodes.h:1387
      Corresponding_Name_Declaration : aliased Element_ID;  -- a_nodes.h:1388
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:1390
      Prefix : aliased Expression_ID;  -- a_nodes.h:1397
      Corresponding_Called_Function : aliased Declaration_ID;  -- a_nodes.h:1400
      Is_Prefix_Call : aliased Extensions.bool;  -- a_nodes.h:1402
      Function_Call_Parameters : aliased Element_List;  -- a_nodes.h:1403
      Index_Expressions : aliased Expression_List;  -- a_nodes.h:1405
      Is_Generalized_Indexing : aliased Extensions.bool;  -- a_nodes.h:1406
      Slice_Range : aliased Discrete_Range_ID;  -- a_nodes.h:1408
      Selector : aliased Expression_ID;  -- a_nodes.h:1410
      atribute_kind : aliased Attribute_Kinds;  -- a_nodes.h:1412
      Attribute_Designator_Identifier : aliased Expression_ID;  -- a_nodes.h:1413
      Attribute_Designator_Expressions : aliased Expression_List;  -- a_nodes.h:1414
      Record_Component_Associations : aliased Association_List;  -- a_nodes.h:1417
      Extension_Aggregate_Expression : aliased Expression_ID;  -- a_nodes.h:1419
      Array_Component_Associations : aliased Association_List;  -- a_nodes.h:1422
      Short_Circuit_Operation_Left_Expression : aliased Expression_ID;  -- a_nodes.h:1425
      Short_Circuit_Operation_Right_Expression : aliased Expression_ID;  -- a_nodes.h:1426
      Membership_Test_Expression : aliased Expression_ID;  -- a_nodes.h:1429
      Membership_Test_Choices : aliased Element_List;  -- a_nodes.h:1430
      Expression_Parenthesized : aliased Expression_ID;  -- a_nodes.h:1432
      Converted_Or_Qualified_Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:1435
      Converted_Or_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1436
      Predicate : aliased Expression_ID;  -- a_nodes.h:1437
      Subpool_Name : aliased Expression_ID;  -- a_nodes.h:1440
      Allocator_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1442
      Allocator_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1444
      Expression_Paths : aliased Expression_Path_List;  -- a_nodes.h:1447
      Iterator_Specification : aliased Declaration_ID;  -- a_nodes.h:1450
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:1453
   end record;
   pragma Convention (C_Pass_By_Copy, Expression_Struct);  -- a_nodes.h:1372

   type Association_Struct is record
      Association_Kind : aliased Association_Kinds;  -- a_nodes.h:1458
      Array_Component_Choices : aliased Expression_List;  -- a_nodes.h:1461
      Record_Component_Choices : aliased Expression_List;  -- a_nodes.h:1463
      Component_Expression : aliased Expression_ID;  -- a_nodes.h:1466
      Formal_Parameter : aliased Expression_ID;  -- a_nodes.h:1470
      Actual_Parameter : aliased Expression_ID;  -- a_nodes.h:1471
      Discriminant_Selector_Names : aliased Expression_List;  -- a_nodes.h:1473
      Discriminant_Expression : aliased Expression_ID;  -- a_nodes.h:1474
      Is_Normalized : aliased Extensions.bool;  -- a_nodes.h:1479
      Is_Defaulted_Association : aliased Extensions.bool;  -- a_nodes.h:1483
   end record;
   pragma Convention (C_Pass_By_Copy, Association_Struct);  -- a_nodes.h:1457

   type Statement_Struct is record
      Statement_Kind : aliased Statement_Kinds;  -- a_nodes.h:1488
      Label_Names : aliased Defining_Name_List;  -- a_nodes.h:1489
      Assignment_Variable_Name : aliased Expression_ID;  -- a_nodes.h:1493
      Assignment_Expression : aliased Expression_ID;  -- a_nodes.h:1494
      Statement_Paths : aliased Path_List;  -- a_nodes.h:1501
      Case_Expression : aliased Expression_ID;  -- a_nodes.h:1503
      Statement_Identifier : aliased Defining_Name_ID;  -- a_nodes.h:1508
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:1512
      While_Condition : aliased Expression_ID;  -- a_nodes.h:1514
      For_Loop_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1516
      Loop_Statements : aliased Statement_List;  -- a_nodes.h:1520
      Is_Declare_Block : aliased Extensions.bool;  -- a_nodes.h:1522
      Block_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1523
      Block_Statements : aliased Statement_List;  -- a_nodes.h:1524
      Block_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1525
      Exit_Loop_Name : aliased Expression_ID;  -- a_nodes.h:1527
      Exit_Condition : aliased Expression_ID;  -- a_nodes.h:1528
      Corresponding_Loop_Exited : aliased Expression_ID;  -- a_nodes.h:1529
      Return_Expression : aliased Expression_ID;  -- a_nodes.h:1531
      Return_Object_Declaration : aliased Declaration_ID;  -- a_nodes.h:1534
      Extended_Return_Statements : aliased Statement_List;  -- a_nodes.h:1535
      Extended_Return_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1536
      Goto_Label : aliased Expression_ID;  -- a_nodes.h:1538
      Corresponding_Destination_Statement : aliased Statement_ID;  -- a_nodes.h:1539
      Called_Name : aliased Expression_ID;  -- a_nodes.h:1542
      Corresponding_Called_Entity : aliased Declaration_ID;  -- a_nodes.h:1543
      Call_Statement_Parameters : aliased Association_List;  -- a_nodes.h:1544
      Accept_Entry_Index : aliased Expression_ID;  -- a_nodes.h:1547
      Accept_Entry_Direct_Name : aliased Name_ID;  -- a_nodes.h:1548
      Accept_Parameters : aliased Parameter_Specification_List;  -- a_nodes.h:1550
      Accept_Body_Statements : aliased Statement_List;  -- a_nodes.h:1551
      Accept_Body_Exception_Handlers : aliased Statement_List;  -- a_nodes.h:1552
      Corresponding_Entry : aliased Declaration_ID;  -- a_nodes.h:1553
      Requeue_Entry_Name : aliased Name_ID;  -- a_nodes.h:1556
      Delay_Expression : aliased Expression_ID;  -- a_nodes.h:1559
      Aborted_Tasks : aliased Expression_List;  -- a_nodes.h:1561
      Raised_Exception : aliased Expression_ID;  -- a_nodes.h:1563
      Associated_Message : aliased Expression_ID;  -- a_nodes.h:1564
      Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1566
   end record;
   pragma Convention (C_Pass_By_Copy, Statement_Struct);  -- a_nodes.h:1487

   type Path_Struct is record
      Path_Kind : aliased Path_Kinds;  -- a_nodes.h:1571
      Condition_Expression : aliased Expression_ID;  -- a_nodes.h:1575
      Sequence_Of_Statements : aliased Statement_List;  -- a_nodes.h:1577
      Case_Path_Alternative_Choices : aliased Element_List;  -- a_nodes.h:1580
      Guard : aliased Expression_ID;  -- a_nodes.h:1583
   end record;
   pragma Convention (C_Pass_By_Copy, Path_Struct);  -- a_nodes.h:1570

   type Clause_Struct is record
      Clause_Kind : aliased Clause_Kinds;  -- a_nodes.h:1588
      Clause_Names : aliased Name_List;  -- a_nodes.h:1594
      Representation_Clause_Name : aliased Name_ID;  -- a_nodes.h:1597
      Representation_Clause_Expression : aliased Expression_ID;  -- a_nodes.h:1604
      Mod_Clause_Expression : aliased Expression_ID;  -- a_nodes.h:1609
      Component_Clauses : aliased Element_List;  -- a_nodes.h:1610
      Component_Clause_Position : aliased Expression_ID;  -- a_nodes.h:1612
      Component_Clause_Range : aliased Element_ID;  -- a_nodes.h:1613
      Trait_Kind : aliased Trait_Kinds;  -- a_nodes.h:1616
   end record;
   pragma Convention (C_Pass_By_Copy, Clause_Struct);  -- a_nodes.h:1587

   type Exception_Handler_Struct is record
      Choice_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1621
      Exception_Choices : aliased Element_List;  -- a_nodes.h:1622
      Handler_Statements : aliased Statement_List;  -- a_nodes.h:1623
   end record;
   pragma Convention (C_Pass_By_Copy, Exception_Handler_Struct);  -- a_nodes.h:1620

   type Element_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1628
         when 1 =>
            The_Pragma : aliased Pragma_Struct;  -- a_nodes.h:1629
         when 2 =>
            Defining_Name : aliased Defining_Name_Struct;  -- a_nodes.h:1630
         when 3 =>
            Declaration : aliased Declaration_Struct;  -- a_nodes.h:1631
         when 4 =>
            Definition : aliased Definition_Struct;  -- a_nodes.h:1632
         when 5 =>
            Expression : aliased Expression_Struct;  -- a_nodes.h:1633
         when 6 =>
            Association : aliased Association_Struct;  -- a_nodes.h:1634
         when 7 =>
            Statement : aliased Statement_Struct;  -- a_nodes.h:1635
         when 8 =>
            Path : aliased Path_Struct;  -- a_nodes.h:1636
         when 9 =>
            Clause : aliased Clause_Struct;  -- a_nodes.h:1637
         when others =>
            Exception_Handler : aliased Exception_Handler_Struct;  -- a_nodes.h:1638
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Union);
   pragma Unchecked_Union (Element_Union);  -- a_nodes.h:1627

   type Source_Location_Struct is record
      Unit_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1643
      First_Line : aliased int;  -- a_nodes.h:1644
      First_Column : aliased int;  -- a_nodes.h:1645
      Last_Line : aliased int;  -- a_nodes.h:1646
      Last_Column : aliased int;  -- a_nodes.h:1647
   end record;
   pragma Convention (C_Pass_By_Copy, Source_Location_Struct);  -- a_nodes.h:1642

   type Element_Struct is record
      ID : aliased Element_ID;  -- a_nodes.h:1653
      Element_Kind : aliased Element_Kinds;  -- a_nodes.h:1654
      Enclosing_Element_ID : aliased Node_ID;  -- a_nodes.h:1655
      Enclosing_Kind : aliased Enclosing_Kinds;  -- a_nodes.h:1656
      Source_Location : aliased Source_Location_Struct;  -- a_nodes.h:1657
      The_Union : aliased Element_Union;  -- a_nodes.h:1658
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Struct);  -- a_nodes.h:1651

   type Node_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1667
         when 1 =>
            Context : aliased Context_Struct;  -- a_nodes.h:1668
         when 2 =>
            Unit : aliased Unit_Struct;  -- a_nodes.h:1669
         when others =>
            Element : aliased Element_Struct;  -- a_nodes.h:1670
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Node_Union);
   pragma Unchecked_Union (Node_Union);  -- a_nodes.h:1666

   type Node_Struct is record
      Node_Kind : aliased Node_Kinds;  -- a_nodes.h:1675
      The_Union : aliased Node_Union;  -- a_nodes.h:1676
   end record;
   pragma Convention (C_Pass_By_Copy, Node_Struct);  -- a_nodes.h:1674

   type List_Node_Struct is record
      Node : aliased Node_Struct;  -- a_nodes.h:1681
      Next : access List_Node_Struct;  -- a_nodes.h:1682
      Next_Count : aliased int;  -- a_nodes.h:1684
   end record;
   pragma Convention (C_Pass_By_Copy, List_Node_Struct);  -- a_nodes.h:1680

   type Node_List_Ptr is access all List_Node_Struct;  -- a_nodes.h:1687

end a_nodes_h;
