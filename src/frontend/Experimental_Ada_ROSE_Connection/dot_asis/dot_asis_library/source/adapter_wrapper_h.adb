with Ada.Text_IO;
with Interfaces.C;

with Asis_Adapter.Tool;
with a_nodes_h.Support;

package body adapter_wrapper_h is

   package anhS renames a_nodes_h.Support;

   ------------
   -- EXPORTED:
   ------------
   function adapter_wrapper
     (target_file_in               : in Interfaces.C.Strings.chars_ptr;
      gnat_home                    : in Interfaces.C.Strings.chars_ptr;
      output_dir                   : in Interfaces.C.Strings.chars_ptr
     )
      return a_nodes_h.Nodes_Struct
   is
      package ICE renames Interfaces.C.Extensions;
   begin
      return adapter_wrapper_with_flags
        (target_file_in               => target_file_in,
         gnat_home                    => gnat_home,
         output_dir                   => output_dir,
         process_predefined_units     => ICE.bool(False),
         process_implementation_units => ICE.bool(False),
         debug                        => ICE.bool(False));
   end adapter_wrapper;

   ------------
   -- EXPORTED:
   ------------
   function adapter_wrapper_with_flags
     (target_file_in               : in Interfaces.C.Strings.chars_ptr;
      gnat_home                    : in Interfaces.C.Strings.chars_ptr;
      output_dir                   : in Interfaces.C.Strings.chars_ptr;
      process_predefined_units     : in Interfaces.C.Extensions.bool;
      process_implementation_units : in Interfaces.C.Extensions.bool;
      debug                        : in Interfaces.C.Extensions.bool
     )
      return a_nodes_h.Nodes_Struct
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".adapter_wrapper_with_flags";
      procedure Log (Message : in String) is
      begin
         Ada.Text_Io.Put_Line (Module_Name & ":  " & Message);
      end;

      Target_File_In_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (target_file_in)));
      GNAT_Home_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (gnat_home)));
      Output_Dir_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (output_dir)));

      Compile_Succeeded : Boolean  := False;
      Tool              : Asis_Adapter.Tool.Class; -- Initialized
      Result            : a_nodes_h.Nodes_Struct := anhs.Default_Nodes_Struct;
   begin
      Log ("BEGIN");
      Tool.Process
        (File_Name                    => Target_File_In_String_Access.all,
         GNAT_Home                    => GNAT_Home_String_Access.all,
         Output_Dir                   => Output_Dir_String_Access.all,
         Process_Predefined_Units     => Boolean (process_predefined_units),
         Process_Implementation_Units => Boolean (process_implementation_units),
         Debug                        => Boolean (debug));
      Result := Tool.Get_Nodes;
      -- Can't take 'Image of an expression, so " + 1" below:
      Log ("Returning " & Result.Units.Next_Count'Image  & " + 1 Units.");
      Log ("Returning " & Result.Elements.Next_Count'Image  & " + 1 Elements.");
      Log ("END");
      return Result;
   end adapter_wrapper_with_flags;

end adapter_wrapper_h;
