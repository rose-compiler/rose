-- This is the main Asis_Tool_2 class.

private with Asis_Tool_2.Context;
private with Dot;
with A_Nodes;
with a_nodes_h;

package Asis_Tool_2.Tool is

   type Class is tagged limited private;

   procedure Process
     (This      : in out Class;
      File_Name : in     String);

   -- Call Process first:
   function Get_Nodes
     (This      : in out Class)
      return a_nodes_h.Node_List_Ptr;

private

   type Class is tagged limited -- Initialized
      record
         My_Context  : Asis_Tool_2.Context.Class; -- Initialized
         Graph       : Dot.Graphs.Access_Class; -- Initialized
         A_Nodes     : Standard.A_Nodes.Access_Class; -- Initialized
      end record;

end Asis_Tool_2.Tool;
