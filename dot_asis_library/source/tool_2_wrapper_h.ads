with Interfaces.C.Strings;
with a_nodes_h;

package tool_2_wrapper_h is

   function tool_2_wrapper
     (target_file_in : in Interfaces.C.Strings.chars_ptr;
      gnat_home      : in Interfaces.C.Strings.chars_ptr;
      output_dir     : in Interfaces.C.Strings.chars_ptr)
      return a_nodes_h.Nodes_Struct;
   pragma Export (C, tool_2_wrapper);

end tool_2_wrapper_h;
