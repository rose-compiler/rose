-- This is the main Asis_Tool_2 class.

private with Asis_Tool_2.Context;
private with Dot;

package Asis_Tool_2.Tool is

   type Class is tagged limited private;

   procedure Process
     (This : in out Class);

private

   type Class is tagged limited -- Initialized
      record
         My_Context : Asis_Tool_2.Context.Class; -- Initialized
         Graph      : Dot.Graphs.Access_Class; -- Initialized
      end record;

end Asis_Tool_2.Tool;
