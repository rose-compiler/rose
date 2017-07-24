with Asis;

with A_Nodes;
with Dot;

package Asis_Tool_2.Unit is

   type Class (Trace : Boolean := False) is tagged limited private; -- Initialized

   procedure Process
     (This    : in out Class;
      Unit    : in     Asis.Compilation_Unit;
      Outputs : in     Output_Accesses_Record);

private

   type Class (Trace : Boolean := False) is tagged limited
      record
         Unit_ID   : A4G.A_Types.Unit_Id := A4G.A_Types.No_Unit_Id;
         Dot_Node  : Dot.Node_Stmt.Class; -- Initialized
         Dot_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         A_Unit    : a_nodes_h.Unit_Struct := anhS.Default_Unit_Struct;
         -- I would like to just pass Outputs through and not store it in the
         -- object, since it is all pointers and we doesn't need to store their
         -- values between calls to Process_Element_Tree. Outputs has to go into
         -- Add_To_Dot_Label, though, so  we'll put it in the object and pass
         -- that:
         Outputs   : Output_Accesses_Record; -- Initialized
      end record;

end Asis_Tool_2.Unit;
