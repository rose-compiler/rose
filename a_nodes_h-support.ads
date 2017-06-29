with Asis;
with Unchecked_Conversion;

package a_nodes_h.Support is

   package ICS renames Interfaces.C.Strings;

   -- Normally these records' fields would already have default values, but
   -- a_nodes_h.ads is generated from C, so they are here.

   Context_Struct_Default : constant Context_Struct :=
     (name        => ICS.Null_Ptr,
      parameters  => ICS.Null_Ptr,
      debug_image => ICS.Null_Ptr);

   Default_Node_ID   : constant Node_ID := -1;
   Default_chars_ptr : constant ICS. chars_ptr := ICS.Null_Ptr;

   Unit_Struct_Default : constant Unit_Struct :=
     (id          => Default_Node_ID,
      kind        => Not_A_Unit,
      the_class   => Not_A_Class,
      origin      => Not_An_Origin,
      full_name   => Default_chars_ptr,
      unique_name => Default_chars_ptr,
      text_name   => Default_chars_ptr,
      debug_image => Default_chars_ptr);


   Element_Union_Default : constant Element_Union :=
     (discr        => 0,
      dummy_member => 0);

   Element_Struct_Default : constant Element_Struct :=
     (id              => Default_Node_ID,
      kind            => Not_An_Element,
      enclosing_id    => Default_Node_ID,
      enclosing_kind  => Not_Enclosing,
      source_location => Default_chars_ptr,
      the_union       => Element_Union_Default);

   Node_Union_Default : constant Node_Union :=
     (discr        => 0,
      dummy_member => 0);

   Node_Struct_Default : constant Node_Struct :=
     (kind      => Not_A_Node,
      the_union => Node_Union_Default);

   List_Node_Struct_Default : constant List_Node_Struct :=
     (node       => Node_Struct_Default,
      next       => null,
      next_count => 0);

   -- Order below is same as in a_nodes.h:
   function To_Unit_Kinds is new Unchecked_Conversion
     (Source => Asis.Unit_Kinds,
      Target => a_nodes_h.Unit_Kinds);

   function To_Unit_Classes is new Unchecked_Conversion
     (Source => Asis.Unit_Classes,
      Target => a_nodes_h.Unit_Classes);

   function To_Unit_Origins is new Unchecked_Conversion
     (Source => Asis.Unit_Origins,
      Target => a_nodes_h.Unit_Origins);

   function To_Element_Kinds is new Unchecked_Conversion
     (Source => Asis.Element_Kinds,
      Target => a_nodes_h.Element_Kinds);

   function To_Pragma_Kinds is new Unchecked_Conversion
     (Source => Asis.Pragma_Kinds,
      Target => a_nodes_h.Pragma_Kinds);

   function To_Defining_Name_Kinds is new Unchecked_Conversion
     (Source => Asis.Defining_Name_Kinds,
      Target => a_nodes_h.Defining_Name_Kinds);

   function To_Declaration_Kinds is new Unchecked_Conversion
     (Source => Asis.Declaration_Kinds,
      Target => a_nodes_h.Declaration_Kinds);

   function To_Definition_Kinds is new Unchecked_Conversion
     (Source => Asis.Definition_Kinds,
      Target => a_nodes_h.Definition_Kinds);

   function To_Expression_Kinds is new Unchecked_Conversion
     (Source => Asis.Expression_Kinds,
      Target => a_nodes_h.Expression_Kinds);

   function To_Association_Kinds is new Unchecked_Conversion
     (Source => Asis.Association_Kinds,
      Target => a_nodes_h.Association_Kinds);

   function To_Statement_Kinds is new Unchecked_Conversion
     (Source => Asis.Statement_Kinds,
      Target => a_nodes_h.Statement_Kinds);

   function To_Path_Kinds is new Unchecked_Conversion
     (Source => Asis.Path_Kinds,
      Target => a_nodes_h.Path_Kinds);

   function To_Clause_Kinds is new Unchecked_Conversion
     (Source => Asis.Clause_Kinds,
      Target => a_nodes_h.Clause_Kinds);

   function To_Operator_Kinds is new Unchecked_Conversion
     (Source => Asis.Operator_Kinds,
      Target => a_nodes_h.Operator_Kinds);

   function To_Declaration_Origins is new Unchecked_Conversion
     (Source => Asis.Declaration_Origins,
      Target => a_nodes_h.Declaration_Origins);

   function To_Mode_Kinds is new Unchecked_Conversion
     (Source => Asis.Mode_Kinds,
      Target => a_nodes_h.Mode_Kinds);

   function To_Subprogram_Default_Kinds is new Unchecked_Conversion
     (Source => Asis.Subprogram_Default_Kinds,
      Target => a_nodes_h.Subprogram_Default_Kinds);

   function To_Trait_Kinds is new Unchecked_Conversion
     (Source => Asis.Trait_Kinds,
      Target => a_nodes_h.Trait_Kinds);

   function To_Attribute_Kinds is new Unchecked_Conversion
     (Source => Asis.Attribute_Kinds,
      Target => a_nodes_h.Attribute_Kinds);

end a_nodes_h.Support;
