with Asis;

with A_Nodes;
with Dot;

package Asis_Tool_2.Context is

   type Class is tagged limited private;

   procedure Process
     (This        : in out Class;
      Graph       : in     Dot.Graphs.Access_Class;
      A_Node_List : in     A_Nodes.Access_Class);

private

   type Class is tagged limited -- Initialized
      record
         Asis_Context : Asis.Context; -- Initialized
         Graph        : Dot.Graphs.Access_Class; -- Initialized
         A_Node_List  : A_Nodes.Access_Class; -- Initialized
      end record;

   -- Process all the compilation units (ADT files) in the context:
   procedure Process_Units (This : in out Class);

end Asis_Tool_2.Context;
