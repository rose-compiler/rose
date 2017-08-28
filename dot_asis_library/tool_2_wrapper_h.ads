with Interfaces.C.Strings;
with a_nodes_h;

package tool_2_wrapper_h is

   function tool_2_wrapper
     (target_file_in : in  interfaces.C.Strings.chars_ptr)
      return a_nodes_h.Node_List_Ptr;
   pragma Export (C, tool_2_wrapper);

end tool_2_wrapper_h;
