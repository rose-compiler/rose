with Ada.Text_IO;
with Interfaces.C;

with a_nodes_h.Support;
with Generic_Logging;
with Lal_Adapter.Tool;

package body lal_adapter_wrapper_h is

   package anhS renames a_nodes_h.Support;

   ------------
   -- EXPORTED:
   ------------
   function lal_adapter_wrapper
     (project_file_name            : in Interfaces.C.Strings.chars_ptr;
      input_file_name              : in Interfaces.C.Strings.chars_ptr;
      output_dir_name              : in Interfaces.C.Strings.chars_ptr;
      process_predefined_units     : in Interfaces.C.Extensions.bool;
      process_implementation_units : in Interfaces.C.Extensions.bool;
      debug                        : in Interfaces.C.Extensions.bool
     )
      return a_nodes_h.Nodes_Struct
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".lal_adapter_wrapper";
      package Logging is new Generic_Logging (Module_Name); use Logging;

      Project_File_Name_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (project_file_name)));
      Input_File_Name_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (input_file_name)));
      Output_Dir_Name_String_Access : access String :=
        new String'(Interfaces.C.To_Ada
                    (Interfaces.C.Strings.Value (output_dir_name)));

      Tool              : Lal_Adapter.Tool.Class; -- Initialized
      Result            : a_nodes_h.Nodes_Struct := anhs.Default_Nodes_Struct;
      -- For = functions:
      use a_nodes_h;
   begin
      Log ("BEGIN");
      Tool.Process
        (Project_File_Name            => Project_File_Name_String_Access.all,
         Input_File_Name              => Input_File_Name_String_Access.all,
         Output_Dir_Name              => Output_Dir_Name_String_Access.all,
         Process_Predefined_Units     => Boolean (process_predefined_units),
         Process_Implementation_Units => Boolean (process_implementation_units),
         Debug                        => Boolean (debug));
      Result := Tool.Get_Nodes;
      if Result.Units = null then
         Log ("Returning NO Units.");
      else
         declare
             -- Can't take 'Image of an expression, so new object below:
             Count : constant Integer := Integer (Result.Units.Next_Count) + 1;
         begin
            Log ("Returning " & Count'Image  & " + 1 Units.");
         end;
      end if;
      if Result.Elements = null then
         Log ("Returning NO Elements.");
      else
         declare
             -- Can't take 'Image of an expression, so new object below:
             Count : constant Integer := Integer (Result.Elements.Next_Count) + 1;
         begin
            Log ("Returning " & Count'Image  & " + 1 Elements.");
         end;
      end if;
      Log ("END");
      return Result;
   exception
      when X: others =>
         -- Now we are probably leaving the Ada call stack and returning to C.
         -- This is our last chance to log the Ada exception info, so do that.
         Logging.Log_Exception (X);
         -- Reraise the exception so the caller knows there was one.
         Logging.Log ("Reraising exception.");
         raise;
   end lal_adapter_wrapper;

end lal_adapter_wrapper_h;
