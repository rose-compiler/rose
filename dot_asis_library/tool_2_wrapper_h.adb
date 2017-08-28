with Ada.Text_IO;
with Asis.Extensions;
with Interfaces.C;

with Asis_Tool_2.Tool;

package body tool_2_wrapper_h is

   function tool_2_wrapper
     (target_file_in : in  Interfaces.C.Strings.chars_ptr)
      return a_nodes_h.Node_List_Ptr
   is
      Target_File_In_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (target_file_in)));
      Compile_Succeeded            : Boolean  := False;
      Tool                         : Asis_Tool_2.Tool.Class; -- Initialized
      Result                       : a_nodes_h.Node_List_Ptr; -- Initialized
      procedure Log (Message : in String) is
      begin
         Ada.Text_Io.Put_Line ("tool_2_wrapper_h.tool_2_wrapper:  " & message);
      end;
   begin
      Log ("BEGIN - target_file_in => """ & Target_File_In_String_Access.all & """");
      Log ("Calling Asis.Extensions.Compile:");
      Asis.Extensions.Compile
        (Source_File  => Target_File_In_String_Access,
         Args         => (1..0 => null),
         Success      => Compile_Succeeded,
         Display_Call => True);
      if Compile_Succeeded then
         Log ("Asis.Extensions.Compile succeeded");
         Tool.Process
           (File_Name => Target_File_In_String_Access.all,
            Debug     => False);
         Result := Tool.Get_Nodes;
         -- Can't take 'Image of an expression, so:
         Log ("END.  Returning " & Result.Next_Count'Image  & " + 1 nodes.");
      else
         Log ("Asis.Extensions.Compile FAILED. NOT calling Tool.Process");
         Log ("END.  Returning null result.");
      end if;
      return Result;
   end tool_2_wrapper;

end tool_2_wrapper_h;
