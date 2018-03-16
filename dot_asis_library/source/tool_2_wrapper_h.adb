with Ada.Text_IO;
with Interfaces.C;

with Asis_Tool_2.Tool;
with a_nodes_h.Support;

package body tool_2_wrapper_h is

   package anhS renames a_nodes_h.Support;

   function tool_2_wrapper
     (target_file_in : in Interfaces.C.Strings.chars_ptr;
      gnat_home      : in Interfaces.C.Strings.chars_ptr;
      output_dir     : in Interfaces.C.Strings.chars_ptr)
      return a_nodes_h.Nodes_Struct
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
      Result                       : a_nodes_h.Nodes_Struct :=
        anhs.Default_Nodes_Struct;
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
      -- Can't take 'Image of an expression, so " + 1" below:
      Log ("Returning " & Result.Units.Next_Count'Image  & " + 1 Units.");
      Log ("Returning " & Result.Elements.Next_Count'Image  & " + 1 Elements.");
      Log ("END");
      return Result;
   end tool_2_wrapper;

end tool_2_wrapper_h;
