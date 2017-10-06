pragma Ada_2005;
pragma Style_Checks (Off);

with Interfaces.C; use Interfaces.C;
with Interfaces.C.Strings;
with Interfaces.C.Extensions;

package a_nodes_h is

   type Program_Text is new Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:11

   subtype ASIS_Integer is int;  -- a_nodes.h:12

   type Node_Kinds is 
     (Not_A_Node,
      A_Context_Node,
      A_Unit_Node,
      An_Element_Node);
   pragma Convention (C, Node_Kinds);  -- a_nodes.h:14

   subtype Node_ID is int;  -- a_nodes.h:21

   subtype Element_ID is Node_ID;  -- a_nodes.h:27

   subtype Element is Element_ID;  -- a_nodes.h:28

   type Element_ID_Ptr is access all Element_ID;  -- a_nodes.h:45

   type Element_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:50
      IDs : Element_ID_Ptr;  -- a_nodes.h:51
   end record;
   pragma Convention (C_Pass_By_Copy, Element_ID_Array_Struct);  -- a_nodes.h:49

   subtype Element_List is Element_ID_Array_Struct;  -- a_nodes.h:53

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
   pragma Convention (C, Element_Kinds);  -- a_nodes.h:55

   subtype Declaration_ID is Element_ID;  -- a_nodes.h:76

   subtype Defining_Name_ID is Element_ID;  -- a_nodes.h:78

   subtype Discrete_Range_ID is Element_ID;  -- a_nodes.h:81

   subtype Discrete_Subtype_Definition_ID is Element_ID;  -- a_nodes.h:82

   subtype Expression_ID is Element_ID;  -- a_nodes.h:83

   subtype Name_ID is Element_ID;  -- a_nodes.h:85

   subtype Statement_ID is Element_ID;  -- a_nodes.h:86

   subtype Subtype_Indication_ID is Element_ID;  -- a_nodes.h:87

   subtype Type_Definition_ID is Element_ID;  -- a_nodes.h:89

   subtype Expression_Path_List is Element_ID_Array_Struct;  -- a_nodes.h:99

   subtype Access_Type_Definition is Element;  -- a_nodes.h:107

   subtype Association is Element;  -- a_nodes.h:108

   subtype Association_List is Element_ID_Array_Struct;  -- a_nodes.h:109

   subtype Case_Statement_Alternative is Element;  -- a_nodes.h:110

   subtype Clause is Element;  -- a_nodes.h:111

   subtype Component_Clause is Element;  -- a_nodes.h:112

   subtype Component_Clause_List is Element_ID_Array_Struct;  -- a_nodes.h:113

   subtype Component_Declaration is Element;  -- a_nodes.h:114

   subtype Component_Definition is Element;  -- a_nodes.h:115

   subtype Constraint is Element;  -- a_nodes.h:116

   subtype Context_Clause is Element;  -- a_nodes.h:117

   subtype Context_Clause_List is Element_ID_Array_Struct;  -- a_nodes.h:118

   subtype Declaration is Element_ID;  -- a_nodes.h:119

   subtype Declaration_List is Element_ID_Array_Struct;  -- a_nodes.h:120

   subtype Declarative_Item_List is Element_ID_Array_Struct;  -- a_nodes.h:121

   subtype Definition is Element_ID;  -- a_nodes.h:122

   subtype Definition_List is Element_ID_Array_Struct;  -- a_nodes.h:123

   subtype Discrete_Range is Element;  -- a_nodes.h:124

   subtype Discrete_Range_List is Element_ID_Array_Struct;  -- a_nodes.h:125

   subtype Discrete_Subtype_Definition is Element;  -- a_nodes.h:126

   subtype Discriminant_Association is Element;  -- a_nodes.h:127

   subtype Discriminant_Association_List is Element_ID_Array_Struct;  -- a_nodes.h:128

   subtype Discriminant_Specification_List is Element_ID_Array_Struct;  -- a_nodes.h:129

   subtype Defining_Name is Element;  -- a_nodes.h:130

   subtype Defining_Name_List is Element_ID_Array_Struct;  -- a_nodes.h:131

   subtype Exception_Handler is Element;  -- a_nodes.h:132

   subtype Exception_Handler_List is Element_ID_Array_Struct;  -- a_nodes.h:133

   subtype Expression is Element_ID;  -- a_nodes.h:134

   subtype Expression_List is Element_ID_Array_Struct;  -- a_nodes.h:135

   subtype Formal_Type_Definition is Element;  -- a_nodes.h:136

   subtype Generic_Formal_Parameter is Element;  -- a_nodes.h:137

   subtype Generic_Formal_Parameter_List is Element_ID_Array_Struct;  -- a_nodes.h:138

   subtype Identifier is Element;  -- a_nodes.h:139

   subtype Identifier_List is Element_ID_Array_Struct;  -- a_nodes.h:140

   subtype Name is Element;  -- a_nodes.h:141

   subtype Name_List is Element_ID_Array_Struct;  -- a_nodes.h:142

   subtype Parameter_Specification is Element;  -- a_nodes.h:143

   subtype Parameter_Specification_List is Element_ID_Array_Struct;  -- a_nodes.h:144

   subtype Path is Element;  -- a_nodes.h:145

   subtype Path_List is Element_ID_Array_Struct;  -- a_nodes.h:146

   subtype Pragma_Element is Element;  -- a_nodes.h:147

   subtype Pragma_Element_List is Element_ID_Array_Struct;  -- a_nodes.h:148

   subtype Range_Constraint is Element;  -- a_nodes.h:149

   subtype Record_Component is Element;  -- a_nodes.h:150

   subtype Record_Definition is Element;  -- a_nodes.h:152

   subtype Representation_Clause is Element;  -- a_nodes.h:153

   subtype Representation_Clause_List is Element_ID_Array_Struct;  -- a_nodes.h:154

   subtype Root_Type_Definition is Element;  -- a_nodes.h:155

   subtype Select_Alternative is Element;  -- a_nodes.h:156

   subtype Statement is Element;  -- a_nodes.h:157

   subtype Statement_List is Element_ID_Array_Struct;  -- a_nodes.h:158

   subtype Subtype_Indication is Element_ID;  -- a_nodes.h:159

   subtype Subtype_Mark is Element;  -- a_nodes.h:160

   subtype Type_Definition is Element;  -- a_nodes.h:161

   subtype Variant is Element;  -- a_nodes.h:162

   subtype Variant_Component_List is Element_ID_Array_Struct;  -- a_nodes.h:163

   subtype Variant_List is Element_ID_Array_Struct;  -- a_nodes.h:164

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
   pragma Convention (C, Operator_Kinds);  -- a_nodes.h:168

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
   pragma Convention (C, Pragma_Kinds);  -- a_nodes.h:198

   type Pragma_Struct is record
      Pragma_Kind : aliased Pragma_Kinds;  -- a_nodes.h:259
      Pragmas : aliased Pragma_Element_List;  -- a_nodes.h:260
      Pragma_Name_Image : Program_Text;  -- a_nodes.h:261
      Pragma_Argument_Associations : aliased Association_List;  -- a_nodes.h:262
   end record;
   pragma Convention (C_Pass_By_Copy, Pragma_Struct);  -- a_nodes.h:258

   type Defining_Name_Kinds is 
     (Not_A_Defining_Name,
      A_Defining_Identifier,
      A_Defining_Character_Literal,
      A_Defining_Enumeration_Literal,
      A_Defining_Operator_Symbol,
      A_Defining_Expanded_Name);
   pragma Convention (C, Defining_Name_Kinds);  -- a_nodes.h:273

   type Defining_Name_Struct is record
      Defining_Name_Kind : aliased Defining_Name_Kinds;  -- a_nodes.h:284
      Defining_Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:285
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:289
      Position_Number_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:292
      Representation_Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:293
      Defining_Prefix : aliased Name_ID;  -- a_nodes.h:295
      Defining_Selector : aliased Defining_Name_ID;  -- a_nodes.h:296
      Corresponding_Constant_Declaration : aliased Declaration_ID;  -- a_nodes.h:298
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:301
   end record;
   pragma Convention (C_Pass_By_Copy, Defining_Name_Struct);  -- a_nodes.h:283

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
   pragma Convention (C, Declaration_Kinds);  -- a_nodes.h:312

   type Declaration_Origins is 
     (Not_A_Declaration_Origin,
      An_Explicit_Declaration,
      An_Implicit_Predefined_Declaration,
      An_Implicit_Inherited_Declaration);
   pragma Convention (C, Declaration_Origins);  -- a_nodes.h:416

   type Mode_Kinds is 
     (Not_A_Mode,
      A_Default_In_Mode,
      An_In_Mode,
      An_Out_Mode,
      An_In_Out_Mode);
   pragma Convention (C, Mode_Kinds);  -- a_nodes.h:432

   type Subprogram_Default_Kinds is 
     (Not_A_Default,
      A_Name_Default,
      A_Box_Default,
      A_Null_Default,
      A_Nil_Default);
   pragma Convention (C, Subprogram_Default_Kinds);  -- a_nodes.h:442

   type Declaration_Struct is record
      Declaration_Kind : aliased Declaration_Kinds;  -- a_nodes.h:455
      Declaration_Origin : aliased Declaration_Origins;  -- a_nodes.h:456
      Corresponding_Pragmas : aliased Pragma_Element_List;  -- a_nodes.h:457
      Names : aliased Defining_Name_List;  -- a_nodes.h:458
      Aspect_Specifications : aliased Element_List;  -- a_nodes.h:459
      Corresponding_Representation_Clauses : aliased Representation_Clause_List;  -- a_nodes.h:460
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:470
      Has_Aliased : aliased Extensions.bool;  -- a_nodes.h:477
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:481
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:484
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:489
      Has_Reverse : aliased Extensions.bool;  -- a_nodes.h:493
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:498
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:502
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:510
      Mode_Kind : aliased Mode_Kinds;  -- a_nodes.h:513
      Default_Kind : aliased Subprogram_Default_Kinds;  -- a_nodes.h:516
      Pragmas : aliased Pragma_Element_List;  -- a_nodes.h:536
      Corresponding_End_Name : aliased Element_ID;  -- a_nodes.h:549
      Discriminant_Part : aliased Definition_ID;  -- a_nodes.h:558
      Type_Declaration_View : aliased Definition_ID;  -- a_nodes.h:565
      Object_Declaration_View : aliased Definition_ID;  -- a_nodes.h:577
      Initialization_Expression : aliased Expression_ID;  -- a_nodes.h:587
      Corresponding_Type_Declaration : aliased Declaration_ID;  -- a_nodes.h:595
      Corresponding_Type_Completion : aliased Declaration_ID;  -- a_nodes.h:600
      Corresponding_Type_Partial_View : aliased Declaration_ID;  -- a_nodes.h:606
      Corresponding_First_Subtype : aliased Declaration_ID;  -- a_nodes.h:614
      Corresponding_Last_Constraint : aliased Declaration_ID;  -- a_nodes.h:615
      Corresponding_Last_Subtype : aliased Declaration_ID;  -- a_nodes.h:616
      Specification_Subtype_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:619
      Iteration_Scheme_Name : aliased Element_ID;  -- a_nodes.h:622
      Subtype_Indication : aliased Element_ID;  -- a_nodes.h:624
      Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:641
      Result_Profile : aliased Element_ID;  -- a_nodes.h:649
      Result_Expression : aliased Expression_ID;  -- a_nodes.h:651
      Is_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:667
      Is_Not_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:668
      Body_Declarative_Items : aliased Element_List;  -- a_nodes.h:674
      Body_Statements : aliased Statement_List;  -- a_nodes.h:675
      Body_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:676
      Body_Block_Statement : aliased Declaration_ID;  -- a_nodes.h:677
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:690
      Corresponding_Declaration : aliased Declaration_ID;  -- a_nodes.h:726
      Corresponding_Body : aliased Declaration_ID;  -- a_nodes.h:742
      Corresponding_Subprogram_Derivation : aliased Declaration_ID;  -- a_nodes.h:745
      Corresponding_Type : aliased Type_Definition_ID;  -- a_nodes.h:749
      Corresponding_Equality_Operator : aliased Declaration_ID;  -- a_nodes.h:751
      Visible_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:754
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:755
      Private_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:756
      Declaration_Interface_List : aliased Expression_List;  -- a_nodes.h:761
      Renamed_Entity : aliased Expression_ID;  -- a_nodes.h:770
      Corresponding_Base_Entity : aliased Expression_ID;  -- a_nodes.h:771
      Protected_Operation_Items : aliased Declaration_List;  -- a_nodes.h:773
      Entry_Family_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:775
      Entry_Index_Specification : aliased Declaration_ID;  -- a_nodes.h:777
      Entry_Barrier : aliased Expression_ID;  -- a_nodes.h:778
      Corresponding_Subunit : aliased Declaration_ID;  -- a_nodes.h:784
      Is_Subunit : aliased Extensions.bool;  -- a_nodes.h:790
      Corresponding_Body_Stub : aliased Declaration_ID;  -- a_nodes.h:791
      Generic_Formal_Part : aliased Element_List;  -- a_nodes.h:795
      Generic_Unit_Name : aliased Expression_ID;  -- a_nodes.h:801
      Generic_Actual_Part : aliased Association_List;  -- a_nodes.h:802
      Formal_Subprogram_Default : aliased Expression_ID;  -- a_nodes.h:805
      Is_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:816
   end record;
   pragma Convention (C_Pass_By_Copy, Declaration_Struct);  -- a_nodes.h:454

   subtype Constraint_ID is Element_ID;  -- a_nodes.h:827

   subtype Definition_ID is Element_ID;  -- a_nodes.h:828

   subtype Definition_ID_List is Element_ID_Array_Struct;  -- a_nodes.h:829

   subtype Declarative_Item_ID_List is Element_ID_Array_Struct;  -- a_nodes.h:830

   subtype Record_Component_List is Element_ID_Array_Struct;  -- a_nodes.h:831

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
   pragma Convention (C, Definition_Kinds);  -- a_nodes.h:833

   type u_Type_Kinds is 
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
   pragma Convention (C, u_Type_Kinds);  -- a_nodes.h:866

   subtype Type_Kinds is u_Type_Kinds;  -- a_nodes.h:888

   type u_Constraint_Kinds is 
     (Not_A_Constraint,
      A_Range_Attribute_Reference,
      A_Simple_Expression_Range,
      A_Digits_Constraint,
      A_Delta_Constraint,
      An_Index_Constraint,
      A_Discriminant_Constraint);
   pragma Convention (C, u_Constraint_Kinds);  -- a_nodes.h:890

   subtype Constraint_Kinds is u_Constraint_Kinds;  -- a_nodes.h:898

   type u_Interface_Kinds is 
     (Not_An_Interface,
      An_Ordinary_Interface,
      A_Limited_Interface,
      A_Task_Interface,
      A_Protected_Interface,
      A_Synchronized_Interface);
   pragma Convention (C, u_Interface_Kinds);  -- a_nodes.h:900

   subtype Interface_Kinds is u_Interface_Kinds;  -- a_nodes.h:907

   type u_Root_Type_Kinds is 
     (Not_A_Root_Type_Definition,
      A_Root_Integer_Definition,
      A_Root_Real_Definition,
      A_Universal_Integer_Definition,
      A_Universal_Real_Definition,
      A_Universal_Fixed_Definition);
   pragma Convention (C, u_Root_Type_Kinds);  -- a_nodes.h:909

   subtype Root_Type_Kinds is u_Root_Type_Kinds;  -- a_nodes.h:916

   type u_Discrete_Range_Kinds is 
     (Not_A_Discrete_Range,
      A_Discrete_Subtype_Indication,
      A_Discrete_Range_Attribute_Reference,
      A_Discrete_Simple_Expression_Range);
   pragma Convention (C, u_Discrete_Range_Kinds);  -- a_nodes.h:918

   subtype Discrete_Range_Kinds is u_Discrete_Range_Kinds;  -- a_nodes.h:923

   type u_Formal_Type_Kinds is 
     (Not_A_Formal_Type_Definition,
      A_Formal_Private_Type_Definition,
      A_Formal_Tagged_Private_Type_Definition,
      A_Formal_Derived_Type_Definition,
      A_Formal_Discrete_Type_Definition,
      A_Formal_Signed_Integer_Type_Definition,
      A_Formal_Modular_Type_Definition,
      A_Formal_Floating_Point_Definition,
      A_Formal_Ordinary_Fixed_Point_Definition,
      A_Formal_Decimal_Fixed_Point_Definition,
      A_Formal_Interface_Type_Definition,
      A_Formal_Unconstrained_Array_Definition,
      A_Formal_Constrained_Array_Definition,
      A_Formal_Access_Type_Definition);
   pragma Convention (C, u_Formal_Type_Kinds);  -- a_nodes.h:925

   subtype Formal_Type_Kinds is u_Formal_Type_Kinds;  -- a_nodes.h:945

   type u_Access_Type_Kinds is 
     (Not_An_Access_Type_Definition,
      A_Pool_Specific_Access_To_Variable,
      An_Access_To_Variable,
      An_Access_To_Constant,
      An_Access_To_Procedure,
      An_Access_To_Protected_Procedure,
      An_Access_To_Function,
      An_Access_To_Protected_Function);
   pragma Convention (C, u_Access_Type_Kinds);  -- a_nodes.h:947

   subtype Access_Type_Kinds is u_Access_Type_Kinds;  -- a_nodes.h:956

   type u_Access_Definition_Kinds is 
     (Not_An_Access_Definition,
      An_Anonymous_Access_To_Variable,
      An_Anonymous_Access_To_Constant,
      An_Anonymous_Access_To_Procedure,
      An_Anonymous_Access_To_Protected_Procedure,
      An_Anonymous_Access_To_Function,
      An_Anonymous_Access_To_Protected_Function);
   pragma Convention (C, u_Access_Definition_Kinds);  -- a_nodes.h:958

   subtype Access_Definition_Kinds is u_Access_Definition_Kinds;  -- a_nodes.h:966

   type u_Access_Type_Struct is record
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:969
      Access_Type_Kind : aliased Access_Type_Kinds;  -- a_nodes.h:970
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:974
      Access_To_Object_Definition : aliased Subtype_Indication;  -- a_nodes.h:978
      Access_To_Subprogram_Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:983
      Access_To_Function_Result_Profile : aliased Element;  -- a_nodes.h:986
   end record;
   pragma Convention (C_Pass_By_Copy, u_Access_Type_Struct);  -- a_nodes.h:968

   subtype Access_Type_Struct is u_Access_Type_Struct;  -- a_nodes.h:987

   type u_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:990
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:991
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:992
      Type_Kind : aliased Type_Kinds;  -- a_nodes.h:993
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:996
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:997
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:999
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:1001
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1006
      Interface_Kind : aliased Interface_Kinds;  -- a_nodes.h:1008
      Root_Type_Kind : aliased Root_Type_Kinds;  -- a_nodes.h:1010
      Parent_Subtype_Indication : aliased Subtype_Indication;  -- a_nodes.h:1013
      Record_Definition : aliased Definition;  -- a_nodes.h:1017
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1020
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1021
      Corresponding_Parent_Subtype : aliased Declaration;  -- a_nodes.h:1022
      Corresponding_Root_Type : aliased Declaration;  -- a_nodes.h:1023
      Corresponding_Type_Structure : aliased Declaration;  -- a_nodes.h:1024
      Enumeration_Literal_Declarations : aliased Declaration_List;  -- a_nodes.h:1026
      Integer_Constraint : aliased Range_Constraint;  -- a_nodes.h:1028
      Mod_Static_Expression : aliased Expression;  -- a_nodes.h:1030
      Digits_Expression : aliased Expression;  -- a_nodes.h:1033
      Delta_Expression : aliased Expression;  -- a_nodes.h:1036
      Real_Range_Constraint : aliased Range_Constraint;  -- a_nodes.h:1040
      Index_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1042
      Discrete_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1044
      Array_Component_Definition : aliased Component_Definition;  -- a_nodes.h:1047
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1050
      Access_Type : aliased Access_Type_Struct;  -- a_nodes.h:1052
   end record;
   pragma Convention (C_Pass_By_Copy, u_Type_Definition_Struct);  -- a_nodes.h:989

   subtype Type_Definition_Struct is u_Type_Definition_Struct;  -- a_nodes.h:1053

   type u_Constraint_Struct is record
      Constraint_Kind : aliased Constraint_Kinds;  -- a_nodes.h:1059
      Digits_Expression : aliased Expression;  -- a_nodes.h:1062
      Delta_Expression : aliased Expression;  -- a_nodes.h:1064
      Real_Range_Constraint : aliased Range_Constraint;  -- a_nodes.h:1067
      Lower_Bound : aliased Expression;  -- a_nodes.h:1069
      Upper_Bound : aliased Expression;  -- a_nodes.h:1070
      Range_Attribute : aliased Expression;  -- a_nodes.h:1072
      Discrete_Ranges : aliased Discrete_Range_List;  -- a_nodes.h:1074
      Discriminant_Associations : aliased Discriminant_Association_List;  -- a_nodes.h:1076
   end record;
   pragma Convention (C_Pass_By_Copy, u_Constraint_Struct);  -- a_nodes.h:1058

   subtype Constraint_Struct is u_Constraint_Struct;  -- a_nodes.h:1077

   type u_Subtype_Indication_Struct is record
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1080
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1081
      Subtype_Constraint : aliased Constraint_Struct;  -- a_nodes.h:1082
   end record;
   pragma Convention (C_Pass_By_Copy, u_Subtype_Indication_Struct);  -- a_nodes.h:1079

   subtype Subtype_Indication_Struct is u_Subtype_Indication_Struct;  -- a_nodes.h:1083

   type u_Component_Definition_Struct is record
      Component_Definition_View : aliased Definition;  -- a_nodes.h:1086
   end record;
   pragma Convention (C_Pass_By_Copy, u_Component_Definition_Struct);  -- a_nodes.h:1085

   subtype Component_Definition_Struct is u_Component_Definition_Struct;  -- a_nodes.h:1087

   type u_Discrete_Subtype_Definition_Struct is record
      Discrete_Range_Kind : aliased Discrete_Range_Kinds;  -- a_nodes.h:1090
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1093
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1094
   end record;
   pragma Convention (C_Pass_By_Copy, u_Discrete_Subtype_Definition_Struct);  -- a_nodes.h:1089

   subtype Discrete_Subtype_Definition_Struct is u_Discrete_Subtype_Definition_Struct;  -- a_nodes.h:1095

   type u_Discrete_Range_Struct is record
      Discrete_Range_Kind : aliased Discrete_Range_Kinds;  -- a_nodes.h:1098
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1101
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1102
      Lower_Bound : aliased Expression;  -- a_nodes.h:1104
      Upper_Bound : aliased Expression;  -- a_nodes.h:1105
      Range_Attribute : aliased Expression;  -- a_nodes.h:1107
   end record;
   pragma Convention (C_Pass_By_Copy, u_Discrete_Range_Struct);  -- a_nodes.h:1097

   subtype Discrete_Range_Struct is u_Discrete_Range_Struct;  -- a_nodes.h:1108

   type u_Known_Discriminant_Part_Struct is record
      Discriminants : aliased Discriminant_Specification_List;  -- a_nodes.h:1111
   end record;
   pragma Convention (C_Pass_By_Copy, u_Known_Discriminant_Part_Struct);  -- a_nodes.h:1110

   subtype Known_Discriminant_Part_Struct is u_Known_Discriminant_Part_Struct;  -- a_nodes.h:1112

   type u_Record_Definition_Struct is record
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1115
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1116
   end record;
   pragma Convention (C_Pass_By_Copy, u_Record_Definition_Struct);  -- a_nodes.h:1114

   subtype Record_Definition_Struct is u_Record_Definition_Struct;  -- a_nodes.h:1117

   type u_Variant_Part_Struct is record
      Discriminant_Direct_Name : aliased Name;  -- a_nodes.h:1120
      Variants : aliased Variant_List;  -- a_nodes.h:1121
   end record;
   pragma Convention (C_Pass_By_Copy, u_Variant_Part_Struct);  -- a_nodes.h:1119

   subtype Variant_Part_Struct is u_Variant_Part_Struct;  -- a_nodes.h:1122

   type u_Variant_Struct is record
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1125
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1126
      Variant_Choices : aliased Element_List;  -- a_nodes.h:1127
   end record;
   pragma Convention (C_Pass_By_Copy, u_Variant_Struct);  -- a_nodes.h:1124

   subtype Variant_Struct is u_Variant_Struct;  -- a_nodes.h:1128

   type u_Access_Definition_Struct is record
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1131
      Access_Definition_Kind : aliased Access_Definition_Kinds;  -- a_nodes.h:1132
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:1136
      Anonymous_Access_To_Object_Subtype_Mark : aliased Expression;  -- a_nodes.h:1139
      Access_To_Subprogram_Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:1144
      Access_To_Function_Result_Profile : aliased Element;  -- a_nodes.h:1147
   end record;
   pragma Convention (C_Pass_By_Copy, u_Access_Definition_Struct);  -- a_nodes.h:1130

   subtype Access_Definition_Struct is u_Access_Definition_Struct;  -- a_nodes.h:1148

   type u_Private_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1151
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1152
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1153
   end record;
   pragma Convention (C_Pass_By_Copy, u_Private_Type_Definition_Struct);  -- a_nodes.h:1150

   subtype Private_Type_Definition_Struct is u_Private_Type_Definition_Struct;  -- a_nodes.h:1154

   type u_Tagged_Private_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1157
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1158
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1159
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:1160
   end record;
   pragma Convention (C_Pass_By_Copy, u_Tagged_Private_Type_Definition_Struct);  -- a_nodes.h:1156

   subtype Tagged_Private_Type_Definition_Struct is u_Tagged_Private_Type_Definition_Struct;  -- a_nodes.h:1161

   type u_Private_Extension_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1164
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1165
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1166
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:1167
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1168
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1169
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1170
      Ancestor_Subtype_Indication : aliased Subtype_Indication;  -- a_nodes.h:1171
   end record;
   pragma Convention (C_Pass_By_Copy, u_Private_Extension_Definition_Struct);  -- a_nodes.h:1163

   subtype Private_Extension_Definition_Struct is u_Private_Extension_Definition_Struct;  -- a_nodes.h:1172

   type u_Task_Definition_Struct is record
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:1175
      Visible_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1176
      Private_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1177
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1178
   end record;
   pragma Convention (C_Pass_By_Copy, u_Task_Definition_Struct);  -- a_nodes.h:1174

   subtype Task_Definition_Struct is u_Task_Definition_Struct;  -- a_nodes.h:1179

   type u_Protected_Definition_Struct is record
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:1182
      Visible_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1183
      Private_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1184
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1185
   end record;
   pragma Convention (C_Pass_By_Copy, u_Protected_Definition_Struct);  -- a_nodes.h:1181

   subtype Protected_Definition_Struct is u_Protected_Definition_Struct;  -- a_nodes.h:1186

   type u_Formal_Type_Definition_Struct is record
      Formal_Type_Kind : aliased Formal_Type_Kinds;  -- a_nodes.h:1189
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1190
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1195
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1196
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1199
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:1201
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:1203
      Interface_Kind : aliased Interface_Kinds;  -- a_nodes.h:1205
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1207
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1208
      Index_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1210
      Discrete_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1212
      Array_Component_Definition : aliased Component_Definition;  -- a_nodes.h:1215
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1217
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1220
      Access_Type : aliased Access_Type_Struct;  -- a_nodes.h:1222
   end record;
   pragma Convention (C_Pass_By_Copy, u_Formal_Type_Definition_Struct);  -- a_nodes.h:1188

   subtype Formal_Type_Definition_Struct is u_Formal_Type_Definition_Struct;  -- a_nodes.h:1223

   type u_Aspect_Specification_Struct is record
      Aspect_Mark : aliased Element;  -- a_nodes.h:1226
      Aspect_Definition : aliased Element;  -- a_nodes.h:1227
   end record;
   pragma Convention (C_Pass_By_Copy, u_Aspect_Specification_Struct);  -- a_nodes.h:1225

   subtype Aspect_Specification_Struct is u_Aspect_Specification_Struct;  -- a_nodes.h:1228

   subtype No_Struct is int;  -- a_nodes.h:1230

   subtype Unknown_Discriminant_Part_Struct is No_Struct;  -- a_nodes.h:1231

   subtype Null_Record_Definition_Struct is No_Struct;  -- a_nodes.h:1232

   subtype Null_Component_Struct is No_Struct;  -- a_nodes.h:1233

   subtype Others_Choice_Struct is No_Struct;  -- a_nodes.h:1234

   type u_Definition_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1237
         when 1 =>
            The_Type_Definition : aliased Type_Definition_Struct;  -- a_nodes.h:1238
         when 2 =>
            The_Subtype_Indication : aliased Subtype_Indication_Struct;  -- a_nodes.h:1239
         when 3 =>
            The_Constraint : aliased Constraint_Struct;  -- a_nodes.h:1240
         when 4 =>
            The_Component_Definition : aliased Component_Definition_Struct;  -- a_nodes.h:1241
         when 5 =>
            The_Discrete_Subtype_Definition : aliased Discrete_Subtype_Definition_Struct;  -- a_nodes.h:1242
         when 6 =>
            The_Discrete_Range : aliased Discrete_Range_Struct;  -- a_nodes.h:1243
         when 7 =>
            The_Unknown_Discriminant_Part : aliased Unknown_Discriminant_Part_Struct;  -- a_nodes.h:1244
         when 8 =>
            The_Known_Discriminant_Part : aliased Known_Discriminant_Part_Struct;  -- a_nodes.h:1245
         when 9 =>
            The_Record_Definition : aliased Record_Definition_Struct;  -- a_nodes.h:1246
         when 10 =>
            The_Null_Record_Definition : aliased Null_Record_Definition_Struct;  -- a_nodes.h:1247
         when 11 =>
            The_Null_Component : aliased Null_Component_Struct;  -- a_nodes.h:1248
         when 12 =>
            The_Variant_Part : aliased Variant_Part_Struct;  -- a_nodes.h:1249
         when 13 =>
            The_Variant : aliased Variant_Struct;  -- a_nodes.h:1250
         when 14 =>
            The_Others_Choice : aliased Others_Choice_Struct;  -- a_nodes.h:1251
         when 15 =>
            The_Access_Definition : aliased Access_Definition_Struct;  -- a_nodes.h:1252
         when 16 =>
            The_Private_Type_Definition : aliased Private_Type_Definition_Struct;  -- a_nodes.h:1253
         when 17 =>
            The_Tagged_Private_Type_Definition : aliased Tagged_Private_Type_Definition_Struct;  -- a_nodes.h:1254
         when 18 =>
            The_Private_Extension_Definition : aliased Private_Extension_Definition_Struct;  -- a_nodes.h:1255
         when 19 =>
            The_Task_Definition : aliased Task_Definition_Struct;  -- a_nodes.h:1256
         when 20 =>
            The_Protected_Definition : aliased Protected_Definition_Struct;  -- a_nodes.h:1257
         when 21 =>
            The_Formal_Type_Definition : aliased Formal_Type_Definition_Struct;  -- a_nodes.h:1258
         when others =>
            The_Aspect_Specification : aliased Aspect_Specification_Struct;  -- a_nodes.h:1259
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, u_Definition_Union);
   pragma Unchecked_Union (u_Definition_Union);  -- a_nodes.h:1236

   subtype Definition_Union is u_Definition_Union;  -- a_nodes.h:1260

   type Definition_Struct is record
      Definition_Kind : aliased Definition_Kinds;  -- a_nodes.h:1264
      The_Union : aliased Definition_Union;  -- a_nodes.h:1265
   end record;
   pragma Convention (C_Pass_By_Copy, Definition_Struct);  -- a_nodes.h:1263

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
   pragma Convention (C, Expression_Kinds);  -- a_nodes.h:1276

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
   pragma Convention (C, Attribute_Kinds);  -- a_nodes.h:1321

   type Expression_Struct is record
      Is_Prefix_Notation : aliased Extensions.bool;  -- a_nodes.h:1434
      Expression_Kind : aliased Expression_Kinds;  -- a_nodes.h:1435
      Corresponding_Expression_Type : aliased Declaration_ID;  -- a_nodes.h:1436
      Corresponding_Expression_Type_Definition : aliased Element;  -- a_nodes.h:1437
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:1441
      Attribute_Kind : aliased Attribute_Kinds;  -- a_nodes.h:1446
      Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1447
      Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1452
      Corresponding_Name_Definition : aliased Defining_Name_ID;  -- a_nodes.h:1453
      Corresponding_Name_Definition_List : aliased Defining_Name_List;  -- a_nodes.h:1454
      Corresponding_Name_Declaration : aliased Element_ID;  -- a_nodes.h:1455
      Prefix : aliased Expression_ID;  -- a_nodes.h:1462
      Index_Expressions : aliased Expression_List;  -- a_nodes.h:1464
      Slice_Range : aliased Discrete_Range_ID;  -- a_nodes.h:1466
      Selector : aliased Expression_ID;  -- a_nodes.h:1468
      Attribute_Designator_Identifier : aliased Expression_ID;  -- a_nodes.h:1470
      Attribute_Designator_Expressions : aliased Expression_List;  -- a_nodes.h:1479
      Record_Component_Associations : aliased Association_List;  -- a_nodes.h:1482
      Extension_Aggregate_Expression : aliased Expression_ID;  -- a_nodes.h:1484
      Array_Component_Associations : aliased Association_List;  -- a_nodes.h:1487
      Expression_Parenthesized : aliased Expression_ID;  -- a_nodes.h:1489
      Is_Prefix_Call : aliased Extensions.bool;  -- a_nodes.h:1491
      Corresponding_Called_Function : aliased Declaration_ID;  -- a_nodes.h:1494
      Function_Call_Parameters : aliased Element_List;  -- a_nodes.h:1496
      Short_Circuit_Operation_Left_Expression : aliased Expression_ID;  -- a_nodes.h:1499
      Short_Circuit_Operation_Right_Expression : aliased Expression_ID;  -- a_nodes.h:1500
      Membership_Test_Expression : aliased Expression_ID;  -- a_nodes.h:1503
      Membership_Test_Choices : aliased Element_List;  -- a_nodes.h:1504
      Converted_Or_Qualified_Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:1507
      Converted_Or_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1508
      Allocator_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1510
      Allocator_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1512
      Expression_Paths : aliased Expression_Path_List;  -- a_nodes.h:1515
      Is_Generalized_Indexing : aliased Extensions.bool;  -- a_nodes.h:1517
      Is_Generalized_Reference : aliased Extensions.bool;  -- a_nodes.h:1519
      Iterator_Specification : aliased Declaration_ID;  -- a_nodes.h:1522
      Predicate : aliased Expression_ID;  -- a_nodes.h:1525
      Subpool_Name : aliased Expression_ID;  -- a_nodes.h:1528
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:1533
      Is_Dispatching_Call : aliased Extensions.bool;  -- a_nodes.h:1535
      Is_Call_On_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1536
   end record;
   pragma Convention (C_Pass_By_Copy, Expression_Struct);  -- a_nodes.h:1433

   type Association_Kinds is 
     (Not_An_Association,
      A_Pragma_Argument_Association,
      A_Discriminant_Association,
      A_Record_Component_Association,
      An_Array_Component_Association,
      A_Parameter_Association,
      A_Generic_Association);
   pragma Convention (C, Association_Kinds);  -- a_nodes.h:1547

   type Association_Struct is record
      Association_Kind : aliased Association_Kinds;  -- a_nodes.h:1560
      Array_Component_Choices : aliased Expression_List;  -- a_nodes.h:1563
      Record_Component_Choices : aliased Expression_List;  -- a_nodes.h:1565
      Component_Expression : aliased Expression_ID;  -- a_nodes.h:1568
      Formal_Parameter : aliased Expression_ID;  -- a_nodes.h:1572
      Actual_Parameter : aliased Expression_ID;  -- a_nodes.h:1573
      Discriminant_Selector_Names : aliased Expression_List;  -- a_nodes.h:1575
      Discriminant_Expression : aliased Expression_ID;  -- a_nodes.h:1576
      Is_Normalized : aliased Extensions.bool;  -- a_nodes.h:1581
      Is_Defaulted_Association : aliased Extensions.bool;  -- a_nodes.h:1585
   end record;
   pragma Convention (C_Pass_By_Copy, Association_Struct);  -- a_nodes.h:1559

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
   pragma Convention (C, Statement_Kinds);  -- a_nodes.h:1596

   type Statement_Struct is record
      Statement_Kind : aliased Statement_Kinds;  -- a_nodes.h:1633
      Corresponding_Pragmas : aliased Pragma_Element_List;  -- a_nodes.h:1634
      Label_Names : aliased Defining_Name_List;  -- a_nodes.h:1635
      Is_Prefix_Notation : aliased Extensions.bool;  -- a_nodes.h:1639
      Pragmas : aliased Pragma_Element_List;  -- a_nodes.h:1646
      Corresponding_End_Name : aliased Element;  -- a_nodes.h:1652
      Assignment_Variable_Name : aliased Expression_ID;  -- a_nodes.h:1654
      Assignment_Expression : aliased Expression_ID;  -- a_nodes.h:1655
      Statement_Paths : aliased Path_List;  -- a_nodes.h:1662
      Case_Expression : aliased Expression_ID;  -- a_nodes.h:1664
      Statement_Identifier : aliased Defining_Name_ID;  -- a_nodes.h:1669
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:1673
      While_Condition : aliased Expression_ID;  -- a_nodes.h:1675
      For_Loop_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1677
      Loop_Statements : aliased Statement_List;  -- a_nodes.h:1681
      Is_Declare_Block : aliased Extensions.bool;  -- a_nodes.h:1683
      Block_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1684
      Block_Statements : aliased Statement_List;  -- a_nodes.h:1685
      Block_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1686
      Exit_Loop_Name : aliased Expression_ID;  -- a_nodes.h:1688
      Exit_Condition : aliased Expression_ID;  -- a_nodes.h:1689
      Corresponding_Loop_Exited : aliased Expression_ID;  -- a_nodes.h:1690
      Return_Expression : aliased Expression_ID;  -- a_nodes.h:1692
      Return_Object_Declaration : aliased Declaration_ID;  -- a_nodes.h:1695
      Extended_Return_Statements : aliased Statement_List;  -- a_nodes.h:1696
      Extended_Return_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1697
      Goto_Label : aliased Expression_ID;  -- a_nodes.h:1699
      Corresponding_Destination_Statement : aliased Statement_ID;  -- a_nodes.h:1700
      Called_Name : aliased Expression_ID;  -- a_nodes.h:1703
      Corresponding_Called_Entity : aliased Declaration_ID;  -- a_nodes.h:1704
      Call_Statement_Parameters : aliased Association_List;  -- a_nodes.h:1705
      Accept_Entry_Index : aliased Expression_ID;  -- a_nodes.h:1708
      Accept_Entry_Direct_Name : aliased Name_ID;  -- a_nodes.h:1709
      Accept_Parameters : aliased Parameter_Specification_List;  -- a_nodes.h:1711
      Accept_Body_Statements : aliased Statement_List;  -- a_nodes.h:1712
      Accept_Body_Exception_Handlers : aliased Statement_List;  -- a_nodes.h:1713
      Corresponding_Entry : aliased Declaration_ID;  -- a_nodes.h:1714
      Requeue_Entry_Name : aliased Name_ID;  -- a_nodes.h:1717
      Delay_Expression : aliased Expression_ID;  -- a_nodes.h:1720
      Aborted_Tasks : aliased Expression_List;  -- a_nodes.h:1722
      Raised_Exception : aliased Expression_ID;  -- a_nodes.h:1724
      Associated_Message : aliased Expression_ID;  -- a_nodes.h:1725
      Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1727
      Is_Dispatching_Call : aliased Extensions.bool;  -- a_nodes.h:1729
      Is_Call_On_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1730
      Corresponding_Called_Entity_Unwound : aliased Declaration;  -- a_nodes.h:1733
   end record;
   pragma Convention (C_Pass_By_Copy, Statement_Struct);  -- a_nodes.h:1632

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
   pragma Convention (C, Path_Kinds);  -- a_nodes.h:1744

   type Path_Struct is record
      Path_Kind : aliased Path_Kinds;  -- a_nodes.h:1800
      Sequence_Of_Statements : aliased Statement_List;  -- a_nodes.h:1801
      Dependent_Expression : aliased Expression;  -- a_nodes.h:1802
      Condition_Expression : aliased Expression_ID;  -- a_nodes.h:1807
      Case_Path_Alternative_Choices : aliased Element_List;  -- a_nodes.h:1810
      Guard : aliased Expression_ID;  -- a_nodes.h:1813
   end record;
   pragma Convention (C_Pass_By_Copy, Path_Struct);  -- a_nodes.h:1799

   type Clause_Kinds is 
     (Not_A_Clause,
      A_Use_Package_Clause,
      A_Use_Type_Clause,
      A_Use_All_Type_Clause,
      A_With_Clause,
      A_Representation_Clause,
      A_Component_Clause);
   pragma Convention (C, Clause_Kinds);  -- a_nodes.h:1824

   type u_Representation_Clause_Kinds is 
     (Not_A_Representation_Clause,
      An_Attribute_Definition_Clause,
      An_Enumeration_Representation_Clause,
      A_Record_Representation_Clause,
      An_At_Clause);
   pragma Convention (C, u_Representation_Clause_Kinds);  -- a_nodes.h:1836

   subtype Representation_Clause_Kinds is u_Representation_Clause_Kinds;  -- a_nodes.h:1842

   type u_Representation_Clause_Struct is record
      Representation_Clause_Kind : aliased Representation_Clause_Kinds;  -- a_nodes.h:1845
      Representation_Clause_Name : aliased Name;  -- a_nodes.h:1846
      Pragmas : aliased Pragma_Element_List;  -- a_nodes.h:1850
      Representation_Clause_Expression : aliased Expression;  -- a_nodes.h:1854
      Mod_Clause_Expression : aliased Expression;  -- a_nodes.h:1856
      Component_Clauses : aliased Component_Clause_List;  -- a_nodes.h:1857
   end record;
   pragma Convention (C_Pass_By_Copy, u_Representation_Clause_Struct);  -- a_nodes.h:1844

   subtype Representation_Clause_Struct is u_Representation_Clause_Struct;  -- a_nodes.h:1858

   type Clause_Struct is record
      Clause_Kind : aliased Clause_Kinds;  -- a_nodes.h:1862
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1865
      Clause_Names : aliased Name_List;  -- a_nodes.h:1870
      Representation_Clause_Name : aliased Name_ID;  -- a_nodes.h:1872
      Representation_Clause_Expression : aliased Expression_ID;  -- a_nodes.h:1873
      Mod_Clause_Expression : aliased Expression_ID;  -- a_nodes.h:1874
      Component_Clauses : aliased Element_List;  -- a_nodes.h:1875
      Component_Clause_Position : aliased Expression_ID;  -- a_nodes.h:1876
      Component_Clause_Range : aliased Element_ID;  -- a_nodes.h:1877
      Representation_Clause : aliased Representation_Clause_Struct;  -- a_nodes.h:1880
   end record;
   pragma Convention (C_Pass_By_Copy, Clause_Struct);  -- a_nodes.h:1861

   type Exception_Handler_Struct is record
      Pragmas : aliased Pragma_Element_List;  -- a_nodes.h:1896
      Choice_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1897
      Exception_Choices : aliased Element_List;  -- a_nodes.h:1898
      Handler_Statements : aliased Statement_List;  -- a_nodes.h:1899
   end record;
   pragma Convention (C_Pass_By_Copy, Exception_Handler_Struct);  -- a_nodes.h:1895

   type Element_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1912
         when 1 =>
            The_Pragma : aliased Pragma_Struct;  -- a_nodes.h:1913
         when 2 =>
            Defining_Name : aliased Defining_Name_Struct;  -- a_nodes.h:1914
         when 3 =>
            Declaration : aliased Declaration_Struct;  -- a_nodes.h:1915
         when 4 =>
            Definition : aliased Definition_Struct;  -- a_nodes.h:1916
         when 5 =>
            Expression : aliased Expression_Struct;  -- a_nodes.h:1917
         when 6 =>
            Association : aliased Association_Struct;  -- a_nodes.h:1918
         when 7 =>
            Statement : aliased Statement_Struct;  -- a_nodes.h:1919
         when 8 =>
            Path : aliased Path_Struct;  -- a_nodes.h:1920
         when 9 =>
            Clause : aliased Clause_Struct;  -- a_nodes.h:1921
         when others =>
            Exception_Handler : aliased Exception_Handler_Struct;  -- a_nodes.h:1922
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Union);
   pragma Unchecked_Union (Element_Union);  -- a_nodes.h:1911

   type Enclosing_Kinds is 
     (Not_Enclosing,
      Enclosing_Element,
      Enclosing_Unit);
   pragma Convention (C, Enclosing_Kinds);  -- a_nodes.h:1925

   type Source_Location_Struct is record
      Unit_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1933
      First_Line : aliased int;  -- a_nodes.h:1934
      First_Column : aliased int;  -- a_nodes.h:1935
      Last_Line : aliased int;  -- a_nodes.h:1936
      Last_Column : aliased int;  -- a_nodes.h:1937
   end record;
   pragma Convention (C_Pass_By_Copy, Source_Location_Struct);  -- a_nodes.h:1932

   type Element_Struct is record
      ID : aliased Element_ID;  -- a_nodes.h:1943
      Enclosing_Compilation_Unit : aliased Node_ID;  -- a_nodes.h:1944
      Element_Kind : aliased Element_Kinds;  -- a_nodes.h:1945
      Is_Part_Of_Implicit : aliased Extensions.bool;  -- a_nodes.h:1946
      Is_Part_Of_Inherited : aliased Extensions.bool;  -- a_nodes.h:1947
      Is_Part_Of_Instance : aliased Extensions.bool;  -- a_nodes.h:1948
      Hash : aliased ASIS_Integer;  -- a_nodes.h:1949
      Enclosing_Element_ID : aliased Node_ID;  -- a_nodes.h:1950
      Enclosing_Kind : aliased Enclosing_Kinds;  -- a_nodes.h:1951
      Source_Location : aliased Source_Location_Struct;  -- a_nodes.h:1952
      The_Union : aliased Element_Union;  -- a_nodes.h:1953
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Struct);  -- a_nodes.h:1941

   subtype Unit_ID is Node_ID;  -- a_nodes.h:1963

   type Unit_ID_Ptr is access all Unit_ID;  -- a_nodes.h:1965

   type Unit_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:1970
      IDs : Unit_ID_Ptr;  -- a_nodes.h:1971
   end record;
   pragma Convention (C_Pass_By_Copy, Unit_ID_Array_Struct);  -- a_nodes.h:1969

   subtype Unit_List is Unit_ID_Array_Struct;  -- a_nodes.h:1973

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
   pragma Convention (C, Unit_Kinds);  -- a_nodes.h:1975

   type Unit_Classes is 
     (Not_A_Class,
      A_Public_Declaration,
      A_Public_Body,
      A_Public_Declaration_And_Body,
      A_Private_Declaration,
      A_Private_Body,
      A_Separate_Body);
   pragma Convention (C, Unit_Classes);  -- a_nodes.h:2042

   type Unit_Origins is 
     (Not_An_Origin,
      A_Predefined_Unit,
      An_Implementation_Unit,
      An_Application_Unit);
   pragma Convention (C, Unit_Origins);  -- a_nodes.h:2063

   type Unit_Struct is record
      ID : aliased Unit_ID;  -- a_nodes.h:2083
      Unit_Kind : aliased Unit_Kinds;  -- a_nodes.h:2084
      Unit_Class : aliased Unit_Classes;  -- a_nodes.h:2085
      Unit_Origin : aliased Unit_Origins;  -- a_nodes.h:2086
      Unit_Full_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2087
      Unique_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2088
      Exists : aliased Extensions.bool;  -- a_nodes.h:2089
      Can_Be_Main_Program : aliased Extensions.bool;  -- a_nodes.h:2090
      Is_Body_Required : aliased Extensions.bool;  -- a_nodes.h:2091
      Text_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2092
      Text_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2093
      Object_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2094
      Object_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2095
      Compilation_Command_Line_Options : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2096
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2097
      Unit_Declaration : aliased Declaration_ID;  -- a_nodes.h:2098
      Context_Clause_Elements : aliased Context_Clause_List;  -- a_nodes.h:2099
      Compilation_Pragmas : aliased Pragma_Element_List;  -- a_nodes.h:2100
      Is_Standard : aliased Extensions.bool;  -- a_nodes.h:2101
      Corresponding_Children : aliased Unit_List;  -- a_nodes.h:2107
      Corresponding_Parent_Declaration : aliased Unit_ID;  -- a_nodes.h:2126
      Corresponding_Declaration : aliased Unit_ID;  -- a_nodes.h:2131
      Corresponding_Body : aliased Unit_ID;  -- a_nodes.h:2139
      Subunits : aliased Unit_List;  -- a_nodes.h:2148
      Corresponding_Subunit_Parent_Body : aliased Unit_ID;  -- a_nodes.h:2154
   end record;
   pragma Convention (C_Pass_By_Copy, Unit_Struct);  -- a_nodes.h:2082

   type Context_Struct is record
      name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2165
      parameters : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2166
      debug_image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2167
   end record;
   pragma Convention (C_Pass_By_Copy, Context_Struct);  -- a_nodes.h:2164

   type Node_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:2176
         when 1 =>
            Element : aliased Element_Struct;  -- a_nodes.h:2177
         when 2 =>
            Unit : aliased Unit_Struct;  -- a_nodes.h:2178
         when others =>
            Context : aliased Context_Struct;  -- a_nodes.h:2179
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Node_Union);
   pragma Unchecked_Union (Node_Union);  -- a_nodes.h:2175

   type Node_Struct is record
      Node_Kind : aliased Node_Kinds;  -- a_nodes.h:2184
      The_Union : aliased Node_Union;  -- a_nodes.h:2185
   end record;
   pragma Convention (C_Pass_By_Copy, Node_Struct);  -- a_nodes.h:2183

   type List_Node_Struct is record
      Node : aliased Node_Struct;  -- a_nodes.h:2190
      Next : access List_Node_Struct;  -- a_nodes.h:2191
      Next_Count : aliased int;  -- a_nodes.h:2193
   end record;
   pragma Convention (C_Pass_By_Copy, List_Node_Struct);  -- a_nodes.h:2189

   type Node_List_Ptr is access all List_Node_Struct;  -- a_nodes.h:2196

end a_nodes_h;
