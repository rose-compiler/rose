package a_nodes_h.Support is

   package ICS renames Interfaces.C.Strings;

   -- Normally these records' fields would already have default values, but
   -- a_nodes_h.ads is generated from C, so they are here.

   Element_Union_Default : constant Element_Union :=
     (discr        => 0,
      dummy_member => 0);

   Element_Struct_Default : constant Element_Struct :=
     (id              => 0,
      kind            => Not_An_Element,
      enclosing_id    => 0,
      enclosing_kind  => Not_Enclosing,
      source_location => Interfaces.C.Strings.Null_Ptr,
      the_union       => Element_Union_Default);

   Context_Struct_Default : constant Context_Struct :=
     (name        => ICS.Null_Ptr,
      parameters  => ICS.Null_Ptr,
      debug_image => ICS.Null_Ptr);

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

end a_nodes_h.Support;
