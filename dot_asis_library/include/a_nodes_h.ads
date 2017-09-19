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
      ID : aliased Unit_ID;  -- a_nodes.h:158
      Unit_Kind : aliased Unit_Kinds;  -- a_nodes.h:159
      Unit_Class : aliased Unit_Classes;  -- a_nodes.h:160
      Unit_Origin : aliased Unit_Origins;  -- a_nodes.h:161
      Corresponding_Children : aliased Unit_List;  -- a_nodes.h:164
      Corresponding_Parent_Declaration : aliased Unit_ID;  -- a_nodes.h:165
      Corresponding_Declaration : aliased Unit_ID;  -- a_nodes.h:166
      Corresponding_Body : aliased Unit_ID;  -- a_nodes.h:167
      Unit_Full_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:168
      Unique_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:169
      Exists : aliased Extensions.bool;  -- a_nodes.h:170
      Can_Be_Main_Program : aliased Extensions.bool;  -- a_nodes.h:171
      Is_Body_Required : aliased Extensions.bool;  -- a_nodes.h:172
      Text_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:173
      Text_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:174
      Object_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:175
      Object_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:176
      Compilation_Command_Line_Options : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:177
      Subunits : aliased Unit_List;  -- a_nodes.h:178
      Corresponding_Subunit_Parent_Body : aliased Unit_ID;  -- a_nodes.h:179
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:180
   end record;
   pragma Convention (C_Pass_By_Copy, Unit_Struct);  -- a_nodes.h:157

   subtype Element_ID is Node_ID;  -- a_nodes.h:190

   subtype Declaration_ID is Element_ID;  -- a_nodes.h:191

   subtype Defining_Name_ID is Element_ID;  -- a_nodes.h:192

   subtype Discrete_Range_ID is Element_ID;  -- a_nodes.h:193

   subtype Discrete_Subtype_Definition_ID is Element_ID;  -- a_nodes.h:194

   subtype Expression_ID is Element_ID;  -- a_nodes.h:195

   subtype Name_ID is Element_ID;  -- a_nodes.h:196

   subtype Statement_ID is Element_ID;  -- a_nodes.h:197

   subtype Subtype_Indication_ID is Element_ID;  -- a_nodes.h:198

   subtype Type_Definition_ID is Element_ID;  -- a_nodes.h:199

   type Element_ID_Ptr is access all Element_ID;  -- a_nodes.h:216

   type Element_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:221
      IDs : Element_ID_Ptr;  -- a_nodes.h:222
   end record;
   pragma Convention (C_Pass_By_Copy, Element_ID_Array_Struct);  -- a_nodes.h:220

   subtype Element_List is Element_ID_Array_Struct;  -- a_nodes.h:224

   subtype Association_List is Element_ID_Array_Struct;  -- a_nodes.h:225

   subtype Component_Clause_List is Element_ID_Array_Struct;  -- a_nodes.h:226

   subtype Declaration_List is Element_ID_Array_Struct;  -- a_nodes.h:227

   subtype Declarative_Item_List is Element_ID_Array_Struct;  -- a_nodes.h:228

   subtype Defining_Name_List is Element_ID_Array_Struct;  -- a_nodes.h:229

   subtype Exception_Handler_List is Element_ID_Array_Struct;  -- a_nodes.h:230

   subtype Expression_List is Element_ID_Array_Struct;  -- a_nodes.h:231

   subtype Expression_Path_List is Element_ID_Array_Struct;  -- a_nodes.h:232

   subtype Name_List is Element_ID_Array_Struct;  -- a_nodes.h:233

   subtype Path_List is Element_ID_Array_Struct;  -- a_nodes.h:234

   subtype Parameter_Specification_List is Element_ID_Array_Struct;  -- a_nodes.h:235

   subtype Representation_Clause_List is Element_ID_Array_Struct;  -- a_nodes.h:236

   subtype Statement_List is Element_ID_Array_Struct;  -- a_nodes.h:237

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
   pragma Convention (C, Element_Kinds);  -- a_nodes.h:239

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
   pragma Convention (C, Pragma_Kinds);  -- a_nodes.h:257

   type Defining_Name_Kinds is 
     (Not_A_Defining_Name,
      A_Defining_Identifier,
      A_Defining_Character_Literal,
      A_Defining_Enumeration_Literal,
      A_Defining_Operator_Symbol,
      A_Defining_Expanded_Name);
   pragma Convention (C, Defining_Name_Kinds);  -- a_nodes.h:317

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
   pragma Convention (C, Declaration_Kinds);  -- a_nodes.h:326

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
   pragma Convention (C, Definition_Kinds);  -- a_nodes.h:430

   subtype Constraint_ID is Element_ID;  -- a_nodes.h:476

   subtype Definition_ID is Element_ID;  -- a_nodes.h:477

   subtype Definition_ID_List is Element_ID_Array_Struct;  -- a_nodes.h:478

   subtype Declarative_Item_ID_List is Element_ID_Array_Struct;  -- a_nodes.h:479

   subtype Record_Component_List is Element_ID_Array_Struct;  -- a_nodes.h:480

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
   pragma Convention (C, Type_Kinds);  -- a_nodes.h:482

   type Constraint_Kinds is 
     (Not_A_Constraint,
      A_Range_Attribute_Reference,
      A_Simple_Expression_Range,
      A_Digits_Constraint,
      A_Delta_Constraint,
      An_Index_Constraint,
      A_Discriminant_Constraint);
   pragma Convention (C, Constraint_Kinds);  -- a_nodes.h:512

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
   pragma Convention (C, Expression_Kinds);  -- a_nodes.h:525

   type Association_Kinds is 
     (Not_An_Association,
      A_Pragma_Argument_Association,
      A_Discriminant_Association,
      A_Record_Component_Association,
      An_Array_Component_Association,
      A_Parameter_Association,
      A_Generic_Association);
   pragma Convention (C, Association_Kinds);  -- a_nodes.h:573

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
   pragma Convention (C, Statement_Kinds);  -- a_nodes.h:584

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
   pragma Convention (C, Path_Kinds);  -- a_nodes.h:625

   type Clause_Kinds is 
     (Not_A_Clause,
      A_Use_Package_Clause,
      A_Use_Type_Clause,
      A_Use_All_Type_Clause,
      A_With_Clause,
      A_Representation_Clause,
      A_Component_Clause);
   pragma Convention (C, Clause_Kinds);  -- a_nodes.h:686

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
   pragma Convention (C, Operator_Kinds);  -- a_nodes.h:712

   type Declaration_Origins is 
     (Not_A_Declaration_Origin,
      An_Explicit_Declaration,
      An_Implicit_Predefined_Declaration,
      An_Implicit_Inherited_Declaration);
   pragma Convention (C, Declaration_Origins);  -- a_nodes.h:739

   type Mode_Kinds is 
     (Not_A_Mode,
      A_Default_In_Mode,
      An_In_Mode,
      An_Out_Mode,
      An_In_Out_Mode);
   pragma Convention (C, Mode_Kinds);  -- a_nodes.h:755

   type Subprogram_Default_Kinds is 
     (Not_A_Default,
      A_Name_Default,
      A_Box_Default,
      A_Null_Default,
      A_Nil_Default);
   pragma Convention (C, Subprogram_Default_Kinds);  -- a_nodes.h:765

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
   pragma Convention (C, Trait_Kinds);  -- a_nodes.h:777

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
   pragma Convention (C, Attribute_Kinds);  -- a_nodes.h:822

   type Enclosing_Kinds is 
     (Not_Enclosing,
      Enclosing_Element,
      Enclosing_Unit);
   pragma Convention (C, Enclosing_Kinds);  -- a_nodes.h:934

   type Pragma_Struct is record
      Pragma_Kind : aliased Pragma_Kinds;  -- a_nodes.h:946
   end record;
   pragma Convention (C_Pass_By_Copy, Pragma_Struct);  -- a_nodes.h:945

   type Defining_Name_Struct is record
      Defining_Name_Kind : aliased Defining_Name_Kinds;  -- a_nodes.h:951
      Defining_Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:952
      Position_Number_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:957
      Representation_Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:958
      Defining_Prefix : aliased Name_ID;  -- a_nodes.h:960
      Defining_Selector : aliased Defining_Name_ID;  -- a_nodes.h:961
      Corresponding_Constant_Declaration : aliased Declaration_ID;  -- a_nodes.h:963
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:965
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:968
   end record;
   pragma Convention (C_Pass_By_Copy, Defining_Name_Struct);  -- a_nodes.h:950

   type Declaration_Struct is record
      Declaration_Kind : aliased Declaration_Kinds;  -- a_nodes.h:973
      Declaration_Origin : aliased Declaration_Origins;  -- a_nodes.h:974
      Mode_Kind : aliased Mode_Kinds;  -- a_nodes.h:979
      Default_Kind : aliased Subprogram_Default_Kinds;  -- a_nodes.h:983
      Trait_Kind : aliased Trait_Kinds;  -- a_nodes.h:995
      Names : aliased Defining_Name_List;  -- a_nodes.h:999
      Discriminant_Part : aliased Definition_ID;  -- a_nodes.h:1008
      Type_Declaration_View : aliased Definition_ID;  -- a_nodes.h:1015
      Object_Declaration_View : aliased Definition_ID;  -- a_nodes.h:1027
      Aspect_Specifications : aliased Element_List;  -- a_nodes.h:1029
      Initialization_Expression : aliased Expression_ID;  -- a_nodes.h:1039
      Corresponding_Type_Declaration : aliased Declaration_ID;  -- a_nodes.h:1047
      Corresponding_Type_Completion : aliased Declaration_ID;  -- a_nodes.h:1052
      Corresponding_Type_Partial_View : aliased Declaration_ID;  -- a_nodes.h:1058
      Corresponding_First_Subtype : aliased Declaration_ID;  -- a_nodes.h:1066
      Corresponding_Last_Constraint : aliased Declaration_ID;  -- a_nodes.h:1067
      Corresponding_Last_Subtype : aliased Declaration_ID;  -- a_nodes.h:1068
      Corresponding_Representation_Clauses : aliased Representation_Clause_List;  -- a_nodes.h:1070
      Specification_Subtype_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:1073
      Iteration_Scheme_Name : aliased Element_ID;  -- a_nodes.h:1076
      Subtype_Indication : aliased Element_ID;  -- a_nodes.h:1078
      Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:1095
      Result_Profile : aliased Element_ID;  -- a_nodes.h:1103
      Result_Expression : aliased Expression_ID;  -- a_nodes.h:1105
      Is_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:1121
      Is_Not_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:1122
      Body_Declarative_Items : aliased Element_List;  -- a_nodes.h:1128
      Body_Statements : aliased Statement_List;  -- a_nodes.h:1129
      Body_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1130
      Body_Block_Statement : aliased Declaration_ID;  -- a_nodes.h:1131
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:1144
      Corresponding_Declaration : aliased Declaration_ID;  -- a_nodes.h:1180
      Corresponding_Body : aliased Declaration_ID;  -- a_nodes.h:1196
      Corresponding_Subprogram_Derivation : aliased Declaration_ID;  -- a_nodes.h:1199
      Corresponding_Type : aliased Type_Definition_ID;  -- a_nodes.h:1203
      Corresponding_Equality_Operator : aliased Declaration_ID;  -- a_nodes.h:1205
      Visible_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1208
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1209
      Private_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1210
      Declaration_Interface_List : aliased Expression_List;  -- a_nodes.h:1215
      Renamed_Entity : aliased Expression_ID;  -- a_nodes.h:1224
      Corresponding_Base_Entity : aliased Expression_ID;  -- a_nodes.h:1225
      Protected_Operation_Items : aliased Declaration_List;  -- a_nodes.h:1227
      Entry_Family_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:1229
      Entry_Index_Specification : aliased Declaration_ID;  -- a_nodes.h:1231
      Entry_Barrier : aliased Expression_ID;  -- a_nodes.h:1232
      Corresponding_Subunit : aliased Declaration_ID;  -- a_nodes.h:1238
      Is_Subunit : aliased Extensions.bool;  -- a_nodes.h:1244
      Corresponding_Body_Stub : aliased Declaration_ID;  -- a_nodes.h:1245
      Generic_Formal_Part : aliased Element_List;  -- a_nodes.h:1249
      Generic_Unit_Name : aliased Expression_ID;  -- a_nodes.h:1255
      Generic_Actual_Part : aliased Association_List;  -- a_nodes.h:1256
      Formal_Subprogram_Default : aliased Expression_ID;  -- a_nodes.h:1259
      Is_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1270
   end record;
   pragma Convention (C_Pass_By_Copy, Declaration_Struct);  -- a_nodes.h:972

   type Definition_Struct is record
      Definition_Kind : aliased Definition_Kinds;  -- a_nodes.h:1275
      Trait_Kind : aliased Trait_Kinds;  -- a_nodes.h:1284
      Type_Kind : aliased Type_Kinds;  -- a_nodes.h:1286
      Parent_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1289
      Record_Definition : aliased Definition_ID;  -- a_nodes.h:1291
      Implicit_Inherited_Declarations : aliased Definition_ID_List;  -- a_nodes.h:1294
      Implicit_Inherited_Subprograms : aliased Definition_ID_List;  -- a_nodes.h:1295
      Corresponding_Parent_Subtype : aliased Definition_ID;  -- a_nodes.h:1296
      Corresponding_Root_Type : aliased Definition_ID;  -- a_nodes.h:1297
      Corresponding_Type_Structure : aliased Definition_ID;  -- a_nodes.h:1298
      Constraint_Kind : aliased Constraint_Kinds;  -- a_nodes.h:1300
      Lower_Bound : aliased Expression_ID;  -- a_nodes.h:1302
      Upper_Bound : aliased Expression_ID;  -- a_nodes.h:1303
      Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:1307
      Subtype_Constraint : aliased Constraint_ID;  -- a_nodes.h:1308
      Component_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1310
      Component_Definition_View : aliased Definition_ID;  -- a_nodes.h:1311
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1315
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1316
      Visible_Part_Items : aliased Declarative_Item_ID_List;  -- a_nodes.h:1318
      Private_Part_Items : aliased Declarative_Item_ID_List;  -- a_nodes.h:1319
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1320
   end record;
   pragma Convention (C_Pass_By_Copy, Definition_Struct);  -- a_nodes.h:1274

   type Expression_Struct is record
      Expression_Kind : aliased Expression_Kinds;  -- a_nodes.h:1326
      Corresponding_Expression_Type : aliased Declaration_ID;  -- a_nodes.h:1327
      Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1333
      Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1338
      Corresponding_Name_Definition : aliased Defining_Name_ID;  -- a_nodes.h:1339
      Corresponding_Name_Definition_List : aliased Defining_Name_List;  -- a_nodes.h:1340
      Corresponding_Name_Declaration : aliased Element_ID;  -- a_nodes.h:1341
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:1343
      Prefix : aliased Expression_ID;  -- a_nodes.h:1350
      Corresponding_Called_Function : aliased Declaration_ID;  -- a_nodes.h:1353
      Is_Prefix_Call : aliased Extensions.bool;  -- a_nodes.h:1355
      Function_Call_Parameters : aliased Element_List;  -- a_nodes.h:1356
      Index_Expressions : aliased Expression_List;  -- a_nodes.h:1358
      Is_Generalized_Indexing : aliased Extensions.bool;  -- a_nodes.h:1359
      Slice_Range : aliased Discrete_Range_ID;  -- a_nodes.h:1361
      Selector : aliased Expression_ID;  -- a_nodes.h:1363
      atribute_kind : aliased Attribute_Kinds;  -- a_nodes.h:1365
      Attribute_Designator_Identifier : aliased Expression_ID;  -- a_nodes.h:1366
      Attribute_Designator_Expressions : aliased Expression_List;  -- a_nodes.h:1367
      Record_Component_Associations : aliased Association_List;  -- a_nodes.h:1370
      Extension_Aggregate_Expression : aliased Expression_ID;  -- a_nodes.h:1372
      Array_Component_Associations : aliased Association_List;  -- a_nodes.h:1375
      Short_Circuit_Operation_Left_Expression : aliased Expression_ID;  -- a_nodes.h:1378
      Short_Circuit_Operation_Right_Expression : aliased Expression_ID;  -- a_nodes.h:1379
      Membership_Test_Expression : aliased Expression_ID;  -- a_nodes.h:1382
      Membership_Test_Choices : aliased Element_List;  -- a_nodes.h:1383
      Expression_Parenthesized : aliased Expression_ID;  -- a_nodes.h:1385
      Converted_Or_Qualified_Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:1388
      Converted_Or_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1389
      Predicate : aliased Expression_ID;  -- a_nodes.h:1390
      Subpool_Name : aliased Expression_ID;  -- a_nodes.h:1393
      Allocator_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1395
      Allocator_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1397
      Expression_Paths : aliased Expression_Path_List;  -- a_nodes.h:1400
      Iterator_Specification : aliased Declaration_ID;  -- a_nodes.h:1403
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:1406
   end record;
   pragma Convention (C_Pass_By_Copy, Expression_Struct);  -- a_nodes.h:1325

   type Association_Struct is record
      Association_Kind : aliased Association_Kinds;  -- a_nodes.h:1411
      Array_Component_Choices : aliased Expression_List;  -- a_nodes.h:1414
      Record_Component_Choices : aliased Expression_List;  -- a_nodes.h:1416
      Component_Expression : aliased Expression_ID;  -- a_nodes.h:1419
      Formal_Parameter : aliased Expression_ID;  -- a_nodes.h:1423
      Actual_Parameter : aliased Expression_ID;  -- a_nodes.h:1424
      Discriminant_Selector_Names : aliased Expression_List;  -- a_nodes.h:1426
      Discriminant_Expression : aliased Expression_ID;  -- a_nodes.h:1427
      Is_Normalized : aliased Extensions.bool;  -- a_nodes.h:1432
      Is_Defaulted_Association : aliased Extensions.bool;  -- a_nodes.h:1438
   end record;
   pragma Convention (C_Pass_By_Copy, Association_Struct);  -- a_nodes.h:1410

   type Statement_Struct is record
      Statement_Kind : aliased Statement_Kinds;  -- a_nodes.h:1443
      Label_Names : aliased Defining_Name_List;  -- a_nodes.h:1444
      Assignment_Variable_Name : aliased Expression_ID;  -- a_nodes.h:1448
      Assignment_Expression : aliased Expression_ID;  -- a_nodes.h:1449
      Statement_Paths : aliased Path_List;  -- a_nodes.h:1456
      Case_Expression : aliased Expression_ID;  -- a_nodes.h:1458
      Statement_Identifier : aliased Defining_Name_ID;  -- a_nodes.h:1463
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:1467
      While_Condition : aliased Expression_ID;  -- a_nodes.h:1469
      For_Loop_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1471
      Loop_Statements : aliased Statement_List;  -- a_nodes.h:1475
      Is_Declare_Block : aliased Extensions.bool;  -- a_nodes.h:1477
      Block_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1478
      Block_Statements : aliased Statement_List;  -- a_nodes.h:1479
      Block_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1480
      Exit_Loop_Name : aliased Expression_ID;  -- a_nodes.h:1482
      Exit_Condition : aliased Expression_ID;  -- a_nodes.h:1483
      Corresponding_Loop_Exited : aliased Expression_ID;  -- a_nodes.h:1484
      Goto_Label : aliased Expression_ID;  -- a_nodes.h:1486
      Corresponding_Destination_Statement : aliased Statement_ID;  -- a_nodes.h:1487
      Called_Name : aliased Expression_ID;  -- a_nodes.h:1490
      Corresponding_Called_Entity : aliased Declaration_ID;  -- a_nodes.h:1491
      Call_Statement_Parameters : aliased Association_List;  -- a_nodes.h:1492
      Return_Expression : aliased Expression_ID;  -- a_nodes.h:1494
      Return_Object_Declaration : aliased Declaration_ID;  -- a_nodes.h:1497
      Extended_Return_Statements : aliased Statement_List;  -- a_nodes.h:1498
      Extended_Return_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1499
      Accept_Entry_Index : aliased Expression_ID;  -- a_nodes.h:1502
      Accept_Entry_Direct_Name : aliased Name_ID;  -- a_nodes.h:1503
      Accept_Parameters : aliased Parameter_Specification_List;  -- a_nodes.h:1505
      Accept_Body_Statements : aliased Statement_List;  -- a_nodes.h:1506
      Accept_Body_Exception_Handlers : aliased Statement_List;  -- a_nodes.h:1507
      Corresponding_Entry : aliased Declaration_ID;  -- a_nodes.h:1508
      Requeue_Entry_Name : aliased Name_ID;  -- a_nodes.h:1511
      Delay_Expression : aliased Expression_ID;  -- a_nodes.h:1514
      Aborted_Tasks : aliased Expression_List;  -- a_nodes.h:1516
      Raised_Exception : aliased Expression_ID;  -- a_nodes.h:1518
      Associated_Message : aliased Expression_ID;  -- a_nodes.h:1519
      Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1521
   end record;
   pragma Convention (C_Pass_By_Copy, Statement_Struct);  -- a_nodes.h:1442

   type Path_Struct is record
      Path_Kind : aliased Path_Kinds;  -- a_nodes.h:1526
      Sequence_Of_Statements : aliased Statement_List;  -- a_nodes.h:1527
      Condition_Expression : aliased Expression_ID;  -- a_nodes.h:1528
      Case_Path_Alternative_Choices : aliased Element_List;  -- a_nodes.h:1529
      Guard : aliased Expression_ID;  -- a_nodes.h:1530
   end record;
   pragma Convention (C_Pass_By_Copy, Path_Struct);  -- a_nodes.h:1525

   type Clause_Struct is record
      Clause_Kind : aliased Clause_Kinds;  -- a_nodes.h:1535
      Clause_Names : aliased Name_List;  -- a_nodes.h:1541
      Trait_Kind : aliased Trait_Kinds;  -- a_nodes.h:1543
   end record;
   pragma Convention (C_Pass_By_Copy, Clause_Struct);  -- a_nodes.h:1534

   type Exception_Handler_Struct is record
      Choice_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1549
      Exception_Choices : aliased Element_List;  -- a_nodes.h:1550
      Handler_Statements : aliased Statement_List;  -- a_nodes.h:1551
   end record;
   pragma Convention (C_Pass_By_Copy, Exception_Handler_Struct);  -- a_nodes.h:1548

   type Element_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1556
         when 1 =>
            The_Pragma : aliased Pragma_Struct;  -- a_nodes.h:1557
         when 2 =>
            Defining_Name : aliased Defining_Name_Struct;  -- a_nodes.h:1558
         when 3 =>
            Declaration : aliased Declaration_Struct;  -- a_nodes.h:1559
         when 4 =>
            Definition : aliased Definition_Struct;  -- a_nodes.h:1560
         when 5 =>
            Expression : aliased Expression_Struct;  -- a_nodes.h:1561
         when 6 =>
            Association : aliased Association_Struct;  -- a_nodes.h:1562
         when 7 =>
            Statement : aliased Statement_Struct;  -- a_nodes.h:1563
         when 8 =>
            Path : aliased Path_Struct;  -- a_nodes.h:1564
         when 9 =>
            Clause : aliased Clause_Struct;  -- a_nodes.h:1565
         when others =>
            Exception_Handler : aliased Exception_Handler_Struct;  -- a_nodes.h:1566
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Union);
   pragma Unchecked_Union (Element_Union);  -- a_nodes.h:1555

   type Source_Location_Struct is record
      Unit_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1571
      First_Line : aliased int;  -- a_nodes.h:1572
      First_Column : aliased int;  -- a_nodes.h:1573
      Last_Line : aliased int;  -- a_nodes.h:1574
      Last_Column : aliased int;  -- a_nodes.h:1575
   end record;
   pragma Convention (C_Pass_By_Copy, Source_Location_Struct);  -- a_nodes.h:1570

   type Element_Struct is record
      ID : aliased Element_ID;  -- a_nodes.h:1580
      Element_Kind : aliased Element_Kinds;  -- a_nodes.h:1581
      Enclosing_Element_ID : aliased Node_ID;  -- a_nodes.h:1582
      Enclosing_Kind : aliased Enclosing_Kinds;  -- a_nodes.h:1583
      Source_Location : aliased Source_Location_Struct;  -- a_nodes.h:1584
      The_Union : aliased Element_Union;  -- a_nodes.h:1585
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Struct);  -- a_nodes.h:1579

   type Node_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1594
         when 1 =>
            Context : aliased Context_Struct;  -- a_nodes.h:1595
         when 2 =>
            Unit : aliased Unit_Struct;  -- a_nodes.h:1596
         when others =>
            Element : aliased Element_Struct;  -- a_nodes.h:1597
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Node_Union);
   pragma Unchecked_Union (Node_Union);  -- a_nodes.h:1593

   type Node_Struct is record
      Node_Kind : aliased Node_Kinds;  -- a_nodes.h:1602
      The_Union : aliased Node_Union;  -- a_nodes.h:1603
   end record;
   pragma Convention (C_Pass_By_Copy, Node_Struct);  -- a_nodes.h:1601

   type List_Node_Struct is record
      Node : aliased Node_Struct;  -- a_nodes.h:1608
      Next : access List_Node_Struct;  -- a_nodes.h:1609
      Next_Count : aliased int;  -- a_nodes.h:1611
   end record;
   pragma Convention (C_Pass_By_Copy, List_Node_Struct);  -- a_nodes.h:1607

   type Node_List_Ptr is access all List_Node_Struct;  -- a_nodes.h:1614

end a_nodes_h;
