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

   type u_Element_ID_Struct is record
      Node : aliased Node_ID;  -- a_nodes.h:32
      Kind : aliased int;  -- a_nodes.h:33
   end record;
   pragma Convention (C_Pass_By_Copy, u_Element_ID_Struct);  -- a_nodes.h:31

   subtype Element_ID_Struct is u_Element_ID_Struct;

   subtype Element_ID is u_Element_ID_Struct;

   type Element_ID_Ptr is access all Element_ID;  -- a_nodes.h:53

   type u_Element_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:58
      IDs : Element_ID_Ptr;  -- a_nodes.h:59
   end record;
   pragma Convention (C_Pass_By_Copy, u_Element_ID_Array_Struct);  -- a_nodes.h:57

   subtype Element_ID_Array_Struct is u_Element_ID_Array_Struct;

   subtype Element_ID_List is u_Element_ID_Array_Struct;

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
   pragma Convention (C, Element_Kinds);  -- a_nodes.h:64

   subtype Access_Type_Definition is u_Element_ID_Struct;

   subtype Association is u_Element_ID_Struct;

   subtype Association_List is u_Element_ID_Array_Struct;

   subtype Case_Statement_Alternative is u_Element_ID_Struct;

   subtype Clause is u_Element_ID_Struct;

   subtype Component_Clause is u_Element_ID_Struct;

   subtype Component_Clause_List is u_Element_ID_Array_Struct;

   subtype Component_Declaration is u_Element_ID_Struct;

   subtype Component_Definition is u_Element_ID_Struct;

   subtype Constraint_ID is u_Element_ID_Struct;

   subtype Constraint is u_Element_ID_Struct;

   subtype Context_Clause is u_Element_ID_Struct;

   subtype Context_Clause_List is u_Element_ID_Array_Struct;

   subtype Declaration is u_Element_ID_Struct;

   subtype Declaration_ID is u_Element_ID_Struct;

   subtype Declaration_List is u_Element_ID_Array_Struct;

   subtype Declarative_Item_List is u_Element_ID_Array_Struct;

   subtype Defining_Name_ID is u_Element_ID_Struct;

   subtype Definition is u_Element_ID_Struct;

   subtype Definition_ID is u_Element_ID_Struct;

   subtype Definition_List is u_Element_ID_Array_Struct;

   subtype Discrete_Range is u_Element_ID_Struct;

   subtype Discrete_Range_ID is u_Element_ID_Struct;

   subtype Discrete_Range_List is u_Element_ID_Array_Struct;

   subtype Discrete_Subtype_Definition is u_Element_ID_Struct;

   subtype Discrete_Subtype_Definition_ID is u_Element_ID_Struct;

   subtype Discriminant_Association is u_Element_ID_Struct;

   subtype Discriminant_Association_List is u_Element_ID_Array_Struct;

   subtype Discriminant_Specification_List is u_Element_ID_Array_Struct;

   subtype Defining_Name is u_Element_ID_Struct;

   subtype Defining_Name_List is u_Element_ID_Array_Struct;

   subtype Exception_Handler is u_Element_ID_Struct;

   subtype Exception_Handler_List is u_Element_ID_Array_Struct;

   subtype Expression is u_Element_ID_Struct;

   subtype Expression_ID is u_Element_ID_Struct;

   subtype Expression_List is u_Element_ID_Array_Struct;

   subtype Expression_Path_List is u_Element_ID_Array_Struct;

   subtype Formal_Type_Definition is u_Element_ID_Struct;

   subtype Generic_Formal_Parameter is u_Element_ID_Struct;

   subtype Generic_Formal_Parameter_List is u_Element_ID_Array_Struct;

   subtype Identifier is u_Element_ID_Struct;

   subtype Identifier_List is u_Element_ID_Array_Struct;

   subtype Name is u_Element_ID_Struct;

   subtype Name_ID is u_Element_ID_Struct;

   subtype Name_List is u_Element_ID_Array_Struct;

   subtype Parameter_Specification is u_Element_ID_Struct;

   subtype Parameter_Specification_List is u_Element_ID_Array_Struct;

   subtype Path is u_Element_ID_Struct;

   subtype Path_List is u_Element_ID_Array_Struct;

   subtype Pragma_Element is u_Element_ID_Struct;

   subtype Pragma_Element_ID_List is u_Element_ID_Array_Struct;

   subtype Range_Constraint is u_Element_ID_Struct;

   subtype Record_Component is u_Element_ID_Struct;

   subtype Record_Component_List is u_Element_ID_Array_Struct;

   subtype Record_Definition is u_Element_ID_Struct;

   subtype Representation_Clause is u_Element_ID_Struct;

   subtype Representation_Clause_List is u_Element_ID_Array_Struct;

   subtype Root_Type_Definition is u_Element_ID_Struct;

   subtype Select_Alternative is u_Element_ID_Struct;

   subtype Statement is u_Element_ID_Struct;

   subtype Statement_ID is u_Element_ID_Struct;

   subtype Statement_List is u_Element_ID_Array_Struct;

   subtype Subtype_Indication is u_Element_ID_Struct;

   subtype Subtype_Indication_ID is u_Element_ID_Struct;

   subtype Subtype_Mark is u_Element_ID_Struct;

   subtype Type_Definition is u_Element_ID_Struct;

   subtype Type_Definition_ID is u_Element_ID_Struct;

   subtype Variant is u_Element_ID_Struct;

   subtype Variant_Component_List is u_Element_ID_Array_Struct;

   subtype Variant_List is u_Element_ID_Array_Struct;

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
   pragma Convention (C, Operator_Kinds);  -- a_nodes.h:159

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
   pragma Convention (C, Pragma_Kinds);  -- a_nodes.h:189

   type Pragma_Struct is record
      Pragma_Kind : aliased Pragma_Kinds;  -- a_nodes.h:250
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:251
      Pragma_Name_Image : Program_Text;  -- a_nodes.h:252
      Pragma_Argument_Associations : aliased Association_List;  -- a_nodes.h:253
   end record;
   pragma Convention (C_Pass_By_Copy, Pragma_Struct);  -- a_nodes.h:249

   type Defining_Name_Kinds is 
     (Not_A_Defining_Name,
      A_Defining_Identifier,
      A_Defining_Character_Literal,
      A_Defining_Enumeration_Literal,
      A_Defining_Operator_Symbol,
      A_Defining_Expanded_Name);
   pragma Convention (C, Defining_Name_Kinds);  -- a_nodes.h:264

   type Defining_Name_Struct is record
      Defining_Name_Kind : aliased Defining_Name_Kinds;  -- a_nodes.h:275
      Defining_Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:276
      References : aliased Name_List;  -- a_nodes.h:277
      Is_Referenced : aliased Extensions.bool;  -- a_nodes.h:278
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:282
      Position_Number_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:285
      Representation_Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:286
      Defining_Prefix : aliased Name_ID;  -- a_nodes.h:288
      Defining_Selector : aliased Defining_Name_ID;  -- a_nodes.h:289
      Corresponding_Constant_Declaration : aliased Declaration_ID;  -- a_nodes.h:291
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:294
   end record;
   pragma Convention (C_Pass_By_Copy, Defining_Name_Struct);  -- a_nodes.h:274

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
   pragma Convention (C, Declaration_Kinds);  -- a_nodes.h:305

   type Declaration_Origins is 
     (Not_A_Declaration_Origin,
      An_Explicit_Declaration,
      An_Implicit_Predefined_Declaration,
      An_Implicit_Inherited_Declaration);
   pragma Convention (C, Declaration_Origins);  -- a_nodes.h:409

   type Mode_Kinds is 
     (Not_A_Mode,
      A_Default_In_Mode,
      An_In_Mode,
      An_Out_Mode,
      An_In_Out_Mode);
   pragma Convention (C, Mode_Kinds);  -- a_nodes.h:425

   type Subprogram_Default_Kinds is 
     (Not_A_Default,
      A_Name_Default,
      A_Box_Default,
      A_Null_Default,
      A_Nil_Default);
   pragma Convention (C, Subprogram_Default_Kinds);  -- a_nodes.h:435

   type Declaration_Struct is record
      Declaration_Kind : aliased Declaration_Kinds;  -- a_nodes.h:448
      Declaration_Origin : aliased Declaration_Origins;  -- a_nodes.h:449
      Corresponding_Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:450
      Names : aliased Defining_Name_List;  -- a_nodes.h:451
      Aspect_Specifications : aliased Element_ID_List;  -- a_nodes.h:452
      Corresponding_Representation_Clauses : aliased Representation_Clause_List;  -- a_nodes.h:453
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:463
      Has_Aliased : aliased Extensions.bool;  -- a_nodes.h:470
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:474
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:477
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:482
      Has_Reverse : aliased Extensions.bool;  -- a_nodes.h:486
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:491
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:495
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:503
      Mode_Kind : aliased Mode_Kinds;  -- a_nodes.h:506
      Default_Kind : aliased Subprogram_Default_Kinds;  -- a_nodes.h:509
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:529
      Corresponding_End_Name : aliased Element_ID;  -- a_nodes.h:542
      Discriminant_Part : aliased Definition_ID;  -- a_nodes.h:551
      Type_Declaration_View : aliased Definition_ID;  -- a_nodes.h:558
      Object_Declaration_View : aliased Definition_ID;  -- a_nodes.h:570
      Initialization_Expression : aliased Expression_ID;  -- a_nodes.h:580
      Corresponding_Type_Declaration : aliased Declaration_ID;  -- a_nodes.h:588
      Corresponding_Type_Completion : aliased Declaration_ID;  -- a_nodes.h:593
      Corresponding_Type_Partial_View : aliased Declaration_ID;  -- a_nodes.h:599
      Corresponding_First_Subtype : aliased Declaration_ID;  -- a_nodes.h:607
      Corresponding_Last_Constraint : aliased Declaration_ID;  -- a_nodes.h:608
      Corresponding_Last_Subtype : aliased Declaration_ID;  -- a_nodes.h:609
      Specification_Subtype_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:612
      Iteration_Scheme_Name : aliased Element_ID;  -- a_nodes.h:615
      Subtype_Indication : aliased Element_ID;  -- a_nodes.h:617
      Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:634
      Result_Profile : aliased Element_ID;  -- a_nodes.h:642
      Result_Expression : aliased Expression_ID;  -- a_nodes.h:644
      Is_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:660
      Is_Not_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:661
      Body_Declarative_Items : aliased Element_ID_List;  -- a_nodes.h:667
      Body_Statements : aliased Statement_List;  -- a_nodes.h:668
      Body_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:669
      Body_Block_Statement : aliased Declaration_ID;  -- a_nodes.h:670
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:683
      Corresponding_Declaration : aliased Declaration_ID;  -- a_nodes.h:719
      Corresponding_Body : aliased Declaration_ID;  -- a_nodes.h:735
      Corresponding_Subprogram_Derivation : aliased Declaration_ID;  -- a_nodes.h:738
      Corresponding_Type : aliased Type_Definition_ID;  -- a_nodes.h:742
      Corresponding_Equality_Operator : aliased Declaration_ID;  -- a_nodes.h:744
      Visible_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:747
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:748
      Private_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:749
      Declaration_Interface_List : aliased Expression_List;  -- a_nodes.h:754
      Renamed_Entity : aliased Expression_ID;  -- a_nodes.h:763
      Corresponding_Base_Entity : aliased Expression_ID;  -- a_nodes.h:764
      Protected_Operation_Items : aliased Declaration_List;  -- a_nodes.h:766
      Entry_Family_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:768
      Entry_Index_Specification : aliased Declaration_ID;  -- a_nodes.h:770
      Entry_Barrier : aliased Expression_ID;  -- a_nodes.h:771
      Corresponding_Subunit : aliased Declaration_ID;  -- a_nodes.h:777
      Is_Subunit : aliased Extensions.bool;  -- a_nodes.h:783
      Corresponding_Body_Stub : aliased Declaration_ID;  -- a_nodes.h:784
      Generic_Formal_Part : aliased Element_ID_List;  -- a_nodes.h:788
      Generic_Unit_Name : aliased Expression_ID;  -- a_nodes.h:794
      Generic_Actual_Part : aliased Association_List;  -- a_nodes.h:795
      Formal_Subprogram_Default : aliased Expression_ID;  -- a_nodes.h:798
      Is_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:809
   end record;
   pragma Convention (C_Pass_By_Copy, Declaration_Struct);  -- a_nodes.h:447

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
   pragma Convention (C, Definition_Kinds);  -- a_nodes.h:820

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
   pragma Convention (C, u_Type_Kinds);  -- a_nodes.h:853

   subtype Type_Kinds is u_Type_Kinds;

   type u_Constraint_Kinds is 
     (Not_A_Constraint,
      A_Range_Attribute_Reference,
      A_Simple_Expression_Range,
      A_Digits_Constraint,
      A_Delta_Constraint,
      An_Index_Constraint,
      A_Discriminant_Constraint);
   pragma Convention (C, u_Constraint_Kinds);  -- a_nodes.h:877

   subtype Constraint_Kinds is u_Constraint_Kinds;

   type u_Interface_Kinds is 
     (Not_An_Interface,
      An_Ordinary_Interface,
      A_Limited_Interface,
      A_Task_Interface,
      A_Protected_Interface,
      A_Synchronized_Interface);
   pragma Convention (C, u_Interface_Kinds);  -- a_nodes.h:887

   subtype Interface_Kinds is u_Interface_Kinds;

   type u_Root_Type_Kinds is 
     (Not_A_Root_Type_Definition,
      A_Root_Integer_Definition,
      A_Root_Real_Definition,
      A_Universal_Integer_Definition,
      A_Universal_Real_Definition,
      A_Universal_Fixed_Definition);
   pragma Convention (C, u_Root_Type_Kinds);  -- a_nodes.h:896

   subtype Root_Type_Kinds is u_Root_Type_Kinds;

   type u_Discrete_Range_Kinds is 
     (Not_A_Discrete_Range,
      A_Discrete_Subtype_Indication,
      A_Discrete_Range_Attribute_Reference,
      A_Discrete_Simple_Expression_Range);
   pragma Convention (C, u_Discrete_Range_Kinds);  -- a_nodes.h:905

   subtype Discrete_Range_Kinds is u_Discrete_Range_Kinds;

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
   pragma Convention (C, u_Formal_Type_Kinds);  -- a_nodes.h:912

   subtype Formal_Type_Kinds is u_Formal_Type_Kinds;

   type u_Access_Type_Kinds is 
     (Not_An_Access_Type_Definition,
      A_Pool_Specific_Access_To_Variable,
      An_Access_To_Variable,
      An_Access_To_Constant,
      An_Access_To_Procedure,
      An_Access_To_Protected_Procedure,
      An_Access_To_Function,
      An_Access_To_Protected_Function);
   pragma Convention (C, u_Access_Type_Kinds);  -- a_nodes.h:934

   subtype Access_Type_Kinds is u_Access_Type_Kinds;

   type u_Access_Definition_Kinds is 
     (Not_An_Access_Definition,
      An_Anonymous_Access_To_Variable,
      An_Anonymous_Access_To_Constant,
      An_Anonymous_Access_To_Procedure,
      An_Anonymous_Access_To_Protected_Procedure,
      An_Anonymous_Access_To_Function,
      An_Anonymous_Access_To_Protected_Function);
   pragma Convention (C, u_Access_Definition_Kinds);  -- a_nodes.h:945

   subtype Access_Definition_Kinds is u_Access_Definition_Kinds;

   type u_Access_Type_Struct is record
      Access_Type_Kind : aliased Access_Type_Kinds;  -- a_nodes.h:956
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:957
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:961
      Access_To_Object_Definition : aliased Subtype_Indication;  -- a_nodes.h:965
      Access_To_Subprogram_Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:970
      Access_To_Function_Result_Profile : aliased Element_ID;  -- a_nodes.h:973
   end record;
   pragma Convention (C_Pass_By_Copy, u_Access_Type_Struct);  -- a_nodes.h:955

   subtype Access_Type_Struct is u_Access_Type_Struct;

   type u_Type_Definition_Struct is record
      Type_Kind : aliased Type_Kinds;  -- a_nodes.h:977
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:978
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:979
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:980
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:981
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:984
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:985
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:987
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:989
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:994
      Interface_Kind : aliased Interface_Kinds;  -- a_nodes.h:996
      Root_Type_Kind : aliased Root_Type_Kinds;  -- a_nodes.h:998
      Parent_Subtype_Indication : aliased Subtype_Indication;  -- a_nodes.h:1001
      Record_Definition : aliased Definition;  -- a_nodes.h:1005
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1008
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1009
      Corresponding_Parent_Subtype : aliased Declaration;  -- a_nodes.h:1010
      Corresponding_Root_Type : aliased Declaration;  -- a_nodes.h:1011
      Corresponding_Type_Structure : aliased Declaration;  -- a_nodes.h:1012
      Enumeration_Literal_Declarations : aliased Declaration_List;  -- a_nodes.h:1014
      Integer_Constraint : aliased Range_Constraint;  -- a_nodes.h:1016
      Mod_Static_Expression : aliased Expression;  -- a_nodes.h:1018
      Digits_Expression : aliased Expression;  -- a_nodes.h:1021
      Delta_Expression : aliased Expression;  -- a_nodes.h:1024
      Real_Range_Constraint : aliased Range_Constraint;  -- a_nodes.h:1028
      Index_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1030
      Discrete_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1032
      Array_Component_Definition : aliased Component_Definition;  -- a_nodes.h:1035
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1038
      Access_Type : aliased Access_Type_Struct;  -- a_nodes.h:1040
   end record;
   pragma Convention (C_Pass_By_Copy, u_Type_Definition_Struct);  -- a_nodes.h:976

   subtype Type_Definition_Struct is u_Type_Definition_Struct;

   type u_Subtype_Indication_Struct is record
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1044
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1045
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1046
   end record;
   pragma Convention (C_Pass_By_Copy, u_Subtype_Indication_Struct);  -- a_nodes.h:1043

   subtype Subtype_Indication_Struct is u_Subtype_Indication_Struct;

   type u_Constraint_Struct is record
      Constraint_Kind : aliased Constraint_Kinds;  -- a_nodes.h:1050
      Digits_Expression : aliased Expression;  -- a_nodes.h:1053
      Delta_Expression : aliased Expression;  -- a_nodes.h:1055
      Real_Range_Constraint : aliased Range_Constraint;  -- a_nodes.h:1058
      Lower_Bound : aliased Expression;  -- a_nodes.h:1060
      Upper_Bound : aliased Expression;  -- a_nodes.h:1061
      Range_Attribute : aliased Expression;  -- a_nodes.h:1063
      Discrete_Ranges : aliased Discrete_Range_List;  -- a_nodes.h:1065
      Discriminant_Associations : aliased Discriminant_Association_List;  -- a_nodes.h:1067
   end record;
   pragma Convention (C_Pass_By_Copy, u_Constraint_Struct);  -- a_nodes.h:1049

   subtype Constraint_Struct is u_Constraint_Struct;

   type u_Component_Definition_Struct is record
      Component_Definition_View : aliased Definition;  -- a_nodes.h:1071
   end record;
   pragma Convention (C_Pass_By_Copy, u_Component_Definition_Struct);  -- a_nodes.h:1070

   subtype Component_Definition_Struct is u_Component_Definition_Struct;

   type u_Discrete_Subtype_Definition_Struct is record
      Discrete_Range_Kind : aliased Discrete_Range_Kinds;  -- a_nodes.h:1075
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1078
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1079
   end record;
   pragma Convention (C_Pass_By_Copy, u_Discrete_Subtype_Definition_Struct);  -- a_nodes.h:1074

   subtype Discrete_Subtype_Definition_Struct is u_Discrete_Subtype_Definition_Struct;

   type u_Discrete_Range_Struct is record
      Discrete_Range_Kind : aliased Discrete_Range_Kinds;  -- a_nodes.h:1083
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1086
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1087
      Lower_Bound : aliased Expression;  -- a_nodes.h:1089
      Upper_Bound : aliased Expression;  -- a_nodes.h:1090
      Range_Attribute : aliased Expression;  -- a_nodes.h:1092
   end record;
   pragma Convention (C_Pass_By_Copy, u_Discrete_Range_Struct);  -- a_nodes.h:1082

   subtype Discrete_Range_Struct is u_Discrete_Range_Struct;

   type u_Known_Discriminant_Part_Struct is record
      Discriminants : aliased Discriminant_Specification_List;  -- a_nodes.h:1096
   end record;
   pragma Convention (C_Pass_By_Copy, u_Known_Discriminant_Part_Struct);  -- a_nodes.h:1095

   subtype Known_Discriminant_Part_Struct is u_Known_Discriminant_Part_Struct;

   type u_Record_Definition_Struct is record
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1100
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1101
   end record;
   pragma Convention (C_Pass_By_Copy, u_Record_Definition_Struct);  -- a_nodes.h:1099

   subtype Record_Definition_Struct is u_Record_Definition_Struct;

   type u_Variant_Part_Struct is record
      Discriminant_Direct_Name : aliased Name;  -- a_nodes.h:1105
      Variants : aliased Variant_List;  -- a_nodes.h:1106
   end record;
   pragma Convention (C_Pass_By_Copy, u_Variant_Part_Struct);  -- a_nodes.h:1104

   subtype Variant_Part_Struct is u_Variant_Part_Struct;

   type u_Variant_Struct is record
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1110
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1111
      Variant_Choices : aliased Element_ID_List;  -- a_nodes.h:1112
   end record;
   pragma Convention (C_Pass_By_Copy, u_Variant_Struct);  -- a_nodes.h:1109

   subtype Variant_Struct is u_Variant_Struct;

   type u_Access_Definition_Struct is record
      Access_Definition_Kind : aliased Access_Definition_Kinds;  -- a_nodes.h:1116
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1117
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:1121
      Anonymous_Access_To_Object_Subtype_Mark : aliased Expression;  -- a_nodes.h:1124
      Access_To_Subprogram_Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:1129
      Access_To_Function_Result_Profile : aliased Element_ID;  -- a_nodes.h:1132
   end record;
   pragma Convention (C_Pass_By_Copy, u_Access_Definition_Struct);  -- a_nodes.h:1115

   subtype Access_Definition_Struct is u_Access_Definition_Struct;

   type u_Private_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1136
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1137
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1138
   end record;
   pragma Convention (C_Pass_By_Copy, u_Private_Type_Definition_Struct);  -- a_nodes.h:1135

   subtype Private_Type_Definition_Struct is u_Private_Type_Definition_Struct;

   type u_Tagged_Private_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1142
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1143
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1144
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:1145
   end record;
   pragma Convention (C_Pass_By_Copy, u_Tagged_Private_Type_Definition_Struct);  -- a_nodes.h:1141

   subtype Tagged_Private_Type_Definition_Struct is u_Tagged_Private_Type_Definition_Struct;

   type u_Private_Extension_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1149
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1150
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1151
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:1152
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1153
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1154
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1155
      Ancestor_Subtype_Indication : aliased Subtype_Indication;  -- a_nodes.h:1156
   end record;
   pragma Convention (C_Pass_By_Copy, u_Private_Extension_Definition_Struct);  -- a_nodes.h:1148

   subtype Private_Extension_Definition_Struct is u_Private_Extension_Definition_Struct;

   type u_Task_Definition_Struct is record
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:1160
      Visible_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1161
      Private_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1162
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1163
   end record;
   pragma Convention (C_Pass_By_Copy, u_Task_Definition_Struct);  -- a_nodes.h:1159

   subtype Task_Definition_Struct is u_Task_Definition_Struct;

   type u_Protected_Definition_Struct is record
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:1167
      Visible_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1168
      Private_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1169
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1170
   end record;
   pragma Convention (C_Pass_By_Copy, u_Protected_Definition_Struct);  -- a_nodes.h:1166

   subtype Protected_Definition_Struct is u_Protected_Definition_Struct;

   type u_Formal_Type_Definition_Struct is record
      Formal_Type_Kind : aliased Formal_Type_Kinds;  -- a_nodes.h:1174
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1175
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1180
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1181
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1184
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:1186
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:1188
      Interface_Kind : aliased Interface_Kinds;  -- a_nodes.h:1190
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1192
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1193
      Index_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1195
      Discrete_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1197
      Array_Component_Definition : aliased Component_Definition;  -- a_nodes.h:1200
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1202
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1205
      Access_Type : aliased Access_Type_Struct;  -- a_nodes.h:1207
   end record;
   pragma Convention (C_Pass_By_Copy, u_Formal_Type_Definition_Struct);  -- a_nodes.h:1173

   subtype Formal_Type_Definition_Struct is u_Formal_Type_Definition_Struct;

   type u_Aspect_Specification_Struct is record
      Aspect_Mark : aliased Element_ID;  -- a_nodes.h:1211
      Aspect_Definition : aliased Element_ID;  -- a_nodes.h:1212
   end record;
   pragma Convention (C_Pass_By_Copy, u_Aspect_Specification_Struct);  -- a_nodes.h:1210

   subtype Aspect_Specification_Struct is u_Aspect_Specification_Struct;

   subtype No_Struct is int;  -- a_nodes.h:1215

   subtype Unknown_Discriminant_Part_Struct is No_Struct;  -- a_nodes.h:1216

   subtype Null_Record_Definition_Struct is No_Struct;  -- a_nodes.h:1217

   subtype Null_Component_Struct is No_Struct;  -- a_nodes.h:1218

   subtype Others_Choice_Struct is No_Struct;  -- a_nodes.h:1219

   type u_Definition_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1222
         when 1 =>
            The_Type_Definition : aliased Type_Definition_Struct;  -- a_nodes.h:1223
         when 2 =>
            The_Subtype_Indication : aliased Subtype_Indication_Struct;  -- a_nodes.h:1224
         when 3 =>
            The_Constraint : aliased Constraint_Struct;  -- a_nodes.h:1225
         when 4 =>
            The_Component_Definition : aliased Component_Definition_Struct;  -- a_nodes.h:1226
         when 5 =>
            The_Discrete_Subtype_Definition : aliased Discrete_Subtype_Definition_Struct;  -- a_nodes.h:1227
         when 6 =>
            The_Discrete_Range : aliased Discrete_Range_Struct;  -- a_nodes.h:1228
         when 7 =>
            The_Unknown_Discriminant_Part : aliased Unknown_Discriminant_Part_Struct;  -- a_nodes.h:1229
         when 8 =>
            The_Known_Discriminant_Part : aliased Known_Discriminant_Part_Struct;  -- a_nodes.h:1230
         when 9 =>
            The_Record_Definition : aliased Record_Definition_Struct;  -- a_nodes.h:1231
         when 10 =>
            The_Null_Record_Definition : aliased Null_Record_Definition_Struct;  -- a_nodes.h:1232
         when 11 =>
            The_Null_Component : aliased Null_Component_Struct;  -- a_nodes.h:1233
         when 12 =>
            The_Variant_Part : aliased Variant_Part_Struct;  -- a_nodes.h:1234
         when 13 =>
            The_Variant : aliased Variant_Struct;  -- a_nodes.h:1235
         when 14 =>
            The_Others_Choice : aliased Others_Choice_Struct;  -- a_nodes.h:1236
         when 15 =>
            The_Access_Definition : aliased Access_Definition_Struct;  -- a_nodes.h:1237
         when 16 =>
            The_Private_Type_Definition : aliased Private_Type_Definition_Struct;  -- a_nodes.h:1238
         when 17 =>
            The_Tagged_Private_Type_Definition : aliased Tagged_Private_Type_Definition_Struct;  -- a_nodes.h:1239
         when 18 =>
            The_Private_Extension_Definition : aliased Private_Extension_Definition_Struct;  -- a_nodes.h:1240
         when 19 =>
            The_Task_Definition : aliased Task_Definition_Struct;  -- a_nodes.h:1241
         when 20 =>
            The_Protected_Definition : aliased Protected_Definition_Struct;  -- a_nodes.h:1242
         when 21 =>
            The_Formal_Type_Definition : aliased Formal_Type_Definition_Struct;  -- a_nodes.h:1243
         when others =>
            The_Aspect_Specification : aliased Aspect_Specification_Struct;  -- a_nodes.h:1244
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, u_Definition_Union);
   pragma Unchecked_Union (u_Definition_Union);  -- a_nodes.h:1221

   subtype Definition_Union is u_Definition_Union;

   type Definition_Struct is record
      Definition_Kind : aliased Definition_Kinds;  -- a_nodes.h:1249
      The_Union : aliased Definition_Union;  -- a_nodes.h:1250
   end record;
   pragma Convention (C_Pass_By_Copy, Definition_Struct);  -- a_nodes.h:1248

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
   pragma Convention (C, Expression_Kinds);  -- a_nodes.h:1261

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
   pragma Convention (C, Attribute_Kinds);  -- a_nodes.h:1306

   type Expression_Struct is record
      Expression_Kind : aliased Expression_Kinds;  -- a_nodes.h:1419
      Is_Prefix_Notation : aliased Extensions.bool;  -- a_nodes.h:1420
      Corresponding_Expression_Type : aliased Declaration_ID;  -- a_nodes.h:1421
      Corresponding_Expression_Type_Definition : aliased Element_ID;  -- a_nodes.h:1422
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:1426
      Attribute_Kind : aliased Attribute_Kinds;  -- a_nodes.h:1431
      Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1432
      Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1437
      Corresponding_Name_Definition : aliased Defining_Name_ID;  -- a_nodes.h:1438
      Corresponding_Name_Definition_List : aliased Defining_Name_List;  -- a_nodes.h:1439
      Corresponding_Name_Declaration : aliased Element_ID;  -- a_nodes.h:1440
      Prefix : aliased Expression_ID;  -- a_nodes.h:1447
      Index_Expressions : aliased Expression_List;  -- a_nodes.h:1449
      Slice_Range : aliased Discrete_Range_ID;  -- a_nodes.h:1451
      Selector : aliased Expression_ID;  -- a_nodes.h:1453
      Attribute_Designator_Identifier : aliased Expression_ID;  -- a_nodes.h:1455
      Attribute_Designator_Expressions : aliased Expression_List;  -- a_nodes.h:1464
      Record_Component_Associations : aliased Association_List;  -- a_nodes.h:1467
      Extension_Aggregate_Expression : aliased Expression_ID;  -- a_nodes.h:1469
      Array_Component_Associations : aliased Association_List;  -- a_nodes.h:1472
      Expression_Parenthesized : aliased Expression_ID;  -- a_nodes.h:1474
      Is_Prefix_Call : aliased Extensions.bool;  -- a_nodes.h:1476
      Corresponding_Called_Function : aliased Declaration_ID;  -- a_nodes.h:1479
      Function_Call_Parameters : aliased Element_ID_List;  -- a_nodes.h:1481
      Short_Circuit_Operation_Left_Expression : aliased Expression_ID;  -- a_nodes.h:1484
      Short_Circuit_Operation_Right_Expression : aliased Expression_ID;  -- a_nodes.h:1485
      Membership_Test_Expression : aliased Expression_ID;  -- a_nodes.h:1488
      Membership_Test_Choices : aliased Element_ID_List;  -- a_nodes.h:1489
      Converted_Or_Qualified_Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:1492
      Converted_Or_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1493
      Allocator_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1495
      Allocator_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1497
      Expression_Paths : aliased Expression_Path_List;  -- a_nodes.h:1500
      Is_Generalized_Indexing : aliased Extensions.bool;  -- a_nodes.h:1502
      Is_Generalized_Reference : aliased Extensions.bool;  -- a_nodes.h:1504
      Iterator_Specification : aliased Declaration_ID;  -- a_nodes.h:1507
      Predicate : aliased Expression_ID;  -- a_nodes.h:1510
      Subpool_Name : aliased Expression_ID;  -- a_nodes.h:1513
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:1518
      Is_Dispatching_Call : aliased Extensions.bool;  -- a_nodes.h:1520
      Is_Call_On_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1521
   end record;
   pragma Convention (C_Pass_By_Copy, Expression_Struct);  -- a_nodes.h:1418

   type Association_Kinds is 
     (Not_An_Association,
      A_Pragma_Argument_Association,
      A_Discriminant_Association,
      A_Record_Component_Association,
      An_Array_Component_Association,
      A_Parameter_Association,
      A_Generic_Association);
   pragma Convention (C, Association_Kinds);  -- a_nodes.h:1532

   type Association_Struct is record
      Association_Kind : aliased Association_Kinds;  -- a_nodes.h:1545
      Array_Component_Choices : aliased Expression_List;  -- a_nodes.h:1548
      Record_Component_Choices : aliased Expression_List;  -- a_nodes.h:1550
      Component_Expression : aliased Expression_ID;  -- a_nodes.h:1553
      Formal_Parameter : aliased Expression_ID;  -- a_nodes.h:1557
      Actual_Parameter : aliased Expression_ID;  -- a_nodes.h:1558
      Discriminant_Selector_Names : aliased Expression_List;  -- a_nodes.h:1560
      Discriminant_Expression : aliased Expression_ID;  -- a_nodes.h:1561
      Is_Normalized : aliased Extensions.bool;  -- a_nodes.h:1566
      Is_Defaulted_Association : aliased Extensions.bool;  -- a_nodes.h:1570
   end record;
   pragma Convention (C_Pass_By_Copy, Association_Struct);  -- a_nodes.h:1544

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
   pragma Convention (C, Statement_Kinds);  -- a_nodes.h:1581

   type Statement_Struct is record
      Statement_Kind : aliased Statement_Kinds;  -- a_nodes.h:1618
      Corresponding_Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1619
      Label_Names : aliased Defining_Name_List;  -- a_nodes.h:1620
      Is_Prefix_Notation : aliased Extensions.bool;  -- a_nodes.h:1624
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1631
      Corresponding_End_Name : aliased Element_ID;  -- a_nodes.h:1637
      Assignment_Variable_Name : aliased Expression_ID;  -- a_nodes.h:1639
      Assignment_Expression : aliased Expression_ID;  -- a_nodes.h:1640
      Statement_Paths : aliased Path_List;  -- a_nodes.h:1647
      Case_Expression : aliased Expression_ID;  -- a_nodes.h:1649
      Statement_Identifier : aliased Defining_Name_ID;  -- a_nodes.h:1654
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:1658
      While_Condition : aliased Expression_ID;  -- a_nodes.h:1660
      For_Loop_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1662
      Loop_Statements : aliased Statement_List;  -- a_nodes.h:1666
      Is_Declare_Block : aliased Extensions.bool;  -- a_nodes.h:1668
      Block_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1669
      Block_Statements : aliased Statement_List;  -- a_nodes.h:1670
      Block_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1671
      Exit_Loop_Name : aliased Expression_ID;  -- a_nodes.h:1673
      Exit_Condition : aliased Expression_ID;  -- a_nodes.h:1674
      Corresponding_Loop_Exited : aliased Expression_ID;  -- a_nodes.h:1675
      Return_Expression : aliased Expression_ID;  -- a_nodes.h:1677
      Return_Object_Declaration : aliased Declaration_ID;  -- a_nodes.h:1680
      Extended_Return_Statements : aliased Statement_List;  -- a_nodes.h:1681
      Extended_Return_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1682
      Goto_Label : aliased Expression_ID;  -- a_nodes.h:1684
      Corresponding_Destination_Statement : aliased Statement_ID;  -- a_nodes.h:1685
      Called_Name : aliased Expression_ID;  -- a_nodes.h:1688
      Corresponding_Called_Entity : aliased Declaration_ID;  -- a_nodes.h:1689
      Call_Statement_Parameters : aliased Association_List;  -- a_nodes.h:1690
      Accept_Entry_Index : aliased Expression_ID;  -- a_nodes.h:1693
      Accept_Entry_Direct_Name : aliased Name_ID;  -- a_nodes.h:1694
      Accept_Parameters : aliased Parameter_Specification_List;  -- a_nodes.h:1696
      Accept_Body_Statements : aliased Statement_List;  -- a_nodes.h:1697
      Accept_Body_Exception_Handlers : aliased Statement_List;  -- a_nodes.h:1698
      Corresponding_Entry : aliased Declaration_ID;  -- a_nodes.h:1699
      Requeue_Entry_Name : aliased Name_ID;  -- a_nodes.h:1702
      Delay_Expression : aliased Expression_ID;  -- a_nodes.h:1705
      Aborted_Tasks : aliased Expression_List;  -- a_nodes.h:1707
      Raised_Exception : aliased Expression_ID;  -- a_nodes.h:1709
      Associated_Message : aliased Expression_ID;  -- a_nodes.h:1710
      Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1712
      Is_Dispatching_Call : aliased Extensions.bool;  -- a_nodes.h:1714
      Is_Call_On_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1715
      Corresponding_Called_Entity_Unwound : aliased Declaration;  -- a_nodes.h:1718
   end record;
   pragma Convention (C_Pass_By_Copy, Statement_Struct);  -- a_nodes.h:1617

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
   pragma Convention (C, Path_Kinds);  -- a_nodes.h:1729

   type Path_Struct is record
      Path_Kind : aliased Path_Kinds;  -- a_nodes.h:1785
      Sequence_Of_Statements : aliased Statement_List;  -- a_nodes.h:1786
      Dependent_Expression : aliased Expression;  -- a_nodes.h:1787
      Condition_Expression : aliased Expression_ID;  -- a_nodes.h:1792
      Case_Path_Alternative_Choices : aliased Element_ID_List;  -- a_nodes.h:1795
      Guard : aliased Expression_ID;  -- a_nodes.h:1798
   end record;
   pragma Convention (C_Pass_By_Copy, Path_Struct);  -- a_nodes.h:1784

   type Clause_Kinds is 
     (Not_A_Clause,
      A_Use_Package_Clause,
      A_Use_Type_Clause,
      A_Use_All_Type_Clause,
      A_With_Clause,
      A_Representation_Clause,
      A_Component_Clause);
   pragma Convention (C, Clause_Kinds);  -- a_nodes.h:1809

   type u_Representation_Clause_Kinds is 
     (Not_A_Representation_Clause,
      An_Attribute_Definition_Clause,
      An_Enumeration_Representation_Clause,
      A_Record_Representation_Clause,
      An_At_Clause);
   pragma Convention (C, u_Representation_Clause_Kinds);  -- a_nodes.h:1821

   subtype Representation_Clause_Kinds is u_Representation_Clause_Kinds;

   type u_Representation_Clause_Struct is record
      Representation_Clause_Kind : aliased Representation_Clause_Kinds;  -- a_nodes.h:1830
      Representation_Clause_Name : aliased Name;  -- a_nodes.h:1831
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1835
      Representation_Clause_Expression : aliased Expression;  -- a_nodes.h:1839
      Mod_Clause_Expression : aliased Expression;  -- a_nodes.h:1841
      Component_Clauses : aliased Component_Clause_List;  -- a_nodes.h:1842
   end record;
   pragma Convention (C_Pass_By_Copy, u_Representation_Clause_Struct);  -- a_nodes.h:1829

   subtype Representation_Clause_Struct is u_Representation_Clause_Struct;

   type Clause_Struct is record
      Clause_Kind : aliased Clause_Kinds;  -- a_nodes.h:1847
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1850
      Clause_Names : aliased Name_List;  -- a_nodes.h:1855
      Representation_Clause_Name : aliased Name_ID;  -- a_nodes.h:1857
      Representation_Clause_Expression : aliased Expression_ID;  -- a_nodes.h:1858
      Mod_Clause_Expression : aliased Expression_ID;  -- a_nodes.h:1859
      Component_Clauses : aliased Element_ID_List;  -- a_nodes.h:1860
      Component_Clause_Position : aliased Expression_ID;  -- a_nodes.h:1861
      Component_Clause_Range : aliased Element_ID;  -- a_nodes.h:1862
      Representation_Clause : aliased Representation_Clause_Struct;  -- a_nodes.h:1865
   end record;
   pragma Convention (C_Pass_By_Copy, Clause_Struct);  -- a_nodes.h:1846

   type Exception_Handler_Struct is record
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1881
      Choice_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1882
      Exception_Choices : aliased Element_ID_List;  -- a_nodes.h:1883
      Handler_Statements : aliased Statement_List;  -- a_nodes.h:1884
   end record;
   pragma Convention (C_Pass_By_Copy, Exception_Handler_Struct);  -- a_nodes.h:1880

   type Element_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1897
         when 1 =>
            The_Pragma : aliased Pragma_Struct;  -- a_nodes.h:1898
         when 2 =>
            Defining_Name : aliased Defining_Name_Struct;  -- a_nodes.h:1899
         when 3 =>
            Declaration : aliased Declaration_Struct;  -- a_nodes.h:1900
         when 4 =>
            Definition : aliased Definition_Struct;  -- a_nodes.h:1901
         when 5 =>
            Expression : aliased Expression_Struct;  -- a_nodes.h:1902
         when 6 =>
            Association : aliased Association_Struct;  -- a_nodes.h:1903
         when 7 =>
            Statement : aliased Statement_Struct;  -- a_nodes.h:1904
         when 8 =>
            Path : aliased Path_Struct;  -- a_nodes.h:1905
         when 9 =>
            Clause : aliased Clause_Struct;  -- a_nodes.h:1906
         when others =>
            Exception_Handler : aliased Exception_Handler_Struct;  -- a_nodes.h:1907
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Union);
   pragma Unchecked_Union (Element_Union);  -- a_nodes.h:1896

   type Enclosing_Kinds is 
     (Not_Enclosing,
      Enclosing_Element,
      Enclosing_Unit);
   pragma Convention (C, Enclosing_Kinds);  -- a_nodes.h:1910

   type Source_Location_Struct is record
      Unit_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1918
      First_Line : aliased int;  -- a_nodes.h:1919
      First_Column : aliased int;  -- a_nodes.h:1920
      Last_Line : aliased int;  -- a_nodes.h:1921
      Last_Column : aliased int;  -- a_nodes.h:1922
   end record;
   pragma Convention (C_Pass_By_Copy, Source_Location_Struct);  -- a_nodes.h:1917

   subtype Unit_ID is Node_ID;  -- a_nodes.h:1926

   type u_Element_Struct is record
      ID : aliased Element_ID;  -- a_nodes.h:1930
      Element_Kind : aliased Element_Kinds;  -- a_nodes.h:1931
      Enclosing_Compilation_Unit : aliased Unit_ID;  -- a_nodes.h:1932
      Is_Part_Of_Implicit : aliased Extensions.bool;  -- a_nodes.h:1933
      Is_Part_Of_Inherited : aliased Extensions.bool;  -- a_nodes.h:1934
      Is_Part_Of_Instance : aliased Extensions.bool;  -- a_nodes.h:1935
      Hash : aliased ASIS_Integer;  -- a_nodes.h:1936
      Enclosing_Element_ID : aliased Element_ID;  -- a_nodes.h:1937
      Enclosing_Kind : aliased Enclosing_Kinds;  -- a_nodes.h:1938
      Source_Location : aliased Source_Location_Struct;  -- a_nodes.h:1939
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1940
      The_Union : aliased Element_Union;  -- a_nodes.h:1941
   end record;
   pragma Convention (C_Pass_By_Copy, u_Element_Struct);  -- a_nodes.h:1929

   subtype Element_Struct is u_Element_Struct;

   type Unit_ID_Ptr is access all Unit_ID;  -- a_nodes.h:1952

   type Unit_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:1957
      IDs : Unit_ID_Ptr;  -- a_nodes.h:1958
   end record;
   pragma Convention (C_Pass_By_Copy, Unit_ID_Array_Struct);  -- a_nodes.h:1956

   subtype Unit_List is Unit_ID_Array_Struct;

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
   pragma Convention (C, Unit_Kinds);  -- a_nodes.h:1962

   type Unit_Classes is 
     (Not_A_Class,
      A_Public_Declaration,
      A_Public_Body,
      A_Public_Declaration_And_Body,
      A_Private_Declaration,
      A_Private_Body,
      A_Separate_Body);
   pragma Convention (C, Unit_Classes);  -- a_nodes.h:2029

   type Unit_Origins is 
     (Not_An_Origin,
      A_Predefined_Unit,
      An_Implementation_Unit,
      An_Application_Unit);
   pragma Convention (C, Unit_Origins);  -- a_nodes.h:2050

   type u_Unit_Struct is record
      ID : aliased Unit_ID;  -- a_nodes.h:2070
      Unit_Kind : aliased Unit_Kinds;  -- a_nodes.h:2071
      Unit_Class : aliased Unit_Classes;  -- a_nodes.h:2072
      Unit_Origin : aliased Unit_Origins;  -- a_nodes.h:2073
      Unit_Full_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2074
      Unique_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2075
      Exists : aliased Extensions.bool;  -- a_nodes.h:2076
      Can_Be_Main_Program : aliased Extensions.bool;  -- a_nodes.h:2077
      Is_Body_Required : aliased Extensions.bool;  -- a_nodes.h:2078
      Text_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2079
      Text_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2080
      Object_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2081
      Object_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2082
      Compilation_Command_Line_Options : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2083
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2084
      Unit_Declaration : aliased Declaration_ID;  -- a_nodes.h:2085
      Context_Clause_Elements : aliased Context_Clause_List;  -- a_nodes.h:2086
      Compilation_Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:2087
      Is_Standard : aliased Extensions.bool;  -- a_nodes.h:2088
      Corresponding_Children : aliased Unit_List;  -- a_nodes.h:2094
      Corresponding_Parent_Declaration : aliased Unit_ID;  -- a_nodes.h:2113
      Corresponding_Declaration : aliased Unit_ID;  -- a_nodes.h:2118
      Corresponding_Body : aliased Unit_ID;  -- a_nodes.h:2126
      Subunits : aliased Unit_List;  -- a_nodes.h:2135
      Corresponding_Subunit_Parent_Body : aliased Unit_ID;  -- a_nodes.h:2141
   end record;
   pragma Convention (C_Pass_By_Copy, u_Unit_Struct);  -- a_nodes.h:2069

   subtype Unit_Struct is u_Unit_Struct;

   type u_Context_Struct is record
      name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2152
      parameters : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2153
      debug_image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2154
   end record;
   pragma Convention (C_Pass_By_Copy, u_Context_Struct);  -- a_nodes.h:2151

   subtype Context_Struct is u_Context_Struct;

   type u_Unit_Struct_List_Struct is record
      Unit : aliased Unit_Struct;  -- a_nodes.h:2162
      Next : access u_Unit_Struct_List_Struct;  -- a_nodes.h:2163
      Next_Count : aliased int;  -- a_nodes.h:2164
   end record;
   pragma Convention (C_Pass_By_Copy, u_Unit_Struct_List_Struct);  -- a_nodes.h:2161

   subtype Unit_Struct_List_Struct is u_Unit_Struct_List_Struct;

   type Unit_Structs_Ptr is access all Unit_Struct_List_Struct;  -- a_nodes.h:2167

   type u_Element_Struct_List_Struct is record
      Element : aliased Element_Struct;  -- a_nodes.h:2170
      Next : access u_Element_Struct_List_Struct;  -- a_nodes.h:2171
      Next_Count : aliased int;  -- a_nodes.h:2172
   end record;
   pragma Convention (C_Pass_By_Copy, u_Element_Struct_List_Struct);  -- a_nodes.h:2169

   subtype Element_Struct_List_Struct is u_Element_Struct_List_Struct;

   type Element_Structs_Ptr is access all Element_Struct_List_Struct;  -- a_nodes.h:2175

   type u_Nodes_Struct is record
      Context : aliased Context_Struct;  -- a_nodes.h:2178
      Units : Unit_Structs_Ptr;  -- a_nodes.h:2179
      Elements : Element_Structs_Ptr;  -- a_nodes.h:2180
   end record;
   pragma Convention (C_Pass_By_Copy, u_Nodes_Struct);  -- a_nodes.h:2177

   subtype Nodes_Struct is u_Nodes_Struct;

   type Nodes_Ptr is access all Nodes_Struct;  -- a_nodes.h:2183

end a_nodes_h;
