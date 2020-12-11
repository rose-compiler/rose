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
      Type_Declaration_View : aliased Definition_ID;  -- a_nodes.h:542
      Object_Declaration_View : aliased Definition_ID;  -- a_nodes.h:554
      Initialization_Expression : aliased Expression_ID;  -- a_nodes.h:564
      Corresponding_Type_Declaration : aliased Declaration_ID;  -- a_nodes.h:572
      Corresponding_Type_Completion : aliased Declaration_ID;  -- a_nodes.h:578
      Corresponding_Type_Partial_View : aliased Declaration_ID;  -- a_nodes.h:585
      Corresponding_First_Subtype : aliased Declaration_ID;  -- a_nodes.h:593
      Corresponding_Last_Constraint : aliased Declaration_ID;  -- a_nodes.h:594
      Corresponding_Last_Subtype : aliased Declaration_ID;  -- a_nodes.h:595
      Specification_Subtype_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:598
      Iteration_Scheme_Name : aliased Element_ID;  -- a_nodes.h:601
      Subtype_Indication : aliased Element_ID;  -- a_nodes.h:603
      Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:620
      Result_Profile : aliased Element_ID;  -- a_nodes.h:628
      Result_Expression : aliased Expression_ID;  -- a_nodes.h:630
      Is_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:646
      Is_Not_Overriding_Declaration : aliased Extensions.bool;  -- a_nodes.h:647
      Body_Declarative_Items : aliased Element_ID_List;  -- a_nodes.h:653
      Body_Statements : aliased Statement_List;  -- a_nodes.h:654
      Body_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:655
      Body_Block_Statement : aliased Declaration_ID;  -- a_nodes.h:656
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:669
      Corresponding_Declaration : aliased Declaration_ID;  -- a_nodes.h:705
      Corresponding_Body : aliased Declaration_ID;  -- a_nodes.h:721
      Corresponding_Subprogram_Derivation : aliased Declaration_ID;  -- a_nodes.h:724
      Corresponding_Type : aliased Type_Definition_ID;  -- a_nodes.h:728
      Corresponding_Equality_Operator : aliased Declaration_ID;  -- a_nodes.h:730
      Visible_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:733
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:734
      Private_Part_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:735
      Declaration_Interface_List : aliased Expression_List;  -- a_nodes.h:740
      Renamed_Entity : aliased Expression_ID;  -- a_nodes.h:749
      Corresponding_Base_Entity : aliased Expression_ID;  -- a_nodes.h:750
      Protected_Operation_Items : aliased Declaration_List;  -- a_nodes.h:752
      Entry_Family_Definition : aliased Discrete_Subtype_Definition_ID;  -- a_nodes.h:754
      Entry_Index_Specification : aliased Declaration_ID;  -- a_nodes.h:756
      Entry_Barrier : aliased Expression_ID;  -- a_nodes.h:757
      Corresponding_Subunit : aliased Declaration_ID;  -- a_nodes.h:763
      Is_Subunit : aliased Extensions.bool;  -- a_nodes.h:769
      Corresponding_Body_Stub : aliased Declaration_ID;  -- a_nodes.h:776
      Generic_Formal_Part : aliased Element_ID_List;  -- a_nodes.h:780
      Generic_Unit_Name : aliased Expression_ID;  -- a_nodes.h:786
      Generic_Actual_Part : aliased Association_List;  -- a_nodes.h:787
      Formal_Subprogram_Default : aliased Expression_ID;  -- a_nodes.h:790
      Is_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:801
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:803
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
   with Convention => C;  -- a_nodes.h:814

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
   with Convention => C;  -- a_nodes.h:847

   subtype Type_Kinds is u_Type_Kinds;  -- a_nodes.h:869

   type u_Constraint_Kinds is 
     (Not_A_Constraint,
      A_Range_Attribute_Reference,
      A_Simple_Expression_Range,
      A_Digits_Constraint,
      A_Delta_Constraint,
      An_Index_Constraint,
      A_Discriminant_Constraint)
   with Convention => C;  -- a_nodes.h:871

   subtype Constraint_Kinds is u_Constraint_Kinds;  -- a_nodes.h:879

   type u_Interface_Kinds is 
     (Not_An_Interface,
      An_Ordinary_Interface,
      A_Limited_Interface,
      A_Task_Interface,
      A_Protected_Interface,
      A_Synchronized_Interface)
   with Convention => C;  -- a_nodes.h:881

   subtype Interface_Kinds is u_Interface_Kinds;  -- a_nodes.h:888

   type u_Root_Type_Kinds is 
     (Not_A_Root_Type_Definition,
      A_Root_Integer_Definition,
      A_Root_Real_Definition,
      A_Universal_Integer_Definition,
      A_Universal_Real_Definition,
      A_Universal_Fixed_Definition)
   with Convention => C;  -- a_nodes.h:890

   subtype Root_Type_Kinds is u_Root_Type_Kinds;  -- a_nodes.h:897

   type u_Discrete_Range_Kinds is 
     (Not_A_Discrete_Range,
      A_Discrete_Subtype_Indication,
      A_Discrete_Range_Attribute_Reference,
      A_Discrete_Simple_Expression_Range)
   with Convention => C;  -- a_nodes.h:899

   subtype Discrete_Range_Kinds is u_Discrete_Range_Kinds;  -- a_nodes.h:904

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
   with Convention => C;  -- a_nodes.h:906

   subtype Formal_Type_Kinds is u_Formal_Type_Kinds;  -- a_nodes.h:926

   type u_Access_Type_Kinds is 
     (Not_An_Access_Type_Definition,
      A_Pool_Specific_Access_To_Variable,
      An_Access_To_Variable,
      An_Access_To_Constant,
      An_Access_To_Procedure,
      An_Access_To_Protected_Procedure,
      An_Access_To_Function,
      An_Access_To_Protected_Function)
   with Convention => C;  -- a_nodes.h:928

   subtype Access_Type_Kinds is u_Access_Type_Kinds;  -- a_nodes.h:937

   type u_Access_Definition_Kinds is 
     (Not_An_Access_Definition,
      An_Anonymous_Access_To_Variable,
      An_Anonymous_Access_To_Constant,
      An_Anonymous_Access_To_Procedure,
      An_Anonymous_Access_To_Protected_Procedure,
      An_Anonymous_Access_To_Function,
      An_Anonymous_Access_To_Protected_Function)
   with Convention => C;  -- a_nodes.h:939

   subtype Access_Definition_Kinds is u_Access_Definition_Kinds;  -- a_nodes.h:947

   type u_Access_Type_Struct is record
      Access_Type_Kind : aliased Access_Type_Kinds;  -- a_nodes.h:950
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:951
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:955
      Access_To_Object_Definition : aliased Subtype_Indication;  -- a_nodes.h:959
      Access_To_Subprogram_Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:964
      Access_To_Function_Result_Profile : aliased Element_ID;  -- a_nodes.h:967
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:949

   subtype Access_Type_Struct is u_Access_Type_Struct;  -- a_nodes.h:968

   type u_Type_Definition_Struct is record
      Type_Kind : aliased Type_Kinds;  -- a_nodes.h:971
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:972
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:973
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:974
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:975
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:978
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:979
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:981
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:983
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:988
      Interface_Kind : aliased Interface_Kinds;  -- a_nodes.h:990
      Root_Type_Kind : aliased Root_Type_Kinds;  -- a_nodes.h:992
      Parent_Subtype_Indication : aliased Subtype_Indication;  -- a_nodes.h:995
      Record_Definition : aliased Definition;  -- a_nodes.h:999
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1002
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1003
      Corresponding_Parent_Subtype : aliased Declaration;  -- a_nodes.h:1004
      Corresponding_Root_Type : aliased Declaration;  -- a_nodes.h:1005
      Corresponding_Type_Structure : aliased Declaration;  -- a_nodes.h:1006
      Enumeration_Literal_Declarations : aliased Declaration_List;  -- a_nodes.h:1008
      Integer_Constraint : aliased Range_Constraint;  -- a_nodes.h:1010
      Mod_Static_Expression : aliased Expression;  -- a_nodes.h:1012
      Digits_Expression : aliased Expression;  -- a_nodes.h:1015
      Delta_Expression : aliased Expression;  -- a_nodes.h:1018
      Real_Range_Constraint : aliased Range_Constraint;  -- a_nodes.h:1022
      Index_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1024
      Discrete_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1026
      Array_Component_Definition : aliased Component_Definition;  -- a_nodes.h:1029
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1032
      Access_Type : aliased Access_Type_Struct;  -- a_nodes.h:1034
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:970

   subtype Type_Definition_Struct is u_Type_Definition_Struct;  -- a_nodes.h:1035

   type u_Subtype_Indication_Struct is record
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1038
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1039
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1040
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1037

   subtype Subtype_Indication_Struct is u_Subtype_Indication_Struct;  -- a_nodes.h:1041

   type u_Constraint_Struct is record
      Constraint_Kind : aliased Constraint_Kinds;  -- a_nodes.h:1044
      Digits_Expression : aliased Expression;  -- a_nodes.h:1047
      Delta_Expression : aliased Expression;  -- a_nodes.h:1049
      Real_Range_Constraint : aliased Range_Constraint;  -- a_nodes.h:1052
      Lower_Bound : aliased Expression;  -- a_nodes.h:1054
      Upper_Bound : aliased Expression;  -- a_nodes.h:1055
      Range_Attribute : aliased Expression;  -- a_nodes.h:1057
      Discrete_Ranges : aliased Discrete_Range_List;  -- a_nodes.h:1059
      Discriminant_Associations : aliased Discriminant_Association_List;  -- a_nodes.h:1061
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1043

   subtype Constraint_Struct is u_Constraint_Struct;  -- a_nodes.h:1062

   type u_Component_Definition_Struct is record
      Has_Aliased : aliased Extensions.bool;  -- a_nodes.h:1065
      Component_Subtype_Indication : aliased Definition;  -- a_nodes.h:1068
      Component_Definition_View : aliased Definition;  -- a_nodes.h:1070
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1064

   subtype Component_Definition_Struct is u_Component_Definition_Struct;  -- a_nodes.h:1071

   type u_Discrete_Subtype_Definition_Struct is record
      Discrete_Range_Kind : aliased Discrete_Range_Kinds;  -- a_nodes.h:1078
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1081
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1082
      Lower_Bound : aliased Expression;  -- a_nodes.h:1084
      Upper_Bound : aliased Expression;  -- a_nodes.h:1085
      Range_Attribute : aliased Expression;  -- a_nodes.h:1087
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1077

   subtype Discrete_Subtype_Definition_Struct is u_Discrete_Subtype_Definition_Struct;  -- a_nodes.h:1088

   type u_Discrete_Range_Struct is record
      Discrete_Range_Kind : aliased Discrete_Range_Kinds;  -- a_nodes.h:1091
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1094
      Subtype_Constraint : aliased Constraint;  -- a_nodes.h:1095
      Lower_Bound : aliased Expression;  -- a_nodes.h:1097
      Upper_Bound : aliased Expression;  -- a_nodes.h:1098
      Range_Attribute : aliased Expression;  -- a_nodes.h:1100
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1090

   subtype Discrete_Range_Struct is u_Discrete_Range_Struct;  -- a_nodes.h:1101

   type u_Known_Discriminant_Part_Struct is record
      Discriminants : aliased Discriminant_Specification_List;  -- a_nodes.h:1104
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1103

   subtype Known_Discriminant_Part_Struct is u_Known_Discriminant_Part_Struct;  -- a_nodes.h:1105

   type u_Record_Definition_Struct is record
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1108
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1109
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1107

   subtype Record_Definition_Struct is u_Record_Definition_Struct;  -- a_nodes.h:1110

   type u_Variant_Part_Struct is record
      Discriminant_Direct_Name : aliased Name;  -- a_nodes.h:1113
      Variants : aliased Variant_List;  -- a_nodes.h:1114
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1112

   subtype Variant_Part_Struct is u_Variant_Part_Struct;  -- a_nodes.h:1115

   type u_Variant_Struct is record
      Record_Components : aliased Record_Component_List;  -- a_nodes.h:1118
      Implicit_Components : aliased Record_Component_List;  -- a_nodes.h:1119
      Variant_Choices : aliased Element_ID_List;  -- a_nodes.h:1120
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1117

   subtype Variant_Struct is u_Variant_Struct;  -- a_nodes.h:1121

   type u_Access_Definition_Struct is record
      Access_Definition_Kind : aliased Access_Definition_Kinds;  -- a_nodes.h:1124
      Has_Null_Exclusion : aliased Extensions.bool;  -- a_nodes.h:1125
      Is_Not_Null_Return : aliased Extensions.bool;  -- a_nodes.h:1129
      Anonymous_Access_To_Object_Subtype_Mark : aliased Expression;  -- a_nodes.h:1132
      Access_To_Subprogram_Parameter_Profile : aliased Parameter_Specification_List;  -- a_nodes.h:1137
      Access_To_Function_Result_Profile : aliased Element_ID;  -- a_nodes.h:1140
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1123

   subtype Access_Definition_Struct is u_Access_Definition_Struct;  -- a_nodes.h:1141

   type u_Private_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1144
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1145
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1146
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1147
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1143

   subtype Private_Type_Definition_Struct is u_Private_Type_Definition_Struct;  -- a_nodes.h:1148

   type u_Tagged_Private_Type_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1151
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1152
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1153
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:1154
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1155
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1150

   subtype Tagged_Private_Type_Definition_Struct is u_Tagged_Private_Type_Definition_Struct;  -- a_nodes.h:1156

   type u_Private_Extension_Definition_Struct is record
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1159
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1160
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1161
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:1162
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1163
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1164
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1165
      Ancestor_Subtype_Indication : aliased Subtype_Indication;  -- a_nodes.h:1166
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1167
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1158

   subtype Private_Extension_Definition_Struct is u_Private_Extension_Definition_Struct;  -- a_nodes.h:1168

   type u_Task_Definition_Struct is record
      Has_Task : aliased Extensions.bool;  -- a_nodes.h:1171
      Visible_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1172
      Private_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1173
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1174
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1175
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1170

   subtype Task_Definition_Struct is u_Task_Definition_Struct;  -- a_nodes.h:1176

   type u_Protected_Definition_Struct is record
      Has_Protected : aliased Extensions.bool;  -- a_nodes.h:1179
      Visible_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1180
      Private_Part_Items : aliased Declarative_Item_List;  -- a_nodes.h:1181
      Is_Private_Present : aliased Extensions.bool;  -- a_nodes.h:1182
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1183
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1178

   subtype Protected_Definition_Struct is u_Protected_Definition_Struct;  -- a_nodes.h:1184

   type u_Formal_Type_Definition_Struct is record
      Formal_Type_Kind : aliased Formal_Type_Kinds;  -- a_nodes.h:1187
      Corresponding_Type_Operators : aliased Declaration_List;  -- a_nodes.h:1188
      Has_Abstract : aliased Extensions.bool;  -- a_nodes.h:1193
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1194
      Has_Private : aliased Extensions.bool;  -- a_nodes.h:1197
      Has_Synchronized : aliased Extensions.bool;  -- a_nodes.h:1199
      Has_Tagged : aliased Extensions.bool;  -- a_nodes.h:1201
      Interface_Kind : aliased Interface_Kinds;  -- a_nodes.h:1203
      Implicit_Inherited_Declarations : aliased Declaration_List;  -- a_nodes.h:1205
      Implicit_Inherited_Subprograms : aliased Declaration_List;  -- a_nodes.h:1206
      Index_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1208
      Discrete_Subtype_Definitions : aliased Expression_List;  -- a_nodes.h:1210
      Array_Component_Definition : aliased Component_Definition;  -- a_nodes.h:1213
      Subtype_Mark : aliased Expression;  -- a_nodes.h:1215
      Definition_Interface_List : aliased Expression_List;  -- a_nodes.h:1218
      Access_Type : aliased Access_Type_Struct;  -- a_nodes.h:1220
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1186

   subtype Formal_Type_Definition_Struct is u_Formal_Type_Definition_Struct;  -- a_nodes.h:1221

   type u_Aspect_Specification_Struct is record
      Aspect_Mark : aliased Element_ID;  -- a_nodes.h:1224
      Aspect_Definition : aliased Element_ID;  -- a_nodes.h:1225
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1223

   subtype Aspect_Specification_Struct is u_Aspect_Specification_Struct;  -- a_nodes.h:1226

   subtype No_Struct is int;  -- a_nodes.h:1228

   subtype Unknown_Discriminant_Part_Struct is No_Struct;  -- a_nodes.h:1229

   subtype Null_Record_Definition_Struct is No_Struct;  -- a_nodes.h:1230

   subtype Null_Component_Struct is No_Struct;  -- a_nodes.h:1231

   subtype Others_Choice_Struct is No_Struct;  -- a_nodes.h:1232

   type u_Definition_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1235
         when 1 =>
            The_Type_Definition : aliased Type_Definition_Struct;  -- a_nodes.h:1236
         when 2 =>
            The_Subtype_Indication : aliased Subtype_Indication_Struct;  -- a_nodes.h:1237
         when 3 =>
            The_Constraint : aliased Constraint_Struct;  -- a_nodes.h:1238
         when 4 =>
            The_Component_Definition : aliased Component_Definition_Struct;  -- a_nodes.h:1239
         when 5 =>
            The_Discrete_Subtype_Definition : aliased Discrete_Subtype_Definition_Struct;  -- a_nodes.h:1240
         when 6 =>
            The_Discrete_Range : aliased Discrete_Range_Struct;  -- a_nodes.h:1241
         when 7 =>
            The_Unknown_Discriminant_Part : aliased Unknown_Discriminant_Part_Struct;  -- a_nodes.h:1242
         when 8 =>
            The_Known_Discriminant_Part : aliased Known_Discriminant_Part_Struct;  -- a_nodes.h:1243
         when 9 =>
            The_Record_Definition : aliased Record_Definition_Struct;  -- a_nodes.h:1244
         when 10 =>
            The_Null_Record_Definition : aliased Null_Record_Definition_Struct;  -- a_nodes.h:1245
         when 11 =>
            The_Null_Component : aliased Null_Component_Struct;  -- a_nodes.h:1246
         when 12 =>
            The_Variant_Part : aliased Variant_Part_Struct;  -- a_nodes.h:1247
         when 13 =>
            The_Variant : aliased Variant_Struct;  -- a_nodes.h:1248
         when 14 =>
            The_Others_Choice : aliased Others_Choice_Struct;  -- a_nodes.h:1249
         when 15 =>
            The_Access_Definition : aliased Access_Definition_Struct;  -- a_nodes.h:1250
         when 16 =>
            The_Private_Type_Definition : aliased Private_Type_Definition_Struct;  -- a_nodes.h:1251
         when 17 =>
            The_Tagged_Private_Type_Definition : aliased Tagged_Private_Type_Definition_Struct;  -- a_nodes.h:1252
         when 18 =>
            The_Private_Extension_Definition : aliased Private_Extension_Definition_Struct;  -- a_nodes.h:1253
         when 19 =>
            The_Task_Definition : aliased Task_Definition_Struct;  -- a_nodes.h:1254
         when 20 =>
            The_Protected_Definition : aliased Protected_Definition_Struct;  -- a_nodes.h:1255
         when 21 =>
            The_Formal_Type_Definition : aliased Formal_Type_Definition_Struct;  -- a_nodes.h:1256
         when others =>
            The_Aspect_Specification : aliased Aspect_Specification_Struct;  -- a_nodes.h:1257
      end case;
   end record
   with Convention => C_Pass_By_Copy,
        Unchecked_Union => True;  -- a_nodes.h:1234

   subtype Definition_Union is u_Definition_Union;  -- a_nodes.h:1258

   type Definition_Struct is record
      Definition_Kind : aliased Definition_Kinds;  -- a_nodes.h:1262
      The_Union : aliased Definition_Union;  -- a_nodes.h:1263
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1261

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
   with Convention => C;  -- a_nodes.h:1274

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
   with Convention => C;  -- a_nodes.h:1319

   type Expression_Struct is record
      Expression_Kind : aliased Expression_Kinds;  -- a_nodes.h:1432
      Is_Prefix_Notation : aliased Extensions.bool;  -- a_nodes.h:1433
      Corresponding_Expression_Type : aliased Declaration_ID;  -- a_nodes.h:1434
      Corresponding_Expression_Type_Definition : aliased Element_ID;  -- a_nodes.h:1435
      Operator_Kind : aliased Operator_Kinds;  -- a_nodes.h:1439
      Attribute_Kind : aliased Attribute_Kinds;  -- a_nodes.h:1444
      Value_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1445
      Name_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1450
      Corresponding_Name_Definition : aliased Defining_Name_ID;  -- a_nodes.h:1451
      Corresponding_Name_Definition_List : aliased Defining_Name_List;  -- a_nodes.h:1452
      Corresponding_Name_Declaration : aliased Element_ID;  -- a_nodes.h:1453
      Prefix : aliased Expression_ID;  -- a_nodes.h:1460
      Index_Expressions : aliased Expression_List;  -- a_nodes.h:1462
      Slice_Range : aliased Discrete_Range_ID;  -- a_nodes.h:1464
      Selector : aliased Expression_ID;  -- a_nodes.h:1466
      Attribute_Designator_Identifier : aliased Expression_ID;  -- a_nodes.h:1468
      Attribute_Designator_Expressions : aliased Expression_List;  -- a_nodes.h:1477
      Record_Component_Associations : aliased Association_List;  -- a_nodes.h:1480
      Extension_Aggregate_Expression : aliased Expression_ID;  -- a_nodes.h:1482
      Array_Component_Associations : aliased Association_List;  -- a_nodes.h:1485
      Expression_Parenthesized : aliased Expression_ID;  -- a_nodes.h:1487
      Is_Prefix_Call : aliased Extensions.bool;  -- a_nodes.h:1489
      Corresponding_Called_Function : aliased Declaration_ID;  -- a_nodes.h:1492
      Function_Call_Parameters : aliased Element_ID_List;  -- a_nodes.h:1494
      Short_Circuit_Operation_Left_Expression : aliased Expression_ID;  -- a_nodes.h:1497
      Short_Circuit_Operation_Right_Expression : aliased Expression_ID;  -- a_nodes.h:1498
      Membership_Test_Expression : aliased Expression_ID;  -- a_nodes.h:1501
      Membership_Test_Choices : aliased Element_ID_List;  -- a_nodes.h:1502
      Converted_Or_Qualified_Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:1505
      Converted_Or_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1506
      Allocator_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:1508
      Allocator_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1510
      Expression_Paths : aliased Expression_Path_List;  -- a_nodes.h:1513
      Is_Generalized_Indexing : aliased Extensions.bool;  -- a_nodes.h:1515
      Is_Generalized_Reference : aliased Extensions.bool;  -- a_nodes.h:1517
      Iterator_Specification : aliased Declaration_ID;  -- a_nodes.h:1520
      Predicate : aliased Expression_ID;  -- a_nodes.h:1523
      Subpool_Name : aliased Expression_ID;  -- a_nodes.h:1526
      Corresponding_Generic_Element : aliased Defining_Name_ID;  -- a_nodes.h:1531
      Is_Dispatching_Call : aliased Extensions.bool;  -- a_nodes.h:1533
      Is_Call_On_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1534
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1431

   type Association_Kinds is 
     (Not_An_Association,
      A_Pragma_Argument_Association,
      A_Discriminant_Association,
      A_Record_Component_Association,
      An_Array_Component_Association,
      A_Parameter_Association,
      A_Generic_Association)
   with Convention => C;  -- a_nodes.h:1545

   type Association_Struct is record
      Association_Kind : aliased Association_Kinds;  -- a_nodes.h:1558
      Array_Component_Choices : aliased Expression_List;  -- a_nodes.h:1561
      Record_Component_Choices : aliased Expression_List;  -- a_nodes.h:1563
      Component_Expression : aliased Expression_ID;  -- a_nodes.h:1566
      Formal_Parameter : aliased Expression_ID;  -- a_nodes.h:1570
      Actual_Parameter : aliased Expression_ID;  -- a_nodes.h:1571
      Discriminant_Selector_Names : aliased Expression_List;  -- a_nodes.h:1573
      Discriminant_Expression : aliased Expression_ID;  -- a_nodes.h:1574
      Is_Normalized : aliased Extensions.bool;  -- a_nodes.h:1579
      Is_Defaulted_Association : aliased Extensions.bool;  -- a_nodes.h:1583
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1557

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
   with Convention => C;  -- a_nodes.h:1594

   type Statement_Struct is record
      Statement_Kind : aliased Statement_Kinds;  -- a_nodes.h:1631
      Corresponding_Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1632
      Label_Names : aliased Defining_Name_List;  -- a_nodes.h:1633
      Is_Prefix_Notation : aliased Extensions.bool;  -- a_nodes.h:1637
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1644
      Corresponding_End_Name : aliased Element_ID;  -- a_nodes.h:1650
      Assignment_Variable_Name : aliased Expression_ID;  -- a_nodes.h:1652
      Assignment_Expression : aliased Expression_ID;  -- a_nodes.h:1653
      Statement_Paths : aliased Path_List;  -- a_nodes.h:1660
      Case_Expression : aliased Expression_ID;  -- a_nodes.h:1662
      Statement_Identifier : aliased Defining_Name_ID;  -- a_nodes.h:1667
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:1671
      While_Condition : aliased Expression_ID;  -- a_nodes.h:1673
      For_Loop_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1675
      Loop_Statements : aliased Statement_List;  -- a_nodes.h:1679
      Is_Declare_Block : aliased Extensions.bool;  -- a_nodes.h:1681
      Block_Declarative_Items : aliased Declarative_Item_List;  -- a_nodes.h:1682
      Block_Statements : aliased Statement_List;  -- a_nodes.h:1683
      Block_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1684
      Exit_Loop_Name : aliased Expression_ID;  -- a_nodes.h:1686
      Exit_Condition : aliased Expression_ID;  -- a_nodes.h:1687
      Corresponding_Loop_Exited : aliased Expression_ID;  -- a_nodes.h:1688
      Return_Expression : aliased Expression_ID;  -- a_nodes.h:1690
      Return_Object_Declaration : aliased Declaration_ID;  -- a_nodes.h:1693
      Extended_Return_Statements : aliased Statement_List;  -- a_nodes.h:1694
      Extended_Return_Exception_Handlers : aliased Exception_Handler_List;  -- a_nodes.h:1695
      Goto_Label : aliased Expression_ID;  -- a_nodes.h:1697
      Corresponding_Destination_Statement : aliased Statement_ID;  -- a_nodes.h:1698
      Called_Name : aliased Expression_ID;  -- a_nodes.h:1701
      Corresponding_Called_Entity : aliased Declaration_ID;  -- a_nodes.h:1702
      Call_Statement_Parameters : aliased Association_List;  -- a_nodes.h:1703
      Accept_Entry_Index : aliased Expression_ID;  -- a_nodes.h:1706
      Accept_Entry_Direct_Name : aliased Name_ID;  -- a_nodes.h:1707
      Accept_Parameters : aliased Parameter_Specification_List;  -- a_nodes.h:1709
      Accept_Body_Statements : aliased Statement_List;  -- a_nodes.h:1710
      Accept_Body_Exception_Handlers : aliased Statement_List;  -- a_nodes.h:1711
      Corresponding_Entry : aliased Declaration_ID;  -- a_nodes.h:1712
      Requeue_Entry_Name : aliased Name_ID;  -- a_nodes.h:1715
      Delay_Expression : aliased Expression_ID;  -- a_nodes.h:1718
      Aborted_Tasks : aliased Expression_List;  -- a_nodes.h:1720
      Raised_Exception : aliased Expression_ID;  -- a_nodes.h:1722
      Associated_Message : aliased Expression_ID;  -- a_nodes.h:1723
      Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:1725
      Is_Dispatching_Call : aliased Extensions.bool;  -- a_nodes.h:1727
      Is_Call_On_Dispatching_Operation : aliased Extensions.bool;  -- a_nodes.h:1728
      Corresponding_Called_Entity_Unwound : aliased Declaration;  -- a_nodes.h:1731
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1630

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
   with Convention => C;  -- a_nodes.h:1742

   type Path_Struct is record
      Path_Kind : aliased Path_Kinds;  -- a_nodes.h:1798
      Sequence_Of_Statements : aliased Statement_List;  -- a_nodes.h:1799
      Dependent_Expression : aliased Expression;  -- a_nodes.h:1800
      Condition_Expression : aliased Expression_ID;  -- a_nodes.h:1805
      Case_Path_Alternative_Choices : aliased Element_ID_List;  -- a_nodes.h:1808
      Guard : aliased Expression_ID;  -- a_nodes.h:1811
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1797

   type Clause_Kinds is 
     (Not_A_Clause,
      A_Use_Package_Clause,
      A_Use_Type_Clause,
      A_Use_All_Type_Clause,
      A_With_Clause,
      A_Representation_Clause,
      A_Component_Clause)
   with Convention => C;  -- a_nodes.h:1822

   type u_Representation_Clause_Kinds is 
     (Not_A_Representation_Clause,
      An_Attribute_Definition_Clause,
      An_Enumeration_Representation_Clause,
      A_Record_Representation_Clause,
      An_At_Clause)
   with Convention => C;  -- a_nodes.h:1834

   subtype Representation_Clause_Kinds is u_Representation_Clause_Kinds;  -- a_nodes.h:1840

   type u_Representation_Clause_Struct is record
      Representation_Clause_Kind : aliased Representation_Clause_Kinds;  -- a_nodes.h:1843
      Representation_Clause_Name : aliased Name;  -- a_nodes.h:1844
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1847
      Representation_Clause_Expression : aliased Expression;  -- a_nodes.h:1851
      Mod_Clause_Expression : aliased Expression;  -- a_nodes.h:1853
      Component_Clauses : aliased Component_Clause_List;  -- a_nodes.h:1854
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1842

   subtype Representation_Clause_Struct is u_Representation_Clause_Struct;  -- a_nodes.h:1855

   type Clause_Struct is record
      Clause_Kind : aliased Clause_Kinds;  -- a_nodes.h:1859
      Has_Limited : aliased Extensions.bool;  -- a_nodes.h:1862
      Clause_Names : aliased Name_List;  -- a_nodes.h:1867
      Representation_Clause_Name : aliased Name_ID;  -- a_nodes.h:1869
      Component_Clause_Position : aliased Expression_ID;  -- a_nodes.h:1870
      Component_Clause_Range : aliased Element_ID;  -- a_nodes.h:1871
      Representation_Clause : aliased Representation_Clause_Struct;  -- a_nodes.h:1874
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1858

   type Exception_Handler_Struct is record
      Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:1890
      Choice_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:1891
      Exception_Choices : aliased Element_ID_List;  -- a_nodes.h:1892
      Handler_Statements : aliased Statement_List;  -- a_nodes.h:1893
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1889

   type Element_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            Dummy_Member : aliased int;  -- a_nodes.h:1906
         when 1 =>
            The_Pragma : aliased Pragma_Struct;  -- a_nodes.h:1907
         when 2 =>
            Defining_Name : aliased Defining_Name_Struct;  -- a_nodes.h:1908
         when 3 =>
            Declaration : aliased Declaration_Struct;  -- a_nodes.h:1909
         when 4 =>
            Definition : aliased Definition_Struct;  -- a_nodes.h:1910
         when 5 =>
            Expression : aliased Expression_Struct;  -- a_nodes.h:1911
         when 6 =>
            Association : aliased Association_Struct;  -- a_nodes.h:1912
         when 7 =>
            Statement : aliased Statement_Struct;  -- a_nodes.h:1913
         when 8 =>
            Path : aliased Path_Struct;  -- a_nodes.h:1914
         when 9 =>
            Clause : aliased Clause_Struct;  -- a_nodes.h:1915
         when others =>
            Exception_Handler : aliased Exception_Handler_Struct;  -- a_nodes.h:1916
      end case;
   end record
   with Convention => C_Pass_By_Copy,
        Unchecked_Union => True;  -- a_nodes.h:1905

   type Enclosing_Kinds is 
     (Not_Enclosing,
      Enclosing_Element,
      Enclosing_Unit)
   with Convention => C;  -- a_nodes.h:1919

   type Source_Location_Struct is record
      Unit_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1927
      First_Line : aliased int;  -- a_nodes.h:1928
      First_Column : aliased int;  -- a_nodes.h:1929
      Last_Line : aliased int;  -- a_nodes.h:1930
      Last_Column : aliased int;  -- a_nodes.h:1931
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1926

   subtype Unit_ID is int;  -- a_nodes.h:1935

   type u_Element_Struct is record
      ID : aliased Element_ID;  -- a_nodes.h:1939
      Element_Kind : aliased Element_Kinds;  -- a_nodes.h:1940
      Enclosing_Compilation_Unit : aliased Unit_ID;  -- a_nodes.h:1941
      Is_Part_Of_Implicit : aliased Extensions.bool;  -- a_nodes.h:1942
      Is_Part_Of_Inherited : aliased Extensions.bool;  -- a_nodes.h:1943
      Is_Part_Of_Instance : aliased Extensions.bool;  -- a_nodes.h:1944
      Hash : aliased ASIS_Integer;  -- a_nodes.h:1945
      Enclosing_Element_ID : aliased Element_ID;  -- a_nodes.h:1946
      Enclosing_Kind : aliased Enclosing_Kinds;  -- a_nodes.h:1947
      Source_Location : aliased Source_Location_Struct;  -- a_nodes.h:1948
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:1949
      The_Union : aliased Element_Union;  -- a_nodes.h:1950
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1938

   subtype Element_Struct is u_Element_Struct;  -- a_nodes.h:1951

   type Unit_ID_Ptr is access all Unit_ID;  -- a_nodes.h:1961

   type Unit_ID_Array_Struct is record
      Length : aliased int;  -- a_nodes.h:1966
      IDs : Unit_ID_Ptr;  -- a_nodes.h:1967
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:1965

   subtype Unit_List is Unit_ID_Array_Struct;  -- a_nodes.h:1969

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
   with Convention => C;  -- a_nodes.h:1971

   type Unit_Classes is 
     (Not_A_Class,
      A_Public_Declaration,
      A_Public_Body,
      A_Public_Declaration_And_Body,
      A_Private_Declaration,
      A_Private_Body,
      A_Separate_Body)
   with Convention => C;  -- a_nodes.h:2038

   type Unit_Origins is 
     (Not_An_Origin,
      A_Predefined_Unit,
      An_Implementation_Unit,
      An_Application_Unit)
   with Convention => C;  -- a_nodes.h:2059

   type u_Unit_Struct is record
      ID : aliased Unit_ID;  -- a_nodes.h:2079
      Unit_Kind : aliased Unit_Kinds;  -- a_nodes.h:2080
      Unit_Class : aliased Unit_Classes;  -- a_nodes.h:2081
      Unit_Origin : aliased Unit_Origins;  -- a_nodes.h:2082
      Unit_Full_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2083
      Unique_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2084
      Exists : aliased Extensions.bool;  -- a_nodes.h:2085
      Can_Be_Main_Program : aliased Extensions.bool;  -- a_nodes.h:2086
      Is_Body_Required : aliased Extensions.bool;  -- a_nodes.h:2087
      Text_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2088
      Text_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2089
      Object_Name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2090
      Object_Form : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2091
      Compilation_Command_Line_Options : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2092
      Debug_Image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2093
      Unit_Declaration : aliased Declaration_ID;  -- a_nodes.h:2094
      Context_Clause_Elements : aliased Context_Clause_List;  -- a_nodes.h:2095
      Compilation_Pragmas : aliased Pragma_Element_ID_List;  -- a_nodes.h:2096
      Is_Standard : aliased Extensions.bool;  -- a_nodes.h:2097
      Corresponding_Children : aliased Unit_List;  -- a_nodes.h:2103
      Corresponding_Parent_Declaration : aliased Unit_ID;  -- a_nodes.h:2122
      Corresponding_Declaration : aliased Unit_ID;  -- a_nodes.h:2127
      Corresponding_Body : aliased Unit_ID;  -- a_nodes.h:2135
      Subunits : aliased Unit_List;  -- a_nodes.h:2144
      Corresponding_Subunit_Parent_Body : aliased Unit_ID;  -- a_nodes.h:2150
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2078

   subtype Unit_Struct is u_Unit_Struct;  -- a_nodes.h:2151

   type u_Context_Struct is record
      name : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2161
      parameters : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2162
      debug_image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:2163
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2160

   subtype Context_Struct is u_Context_Struct;  -- a_nodes.h:2164

   type u_Unit_Struct_List_Struct;
   type u_Unit_Struct_List_Struct is record
      Unit : aliased Unit_Struct;  -- a_nodes.h:2171
      Next : access u_Unit_Struct_List_Struct;  -- a_nodes.h:2172
      Next_Count : aliased int;  -- a_nodes.h:2173
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2170

   subtype Unit_Struct_List_Struct is u_Unit_Struct_List_Struct;  -- a_nodes.h:2174

   type Unit_Structs_Ptr is access all Unit_Struct_List_Struct;  -- a_nodes.h:2176

   type u_Element_Struct_List_Struct;
   type u_Element_Struct_List_Struct is record
      Element : aliased Element_Struct;  -- a_nodes.h:2179
      Next : access u_Element_Struct_List_Struct;  -- a_nodes.h:2180
      Next_Count : aliased int;  -- a_nodes.h:2181
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2178

   subtype Element_Struct_List_Struct is u_Element_Struct_List_Struct;  -- a_nodes.h:2182

   type Element_Structs_Ptr is access all Element_Struct_List_Struct;  -- a_nodes.h:2184

   type u_Nodes_Struct is record
      Context : aliased Context_Struct;  -- a_nodes.h:2187
      Units : Unit_Structs_Ptr;  -- a_nodes.h:2188
      Elements : Element_Structs_Ptr;  -- a_nodes.h:2189
   end record
   with Convention => C_Pass_By_Copy;  -- a_nodes.h:2186

   subtype Nodes_Struct is u_Nodes_Struct;  -- a_nodes.h:2190

end a_nodes_h;
