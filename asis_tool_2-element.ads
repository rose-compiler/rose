with Asis;

with Dot;

package Asis_Tool_2.Element is

   type Class is tagged private;

   -- Process an element and all of its components:
   procedure Process_Element_Tree  -- Initialized
     (This    : in out Class;
      Element : in     Asis.Element;
      Graph   : in     Dot.Graphs.Access_Class);

private

   -- Can't be limited because generic Asis.Iterator.Traverse_Element doesn't
   -- want limited state information:
   type Class is tagged -- Initialized
      record
         The_Element : Asis.Element; -- Initialized
         Text        : Indented_Text.Class; -- Initialized
         Graph       : Dot.Graphs.Access_Class; -- Initialized
      end record;

end Asis_Tool_2.Element;
