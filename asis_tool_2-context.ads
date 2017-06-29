with Asis;

with A_Nodes;
with Dot;

package Asis_Tool_2.Context is

   type Class is tagged limited private;

   procedure Process
     (This    : in out Class;
      Graph   : in     Dot.Graphs.Access_Class;
      A_Nodes : in     Standard.A_Nodes.Access_Class);

private

   type Class is tagged limited -- Initialized
      record
         Asis_Context : Asis.Context; -- Initialized
         Graph        : Dot.Graphs.Access_Class; -- Initialized
         A_Nodes      : Standard.A_Nodes.Access_Class; -- Initialized
      end record;

   -- No children call this.  Just wanted to use object.operation in the body:
   -- Process all the compilation units (ADT files) in the context:
   procedure Process_Units
     (This    : in out Class;
      A_Nodes : in     Standard.A_Nodes.Access_Class);

end Asis_Tool_2.Context;
