with Interfaces.C.Extensions;
with Interfaces.C.Strings;
with Interfaces.C.Pointers;
with a_nodes_h;

package adapter_wrapper_h is

   function adapter_wrapper
     (target_file_in               : in Interfaces.C.Strings.chars_ptr;
      gnat_home                    : in Interfaces.C.Strings.chars_ptr;
      AsisArgs                     : in Interfaces.C.Strings.chars_ptr;
      output_dir                   : in Interfaces.C.Strings.chars_ptr
     )
      return a_nodes_h.Nodes_Struct;
   pragma Export (C, adapter_wrapper);

   function adapter_wrapper_with_flags
     (target_file_in               : in Interfaces.C.Strings.chars_ptr;
      gnat_home                    : in Interfaces.C.Strings.chars_ptr;
      AsisArgs                     : in Interfaces.C.Strings.chars_ptr;
      output_dir                   : in Interfaces.C.Strings.chars_ptr;
      process_predefined_units     : in Interfaces.C.Extensions.bool;
      process_implementation_units : in Interfaces.C.Extensions.bool;
      debug                        : in Interfaces.C.Extensions.bool
     )
      return a_nodes_h.Nodes_Struct;
   pragma Export (C, adapter_wrapper_with_flags);

private

   -- for debugging:
   Module_Name : constant String := "adapter_wrapper_h";

end adapter_wrapper_h;
