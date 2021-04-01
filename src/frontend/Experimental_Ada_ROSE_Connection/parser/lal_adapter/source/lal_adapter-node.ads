with Libadalang.Analysis;
with Libadalang.Common;

package Lal_Adapter.Node is
   package LAL renames Libadalang.Analysis;
   package LALCO renames Libadalang.Common;

   type Class (Trace : Boolean := False) is tagged limited private; -- Initialized

   procedure Process
     (This    : in out Class;
      Node    : in     LAL.Ada_Node'Class;
      --  Options : in     Options_Record;
      Outputs : in     Output_Accesses_Record);

private
   -- For debuggng:
   Parent_Name : constant String := Module_Name;
   Module_Name : constant String := Parent_Name & ".Node";

   type Class (Trace : Boolean := False) is tagged limited -- Initialized
      record
         -- Current, in-progress intermediate output products:
         -- Used when making dot edges to child nodes:
         Dot_Node  : Dot.Node_Stmt.Class; -- Initialized
         Dot_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         A_Node    : a_nodes_h.Unit_Struct := anhS.Default_Unit_Struct;
         -- I would like to just pass Outputs through and not store it in the
         -- object, since it is all pointers and we don't need to store their
         -- values between calls to Traverse. Outputs has to go into
         -- Add_To_Dot_Label, though, so we'll put it in the object and pass
         -- the object to Add_To_Dot_Label:
         Outputs   : Output_Accesses_Record; -- Initialized
      end record;
end Lal_Adapter.Node;
