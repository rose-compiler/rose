with Interfaces.C.Extensions;
with Interfaces.C.Strings;
with a_nodes_h;

package tool_2_wrapper_h is

   function tool_2_wrapper
     (target_file_in               : in Interfaces.C.Strings.chars_ptr;
      gnat_home                    : in Interfaces.C.Strings.chars_ptr;
      output_dir                   : in Interfaces.C.Strings.chars_ptr
     )
      return a_nodes_h.Nodes_Struct;
   pragma Export (C, tool_2_wrapper);

   function tool_2_wrapper_with_flags
     (target_file_in               : in Interfaces.C.Strings.chars_ptr;
      gnat_home                    : in Interfaces.C.Strings.chars_ptr;
      output_dir                   : in Interfaces.C.Strings.chars_ptr;
      process_predefined_units     : in Interfaces.C.Extensions.bool;
      process_implementation_units : in Interfaces.C.Extensions.bool;
      debug                        : in Interfaces.C.Extensions.bool
     )
      return a_nodes_h.Nodes_Struct;
   pragma Export (C, tool_2_wrapper_with_flags);

private

   -- for debugging:
   Module_Name : constant String := "tool_2_wrapper_h";

end tool_2_wrapper_h;
