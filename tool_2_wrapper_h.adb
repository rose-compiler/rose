with Asis.Extensions;
with Interfaces.C;

with Asis_Tool_2.Tool;

with a_nodes_h.Support;

package body tool_2_wrapper_h is

   function tool_2_wrapper
     (target_file_in : in  Interfaces.C.Strings.chars_ptr)
      return a_nodes_h.Node_List_Ptr
   is
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
      return Tool.Get_Nodes;
   end tool_2_wrapper;

end tool_2_wrapper_h;
