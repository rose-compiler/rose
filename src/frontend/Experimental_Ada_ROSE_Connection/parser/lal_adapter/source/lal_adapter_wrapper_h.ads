with Interfaces.C.Extensions;
with Interfaces.C.Strings;
with a_nodes_h;

package lal_adapter_wrapper_h is

   function lal_adapter_wrapper
     (project_file_name            : in Interfaces.C.Strings.chars_ptr;
      input_file_name              : in Interfaces.C.Strings.chars_ptr;
      output_dir_name              : in Interfaces.C.Strings.chars_ptr;
      process_predefined_units     : in Interfaces.C.Extensions.bool;
      process_implementation_units : in Interfaces.C.Extensions.bool;
      debug                        : in Interfaces.C.Extensions.bool
     )
      return a_nodes_h.Nodes_Struct;
   pragma Export (C, lal_adapter_wrapper);

private

   -- for debugging:
   Module_Name : constant String := "lal_adapter_wrapper_h";

end lal_adapter_wrapper_h;
