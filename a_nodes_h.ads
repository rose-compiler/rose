pragma Ada_2005;
pragma Style_Checks (Off);

with Interfaces.C; use Interfaces.C;
with Interfaces.C.Strings;
with Interfaces.C.Extensions;

package a_nodes_h is

   subtype Node_ID is int;  -- a_nodes.h:6

   subtype Unit_ID is Node_ID;  -- a_nodes.h:7

   subtype Element_ID is Node_ID;  -- a_nodes.h:8

   subtype Declaration_ID is Element_ID;  -- a_nodes.h:9

   subtype Defining_Name_ID is Element_ID;  -- a_nodes.h:10

   subtype Discrete_Range_ID is Element_ID;  -- a_nodes.h:11

   subtype Expression_ID is Element_ID;  -- a_nodes.h:12

   subtype Name_ID is Element_ID;  -- a_nodes.h:13

   subtype Statement_ID is Element_ID;  -- a_nodes.h:14

   subtype Subtype_Indication_ID is Element_ID;  -- a_nodes.h:15

   type Element_ID_List_Struct_elements_array is array (size_t) of aliased Element_ID;
   type Element_ID_List_Struct is record
      length : aliased int;  -- a_nodes.h:18
      elements : aliased Element_ID_List_Struct_elements_array;  -- a_nodes.h:19
   end record;
   pragma Convention (C_Pass_By_Copy, Element_ID_List_Struct);  -- a_nodes.h:17

   type Element_List is access all Element_ID_List_Struct;  -- a_nodes.h:21

   subtype Association_List is Element_List;  -- a_nodes.h:22

   subtype Declarative_Item_List is Element_List;  -- a_nodes.h:23

   subtype Defining_Name_List is Element_List;  -- a_nodes.h:24

   subtype Exception_Handler_List is Element_List;  -- a_nodes.h:25

   subtype Expression_List is Element_List;  -- a_nodes.h:26

   subtype Expression_Path_List is Element_List;  -- a_nodes.h:27

   subtype Path_List is Element_List;  -- a_nodes.h:28

   subtype Parameter_Specification_List is Element_List;  -- a_nodes.h:29

   subtype Statement_List is Element_List;  -- a_nodes.h:30

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
   pragma Convention (C, Element_Kinds);  -- a_nodes.h:32

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
   pragma Convention (C, Pragma_Kinds);  -- a_nodes.h:50

   type Defining_Name_Kinds is 
     (Not_A_Defining_Name,
      A_Defining_Identifier,
      A_Defining_Character_Literal,
      A_Defining_Enumeration_Literal,
      A_Defining_Operator_Symbol,
      A_Defining_Expanded_Name);
   pragma Convention (C, Defining_Name_Kinds);  -- a_nodes.h:110

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
   pragma Convention (C, Declaration_Kinds);  -- a_nodes.h:119

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
   pragma Convention (C, Definition_Kinds);  -- a_nodes.h:223

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
   pragma Convention (C, Expression_Kinds);  -- a_nodes.h:266

   type Association_Kinds is 
     (Not_An_Association,
      A_Pragma_Argument_Association,
      A_Discriminant_Association,
      A_Record_Component_Association,
      An_Array_Component_Association,
      A_Parameter_Association,
      A_Generic_Association);
   pragma Convention (C, Association_Kinds);  -- a_nodes.h:314

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
   pragma Convention (C, Statement_Kinds);  -- a_nodes.h:325

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
   pragma Convention (C, Path_Kinds);  -- a_nodes.h:366

   type Clause_Kinds is 
     (Not_A_Clause,
      A_Use_Package_Clause,
      A_Use_Type_Clause,
      A_Use_All_Type_Clause,
      A_With_Clause,
      A_Representation_Clause,
      A_Component_Clause);
   pragma Convention (C, Clause_Kinds);  -- a_nodes.h:427

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
   pragma Convention (C, Operator_Kinds);  -- a_nodes.h:453

   type Declaration_Origins is 
     (Not_A_Declaration_Origin,
      An_Explicit_Declaration,
      An_Implicit_Predefined_Declaration,
      An_Implicit_Inherited_Declaration);
   pragma Convention (C, Declaration_Origins);  -- a_nodes.h:480

   type Mode_Kinds is 
     (Not_A_Mode,
      A_Default_In_Mode,
      An_In_Mode,
      An_Out_Mode,
      An_In_Out_Mode);
   pragma Convention (C, Mode_Kinds);  -- a_nodes.h:496

   type Subprogram_Default_Kinds is 
     (Not_A_Default,
      A_Name_Default,
      A_Box_Default,
      A_Null_Default,
      A_Nil_Default);
   pragma Convention (C, Subprogram_Default_Kinds);  -- a_nodes.h:506

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
   pragma Convention (C, Trait_Kinds);  -- a_nodes.h:518

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
   pragma Convention (C, Attribute_Kinds);  -- a_nodes.h:563

   type Enclosing_Kinds is 
     (Element_Kind,
      Unit_Kind);
   pragma Convention (C, Enclosing_Kinds);  -- a_nodes.h:675

   type Pragma_Struct is record
      kind : aliased Pragma_Kinds;  -- a_nodes.h:685
   end record;
   pragma Convention (C_Pass_By_Copy, Pragma_Struct);  -- a_nodes.h:684

   type Defining_Operator_Symbol_Struct is record
      kind : aliased Operator_Kinds;  -- a_nodes.h:690
   end record;
   pragma Convention (C_Pass_By_Copy, Defining_Operator_Symbol_Struct);  -- a_nodes.h:689

   type Defining_Name_Union (discr : unsigned := 0) is record
      case discr is
         when others =>
            defining_operator_symbol : aliased Defining_Operator_Symbol_Struct;  -- a_nodes.h:695
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Defining_Name_Union);
   pragma Unchecked_Union (Defining_Name_Union);  -- a_nodes.h:694

   type Defining_Name_Struct is record
      kind : aliased Defining_Name_Kinds;  -- a_nodes.h:699
      name_image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:700
      defining_name : aliased Defining_Name_Union;  -- a_nodes.h:704
   end record;
   pragma Convention (C_Pass_By_Copy, Defining_Name_Struct);  -- a_nodes.h:698

   type Declaration_Struct is record
      kind : aliased Declaration_Kinds;  -- a_nodes.h:708
      origin : aliased Declaration_Origins;  -- a_nodes.h:709
      mode : aliased Mode_Kinds;  -- a_nodes.h:714
      subprogram_default : aliased Subprogram_Default_Kinds;  -- a_nodes.h:717
      trait : aliased Trait_Kinds;  -- a_nodes.h:729
   end record;
   pragma Convention (C_Pass_By_Copy, Declaration_Struct);  -- a_nodes.h:707

   type Definition_Struct is record
      kind : aliased Definition_Kinds;  -- a_nodes.h:733
   end record;
   pragma Convention (C_Pass_By_Copy, Definition_Struct);  -- a_nodes.h:732

   type Expression_Struct is record
      kind : aliased Expression_Kinds;  -- a_nodes.h:738
      name_image : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:745
      Corresponding_Name_Definition : aliased Defining_Name_ID;  -- a_nodes.h:746
      Corresponding_Name_Definition_List : Defining_Name_List;  -- a_nodes.h:747
      Corresponding_Name_Declaration : aliased Element_ID;  -- a_nodes.h:748
      operator_kind : aliased Operator_Kinds;  -- a_nodes.h:750
      Prefix : aliased Expression_ID;  -- a_nodes.h:757
      Corresponding_Called_Function : aliased Declaration_ID;  -- a_nodes.h:760
      Is_Prefix_Call : aliased Extensions.bool;  -- a_nodes.h:762
      Function_Call_Parameters : Element_List;  -- a_nodes.h:763
      Index_Expressions : Expression_List;  -- a_nodes.h:765
      Is_Generalized_Indexing : aliased Extensions.bool;  -- a_nodes.h:766
      Slice_Range : aliased Discrete_Range_ID;  -- a_nodes.h:768
      Selector : aliased Expression_ID;  -- a_nodes.h:770
      atribute_kind : aliased Attribute_Kinds;  -- a_nodes.h:772
      Attribute_Designator_Identifier : aliased Expression_ID;  -- a_nodes.h:773
      Attribute_Designator_Expressions : Expression_List;  -- a_nodes.h:774
      Record_Component_Associations : Association_List;  -- a_nodes.h:777
      Extension_Aggregate_Expression : aliased Expression_ID;  -- a_nodes.h:779
      Array_Component_Associations : Association_List;  -- a_nodes.h:782
      Short_Circuit_Operation_Left_Expression : aliased Expression_ID;  -- a_nodes.h:785
      Short_Circuit_Operation_Right_Expression : aliased Expression_ID;  -- a_nodes.h:786
      Membership_Test_Expression : aliased Expression_ID;  -- a_nodes.h:789
      Membership_Test_Choices : Element_List;  -- a_nodes.h:790
      Expression_Parenthesized : aliased Expression_ID;  -- a_nodes.h:792
      Converted_Or_Qualified_Subtype_Mark : aliased Expression_ID;  -- a_nodes.h:795
      Converted_Or_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:796
      Predicate : aliased Expression_ID;  -- a_nodes.h:797
      Subpool_Name : aliased Expression_ID;  -- a_nodes.h:800
      Allocator_Subtype_Indication : aliased Subtype_Indication_ID;  -- a_nodes.h:802
      Allocator_Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:804
      Expression_Paths : Expression_Path_List;  -- a_nodes.h:807
      Iterator_Specification : aliased Declaration_ID;  -- a_nodes.h:810
   end record;
   pragma Convention (C_Pass_By_Copy, Expression_Struct);  -- a_nodes.h:737

   type Association_Struct is record
      kind : aliased Association_Kinds;  -- a_nodes.h:814
   end record;
   pragma Convention (C_Pass_By_Copy, Association_Struct);  -- a_nodes.h:813

   type Statement_Struct is record
      kind : aliased Statement_Kinds;  -- a_nodes.h:819
      Assignment_Variable_Name : aliased Expression_ID;  -- a_nodes.h:823
      Assignment_Expression : aliased Expression_ID;  -- a_nodes.h:824
      Statement_Paths : Path_List;  -- a_nodes.h:831
      Case_Expression : aliased Expression_ID;  -- a_nodes.h:833
      Statement_Identifier : aliased Defining_Name_ID;  -- a_nodes.h:838
      Is_Name_Repeated : aliased Extensions.bool;  -- a_nodes.h:842
      While_Condition : aliased Expression_ID;  -- a_nodes.h:844
      For_Loop_Parameter_Specification : aliased Declaration_ID;  -- a_nodes.h:846
      Loop_Statements : Statement_List;  -- a_nodes.h:850
      Is_Declare_Block : aliased Extensions.bool;  -- a_nodes.h:852
      Block_Declarative_Items : Declarative_Item_List;  -- a_nodes.h:853
      Block_Statements : Statement_List;  -- a_nodes.h:854
      Block_Exception_Handlers : Exception_Handler_List;  -- a_nodes.h:855
      Exit_Loop_Name : aliased Expression_ID;  -- a_nodes.h:857
      Exit_Condition : aliased Expression_ID;  -- a_nodes.h:858
      Corresponding_Loop_Exited : aliased Expression_ID;  -- a_nodes.h:859
      Goto_Label : aliased Expression_ID;  -- a_nodes.h:861
      Corresponding_Destination_Statement : aliased Statement_ID;  -- a_nodes.h:862
      Called_Name : aliased Expression_ID;  -- a_nodes.h:865
      Corresponding_Called_Entity : aliased Declaration_ID;  -- a_nodes.h:866
      Call_Statement_Parameters : Association_List;  -- a_nodes.h:867
      Return_Expression : aliased Expression_ID;  -- a_nodes.h:869
      Return_Object_Declaration : aliased Declaration_ID;  -- a_nodes.h:872
      Extended_Return_Statements : Statement_List;  -- a_nodes.h:873
      Extended_Return_Exception_Handlers : Exception_Handler_List;  -- a_nodes.h:874
      Accept_Entry_Index : aliased Expression_ID;  -- a_nodes.h:877
      Accept_Entry_Direct_Name : aliased Name_ID;  -- a_nodes.h:878
      Accept_Parameters : Parameter_Specification_List;  -- a_nodes.h:880
      Accept_Body_Statements : Statement_List;  -- a_nodes.h:881
      Accept_Body_Exception_Handlers : Statement_List;  -- a_nodes.h:882
      Corresponding_Entry : aliased Declaration_ID;  -- a_nodes.h:883
      Requeue_Entry_Name : aliased Name_ID;  -- a_nodes.h:886
      Delay_Expression : aliased Expression_ID;  -- a_nodes.h:889
      Aborted_Tasks : Expression_List;  -- a_nodes.h:891
      Raised_Exception : aliased Expression_ID;  -- a_nodes.h:893
      Associated_Message : aliased Expression_ID;  -- a_nodes.h:894
      Qualified_Expression : aliased Expression_ID;  -- a_nodes.h:896
   end record;
   pragma Convention (C_Pass_By_Copy, Statement_Struct);  -- a_nodes.h:818

   type Path_Struct is record
      kind : aliased Path_Kinds;  -- a_nodes.h:900
   end record;
   pragma Convention (C_Pass_By_Copy, Path_Struct);  -- a_nodes.h:899

   type Clause_Struct is record
      kind : aliased Clause_Kinds;  -- a_nodes.h:904
   end record;
   pragma Convention (C_Pass_By_Copy, Clause_Struct);  -- a_nodes.h:903

   type Exception_Handler_Struct is record
      null;
   end record;
   pragma Convention (C_Pass_By_Copy, Exception_Handler_Struct);  -- a_nodes.h:907

   type Element_Union (discr : unsigned := 0) is record
      case discr is
         when 0 =>
            the_pragma : aliased Pragma_Struct;  -- a_nodes.h:912
         when 1 =>
            defining_name : aliased Defining_Name_Struct;  -- a_nodes.h:913
         when 2 =>
            declaration : aliased Declaration_Struct;  -- a_nodes.h:914
         when 3 =>
            definition : aliased Definition_Struct;  -- a_nodes.h:915
         when 4 =>
            expression : aliased Expression_Struct;  -- a_nodes.h:916
         when 5 =>
            association : aliased Association_Struct;  -- a_nodes.h:917
         when 6 =>
            statement : aliased Statement_Struct;  -- a_nodes.h:918
         when 7 =>
            path : aliased Path_Struct;  -- a_nodes.h:919
         when 8 =>
            clause : aliased Clause_Struct;  -- a_nodes.h:920
         when others =>
            exception_handler : aliased Exception_Handler_Struct;  -- a_nodes.h:921
      end case;
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Union);
   pragma Unchecked_Union (Element_Union);  -- a_nodes.h:911

   type Element_Struct is record
      id : aliased Element_ID;  -- a_nodes.h:926
      kind : aliased Element_Kinds;  -- a_nodes.h:927
      enclosing_id : aliased Node_ID;  -- a_nodes.h:928
      enclosing_kind : aliased Enclosing_Kinds;  -- a_nodes.h:929
      source_location : Interfaces.C.Strings.chars_ptr;  -- a_nodes.h:930
      element : aliased Element_Union;  -- a_nodes.h:931
   end record;
   pragma Convention (C_Pass_By_Copy, Element_Struct);  -- a_nodes.h:925

end a_nodes_h;
