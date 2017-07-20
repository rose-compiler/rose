with Asis;

with Dot;
with A_Nodes;

package Asis_Tool_2.Element is

   type Class is tagged private; -- Initialized

   -- Process an element and all of its components:
   procedure Process_Element_Tree
     (This    : in out Class;
      Element : in     Asis.Element;
      Outputs : in     Output_Accesses_Record);

private

   -- Can't be limited because generic Asis.Iterator.Traverse_Element doesn't
   -- want limited state information:
   type Class is tagged -- Initialized
      record
         -- Current, in-progress intermediate output products.  These need to be
         -- turned into stacks if they are ever used in Post_Operation.  Now
         -- their usage ends at the end of Pre_Operation:
         Dot_Node  : Dot.Node_Stmt.Class; -- Initialized
         Dot_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         A_Element : a_nodes_h.Element_Struct := anhS.Default_Element_Struct;
      -- I would like to just pass Outputs through and not store it in the
      -- object, since it is all pointers and we doesn't need to store their
      -- values between calls to Process_Element_Tree. Outputs has to go into
      -- State_Information in the Traverse_Element instatiation, though, so
      -- we'll put it in the object and pass that:
         Outputs   : Output_Accesses_Record; -- Initialized
      end record;

   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String);
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String);

   -- Add <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String);

   -- Add attribute: Traversal="***NOT_IMPLEMENTED"
   procedure Add_Not_Implemented
     (This  : in out Class);

end Asis_Tool_2.Element;
