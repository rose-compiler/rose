with Ada.Characters.Handling;
with Ada.Wide_Text_IO;
with Asis.Declarations;
with Asis.Elements;
with Asis.Iterator;

package body Asis_Tool_2.Element is

   type Traversal_State is tagged limited record
      Indent : Natural := 0;
   end record;


   function White_Space
     (This : in Traversal_State)
      return Wide_String is
   begin
      return (1 .. This.Indent => ' ');
   end White_Space;


   procedure Put_Indented_Line
     (This    : in Traversal_State;
      Message : in Wide_String) is
   begin
      Ada.Wide_Text_IO.Put_line (This.White_Space & Message);
   end Put_Indented_Line;


   -- Return the name image for declarations:
   function Name
     (Element : in Asis.Element)
      return Wide_String
   is
   begin
      case Asis.Elements.Element_Kind (Element) is
         when Asis.A_Defining_Name =>
            return '"' & Asis.Declarations.Defining_Name_Image (Element) & '"';
         when others =>
            return "";
      end case;
   end Name;


   procedure Pre_Op
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Traversal_State) is
      Kind : constant Wide_String := Ada.Characters.Handling.To_Wide_String
        (Asis.Elements.Element_Kind (Element)'Image);
   begin
      State.Put_Indented_Line ("(" & Kind & ": " & Name (Element));
      State.Indent := State.Indent + 1;
   end Pre_Op;


   procedure Post_Op
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Traversal_State) is
   begin
      State.Indent := State.Indent - 1;
      State.Put_Indented_Line (")");
   end Post_Op;


   -- Call Pre_Operation on ths element, call Traverse_Element on all children,
   -- then call Post_Operation on this element:
   procedure Traverse_Element is new
     Asis.Iterator.Traverse_Element
       (State_Information => Traversal_State,
        Pre_Operation     => Pre_Op,
        Post_Operation    => Post_Op);

   ------------
   -- EXPORTED:
   ------------
   procedure Process_Element_Tree (The_Element : Asis.Element) is
      Process_Control : Asis.Traverse_Control := Asis.Continue;
      Process_State   : Traversal_State; -- Automatically initialized
   begin
      Traverse_Element
        (Element => The_Element,
         Control => Process_Control,
         State   => Process_State);
   end Process_Element_Tree;

end Asis_Tool_2.Element;
