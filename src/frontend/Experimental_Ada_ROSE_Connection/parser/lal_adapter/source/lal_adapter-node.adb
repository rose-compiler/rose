with Generic_Logging;
with Langkit_Support.Slocs;
with Langkit_Support.Text;

package body Lal_Adapter.Node is
   package Slocs renames Langkit_Support.Slocs;
   package Text renames Langkit_Support.Text;

   ------------
   -- Exported:
   ------------
   procedure Process
     (This    : in out Class;
      Node    : in     LAL.Ada_Node'Class;
      --  Options : in     Options_Record;
      Outputs : in     Output_Accesses_Record)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      Sloc_Range_Image : constant string := Slocs.Image (Node.Sloc_Range);
      Kind_Image : constant String := LALCO.Ada_Node_Kind_Type'Image (Node.Kind);
      Debug_Text : constant String := Node.Debug_Text;
      use type LALCO.Ada_Node_Kind_Type; -- For "="
   begin -- Process
      This.Outputs := Outputs;
      --  Log ("Line" & Start_Line_Image & ": " & Kind_Image  & ": " & Debug_Text);
      --  if Node.Kind /= LALCO.Ada_Compilation_Unit then
      Log (Kind_Image & " " & Node.Kind_Name & " at " & Sloc_Range_Image);
      Log (LAL.Image(Node));
      case Node.Kind is
         when others => null;
      end case;

      --  end if;
   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Log_Exception (X);
         Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Process;

end Lal_Adapter.Node;
