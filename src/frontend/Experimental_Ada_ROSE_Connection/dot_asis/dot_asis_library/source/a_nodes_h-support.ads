with Asis.Set_Get;
with Unchecked_Conversion;

package a_nodes_h.Support is
   -- Records written in good Ada style should already have default values for
   -- their components. a_nodes_h.ads is generated from C, so this package
   -- supplies constant records for safe initialization.

   package ICE renames Interfaces.C.Extensions;
   package ICS renames Interfaces.C.Strings;

   Invalid_bool         : constant ICE.bool := ICE.False;
   Invalid_chars_ptr    : constant ICS.chars_ptr := ICS.Null_Ptr;
   Invalid_ID           : constant := -1;
   Empty_ID             : constant := 0;

   function Is_Valid (This : in int) return Boolean is
      (This /= Invalid_ID);

   function Is_Empty (This : in int) return Boolean is
      (This = Empty_ID);

   -- Order below is same as in a_nodes.h:

   Invalid_Program_Text : constant Program_Text := Program_Text(ICS.Null_Ptr);

   Invalid_Element_ID   : constant Element_ID := Invalid_ID;

   Empty_Element_ID_List   : constant Element_ID_List :=
     (length => 0,
      IDs => null);
   Empty_Name_List      : constant Name_List :=
     Name_List (Empty_Element_ID_List);

   Default_Context_Struct : constant Context_Struct :=
     (name        => Invalid_chars_ptr,
      parameters  => Invalid_chars_ptr,
      debug_image => Invalid_chars_ptr);

   -- Element union component default structs go here


   Default_Pragma_Struct : constant Pragma_Struct :=
     (Pragma_Kind                  => Not_A_Pragma,
      Pragmas                      => Empty_Element_ID_List,
      Pragma_Name_Image            => Invalid_Program_Text,
      Pragma_Argument_Associations => Empty_Element_ID_List
     );

   Default_Defining_Name_Struct : constant Defining_Name_Struct :=
     (Defining_Name_Kind                 => Not_A_Defining_Name,
      Defining_Name_Image                => Invalid_chars_ptr,
      References                         => Empty_Name_List,
      Is_Referenced                      => Invalid_bool,
      Position_Number_Image              => Invalid_chars_ptr,
      Representation_Value_Image         => Invalid_chars_ptr,
      Defining_Prefix                    => Invalid_Element_ID,
      Defining_Selector                  => Invalid_Element_ID,
      Corresponding_Constant_Declaration => Invalid_Element_ID,
      Operator_Kind                      => Not_An_Operator,
      Corresponding_Generic_Element      => Invalid_Element_ID);

   Default_Declaration_Struct : constant Declaration_Struct :=
     (Declaration_Kind                     => Not_A_Declaration,
      Declaration_Origin                   => Not_A_Declaration_Origin,
      Corresponding_Pragmas                => Empty_Element_ID_List,
      Names                                => Empty_Name_List,
      Aspect_Specifications                => Empty_Element_ID_List,
      Corresponding_Representation_Clauses => Empty_Element_ID_List,
      Has_Abstract                         => Invalid_bool,
      Has_Aliased                          => Invalid_bool,
      Has_Limited                          => Invalid_bool,
      Has_Private                          => Invalid_bool,
      Has_Protected                        => Invalid_bool,
      Has_Reverse                          => Invalid_bool,
      Has_Task                             => Invalid_bool,
      Has_Null_Exclusion                   => Invalid_bool,
      Is_Not_Null_Return                   => Invalid_bool,
      Mode_Kind                            => Not_A_Mode,
      Default_Kind                         => Not_A_Default,
      Pragmas                              => Empty_Element_ID_List,
      Corresponding_End_Name               => Invalid_Element_ID,
      Discriminant_Part                    => Invalid_Element_ID,
      Type_Declaration_View                => Invalid_Element_ID,
      Object_Declaration_View              => Invalid_Element_ID,
      Initialization_Expression            => Invalid_Element_ID,
      Corresponding_Type_Declaration       => Invalid_Element_ID,
      Corresponding_Type_Completion        => Invalid_Element_ID,
      Corresponding_Type_Partial_View      => Invalid_Element_ID,
      Corresponding_First_Subtype          => Invalid_Element_ID,
      Corresponding_Last_Constraint        => Invalid_Element_ID,
      Corresponding_Last_Subtype           => Invalid_Element_ID,
      Specification_Subtype_Definition     => Invalid_Element_ID,
      Iteration_Scheme_Name                => Invalid_Element_ID,
      Subtype_Indication                   => Invalid_Element_ID,
      Parameter_Profile                    => Empty_Element_ID_List,
      Result_Profile                       => Invalid_Element_ID,
      Result_Expression                    => Invalid_Element_ID,
      Is_Overriding_Declaration            => Invalid_bool,
      Is_Not_Overriding_Declaration        => Invalid_bool,
      Body_Declarative_Items               => Empty_Element_ID_List,
      Body_Statements                      => Empty_Element_ID_List,
      Body_Exception_Handlers              => Empty_Element_ID_List,
      Body_Block_Statement                 => Invalid_Element_ID,
      Is_Name_Repeated                     => Invalid_bool,
      Corresponding_Declaration            => Invalid_Element_ID,
      Corresponding_Body                   => Invalid_Element_ID,
      Corresponding_Subprogram_Derivation  => Invalid_Element_ID,
      Corresponding_Type                   => Invalid_Element_ID,
      Corresponding_Equality_Operator      => Invalid_Element_ID,
      Visible_Part_Declarative_Items       => Empty_Element_ID_List,
      Is_Private_Present                   => Invalid_bool,
      Private_Part_Declarative_Items       => Empty_Element_ID_List,
      Declaration_Interface_List           => Empty_Element_ID_List,
      Renamed_Entity                       => Invalid_Element_ID,
      Corresponding_Base_Entity            => Invalid_Element_ID,
      Protected_Operation_Items            => Empty_Element_ID_List,
      Entry_Family_Definition              => Invalid_Element_ID,
      Entry_Index_Specification            => Invalid_Element_ID,
      Entry_Barrier                        => Invalid_Element_ID,
      Corresponding_Subunit                => Invalid_Element_ID,
      Is_Subunit                           => Invalid_bool,
      Corresponding_Body_Stub              => Invalid_Element_ID,
      Generic_Formal_Part                  => Empty_Element_ID_List,
      Generic_Unit_Name                    => Invalid_Element_ID,
      Generic_Actual_Part                  => Empty_Element_ID_List,
      Formal_Subprogram_Default            => Invalid_Element_ID,
      Is_Dispatching_Operation             => Invalid_bool);

   Default_Access_Type_Struct : constant Access_Type_Struct :=
     (Access_Type_Kind                       => Not_An_Access_Type_Definition,
      Has_Null_Exclusion                     => Invalid_bool,
      Is_Not_Null_Return                     => Invalid_bool,
      Access_To_Object_Definition            => Invalid_Element_ID,
      Access_To_Subprogram_Parameter_Profile => Empty_Element_ID_List,
      Access_To_Function_Result_Profile      => Invalid_Element_ID);

   Default_Type_Definition_Struct : constant Type_Definition_Struct :=
     (Type_Kind                        => Not_A_Type_Definition,
      Has_Abstract                     => Invalid_bool,
      Has_Limited                      => Invalid_bool,
      Has_Private                      => Invalid_bool,
      Corresponding_Type_Operators     => Empty_Element_ID_List,
      Has_Protected                    => Invalid_bool,
      Has_Synchronized                 => Invalid_bool,
      Has_Tagged                       => Invalid_bool,
      Has_Task                         => Invalid_bool,
      Has_Null_Exclusion               => Invalid_bool,
      Interface_Kind                   => Not_An_Interface,
      Root_Type_Kind                   => Not_A_Root_Type_Definition,
      Parent_Subtype_Indication        => Invalid_Element_ID,
      Record_Definition                => Invalid_Element_ID,
      Implicit_Inherited_Declarations  => Empty_Element_ID_List,
      Implicit_Inherited_Subprograms   => Empty_Element_ID_List,
      Corresponding_Parent_Subtype     => Invalid_Element_ID,
      Corresponding_Root_Type          => Invalid_Element_ID,
      Corresponding_Type_Structure     => Invalid_Element_ID,
      Enumeration_Literal_Declarations => Empty_Element_ID_List,
      Integer_Constraint               => Invalid_Element_ID,
      Mod_Static_Expression            => Invalid_Element_ID,
      Digits_Expression                => Invalid_Element_ID,
      Delta_Expression                 => Invalid_Element_ID,
      Real_Range_Constraint            => Invalid_Element_ID,
      Index_Subtype_Definitions        => Empty_Element_ID_List,
      Discrete_Subtype_Definitions     => Empty_Element_ID_List,
      Array_Component_Definition       => Invalid_Element_ID,
      Definition_Interface_List        => Empty_Element_ID_List,
      Access_Type                      => Default_Access_Type_Struct);

   Default_Constraint_Struct : constant Constraint_Struct :=
     (Constraint_Kind           => Not_A_Constraint,
      Digits_Expression         => Invalid_Element_ID,
      Delta_Expression          => Invalid_Element_ID,
      Real_Range_Constraint     => Invalid_Element_ID,
      Lower_Bound               => Invalid_Element_ID,
      Upper_Bound               => Invalid_Element_ID,
      Range_Attribute           => Invalid_Element_ID,
      Discrete_Ranges           => Empty_Element_ID_List,
      Discriminant_Associations => Empty_Element_ID_List);

   Default_Subtype_Indication_Struct : constant Subtype_Indication_Struct :=
     (Has_Null_Exclusion => Invalid_bool,
      Subtype_Mark       => Invalid_Element_ID,
      Subtype_Constraint => Invalid_Element_ID);

   Default_Component_Definition_Struct : constant Component_Definition_Struct :=
     (Component_Definition_View => Invalid_Element_ID);

   Default_Discrete_Subtype_Definition_Struct :
   constant Discrete_Subtype_Definition_Struct :=
     (Discrete_Range_Kind => Not_A_Discrete_Range,
      Subtype_Mark        => Invalid_Element_ID,
      Subtype_Constraint  => Invalid_Element_ID);

   Default_Discrete_Range_Struct : constant Discrete_Range_Struct :=
     (Discrete_Range_Kind => Not_A_Discrete_Range,
      Subtype_Mark        => Invalid_Element_ID,
      Subtype_Constraint  => Invalid_Element_ID,
      Lower_Bound         => Invalid_Element_ID,
      Upper_Bound         => Invalid_Element_ID,
      Range_Attribute     => Invalid_Element_ID);

   Default_Known_Discriminant_Part_Struct :
   constant Known_Discriminant_Part_Struct :=
     (Discriminants => Empty_Element_ID_List);

   Default_Record_Definition_Struct : constant Record_Definition_Struct :=
     (Record_Components   => Empty_Element_ID_List,
      Implicit_Components => Empty_Element_ID_List);

   Default_Variant_Part_Struct : constant Variant_Part_Struct :=
     (Discriminant_Direct_Name => Invalid_Element_ID,
      Variants                 => Empty_Element_ID_List);

   Default_Variant_Struct : constant Variant_Struct :=
     (Record_Components   => Empty_Element_ID_List,
      Implicit_Components => Empty_Element_ID_List,
      Variant_Choices     => Empty_Element_ID_List);

   Default_Access_Definition_Struct : constant Access_Definition_Struct :=
     (Access_Definition_Kind                  => Not_An_Access_Definition,
      Has_Null_Exclusion                      => Invalid_bool,
      Is_Not_Null_Return                      => Invalid_bool,
      Anonymous_Access_To_Object_Subtype_Mark => Invalid_Element_ID,
      Access_To_Subprogram_Parameter_Profile  => Empty_Element_ID_List,
      Access_To_Function_Result_Profile       => Invalid_Element_ID);

   Default_Private_Type_Definition_Struct :
   constant Private_Type_Definition_Struct :=
     (Has_Abstract => Invalid_bool,
      Has_Limited  => Invalid_bool,
      Has_Private  => Invalid_bool);

   Default_Tagged_Private_Type_Definition_Struct :
   constant Tagged_Private_Type_Definition_Struct :=
     (Has_Abstract => Invalid_bool,
      Has_Limited  => Invalid_bool,
      Has_Private  => Invalid_bool,
      Has_Tagged   => Invalid_bool);

   Default_Private_Extension_Definition_Struct :
   constant Private_Extension_Definition_Struct :=
     (Has_Abstract                    => Invalid_bool,
      Has_Limited                     => Invalid_bool,
      Has_Private                     => Invalid_bool,
      Has_Synchronized                => Invalid_bool,
      Implicit_Inherited_Declarations => Empty_Element_ID_List,
      Implicit_Inherited_Subprograms  => Empty_Element_ID_List,
      Definition_Interface_List       => Empty_Element_ID_List,
      Ancestor_Subtype_Indication     => Invalid_Element_ID);

   Default_Task_Definition_Struct : constant Task_Definition_Struct :=
     (Has_Task           => Invalid_bool,
      Visible_Part_Items => Empty_Element_ID_List,
      Private_Part_Items => Empty_Element_ID_List,
      Is_Private_Present => Invalid_bool);

   Default_Protected_Definition_Struct : constant Protected_Definition_Struct :=
     (Has_Protected      => Invalid_bool,
      Visible_Part_Items => Empty_Element_ID_List,
      Private_Part_Items => Empty_Element_ID_List,
      Is_Private_Present => Invalid_bool);

   Defult_Formal_Type_Definition_Struct :
   constant Formal_Type_Definition_Struct :=
     (Formal_Type_Kind                => Not_A_Formal_Type_Definition,
      Corresponding_Type_Operators    => Empty_Element_ID_List,
      Has_Abstract                    => Invalid_bool,
      Has_Limited                     => Invalid_bool,
      Has_Private                     => Invalid_bool,
      Has_Synchronized                => Invalid_bool,
      Has_Tagged                      => Invalid_bool,
      Interface_Kind                  => Not_An_Interface,
      Implicit_Inherited_Declarations => Empty_Element_ID_List,
      Implicit_Inherited_Subprograms  => Empty_Element_ID_List,
      Index_Subtype_Definitions       => Empty_Element_ID_List,
      Discrete_Subtype_Definitions    => Empty_Element_ID_List,
      Array_Component_Definition      => Invalid_Element_ID,
      Subtype_Mark                    => Invalid_Element_ID,
      Definition_Interface_List       => Empty_Element_ID_List,
      Access_Type                     => Default_Access_Type_Struct);

   Default_Aspect_Specification_Struct : constant Aspect_Specification_Struct :=
     (Aspect_Mark       => Invalid_Element_ID,
      Aspect_Definition => Invalid_Element_ID);

   Default_No_Struct : constant No_Struct := -1;

   Default_Definition_Union : constant Definition_Union :=
     (discr        => 0,
      Dummy_Member => -1);

   Default_Definition_Struct : constant Definition_Struct :=
     (Definition_Kind                 => Not_A_Definition,
      The_Union => Default_Definition_Union);


   Default_Expression_Struct : constant Expression_Struct :=
     (Expression_Kind                          => Not_An_Expression,
      Is_Prefix_Notation                       => Invalid_bool,
      Corresponding_Expression_Type            => Invalid_Element_ID,
      Corresponding_Expression_Type_Definition => Invalid_Element_ID,
      Operator_Kind                            => Not_An_Operator,
      Attribute_Kind                            => Not_An_Attribute,
      Value_Image                              => Invalid_chars_ptr,
      Name_Image                               => Invalid_chars_ptr,
      Corresponding_Name_Definition            => Invalid_Element_ID,
      Corresponding_Name_Definition_List       => Empty_Element_ID_List,
      Corresponding_Name_Declaration           => Invalid_Element_ID,
      Prefix                                   => Invalid_Element_ID,
      Index_Expressions                        => Empty_Element_ID_List,
      Slice_Range                              => Invalid_Element_ID,
      Selector                                 => Invalid_Element_ID,
      Attribute_Designator_Identifier          => Invalid_Element_ID,
      Attribute_Designator_Expressions         => Empty_Element_ID_List,
      Record_Component_Associations            => Empty_Element_ID_List,
      Extension_Aggregate_Expression           => Invalid_Element_ID,
      Array_Component_Associations             => Empty_Element_ID_List,
      Expression_Parenthesized                 => Invalid_Element_ID,
      Is_Prefix_Call                           => Invalid_bool,
      Corresponding_Called_Function            => Invalid_Element_ID,
      Function_Call_Parameters                 => Empty_Element_ID_List,
      Short_Circuit_Operation_Left_Expression  => Invalid_Element_ID,
      Short_Circuit_Operation_Right_Expression => Invalid_Element_ID,
      Membership_Test_Expression               => Invalid_Element_ID,
      Membership_Test_Choices                  => Empty_Element_ID_List,
      Converted_Or_Qualified_Subtype_Mark      => Invalid_Element_ID,
      Converted_Or_Qualified_Expression        => Invalid_Element_ID,
      Allocator_Subtype_Indication             => Invalid_Element_ID,
      Allocator_Qualified_Expression           => Invalid_Element_ID,
      Expression_Paths                         => Empty_Element_ID_List,
      Is_Generalized_Indexing                  => Invalid_bool,
      Is_Generalized_Reference                 => Invalid_bool,
      Iterator_Specification                   => Invalid_Element_ID,
      Predicate                                => Invalid_Element_ID,
      Subpool_Name                             => Invalid_Element_ID,
      Corresponding_Generic_Element            => Invalid_Element_ID,
      Is_Dispatching_Call                      => Invalid_bool,
      Is_Call_On_Dispatching_Operation         => Invalid_bool);

   Default_Association_Struct : constant Association_Struct :=
     (Association_Kind            => Not_An_Association,
      Array_Component_Choices     => Empty_Element_ID_List,
      Record_Component_Choices    => Empty_Element_ID_List,
      Component_Expression        => Invalid_Element_ID,
      Formal_Parameter            => Invalid_Element_ID,
      Actual_Parameter            => Invalid_Element_ID,
      Discriminant_Selector_Names => Empty_Element_ID_List,
      Discriminant_Expression     => Invalid_Element_ID,
      Is_Normalized               => Invalid_bool,
      Is_Defaulted_Association    => Invalid_bool);

   Default_Statement_Struct : constant Statement_Struct :=
     (Statement_Kind                      => Not_A_Statement,
      Corresponding_Pragmas               => Empty_Element_ID_List,
      Label_Names                         => Empty_Element_ID_List,
      Is_Prefix_Notation                  => Invalid_bool,
      Pragmas                             => Empty_Element_ID_List,
      Corresponding_End_Name              => Invalid_Element_ID,
      Assignment_Variable_Name            => Invalid_Element_ID,
      Assignment_Expression               => Invalid_Element_ID,
      Statement_Paths                     => Empty_Element_ID_List,
      Case_Expression                     => Invalid_Element_ID,
      Statement_Identifier                => Invalid_Element_ID,
      Is_Name_Repeated                    => Invalid_bool,
      While_Condition                     => Invalid_Element_ID,
      For_Loop_Parameter_Specification    => Invalid_Element_ID,
      Loop_Statements                     => Empty_Element_ID_List,
      Is_Declare_Block                    => Invalid_bool,
      Block_Declarative_Items             => Empty_Element_ID_List,
      Block_Statements                    => Empty_Element_ID_List,
      Block_Exception_Handlers            => Empty_Element_ID_List,
      Exit_Loop_Name                      => Invalid_Element_ID,
      Exit_Condition                      => Invalid_Element_ID,
      Corresponding_Loop_Exited           => Invalid_Element_ID,
      Return_Expression                   => Invalid_Element_ID,
      Return_Object_Declaration           => Invalid_Element_ID,
      Extended_Return_Statements          => Empty_Element_ID_List,
      Extended_Return_Exception_Handlers  => Empty_Element_ID_List,
      Goto_Label                          => Invalid_Element_ID,
      Corresponding_Destination_Statement => Invalid_Element_ID,
      Called_Name                         => Invalid_Element_ID,
      Corresponding_Called_Entity         => Invalid_Element_ID,
      Call_Statement_Parameters           => Empty_Element_ID_List,
      Accept_Entry_Index                  => Invalid_Element_ID,
      Accept_Entry_Direct_Name            => Invalid_Element_ID,
      Accept_Parameters                   => Empty_Element_ID_List,
      Accept_Body_Statements              => Empty_Element_ID_List,
      Accept_Body_Exception_Handlers      => Empty_Element_ID_List,
      Corresponding_Entry                 => Invalid_Element_ID,
      Requeue_Entry_Name                  => Invalid_Element_ID,
      Delay_Expression                    => Invalid_Element_ID,
      Aborted_Tasks                       => Empty_Element_ID_List,
      Raised_Exception                    => Invalid_Element_ID,
      Associated_Message                  => Invalid_Element_ID,
      Qualified_Expression                => Invalid_Element_ID,
      Is_Dispatching_Call                 => Invalid_bool,
      Is_Call_On_Dispatching_Operation    => Invalid_bool,
      Corresponding_Called_Entity_Unwound => Invalid_Element_ID);

   Default_Path_Struct : constant Path_Struct :=
     (Path_Kind                     => Not_A_Path,
      Sequence_Of_Statements        => Empty_Element_ID_List,
      Dependent_Expression          => Invalid_Element_ID,
      Condition_Expression          => Invalid_Element_ID,
      Case_Path_Alternative_Choices => Empty_Element_ID_List,
      Guard                         => Invalid_Element_ID);

   Default_Representation_Clause_Struct :
   constant Representation_Clause_Struct :=
     (Representation_Clause_Kind       => Not_A_Representation_Clause,
      Representation_Clause_Name       => Invalid_Element_ID,
      Pragmas                          => Empty_Element_ID_List,
      Representation_Clause_Expression => Invalid_Element_ID,
      Mod_Clause_Expression            => Invalid_Element_ID,
      Component_Clauses                => Empty_Element_ID_List);

   Default_Clause_Struct : constant Clause_Struct :=
     (Clause_Kind                      => Not_A_Clause,
      Has_Limited                      => Invalid_bool,
      Clause_Names                     => Empty_Name_List,
      Representation_Clause_Name       => Invalid_Element_ID,
      Representation_Clause_Expression => Invalid_Element_ID,
      Mod_Clause_Expression            => Invalid_Element_ID,
      Component_Clauses                => Empty_Element_ID_List,
      Component_Clause_Position        => Invalid_Element_ID,
      Component_Clause_Range           => Invalid_Element_ID,
      Representation_Clause            => Default_Representation_Clause_Struct);

   Default_Exception_Handler_Struct : constant Exception_Handler_Struct :=
     (Pragmas                        => Empty_Element_ID_List,
      Choice_Parameter_Specification => Invalid_Element_ID,
      Exception_Choices              => Empty_Element_ID_List,
      Handler_Statements             => Empty_Element_ID_List);

   Default_Element_Union : constant Element_Union :=
     (Discr        => 0,
      Dummy_Member => 0);

   Default_Source_Location_Struct : constant Source_Location_Struct :=
     (Unit_Name    => Invalid_chars_ptr,
      First_Line   => -1,
      First_Column => -1,
      Last_Line    => -1,
      Last_Column  => -1);

   Invalid_Unit_ID : constant Unit_ID := Invalid_ID;

   Default_Element_Struct : constant Element_Struct :=
     (ID                         => Invalid_Element_ID,
      Element_Kind               => Not_An_Element,
      Enclosing_Compilation_Unit => Invalid_Unit_ID,
      Is_Part_Of_Implicit        => Invalid_bool,
      Is_Part_Of_Inherited       => Invalid_bool,
      Is_Part_Of_Instance        => Invalid_bool,
      Hash                       => -1,
      Enclosing_Element_Id       => Invalid_Element_ID,
      Enclosing_Kind             => Not_Enclosing,
      Source_Location            => Default_Source_Location_Struct,
      Debug_Image                => Invalid_chars_ptr,
      The_Union                  => Default_Element_Union);

   Empty_Unit_List : constant Unit_List :=
     (length => 0,
      IDs => null);

   Default_Unit_Struct : constant Unit_Struct :=
     (ID                                => Invalid_Unit_ID,
      Unit_Kind                         => Not_A_Unit,
      Unit_Class                        => Not_A_Class,
      Unit_Origin                       => Not_An_Origin,
      Unit_Full_Name                    => Invalid_chars_ptr,
      Unique_Name                       => Invalid_chars_ptr,
      Exists                            => Invalid_bool,
      Can_Be_Main_Program               => Invalid_bool,
      Is_Body_Required                  => Invalid_bool,
      Text_Name                         => Invalid_chars_ptr,
      Text_Form                         => Invalid_chars_ptr,
      Object_Name                       => Invalid_chars_ptr,
      Object_Form                       => Invalid_chars_ptr,
      Compilation_Command_Line_Options  => Invalid_chars_ptr,
      Debug_Image                       => Invalid_chars_ptr,
      Unit_Declaration                  => Invalid_Element_ID,
      Context_Clause_Elements           => Empty_Element_ID_List,
      Compilation_Pragmas               => Empty_Element_ID_List,
      Is_Standard                       => Invalid_bool,
      Corresponding_Children            => Empty_Unit_List,
      Corresponding_Parent_Declaration  => Invalid_Unit_ID,
      Corresponding_Declaration         => Invalid_Unit_ID,
      Corresponding_Body                => Invalid_Unit_ID,
      Subunits                          => Empty_Unit_List,
      Corresponding_Subunit_Parent_Body => Invalid_Unit_ID);

   Default_Unit_Struct_List_Struct : constant Unit_Struct_List_Struct :=
     (Unit       => Default_Unit_Struct,
      Next       => null,
      Next_count => 0);

   Default_Element_Struct_List_Struct : constant Element_Struct_List_Struct :=
     (Element    => Default_Element_Struct,
      Next       => null,
      Next_count => 0);

   Default_Nodes_Struct : constant Nodes_Struct :=
     (Context  => Default_Context_Struct,
      Units    => null,
      Elements => null);

   -- Order below is alphabetical:
   function To_Association_Kinds is new Unchecked_Conversion
     (Source => Asis.Association_Kinds,
      Target => a_nodes_h.Association_Kinds);

   function To_Attribute_Kinds is new Unchecked_Conversion
     (Source => Asis.Attribute_Kinds,
      Target => a_nodes_h.Attribute_Kinds);

   function To_Clause_Kinds is new Unchecked_Conversion
     (Source => Asis.Clause_Kinds,
      Target => a_nodes_h.Clause_Kinds);

   function To_Constraint_Kinds is new Unchecked_Conversion
     (Source => Asis.Constraint_Kinds,
      Target => a_nodes_h.Constraint_Kinds);

   function To_Declaration_Kinds is new Unchecked_Conversion
     (Source => Asis.Declaration_Kinds,
      Target => a_nodes_h.Declaration_Kinds);

   function To_Declaration_Origins is new Unchecked_Conversion
     (Source => Asis.Declaration_Origins,
      Target => a_nodes_h.Declaration_Origins);

   function To_Defining_Name_Kinds is new Unchecked_Conversion
     (Source => Asis.Defining_Name_Kinds,
      Target => a_nodes_h.Defining_Name_Kinds);

   function To_Definition_Kinds is new Unchecked_Conversion
     (Source => Asis.Definition_Kinds,
      Target => a_nodes_h.Definition_Kinds);

   function To_Element_Kinds is new Unchecked_Conversion
     (Source => Asis.Element_Kinds,
      Target => a_nodes_h.Element_Kinds);

   function To_Expression_Kinds is new Unchecked_Conversion
     (Source => Asis.Expression_Kinds,
      Target => a_nodes_h.Expression_Kinds);

   function To_Mode_Kinds is new Unchecked_Conversion
     (Source => Asis.Mode_Kinds,
      Target => a_nodes_h.Mode_Kinds);

   function To_Operator_Kinds is new Unchecked_Conversion
     (Source => Asis.Operator_Kinds,
      Target => a_nodes_h.Operator_Kinds);

   function To_Path_Kinds is new Unchecked_Conversion
     (Source => Asis.Path_Kinds,
      Target => a_nodes_h.Path_Kinds);

   function To_Pragma_Kinds is new Unchecked_Conversion
     (Source => Asis.Pragma_Kinds,
      Target => a_nodes_h.Pragma_Kinds);

   function To_Statement_Kinds is new Unchecked_Conversion
     (Source => Asis.Statement_Kinds,
      Target => a_nodes_h.Statement_Kinds);

   function To_Subprogram_Default_Kinds is new Unchecked_Conversion
     (Source => Asis.Subprogram_Default_Kinds,
      Target => a_nodes_h.Subprogram_Default_Kinds);

   function To_Type_Kinds is new Unchecked_Conversion
     (Source => Asis.Type_Kinds,
      Target => a_nodes_h.Type_Kinds);

   function To_Unit_Classes is new Unchecked_Conversion
     (Source => Asis.Unit_Classes,
      Target => a_nodes_h.Unit_Classes);

   function To_Unit_Kinds is new Unchecked_Conversion
     (Source => Asis.Unit_Kinds,
      Target => a_nodes_h.Unit_Kinds);

   function To_Unit_Origins is new Unchecked_Conversion
     (Source => Asis.Unit_Origins,
      Target => a_nodes_h.Unit_Origins);

   -- End alphabetical order

   -- Not in a_nodes.h:

   function To_bool
     (Item : in Boolean)
      return ICE.bool
   is
     (if Item then ICE.True else ICE.False);

   type Unit_ID_Array is array (Positive range <>) of aliased Unit_ID;
   -- Not called _Ptr so we don't forget a pointer to this is not the same as a
   -- pointer to a C array.  We just need this to create the array on the heap:
   type Unit_ID_Array_Access is access Unit_ID_Array;

   function To_Unit_ID_Ptr
     (Item : not null access Unit_ID_Array)
      return Unit_ID_Ptr is
     (if Item.all'Length = 0 then
         null
      else
         Item.all (Item.all'First)'Unchecked_Access);

   type Element_ID_Array is array (Positive range <>) of aliased Element_ID;
   -- Not called _Ptr so we don't forget a pointer to this is not the same as a
   -- pointer to a C array.  We just need this to create the array on the heap:
   type Element_ID_Array_Access is access Element_ID_Array;

   function To_Element_ID_Ptr
     (Item : not null access Element_ID_Array)
      return Element_ID_Ptr is
     (if Item.all'Length = 0 then
         null
      else
         Item.all (Item.all'First)'Unchecked_Access);

end a_nodes_h.Support;
