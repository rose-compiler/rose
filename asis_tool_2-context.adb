with Asis.Ada_Environments;
with Asis.Compilation_Units;
with Asis.Exceptions;
with Asis.Errors;
with Asis.Implementation;
with GNAT.Directory_Operations;

with Asis_Tool_2.Unit;
with a_nodes_h.Support;

package body Asis_Tool_2.Context is

   procedure Create_And_Push_New_A_Node
     (Asis_Context : in Asis.Context;
      A_Nodes      : in Standard.A_Nodes.Access_Class)
   is
      Context : a_nodes_h.Context_Struct :=
        a_nodes_h.Support.Default_Context_Struct;
      Node    : a_nodes_h.Node_Struct :=
        a_nodes_h.Support.Default_Node_Struct;
      use Asis.Ada_Environments;
   begin
      Context.Name := To_Chars_Ptr (Name (Asis_Context));
      Context.Parameters := To_Chars_Ptr (Parameters (Asis_Context));
      Context.Debug_Image := To_Chars_Ptr (Debug_Image (Asis_Context));

      Node.Node_Kind := a_nodes_h.A_Context_Node;
      Node.the_union.context := Context;
      A_Nodes.Push (Node);
   end;

   procedure Process_Units
     (This    : in out Class;
      Outputs : in     Output_Accesses_Record) is
      use Asis.Exceptions;
      Asis_Units : Asis.Compilation_Unit_List :=
        Asis.Compilation_Units.Compilation_Units (This.Asis_Context);
   begin
      for Asis_Unit of Asis_Units loop
         declare
            Tool_Unit : Asis_Tool_2.Unit.Class;
         begin
            Tool_Unit.Process (Asis_Unit, Outputs);
         end;
      end loop;
   exception
      when Ex : ASIS_Inappropriate_Context          |
           ASIS_Inappropriate_Container        |
           ASIS_Inappropriate_Compilation_Unit |
           ASIS_Inappropriate_Element          |
           ASIS_Inappropriate_Line             |
           ASIS_Inappropriate_Line_Number      |
           ASIS_Failed                         =>
         Print_Exception_Info (Ex);
         Awti.Put_Line
           ("ASIS Error Status is " &
              Asis.Errors.Error_Kinds'Wide_Image (Asis.Implementation.Status));
         Awti.Put_Line
           ("ASIS Diagnosis is " & (Asis.Implementation.Diagnosis));
            Asis.Implementation.Set_Status (Asis.Errors.Not_An_Error);
         Awti.Put_Line ("Continuing...");

      when X : others =>
         Print_Exception_Info (X);
         Awti.Put_Line ("Continuing...");
   end Process_Units;

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This    : in out Class;
      Outputs : in     Output_Accesses_Record)
   is
      Directory : constant String := GNAT.Directory_Operations.Get_Current_Dir;
      procedure Begin_Environment is begin
         -- This just names the Context.  It does not control what it processes:
         Asis.Ada_Environments.Associate
           (This.Asis_Context,
            To_Wide_String (Directory));
         Asis.Ada_Environments.Open (This.Asis_Context);
         Trace_Put_Line ("Context info: " & Asis.Ada_Environments.Debug_Image
                         (This.Asis_Context));
      end;
      procedure End_Environment is begin
         Asis.Ada_Environments.Close (This.Asis_Context);
         Asis.Ada_Environments.Dissociate (This.Asis_Context);
      end;
   begin
      Begin_Environment;
      -- Call Begin_Environment first:
      Outputs.Graph.Set_ID
        ("""" & To_String (Asis.Ada_Environments.Name (This.Asis_Context)) & """");
      Create_And_Push_New_A_Node (This.Asis_Context, Outputs.A_Nodes);
      Process_Units (This, Outputs);
      End_Environment;
   end Process;

end Asis_Tool_2.Context;
