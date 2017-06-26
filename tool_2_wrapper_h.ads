pragma Ada_2005;
pragma Style_Checks (Off);

with Interfaces.C; use Interfaces.C;
with Interfaces.C.Strings;
limited with a_nodes_h;

package tool_2_wrapper_h is

   procedure tool_2_wrapper
     (target_file_in : in     Interfaces.C.Strings.chars_ptr;
      elements_out   : access a_nodes_h.Element_Struct);  -- tool_2_wrapper.h:6
   pragma Export (CPP, tool_2_wrapper);

end tool_2_wrapper_h;
