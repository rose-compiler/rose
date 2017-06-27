with Ada.Text_IO; use Ada.Text_IO;
with Interfaces.C;

with Asis.Extensions;

with Asis_Tool_2.Tool;
with a_nodes_h.Support;

package body tool_2_wrapper_h is

   function tool_2_wrapper
     (target_file_in : in  interfaces.C.Strings.chars_ptr)
      return a_nodes_h.Element_Struct_Ptr
   is
      last_element_out : a_nodes_h.Element_Struct_Ptr := new
        a_nodes_h.Element_Struct'(a_nodes_h.Support.Element_Struct_Default);
      Target_File_In_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (target_file_in)));
      Compile_Succeeded : Boolean  := False;
      Tool              : Asis_Tool_2.Tool.Class; -- Initialized
   begin
      Asis.Extensions.Compile
        (Source_File  => Target_File_In_String_Access,
         Args         => (1..0 => null),
         Success      => Compile_Succeeded,
         Display_Call => True);
      if Compile_Succeeded then
         Tool.Process (Target_File_In_String_Access.all);
      end if;
      return last_element_out;
   end tool_2_wrapper;

end tool_2_wrapper_h;
