with Asis;

with A_Nodes;
with Dot;

package Asis_Tool_2.Unit is

   type Class (Trace : Boolean := False) is tagged limited private; -- Initialized

   procedure Process
     (This    : in out Class;
      Unit    : in     Asis.Compilation_Unit;
      Outputs : in     Outputs_Record);

private

   type Class (Trace : Boolean := False) is tagged limited
      record
         -- Current, in-progress intermediate output products:
         -- Used when making dot edges to child nodes:
         Unit_ID   : a_nodes_h.Unit_ID := anhS.Invalid_Unit_ID;
         Dot_Node  : Dot.Node_Stmt.Class; -- Initialized
         Dot_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         A_Unit    : a_nodes_h.Unit_Struct := anhS.Default_Unit_Struct;
         -- I would like to just pass Outputs through and not store it in the
         -- object, since it is all pointers and we doesn't need to store their
         -- values between calls to Process_Element_Tree. Outputs has to go into
         -- Add_To_Dot_Label, though, so  we'll put it in the object and pass
         -- that:
         Outputs   : Outputs_Record; -- Initialized
      end record;

end Asis_Tool_2.Unit;
