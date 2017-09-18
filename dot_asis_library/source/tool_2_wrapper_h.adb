with Ada.Text_IO;
with Interfaces.C;

with Asis_Tool_2.Tool;

package body tool_2_wrapper_h is

   function tool_2_wrapper
     (target_file_in : in Interfaces.C.Strings.chars_ptr;
      gnat_home      : in Interfaces.C.Strings.chars_ptr;
      output_dir     : in Interfaces.C.Strings.chars_ptr)
      return a_nodes_h.Node_List_Ptr
   is
      Target_File_In_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (target_file_in)));
      GNAT_Home_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (gnat_home)));
      Output_Dir_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (output_dir)));
      Compile_Succeeded            : Boolean  := False;
      Tool                         : Asis_Tool_2.Tool.Class; -- Initialized
      Result                       : a_nodes_h.Node_List_Ptr; -- Initialized
      procedure Log (Message : in String) is
      begin
         Ada.Text_Io.Put_Line ("tool_2_wrapper_h.tool_2_wrapper:  " & Message);
      end;
   begin
      Log ("BEGIN");
      Tool.Process
        (File_Name  => Target_File_In_String_Access.all,
         GNAT_Home  => GNAT_Home_String_Access.all,
         Output_Dir => Output_Dir_String_Access.all,
         Debug      => False);
      Result := Tool.Get_Nodes;
      -- Can't take 'Image of an expression, so:
      Log ("END.  Returning " & Result.Next_Count'Image  & " + 1 nodes.");
      return Result;
   end tool_2_wrapper;

end tool_2_wrapper_h;
