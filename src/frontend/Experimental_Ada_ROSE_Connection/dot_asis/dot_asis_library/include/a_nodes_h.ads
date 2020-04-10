pragma Ada_2012;
pragma Style_Checks (Off);

with Interfaces.C; use Interfaces.C;
with Interfaces.C.Strings;
with Interfaces.C.Extensions;

package a_nodes_h is

   type Program_Text is new Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:11

   subtype ASIS_Integer is int;  -- a_nodes.h:12

   subtype Element_ID is int;  -- a_nodes.h:19

   type Element_ID_Ptr is access all Element_ID;  -- a_nodes.h:36

   type u_Element_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:41
      IDs : Element_ID_Ptr;  -- a_nodes.h:42
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:40

   subtype Element_ID_Array_Struct is u_Element_ID_Array_Struct;  -- a_nodes.h:43

   subtype Element_ID_List is Element_ID_Array_Struct;  -- a_nodes.h:45

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
      An_Exception_Handler)
   with Convention => C;  -- a_nodes.h:47

   subtype Access_Type_Definition is Element_ID;  -- a_nodes.h:69

   subtype Association is Element_ID;  -- a_nodes.h:70

   subtype Association_List is Element_ID_List;  -- a_nodes.h:71

   subtype Case_Statement_Alternative is Element_ID;  -- a_nodes.h:72

   subtype Clause is Element_ID;  -- a_nodes.h:73

   subtype Component_Clause is Element_ID;  -- a_nodes.h:74

   subtype Component_Clause_List is Element_ID_List;  -- a_nodes.h:75

   subtype Component_Declaration is Element_ID;  -- a_nodes.h:76

   subtype Component_Definition is Element_ID;  -- a_nodes.h:77

   subtype Constraint_ID is Element_ID;  -- a_nodes.h:78

   subtype Constraint is Element_ID;  -- a_nodes.h:79

   subtype Context_Clause is Element_ID;  -- a_nodes.h:80

   subtype Context_Clause_List is Element_ID_List;  -- a_nodes.h:81

   subtype Declaration is Element_ID;  -- a_nodes.h:82

   subtype Declaration_ID is Element_ID;  -- a_nodes.h:83

   subtype Declaration_List is Element_ID_List;  -- a_nodes.h:84

   subtype Declarative_Item_List is Element_ID_List;  -- a_nodes.h:85

   subtype Defining_Name_ID is Element_ID;  -- a_nodes.h:86

   subtype Definition is Element_ID;  -- a_nodes.h:87

   subtype Definition_ID is Element_ID;  -- a_nodes.h:88

   subtype Definition_List is Element_ID_List;  -- a_nodes.h:89

   subtype Discrete_Range is Element_ID;  -- a_nodes.h:90

   subtype Discrete_Range_ID is Element_ID;  -- a_nodes.h:91

   subtype Discrete_Range_List is Element_ID_List;  -- a_nodes.h:92

   subtype Discrete_Subtype_Definition is Element_ID;  -- a_nodes.h:93

   subtype Discrete_Subtype_Definition_ID is Element_ID;  -- a_nodes.h:94

   subtype Discriminant_Association is Element_ID;  -- a_nodes.h:95

   subtype Discriminant_Association_List is Element_ID_List;  -- a_nodes.h:96

   subtype Discriminant_Specification_List is Element_ID_List;  -- a_nodes.h:97

   subtype Defining_Name is Element_ID;  -- a_nodes.h:98

   subtype Defining_Name_List is Element_ID_List;  -- a_nodes.h:99

   subtype Exception_Handler is Element_ID;  -- a_nodes.h:100

   subtype Exception_Handler_List is Element_ID_List;  -- a_nodes.h:101

   subtype Expression is Element_ID;  -- a_nodes.h:102

   subtype Expression_ID is Element_ID;  -- a_nodes.h:103

   subtype Expression_List is Element_ID_List;  -- a_nodes.h:104

   subtype Expression_Path_List is Element_ID_List;  -- a_nodes.h:105

   subtype Formal_Type_Definition is Element_ID;  -- a_nodes.h:106

   subtype Generic_Formal_Parameter is Element_ID;  -- a_nodes.h:107

   subtype Generic_Formal_Parameter_List is Element_ID_List;  -- a_nodes.h:108

   subtype Identifier is Element_ID;  -- a_nodes.h:109

   subtype Identifier_List is Element_ID_List;  -- a_nodes.h:110

   subtype Name is Element_ID;  -- a_nodes.h:111

   subtype Name_ID is Element_ID;  -- a_nodes.h:112

   subtype Name_List is Element_ID_List;  -- a_nodes.h:113

   subtype Parameter_Specification is Element_ID;  -- a_nodes.h:114

   subtype Parameter_Specification_List is Element_ID_List;  -- a_nodes.h:115

   subtype Path is Element_ID;  -- a_nodes.h:116

   subtype Path_List is Element_ID_List;  -- a_nodes.h:117

   subtype Pragma_Element is Element_ID;  -- a_nodes.h:118

   subtype Pragma_Element_ID_List is Element_ID_List;  -- a_nodes.h:119

   subtype Range_Constraint is Element_ID;  -- a_nodes.h:120

   subtype Record_Component is Element_ID;  -- a_nodes.h:121

   subtype Record_Component_List is Element_ID_List;  -- a_nodes.h:122

   subtype Record_Definition is Element_ID;  -- a_nodes.h:123

   subtype Representation_Clause is Element_ID;  -- a_nodes.h:124

   subtype Representation_Clause_List is Element_ID_List;  -- a_nodes.h:125

   subtype Root_Type_Definition is Element_ID;  -- a_nodes.h:126

   subtype Select_Alternative is Element_ID;  -- a_nodes.h:127

   subtype Statement is Element_ID;  -- a_nodes.h:128

   subtype Statement_ID is Element_ID;  -- a_nodes.h:129

   subtype Statement_List is Element_ID_List;  -- a_nodes.h:130

   subtype Subtype_Indication is Element_ID;  -- a_nodes.h:131

   subtype Subtype_Indication_ID is Element_ID;  -- a_nodes.h:132

   subtype Subtype_Mark is Element_ID;  -- a_nodes.h:133

   subtype Type_Definition is Element_ID;  -- a_nodes.h:134

   subtype Type_Definition_ID is Element_ID;  -- a_nodes.h:135

   subtype Variant is Element_ID;  -- a_nodes.h:136

   subtype Variant_Component_List is Element_ID_List;  -- a_nodes.h:137

   subtype Variant_List is Element_ID_List;  -- a_nodes.h:138

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
      A_Not_Operator)
   with Convention => C;  -- a_nodes.h:142

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
      An_Implementation_Defined_Pragma)
   with Convention => C;  -- a_nodes.h:172

   type Pragma_Struct is record
      Pragma_Kind : aliased Pragma_Kinds;  -- a_nodes.h:233
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:234
      Pragma_Name_Image : Program_Text;  -- a_nodes.h:235
      Pragma_Argument_Associations : aliased Association_List;  -- a_nodes.h:236
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:232

   type Defining_Name_Kinds is 
     (Not_A_Defining_Name,
      A_Defining_Identifier,
      A_Defining_Character_Literal,
      A_Defining_Enumeration_Literal,
      A_Defining_Operator_Symbol,
      A_Defining_Expanded_Name)
   with Convention => C;  -- a_nodes.h:247

   type Defining_Name_Struct is record
      Defining_Name_Kind : aliased Defining_Name_Kinds;  -- a_nodes.h:258
      Defining_Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:259
      References : aliased Name_List;  -- a_nodes.h:260
      Is_Referenced : aliased Extensions.bool;  -- a_nodes.h:261
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:265
      Position_Number_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:268
      Representation_Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:269
      Defining_Prefix : aliased Name_ID;  -- a_nodes.h:271
      Defining_Selector : aliased Defining_Name_ID;  -- a_nodes.h:272
      Corresponding_Constant_Declaration : aliased Declaration_ID;  -- a_nodes.h:274
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:277
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:257

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
      A_Formal_Package_Declaration_With_Box)
   with Convention => C;  -- a_nodes.h:288

   type Declaration_Origins is 
     (Not_A_Declaration_Origin,
      An_Explicit_Declaration,
      An_Implicit_Predefined_Declaration,
      An_Implicit_Inherited_Declaration)
   with Convention => C;  -- a_nodes.h:392

   type Mode_Kinds is 
     (Not_A_Mode,
      A_Default_In_Mode,
      An_In_Mode,
      An_Out_Mode,
      An_In_Out_Mode)
   with Convention => C;  -- a_nodes.h:408

   type Subprogram_Default_Kinds is 
     (Not_A_Default,
      A_Name_Default,
      A_Box_Default,
      A_Null_Default,
      A_Nil_Default)
   with Convention => C;  -- a_nodes.h:418

   type Declaration_Struct is record
      Declaration_Kind : aliased Declaration_Kinds;  -- a_nodes.h:431
      Declaration_Origin : aliased Declaration_Origins;  -- a_nodes.h:432
      Corresponding_Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:433
      Names : aliased Defining_Name_List;  -- a_nodes.h:434
      Aspect_Specifications : aliased Element_ID_List;  -- a_nodes.h:435
      Corresponding_Representation_Clauses : aliased Representation_Clause_List;  -- a_nodes.h:436
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:446
      Has_Aliased : aliased Extensions.bool;  -- a_nodes.h:453
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:457
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:460
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:465
      Has_Reverse : aliased Extensions.bool;  -- a_nodes.h:469
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:474
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:478
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:486
      Mode_Kind : aliased Mode_Kinds;  -- a_nodes.h:489
      Default_Kind : aliased Subprogram_Default_Kinds;  -- a_nodes.h:492
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:512
      Corresponding_End_Name : aliased Element_ID;  -- a_nodes.h:525
      Discriminant_Part : aliased Definition_ID;  -- a_nodes.h:534
      Type_Declaration_View : aliased Definition_ID;  -- a_nodes.h:541
      Object_Declaration_View : aliased Definition_ID;  -- a_nodes.h:553
      Initialization_Expression : aliased Expression_ID;  -- a_nodes.h:563
      Corresponding_Type_Declaration : aliased Declaration_ID;  -- a_nodes.h:571
      Corresponding_Type_Completion : aliased Declaration_ID;  -- a_nodes.h:576
      Corresponding_Type_Partial_View : aliased Declaration_ID;  -- a_nodes.h:582
      Corresponding_First_Subtype : aliased Declaration_ID;  -- a_nodes.h:590
      Corresponding_Last_Constraint : aliased Declaration_ID;  -- a_nodes.h:591
      Corresponding_Last_Subtype : aliased Declaration_ID;  -- a_nodes.h:592
      Specification_Subtype_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:595
      Iteration_Scheme_Name : aliased Element_ID;  -- a_nodes.h:598
      Subtype_Indication : aliased Element_ID;  -- a_nodes.h:600
      Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:617
      Result_Profile : aliased Element_ID;  -- a_nodes.h:625
      Result_Expression : aliased Expression_ID;  -- a_nodes.h:627
      Is_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:643
      Is_Not_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:644
      Body_Declarative_Items : aliased Element_ID_List;  -- a_nodes.h:650
      Body_Statements : aliased Statement_List;  -- a_nodes.h:651
      Body_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:652
      Body_Block_Statement : aliased Declaration_ID;  -- a_nodes.h:653
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:666
      Corresponding_Declaration : aliased Declaration_ID;  -- a_nodes.h:702
      Corresponding_Body : aliased Declaration_ID;  -- a_nodes.h:718
      Corresponding_Subprogram_Derivation : aliased Declaration_ID;  -- a_nodes.h:721
      Corresponding_Type : aliased Type_Definition_ID;  -- a_nodes.h:725
      Corresponding_Equality_Operator : aliased Declaration_ID;  -- a_nodes.h:727
      Visible_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:730
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:731
      Private_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:732
      Declaration_Interface_List : aliased Expression_List;  -- a_nodes.h:737
      Renamed_Entity : aliased Expression_ID;  -- a_nodes.h:746
      Corresponding_Base_Entity : aliased Expression_ID;  -- a_nodes.h:747
      Protected_Operation_Items : aliased Declaration_List;  -- a_nodes.h:749
      Entry_Family_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:751
      Entry_Index_Specification : aliased Declaration_ID;  -- a_nodes.h:753
      Entry_Barrier : aliased Expression_ID;  -- a_nodes.h:754
      Corresponding_Subunit : aliased Declaration_ID;  -- a_nodes.h:760
      Is_Subunit : aliased Extensions.bool;  -- a_nodes.h:766
      Corresponding_Body_Stub : aliased Declaration_ID;  -- a_nodes.h:767
      Generic_Formal_Part : aliased Element_ID_List;  -- a_nodes.h:771
      Generic_Unit_Name : aliased Expression_ID;  -- a_nodes.h:777
      Generic_Actual_Part : aliased Association_List;  -- a_nodes.h:778
      Formal_Subprogram_Default : aliased Expression_ID;  -- a_nodes.h:781
      Is_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:792
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:430

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
      An_Aspect_Specification)
   with Convention => C;  -- a_nodes.h:803

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
      An_Access_Type_Definition)
   with Convention => C;  -- a_nodes.h:836

   subtype Type_Kinds is u_Type_Kinds;  -- a_nodes.h:858

   type u_Constraint_Kinds is 
     (Not_A_Constraint,
      A_Range_Attribute_Reference,
      A_Simple_Expression_Range,
      A_Digits_Constraint,
      A_Delta_Constraint,
      An_Index_Constraint,
      A_Discriminant_Constraint)
   with Convention => C;  -- a_nodes.h:860

   subtype Constraint_Kinds is u_Constraint_Kinds;  -- a_nodes.h:868

   type u_Interface_Kinds is 
     (Not_An_Interface,
      An_Ordinary_Interface,
      A_Limited_Interface,
      A_Task_Interface,
      A_Protected_Interface,
      A_Synchronized_Interface)
   with Convention => C;  -- a_nodes.h:870

   subtype Interface_Kinds is u_Interface_Kinds;  -- a_nodes.h:877

   type u_Root_Type_Kinds is 
     (Not_A_Root_Type_Definition,
      A_Root_Integer_Definition,
      A_Root_Real_Definition,
      A_Universal_Integer_Definition,
      A_Universal_Real_Definition,
      A_Universal_Fixed_Definition)
   with Convention => C;  -- a_nodes.h:879

   subtype Root_Type_Kinds is u_Root_Type_Kinds;  -- a_nodes.h:886

   type u_Discrete_Range_Kinds is 
     (Not_A_Discrete_Range,
      A_Discrete_Subtype_Indication,
      A_Discrete_Range_Attribute_Reference,
      A_Discrete_Simple_Expression_Range)
   with Convention => C;  -- a_nodes.h:888

   subtype Discrete_Range_Kinds is u_Discrete_Range_Kinds;  -- a_nodes.h:893

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
      A_Formal_Access_Type_Definition)
   with Convention => C;  -- a_nodes.h:895

   subtype Formal_Type_Kinds is u_Formal_Type_Kinds;  -- a_nodes.h:915

   type u_Access_Type_Kinds is 
     (Not_An_Access_Type_Definition,
      A_Pool_Specific_Access_To_Variable,
      An_Access_To_Variable,
      An_Access_To_Constant,
      An_Access_To_Procedure,
      An_Access_To_Protected_Procedure,
      An_Access_To_Function,
      An_Access_To_Protected_Function)
   with Convention => C;  -- a_nodes.h:917

   subtype Access_Type_Kinds is u_Access_Type_Kinds;  -- a_nodes.h:926

   type u_Access_Definition_Kinds is 
     (Not_An_Access_Definition,
      An_Anonymous_Access_To_Variable,
      An_Anonymous_Access_To_Constant,
      An_Anonymous_Access_To_Procedure,
      An_Anonymous_Access_To_Protected_Procedure,
      An_Anonymous_Access_To_Function,
      An_Anonymous_Access_To_Protected_Function)
   with Convention => C;  -- a_nodes.h:928

   subtype Access_Definition_Kinds is u_Access_Definition_Kinds;  -- a_nodes.h:936

   type u_Access_Type_Struct is record
      Access_Type_Kind : aliased Access_Type_Kinds;  -- a_nodes.h:939
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:940
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:944
      Access_To_Object_Definition : aliased Subtype_Indication;  -- a_nodes.h:948
      Access_To_Subprogram_Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:953
      Access_To_Function_Result_Profile : aliased Element_ID;  -- a_nodes.h:956
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:938

   subtype Access_Type_Struct is u_Access_Type_Struct;  -- a_nodes.h:957

   type u_Type_Definition_Struct is record
      Type_Kind : aliased Type_Kinds;  -- a_nodes.h:960
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:961
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:962
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:963
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:964
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:967
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:968
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:970
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:972
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:977
      Interface_Kind : aliased Interface_Kinds;  -- a_nodes.h:979
      Root_Type_Kind : aliased Root_Type_Kinds;  -- a_nodes.h:981
      Parent_Subtype_Indication : aliased Subtype_Indication;  -- a_nodes.h:984
      Record_Definition : aliased Definition;  -- a_nodes.h:988
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:991
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:992
      Corresponding_Parent_Subtype : aliased Declaration;  -- a_nodes.h:993
      Corresponding_Root_Type : aliased Declaration;  -- a_nodes.h:994
      Corresponding_Type_Structure : aliased Declaration;  -- a_nodes.h:995
      Enumeration_Literal_Declarations : aliased Declaration_List;  -- a_nodes.h:997
      Integer_Constraint : aliased Range_Constraint;  -- a_nodes.h:999
      Mod_Static_Expression : aliased Expression;  -- a_nodes.h:1001
      Digits_Expression : aliased Expression;  -- a_nodes.h:1004
      Delta_Expression : aliased Expression;  -- a_nodes.h:1007
      Real_Range_Constraint : aliased Range_Constraint;  -- a_nodes.h:1011
      Index_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1013
      Discrete_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1015
      Array_Component_Definition : aliased Component_Definition;  -- a_nodes.h:1018
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1021
      Access_Type : aliased Access_Type_Struct;  -- a_nodes.h:1023
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:959

   subtype Type_Definition_Struct is u_Type_Definition_Struct;  -- a_nodes.h:1024

   type u_Subtype_Indication_Struct is record
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1027
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1028
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1029
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1026

   subtype Subtype_Indication_Struct is u_Subtype_Indication_Struct;  -- a_nodes.h:1030

   type u_Constraint_Struct is record
      Constraint_Kind : aliased Constraint_Kinds;  -- a_nodes.h:1033
      Digits_Expression : aliased Expression;  -- a_nodes.h:1036
      Delta_Expression : aliased Expression;  -- a_nodes.h:1038
      Real_Range_Constraint : aliased Range_Constraint;  -- a_nodes.h:1041
      Lower_Bound : aliased Expression;  -- a_nodes.h:1043
      Upper_Bound : aliased Expression;  -- a_nodes.h:1044
      Range_Attribute : aliased Expression;  -- a_nodes.h:1046
      Discrete_Ranges : aliased Discrete_Range_List;  -- a_nodes.h:1048
      Discriminant_Associations : aliased Discriminant_Association_List;  -- a_nodes.h:1050
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1032

   subtype Constraint_Struct is u_Constraint_Struct;  -- a_nodes.h:1051

   type u_Component_Definition_Struct is record
      Component_Definition_View : aliased Definition;  -- a_nodes.h:1054
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1053

   subtype Component_Definition_Struct is u_Component_Definition_Struct;  -- a_nodes.h:1055

   type u_Discrete_Subtype_Definition_Struct is record
      Discrete_Range_Kind : aliased Discrete_Range_Kinds;  -- a_nodes.h:1058
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1061
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1062
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1057

   subtype Discrete_Subtype_Definition_Struct is u_Discrete_Subtype_Definition_Struct;  -- a_nodes.h:1063

   type u_Discrete_Range_Struct is record
      Discrete_Range_Kind : aliased Discrete_Range_Kinds;  -- a_nodes.h:1066
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1069
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1070
      Lower_Bound : aliased Expression;  -- a_nodes.h:1072
      Upper_Bound : aliased Expression;  -- a_nodes.h:1073
      Range_Attribute : aliased Expression;  -- a_nodes.h:1075
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1065

   subtype Discrete_Range_Struct is u_Discrete_Range_Struct;  -- a_nodes.h:1076

   type u_Known_Discriminant_Part_Struct is record
      Discriminants : aliased Discriminant_Specification_List;  -- a_nodes.h:1079
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1078

   subtype Known_Discriminant_Part_Struct is u_Known_Discriminant_Part_Struct;  -- a_nodes.h:1080

   type u_Record_Definition_Struct is record
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1083
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1084
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1082

   subtype Record_Definition_Struct is u_Record_Definition_Struct;  -- a_nodes.h:1085

   type u_Variant_Part_Struct is record
      Discriminant_Direct_Name : aliased Name;  -- a_nodes.h:1088
      Variants : aliased Variant_List;  -- a_nodes.h:1089
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1087

   subtype Variant_Part_Struct is u_Variant_Part_Struct;  -- a_nodes.h:1090

   type u_Variant_Struct is record
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1093
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1094
      Variant_Choices : aliased Element_ID_List;  -- a_nodes.h:1095
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1092

   subtype Variant_Struct is u_Variant_Struct;  -- a_nodes.h:1096

   type u_Access_Definition_Struct is record
      Access_Definition_Kind : aliased Access_Definition_Kinds;  -- a_nodes.h:1099
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1100
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:1104
      Anonymous_Access_To_Object_Subtype_Mark : aliased Expression;  -- a_nodes.h:1107
      Access_To_Subprogram_Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:1112
      Access_To_Function_Result_Profile : aliased Element_ID;  -- a_nodes.h:1115
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1098

   subtype Access_Definition_Struct is u_Access_Definition_Struct;  -- a_nodes.h:1116

   type u_Private_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1119
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1120
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1121
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1118

   subtype Private_Type_Definition_Struct is u_Private_Type_Definition_Struct;  -- a_nodes.h:1122

   type u_Tagged_Private_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1125
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1126
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1127
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:1128
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1124

   subtype Tagged_Private_Type_Definition_Struct is u_Tagged_Private_Type_Definition_Struct;  -- a_nodes.h:1129

   type u_Private_Extension_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1132
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1133
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1134
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:1135
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1136
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1137
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1138
      Ancestor_Subtype_Indication : aliased Subtype_Indication;  -- a_nodes.h:1139
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1131

   subtype Private_Extension_Definition_Struct is u_Private_Extension_Definition_Struct;  -- a_nodes.h:1140

   type u_Task_Definition_Struct is record
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:1143
      Visible_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1144
      Private_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1145
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1146
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1142

   subtype Task_Definition_Struct is u_Task_Definition_Struct;  -- a_nodes.h:1147

   type u_Protected_Definition_Struct is record
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:1150
      Visible_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1151
      Private_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1152
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1153
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1149

   subtype Protected_Definition_Struct is u_Protected_Definition_Struct;  -- a_nodes.h:1154

   type u_Formal_Type_Definition_Struct is record
      Formal_Type_Kind : aliased Formal_Type_Kinds;  -- a_nodes.h:1157
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1158
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1163
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1164
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1167
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:1169
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:1171
      Interface_Kind : aliased Interface_Kinds;  -- a_nodes.h:1173
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1175
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1176
      Index_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1178
      Discrete_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1180
      Array_Component_Definition : aliased Component_Definition;  -- a_nodes.h:1183
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1185
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1188
      Access_Type : aliased Access_Type_Struct;  -- a_nodes.h:1190
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1156

   subtype Formal_Type_Definition_Struct is u_Formal_Type_Definition_Struct;  -- a_nodes.h:1191

   type u_Aspect_Specification_Struct is record
      Aspect_Mark : aliased Element_ID;  -- a_nodes.h:1194
      Aspect_Definition : aliased Element_ID;  -- a_nodes.h:1195
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1193

   subtype Aspect_Specification_Struct is u_Aspect_Specification_Struct;  -- a_nodes.h:1196

   subtype No_Struct is int;  -- a_nodes.h:1198

   subtype Unknown_Discriminant_Part_Struct is No_Struct;  -- a_nodes.h:1199

   subtype Null_Record_Definition_Struct is No_Struct;  -- a_nodes.h:1200

   subtype Null_Component_Struct is No_Struct;  -- a_nodes.h:1201

   subtype Others_Choice_Struct is No_Struct;  -- a_nodes.h:1202

   type u_Definition_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1205
         when 1 =>
            The_Type_Definition : aliased Type_Definition_Struct;  -- a_nodes.h:1206
         when 2 =>
            The_Subtype_Indication : aliased Subtype_Indication_Struct;  -- a_nodes.h:1207
         when 3 =>
            The_Constraint : aliased Constraint_Struct;  -- a_nodes.h:1208
         when 4 =>
            The_Component_Definition : aliased Component_Definition_Struct;  -- a_nodes.h:1209
         when 5 =>
            The_Discrete_Subtype_Definition : aliased Discrete_Subtype_Definition_Struct;  -- a_nodes.h:1210
         when 6 =>
            The_Discrete_Range : aliased Discrete_Range_Struct;  -- a_nodes.h:1211
         when 7 =>
            The_Unknown_Discriminant_Part : aliased Unknown_Discriminant_Part_Struct;  -- a_nodes.h:1212
         when 8 =>
            The_Known_Discriminant_Part : aliased Known_Discriminant_Part_Struct;  -- a_nodes.h:1213
         when 9 =>
            The_Record_Definition : aliased Record_Definition_Struct;  -- a_nodes.h:1214
         when 10 =>
            The_Null_Record_Definition : aliased Null_Record_Definition_Struct;  -- a_nodes.h:1215
         when 11 =>
            The_Null_Component : aliased Null_Component_Struct;  -- a_nodes.h:1216
         when 12 =>
            The_Variant_Part : aliased Variant_Part_Struct;  -- a_nodes.h:1217
         when 13 =>
            The_Variant : aliased Variant_Struct;  -- a_nodes.h:1218
         when 14 =>
            The_Others_Choice : aliased Others_Choice_Struct;  -- a_nodes.h:1219
         when 15 =>
            The_Access_Definition : aliased Access_Definition_Struct;  -- a_nodes.h:1220
         when 16 =>
            The_Private_Type_Definition : aliased Private_Type_Definition_Struct;  -- a_nodes.h:1221
         when 17 =>
            The_Tagged_Private_Type_Definition : aliased Tagged_Private_Type_Definition_Struct;  -- a_nodes.h:1222
         when 18 =>
            The_Private_Extension_Definition : aliased Private_Extension_Definition_Struct;  -- a_nodes.h:1223
         when 19 =>
            The_Task_Definition : aliased Task_Definition_Struct;  -- a_nodes.h:1224
         when 20 =>
            The_Protected_Definition : aliased Protected_Definition_Struct;  -- a_nodes.h:1225
         when 21 =>
            The_Formal_Type_Definition : aliased Formal_Type_Definition_Struct;  -- a_nodes.h:1226
         when others =>
            The_Aspect_Specification : aliased Aspect_Specification_Struct;  -- a_nodes.h:1227
      end case;
   end record
   with Convention => C_Pass_By_Copy,
        Unchecked_Union => True;  -- a_nodes.h:1204

   subtype Definition_Union is u_Definition_Union;  -- a_nodes.h:1228

   type Definition_Struct is record
      Definition_Kind : aliased Definition_Kinds;  -- a_nodes.h:1232
      The_Union : aliased Definition_Union;  -- a_nodes.h:1233
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1231

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
      A_For_Some_Quantified_Expression)
   with Convention => C;  -- a_nodes.h:1244

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
      An_Unknown_Attribute)
   with Convention => C;  -- a_nodes.h:1289

   type Expression_Struct is record
      Expression_Kind : aliased Expression_Kinds;  -- a_nodes.h:1402
      Is_Prefix_Notation : aliased Extensions.bool;  -- a_nodes.h:1403
      Corresponding_Expression_Type : aliased Declaration_ID;  -- a_nodes.h:1404
      Corresponding_Expression_Type_Definition : aliased Element_ID;  -- a_nodes.h:1405
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:1409
      Attribute_Kind : aliased Attribute_Kinds;  -- a_nodes.h:1414
      Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1415
      Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1420
      Corresponding_Name_Definition : aliased Defining_Name_ID;  -- a_nodes.h:1421
      Corresponding_Name_Definition_List : aliased Defining_Name_List;  -- a_nodes.h:1422
      Corresponding_Name_Declaration : aliased Element_ID;  -- a_nodes.h:1423
      Prefix : aliased Expression_ID;  -- a_nodes.h:1430
      Index_Expressions : aliased Expression_List;  -- a_nodes.h:1432
      Slice_Range : aliased Discrete_Range_ID;  -- a_nodes.h:1434
      Selector : aliased Expression_ID;  -- a_nodes.h:1436
      Attribute_Designator_Identifier : aliased Expression_ID;  -- a_nodes.h:1438
      Attribute_Designator_Expressions : aliased Expression_List;  -- a_nodes.h:1447
      Record_Component_Associations : aliased Association_List;  -- a_nodes.h:1450
      Extension_Aggregate_Expression : aliased Expression_ID;  -- a_nodes.h:1452
      Array_Component_Associations : aliased Association_List;  -- a_nodes.h:1455
      Expression_Parenthesized : aliased Expression_ID;  -- a_nodes.h:1457
      Is_Prefix_Call : aliased Extensions.bool;  -- a_nodes.h:1459
      Corresponding_Called_Function : aliased Declaration_ID;  -- a_nodes.h:1462
      Function_Call_Parameters : aliased Element_ID_List;  -- a_nodes.h:1464
      Short_Circuit_Operation_Left_Expression : aliased Expression_ID;  -- a_nodes.h:1467
      Short_Circuit_Operation_Right_Expression : aliased Expression_ID;  -- a_nodes.h:1468
      Membership_Test_Expression : aliased Expression_ID;  -- a_nodes.h:1471
      Membership_Test_Choices : aliased Element_ID_List;  -- a_nodes.h:1472
      Converted_Or_Qualified_Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:1475
      Converted_Or_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1476
      Allocator_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1478
      Allocator_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1480
      Expression_Paths : aliased Expression_Path_List;  -- a_nodes.h:1483
      Is_Generalized_Indexing : aliased Extensions.bool;  -- a_nodes.h:1485
      Is_Generalized_Reference : aliased Extensions.bool;  -- a_nodes.h:1487
      Iterator_Specification : aliased Declaration_ID;  -- a_nodes.h:1490
      Predicate : aliased Expression_ID;  -- a_nodes.h:1493
      Subpool_Name : aliased Expression_ID;  -- a_nodes.h:1496
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:1501
      Is_Dispatching_Call : aliased Extensions.bool;  -- a_nodes.h:1503
      Is_Call_On_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1504
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1401

   type Association_Kinds is 
     (Not_An_Association,
      A_Pragma_Argument_Association,
      A_Discriminant_Association,
      A_Record_Component_Association,
      An_Array_Component_Association,
      A_Parameter_Association,
      A_Generic_Association)
   with Convention => C;  -- a_nodes.h:1515

   type Association_Struct is record
      Association_Kind : aliased Association_Kinds;  -- a_nodes.h:1528
      Array_Component_Choices : aliased Expression_List;  -- a_nodes.h:1531
      Record_Component_Choices : aliased Expression_List;  -- a_nodes.h:1533
      Component_Expression : aliased Expression_ID;  -- a_nodes.h:1536
      Formal_Parameter : aliased Expression_ID;  -- a_nodes.h:1540
      Actual_Parameter : aliased Expression_ID;  -- a_nodes.h:1541
      Discriminant_Selector_Names : aliased Expression_List;  -- a_nodes.h:1543
      Discriminant_Expression : aliased Expression_ID;  -- a_nodes.h:1544
      Is_Normalized : aliased Extensions.bool;  -- a_nodes.h:1549
      Is_Defaulted_Association : aliased Extensions.bool;  -- a_nodes.h:1553
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1527

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
      A_Code_Statement)
   with Convention => C;  -- a_nodes.h:1564

   type Statement_Struct is record
      Statement_Kind : aliased Statement_Kinds;  -- a_nodes.h:1601
      Corresponding_Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1602
      Label_Names : aliased Defining_Name_List;  -- a_nodes.h:1603
      Is_Prefix_Notation : aliased Extensions.bool;  -- a_nodes.h:1607
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1614
      Corresponding_End_Name : aliased Element_ID;  -- a_nodes.h:1620
      Assignment_Variable_Name : aliased Expression_ID;  -- a_nodes.h:1622
      Assignment_Expression : aliased Expression_ID;  -- a_nodes.h:1623
      Statement_Paths : aliased Path_List;  -- a_nodes.h:1630
      Case_Expression : aliased Expression_ID;  -- a_nodes.h:1632
      Statement_Identifier : aliased Defining_Name_ID;  -- a_nodes.h:1637
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:1641
      While_Condition : aliased Expression_ID;  -- a_nodes.h:1643
      For_Loop_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1645
      Loop_Statements : aliased Statement_List;  -- a_nodes.h:1649
      Is_Declare_Block : aliased Extensions.bool;  -- a_nodes.h:1651
      Block_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1652
      Block_Statements : aliased Statement_List;  -- a_nodes.h:1653
      Block_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1654
      Exit_Loop_Name : aliased Expression_ID;  -- a_nodes.h:1656
      Exit_Condition : aliased Expression_ID;  -- a_nodes.h:1657
      Corresponding_Loop_Exited : aliased Expression_ID;  -- a_nodes.h:1658
      Return_Expression : aliased Expression_ID;  -- a_nodes.h:1660
      Return_Object_Declaration : aliased Declaration_ID;  -- a_nodes.h:1663
      Extended_Return_Statements : aliased Statement_List;  -- a_nodes.h:1664
      Extended_Return_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1665
      Goto_Label : aliased Expression_ID;  -- a_nodes.h:1667
      Corresponding_Destination_Statement : aliased Statement_ID;  -- a_nodes.h:1668
      Called_Name : aliased Expression_ID;  -- a_nodes.h:1671
      Corresponding_Called_Entity : aliased Declaration_ID;  -- a_nodes.h:1672
      Call_Statement_Parameters : aliased Association_List;  -- a_nodes.h:1673
      Accept_Entry_Index : aliased Expression_ID;  -- a_nodes.h:1676
      Accept_Entry_Direct_Name : aliased Name_ID;  -- a_nodes.h:1677
      Accept_Parameters : aliased Parameter_Specification_List;  -- a_nodes.h:1679
      Accept_Body_Statements : aliased Statement_List;  -- a_nodes.h:1680
      Accept_Body_Exception_Handlers : aliased Statement_List;  -- a_nodes.h:1681
      Corresponding_Entry : aliased Declaration_ID;  -- a_nodes.h:1682
      Requeue_Entry_Name : aliased Name_ID;  -- a_nodes.h:1685
      Delay_Expression : aliased Expression_ID;  -- a_nodes.h:1688
      Aborted_Tasks : aliased Expression_List;  -- a_nodes.h:1690
      Raised_Exception : aliased Expression_ID;  -- a_nodes.h:1692
      Associated_Message : aliased Expression_ID;  -- a_nodes.h:1693
      Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1695
      Is_Dispatching_Call : aliased Extensions.bool;  -- a_nodes.h:1697
      Is_Call_On_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1698
      Corresponding_Called_Entity_Unwound : aliased Declaration;  -- a_nodes.h:1701
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1600

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
      An_Else_Expression_Path)
   with Convention => C;  -- a_nodes.h:1712

   type Path_Struct is record
      Path_Kind : aliased Path_Kinds;  -- a_nodes.h:1768
      Sequence_Of_Statements : aliased Statement_List;  -- a_nodes.h:1769
      Dependent_Expression : aliased Expression;  -- a_nodes.h:1770
      Condition_Expression : aliased Expression_ID;  -- a_nodes.h:1775
      Case_Path_Alternative_Choices : aliased Element_ID_List;  -- a_nodes.h:1778
      Guard : aliased Expression_ID;  -- a_nodes.h:1781
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1767

   type Clause_Kinds is 
     (Not_A_Clause,
      A_Use_Package_Clause,
      A_Use_Type_Clause,
      A_Use_All_Type_Clause,
      A_With_Clause,
      A_Representation_Clause,
      A_Component_Clause)
   with Convention => C;  -- a_nodes.h:1792

   type u_Representation_Clause_Kinds is 
     (Not_A_Representation_Clause,
      An_Attribute_Definition_Clause,
      An_Enumeration_Representation_Clause,
      A_Record_Representation_Clause,
      An_At_Clause)
   with Convention => C;  -- a_nodes.h:1804

   subtype Representation_Clause_Kinds is u_Representation_Clause_Kinds;  -- a_nodes.h:1810

   type u_Representation_Clause_Struct is record
      Representation_Clause_Kind : aliased Representation_Clause_Kinds;  -- a_nodes.h:1813
      Representation_Clause_Name : aliased Name;  -- a_nodes.h:1814
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1818
      Representation_Clause_Expression : aliased Expression;  -- a_nodes.h:1822
      Mod_Clause_Expression : aliased Expression;  -- a_nodes.h:1824
      Component_Clauses : aliased Component_Clause_List;  -- a_nodes.h:1825
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1812

   subtype Representation_Clause_Struct is u_Representation_Clause_Struct;  -- a_nodes.h:1826

   type Clause_Struct is record
      Clause_Kind : aliased Clause_Kinds;  -- a_nodes.h:1830
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1833
      Clause_Names : aliased Name_List;  -- a_nodes.h:1838
      Representation_Clause_Name : aliased Name_ID;  -- a_nodes.h:1840
      Representation_Clause_Expression : aliased Expression_ID;  -- a_nodes.h:1841
      Mod_Clause_Expression : aliased Expression_ID;  -- a_nodes.h:1842
      Component_Clauses : aliased Element_ID_List;  -- a_nodes.h:1843
      Component_Clause_Position : aliased Expression_ID;  -- a_nodes.h:1844
      Component_Clause_Range : aliased Element_ID;  -- a_nodes.h:1845
      Representation_Clause : aliased Representation_Clause_Struct;  -- a_nodes.h:1848
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1829

   type Exception_Handler_Struct is record
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1864
      Choice_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1865
      Exception_Choices : aliased Element_ID_List;  -- a_nodes.h:1866
      Handler_Statements : aliased Statement_List;  -- a_nodes.h:1867
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1863

   type Element_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1880
         when 1 =>
            The_Pragma : aliased Pragma_Struct;  -- a_nodes.h:1881
         when 2 =>
            Defining_Name : aliased Defining_Name_Struct;  -- a_nodes.h:1882
         when 3 =>
            Declaration : aliased Declaration_Struct;  -- a_nodes.h:1883
         when 4 =>
            Definition : aliased Definition_Struct;  -- a_nodes.h:1884
         when 5 =>
            Expression : aliased Expression_Struct;  -- a_nodes.h:1885
         when 6 =>
            Association : aliased Association_Struct;  -- a_nodes.h:1886
         when 7 =>
            Statement : aliased Statement_Struct;  -- a_nodes.h:1887
         when 8 =>
            Path : aliased Path_Struct;  -- a_nodes.h:1888
         when 9 =>
            Clause : aliased Clause_Struct;  -- a_nodes.h:1889
         when others =>
            Exception_Handler : aliased Exception_Handler_Struct;  -- a_nodes.h:1890
      end case;
   end record
   with Convention => C_Pass_By_Copy,
        Unchecked_Union => True;  -- a_nodes.h:1879

   type Enclosing_Kinds is 
     (Not_Enclosing,
      Enclosing_Element,
      Enclosing_Unit)
   with Convention => C;  -- a_nodes.h:1893

   type Source_Location_Struct is record
      Unit_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1901
      First_Line : aliased int;  -- a_nodes.h:1902
      First_Column : aliased int;  -- a_nodes.h:1903
      Last_Line : aliased int;  -- a_nodes.h:1904
      Last_Column : aliased int;  -- a_nodes.h:1905
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1900

   subtype Unit_ID is int;  -- a_nodes.h:1909

   type u_Element_Struct is record
      ID : aliased Element_ID;  -- a_nodes.h:1913
      Element_Kind : aliased Element_Kinds;  -- a_nodes.h:1914
      Enclosing_Compilation_Unit : aliased Unit_ID;  -- a_nodes.h:1915
      Is_Part_Of_Implicit : aliased Extensions.bool;  -- a_nodes.h:1916
      Is_Part_Of_Inherited : aliased Extensions.bool;  -- a_nodes.h:1917
      Is_Part_Of_Instance : aliased Extensions.bool;  -- a_nodes.h:1918
      Hash : aliased ASIS_Integer;  -- a_nodes.h:1919
      Enclosing_Element_ID : aliased Element_ID;  -- a_nodes.h:1920
      Enclosing_Kind : aliased Enclosing_Kinds;  -- a_nodes.h:1921
      Source_Location : aliased Source_Location_Struct;  -- a_nodes.h:1922
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1923
      The_Union : aliased Element_Union;  -- a_nodes.h:1924
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1912

   subtype Element_Struct is u_Element_Struct;  -- a_nodes.h:1925

   type Unit_ID_Ptr is access all Unit_ID;  -- a_nodes.h:1935

   type Unit_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:1940
      IDs : Unit_ID_Ptr;  -- a_nodes.h:1941
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1939

   subtype Unit_List is Unit_ID_Array_Struct;  -- a_nodes.h:1943

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
      An_Unknown_Unit)
   with Convention => C;  -- a_nodes.h:1945

   type Unit_Classes is 
     (Not_A_Class,
      A_Public_Declaration,
      A_Public_Body,
      A_Public_Declaration_And_Body,
      A_Private_Declaration,
      A_Private_Body,
      A_Separate_Body)
   with Convention => C;  -- a_nodes.h:2012

   type Unit_Origins is 
     (Not_An_Origin,
      A_Predefined_Unit,
      An_Implementation_Unit,
      An_Application_Unit)
   with Convention => C;  -- a_nodes.h:2033

   type u_Unit_Struct is record
      ID : aliased Unit_ID;  -- a_nodes.h:2053
      Unit_Kind : aliased Unit_Kinds;  -- a_nodes.h:2054
      Unit_Class : aliased Unit_Classes;  -- a_nodes.h:2055
      Unit_Origin : aliased Unit_Origins;  -- a_nodes.h:2056
      Unit_Full_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2057
      Unique_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2058
      Exists : aliased Extensions.bool;  -- a_nodes.h:2059
      Can_Be_Main_Program : aliased Extensions.bool;  -- a_nodes.h:2060
      Is_Body_Required : aliased Extensions.bool;  -- a_nodes.h:2061
      Text_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2062
      Text_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2063
      Object_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2064
      Object_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2065
      Compilation_Command_Line_Options : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2066
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2067
      Unit_Declaration : aliased Declaration_ID;  -- a_nodes.h:2068
      Context_Clause_Elements : aliased Context_Clause_List;  -- a_nodes.h:2069
      Compilation_Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:2070
      Is_Standard : aliased Extensions.bool;  -- a_nodes.h:2071
      Corresponding_Children : aliased Unit_List;  -- a_nodes.h:2077
      Corresponding_Parent_Declaration : aliased Unit_ID;  -- a_nodes.h:2096
      Corresponding_Declaration : aliased Unit_ID;  -- a_nodes.h:2101
      Corresponding_Body : aliased Unit_ID;  -- a_nodes.h:2109
      Subunits : aliased Unit_List;  -- a_nodes.h:2118
      Corresponding_Subunit_Parent_Body : aliased Unit_ID;  -- a_nodes.h:2124
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2052

   subtype Unit_Struct is u_Unit_Struct;  -- a_nodes.h:2125

   type u_Context_Struct is record
      name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2135
      parameters : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2136
      debug_image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2137
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2134

   subtype Context_Struct is u_Context_Struct;  -- a_nodes.h:2138

   type u_Unit_Struct_List_Struct;
   type u_Unit_Struct_List_Struct is record
      Unit : aliased Unit_Struct;  -- a_nodes.h:2145
      Next : access u_Unit_Struct_List_Struct;  -- a_nodes.h:2146
      Next_Count : aliased int;  -- a_nodes.h:2147
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2144

   subtype Unit_Struct_List_Struct is u_Unit_Struct_List_Struct;  -- a_nodes.h:2148

   type Unit_Structs_Ptr is access all Unit_Struct_List_Struct;  -- a_nodes.h:2150

   type u_Element_Struct_List_Struct;
   type u_Element_Struct_List_Struct is record
      Element : aliased Element_Struct;  -- a_nodes.h:2153
      Next : access u_Element_Struct_List_Struct;  -- a_nodes.h:2154
      Next_Count : aliased int;  -- a_nodes.h:2155
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2152

   subtype Element_Struct_List_Struct is u_Element_Struct_List_Struct;  -- a_nodes.h:2156

   type Element_Structs_Ptr is access all Element_Struct_List_Struct;  -- a_nodes.h:2158

   type u_Nodes_Struct is record
      Context : aliased Context_Struct;  -- a_nodes.h:2161
      Units : Unit_Structs_Ptr;  -- a_nodes.h:2162
      Elements : Element_Structs_Ptr;  -- a_nodes.h:2163
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2160

   subtype Nodes_Struct is u_Nodes_Struct;  -- a_nodes.h:2164

end a_nodes_h;
