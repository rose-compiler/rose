package a_nodes_h.Support is

   -- Normally these records' fields would already have default values, but
   -- a_nodes_h.ads is generated from C, so they are here.

   Element_Union_Default : constant Element_Union :=
     (discr => 0,
      no_element => 0);

   Element_Struct_Default : constant Element_Struct :=
     (id => 0,
      kind => Not_An_Element,
      enclosing_id => 0,
      enclosing_kind => Not_Enclosing,
      source_location => Interfaces.C.Strings.Null_Ptr,
      next => null,
      next_count => 0,
      element => Element_Union_Default);

end a_nodes_h.Support;
