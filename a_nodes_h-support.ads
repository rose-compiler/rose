with Asis.Set_Get;
with Unchecked_Conversion;

package a_nodes_h.Support is

   package ICS renames Interfaces.C.Strings;

   Default_Node_ID   : constant Node_ID := -1;
   Default_chars_ptr : constant ICS.chars_ptr := ICS.Null_Ptr;

   -- Normally these records' fields would already have default values, but
   -- a_nodes_h.ads is generated from C, so they are here.
   --
   -- Order below is same as in a_nodes.h:

   Default_Context_Struct : constant Context_Struct :=
     (name        => ICS.Null_Ptr,
      parameters  => ICS.Null_Ptr,
      debug_image => ICS.Null_Ptr);

   Default_Unit_Struct : constant Unit_Struct :=
     (id          => Default_Node_ID,
      kind        => Not_A_Unit,
      the_class   => Not_A_Class,
      origin      => Not_An_Origin,
      full_name   => Default_chars_ptr,
      unique_name => Default_chars_ptr,
      text_name   => Default_chars_ptr,
      debug_image => Default_chars_ptr);

   -- Element union component default structs go here

   Default_Element_ID   : constant Element_ID := Default_Node_ID;
   Default_Element_List : constant Element_List :=
     (length => 0,
      IDs => null);
   Default_Name_List    : constant Name_List :=
     Name_List (Default_Element_List);
   Default_Bool         : constant Extensions.bool := 0;

   Default_Pragma_Struct : constant Pragma_Struct :=
     (kind => Not_A_Pragma);

   Default_Defining_Name_Struct : constant Defining_Name_Struct :=
     (kind          => Not_A_Defining_Name,
      name_image    => ICS.Null_Ptr,
      operator_kind => Not_An_Operator);

   Default_Declaration_Struct : constant Declaration_Struct :=
     (kind               => Not_A_Declaration,
      Trait_Kind         => Not_A_Trait,
      origin             => Not_A_Declaration_Origin,
      mode               => Not_A_Mode,
      subprogram_default => Not_A_Default);

   Default_Definition_Struct : constant Definition_Struct :=
     (Kind                            => Not_A_Definition,
      Trait_Kind                      => Not_A_Trait,
      Type_Kind                       => Not_A_Type_Definition,
      Parent_Subtype_Indication       => Default_Element_ID,
      Record_Definition               => Default_Element_ID,
      Implicit_Inherited_Declarations => Default_Element_List,
      Implicit_Inherited_Subprograms  => Default_Element_List,
      Corresponding_Parent_Subtype    => Default_Element_ID,
      Corresponding_Root_Type         => Default_Element_ID,
      Corresponding_Type_Structure    => Default_Element_ID,
      Constraint_Kind                 => Not_A_Constraint,
      Lower_Bound                     => Default_Element_ID,
      Upper_Bound                     => Default_Element_ID,
      Component_Subtype_Indication    => Default_Element_ID,
      Component_Definition_View       => Default_Element_ID);

   Default_Expression_Struct : constant Expression_Struct :=
     (kind                                     => Not_An_Expression,
      value_image                               => ICS.Null_Ptr,
      name_image                               => ICS.Null_Ptr,
      Corresponding_Name_Definition            => Default_Element_ID,
      Corresponding_Name_Definition_List       => Default_Element_List,
      Corresponding_Name_Declaration           => Default_Element_ID,
      operator_kind                            => Not_An_Operator,
      Prefix                                   => Default_Element_ID,
      Corresponding_Called_Function            => Default_Element_ID,
      Is_Prefix_Call                           => Default_Bool,
      Function_Call_Parameters                 => Default_Element_List,
      Index_Expressions                        => Default_Element_List,
      Is_Generalized_Indexing                  => Default_Bool,
      Slice_Range                              => Default_Element_ID,
      Selector                                 => Default_Element_ID,
      atribute_kind                            => Not_An_Attribute,
      Attribute_Designator_Identifier          => Default_Element_ID,
      Attribute_Designator_Expressions         => Default_Element_List,
      Record_Component_Associations            => Default_Element_List,
      Extension_Aggregate_Expression           => Default_Element_ID,
      Array_Component_Associations             => Default_Element_List,
      Short_Circuit_Operation_Left_Expression  => Default_Element_ID,
      Short_Circuit_Operation_Right_Expression => Default_Element_ID,
      Membership_Test_Expression               => Default_Element_ID,
      Membership_Test_Choices                  => Default_Element_List,
      Expression_Parenthesized                 => Default_Element_ID,
      Converted_Or_Qualified_Subtype_Mark      => Default_Element_ID,
      Converted_Or_Qualified_Expression        => Default_Element_ID,
      Predicate                                => Default_Element_ID,
      Subpool_Name                             => Default_Element_ID,
      Allocator_Subtype_Indication             => Default_Element_ID,
      Allocator_Qualified_Expression           => Default_Element_ID,
      Expression_Paths                         => Default_Element_List,
      Iterator_Specification                   => Default_Element_ID);

   Default_Association_Struct : constant Association_Struct :=
     (kind                        => Not_An_Association,
      Array_Component_Choices     => Default_Element_List,
      Record_Component_Choices    => Default_Element_List,
      Component_Expression        => Default_Element_ID,
      Formal_Parameter            => Default_Element_ID,
      Actual_Parameter            => Default_Element_ID,
      Discriminant_Selector_Names => Default_Element_List,
      Discriminant_Expression     => Default_Element_ID,
      Is_Normalized               => Default_Bool,
      Is_Defaulted_Association    => Default_Bool);

   Default_Statement_Struct : constant Statement_Struct :=
     (kind                                => Not_A_Statement,
      Assignment_Variable_Name            => Default_Element_ID,
      Assignment_Expression               => Default_Element_ID,
      Statement_Paths                     => Default_Element_List,
      Case_Expression                     => Default_Element_ID,
      Statement_Identifier                => Default_Element_ID,
      Is_Name_Repeated                    => Default_Bool,
      While_Condition                     => Default_Element_ID,
      For_Loop_Parameter_Specification    => Default_Element_ID,
      Loop_Statements                     => Default_Element_List,
      Is_Declare_Block                    => Default_Bool,
      Block_Declarative_Items             => Default_Element_List,
      Block_Statements                    => Default_Element_List,
      Block_Exception_Handlers            => Default_Element_List,
      Exit_Loop_Name                      => Default_Element_ID,
      Exit_Condition                      => Default_Element_ID,
      Corresponding_Loop_Exited           => Default_Element_ID,
      Goto_Label                          => Default_Element_ID,
      Corresponding_Destination_Statement => Default_Element_ID,
      Called_Name                         => Default_Element_ID,
      Corresponding_Called_Entity         => Default_Element_ID,
      Call_Statement_Parameters           => Default_Element_List,
      Return_Expression                   => Default_Element_ID,
      Return_Object_Declaration           => Default_Element_ID,
      Extended_Return_Statements          => Default_Element_List,
      Extended_Return_Exception_Handlers  => Default_Element_List,
      Accept_Entry_Index                  => Default_Element_ID,
      Accept_Entry_Direct_Name            => Default_Element_ID,
      Accept_Parameters                   => Default_Element_List,
      Accept_Body_Statements              => Default_Element_List,
      Accept_Body_Exception_Handlers      => Default_Element_List,
      Corresponding_Entry                 => Default_Element_ID,
      Requeue_Entry_Name                  => Default_Element_ID,
      Delay_Expression                    => Default_Element_ID,
      Aborted_Tasks                       => Default_Element_List,
      Raised_Exception                    => Default_Element_ID,
      Associated_Message                  => Default_Element_ID,
      Qualified_Expression                => Default_Element_ID);

   Default_Path_Struct : constant Path_Struct :=
     (kind => Not_A_Path);

   Default_Clause_Struct : constant Clause_Struct :=
     (kind         => Not_A_Clause,
      Trait_Kind   => Not_A_Trait,
      Clause_Names => Default_Name_List);

   -- Currently a null record, so we can't do this:
   -- Default_Exception_Handler_Struct : constant Exception_Handler_Struct :=
   -- (
   --  );
   Default_Exception_Handler_Struct : Exception_Handler_Struct;

   Default_Element_Union : constant Element_Union :=
     (discr        => 0,
      dummy_member => 0);

   Default_Element_Struct : constant Element_Struct :=
     (id              => Default_Node_ID,
      kind            => Not_An_Element,
      enclosing_id    => Default_Node_ID,
      enclosing_kind  => Not_Enclosing,
      source_location => Default_chars_ptr,
      the_union       => Default_Element_Union);

   Default_Node_Union : constant Node_Union :=
     (discr        => 0,
      dummy_member => 0);

   Default_Node_Struct : constant Node_Struct :=
     (kind      => Not_A_Node,
      the_union => Default_Node_Union);

   Default_List_Node_Struct : constant List_Node_Struct :=
     (node       => Default_Node_Struct,
      next       => null,
      next_count => 0);


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

   function To_Trait_Kinds is new Unchecked_Conversion
     (Source => Asis.Trait_Kinds,
      Target => a_nodes_h.Trait_Kinds);

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

   function To_Element_ID
     (Item : in Asis.Element)
      return a_nodes_h.Node_ID
   is
     (a_nodes_h.Node_ID (Asis.Set_Get.Node_Value (Item)));

   -- Not in a_nodes.h:

   function To_bool
     (Item : in Boolean)
      return Interfaces.C.Extensions.bool
   is
     (if Item then 1 else 0);

   type Element_ID_Array is array (Positive range <>) of aliased Element_ID;
   -- Not called _Ptr so we don't forget a pointer to this is not the same as a
   -- pointer to a C array.  We just need this to create the array on the hea:
   type Element_ID_Array_Access is access Element_ID_Array;

   function To_Element_ID_Ptr
     (Item : not null access Element_ID_Array)
      return Element_ID_Ptr is
     (if Item.all'Length = 0 then
         null
      else
         Item.all (Item.all'First)'Unchecked_Access);


end a_nodes_h.Support;
