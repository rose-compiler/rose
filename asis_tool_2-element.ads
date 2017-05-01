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
         The_Element   : Asis.Element; -- Initialized
         Text          : Indented_Text.Class; -- Initialized
         Graph         : Dot.Graphs.Access_Class; -- Initialized
         Current_Node  : Dot.Node_Stmt.Class; -- Initialized
         Current_Label : Dot.HTML_Like_Labels.Class; -- Initialized
      end record;

   -- Add this info to the label, and print it if trace is on:
   procedure Add_To_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String);
   procedure Add_To_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String);

   -- Add attribute: Traversal="***NOT_IMPLEMENTED"
   procedure Add_Not_Implemented
     (This  : in out Class);

end Asis_Tool_2.Element;
