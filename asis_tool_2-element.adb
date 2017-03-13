with Ada.Characters.Handling;
with Ada.Wide_Text_IO;
with Asis.Clauses;
with Asis.Declarations;
with Asis.Elements;
with Asis.Expressions;
with Asis.Iterator;

package body Asis_Tool_2.Element is

   type Traversal_State is tagged limited record
      Indent_Level : Natural := 0;
   end record;


   procedure Indent (This : in out Traversal_State) is
   begin
      This.Indent_Level := This.Indent_Level + 1;
   end Indent;


   procedure Dedent (This : in out Traversal_State) is
   begin
      This.Indent_Level := This.Indent_Level - 1;
   end Dedent;


   function White_Space
     (This : in Traversal_State)
      return Wide_String is
   begin
      return (1 .. This.Indent_Level * 2 => ' ');
   end White_Space;


   procedure Put_Indented_Line
     (This    : in Traversal_State;
      Message : in Wide_String) is
   begin
      Ada.Wide_Text_IO.Put_Line (This.White_Space & Message);
   end Put_Indented_Line;


   procedure Put
     (This    : in Traversal_State;
      Message : in Wide_String) is
   begin
      Ada.Wide_Text_IO.Put (Message);
   end Put;


   procedure Put_Indent
     (This : in Traversal_State) is
   begin
      Ada.Wide_Text_IO.Put (This.White_Space);
   end Put_Indent;


   procedure New_Line
     (This : in Traversal_State) is
   begin
      Ada.Wide_Text_IO.New_Line;
   end New_Line;


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


   procedure Put_Expression_Info
     (Element : in Asis.Element;
      State   : in out Traversal_State)
   is
      Expression_Kind : constant Asis.Expression_Kinds :=
        Asis.Elements.Expression_Kind (Element);
   begin
      State.Put ("." & Asis.Expression_Kinds'Wide_Image(Expression_Kind));
      case Expression_Kind is
         when Asis.An_Identifier =>
            State.Put (" """ & Asis.Expressions.Name_Image (Element) & '"');
         when Asis.An_Operator_Symbol =>
            State.Put (' ' & Asis.Expressions.Name_Image (Element));
         when others =>
            null;
      end case;
    end Put_Expression_Info;


   procedure Put_With_Clause_Info
     (Element : in Asis.Element;
      State   : in out Traversal_State)
   is
   begin
      null;
   end Put_With_Clause_Info;


   procedure Put_Clause_Info
     (Element : in Asis.Element;
      State   : in out Traversal_State)
   is
     Clause_Kind : constant Asis.Clause_Kinds :=
       Asis.Elements.Clause_Kind (Element);
   begin
      State.Put ("." & Asis.Clause_Kinds'Wide_Image (Clause_Kind));
      case Clause_Kind is
         when Asis.Not_A_Clause =>
            null;
         when Asis.A_Use_Package_Clause =>
            null;
         when Asis.A_Use_Type_Clause =>
            null;
         when Asis.A_Use_All_Type_Clause =>
            null;
         when Asis.A_With_Clause =>
            Put_With_Clause_Info (Element, State);
         when Asis.A_Representation_Clause =>
            null;
         when Asis.A_Component_Clause =>
            null;
      end case;
   end Put_Clause_Info;



   procedure Put_Element_Line
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Traversal_State)
   is
      Kind : constant Wide_String := Ada.Characters.Handling.To_Wide_String
        (Asis.Elements.Element_Kind (Element)'Image);
   begin
      State.Put_Indent;
      State.Put(Kind);
      case Asis.Elements.Element_Kind (Element) is
         when Asis.Not_An_Element =>
            Null;
         when Asis.A_Pragma =>
            Null;
         when Asis.A_Defining_Name =>
            State.Put (" => " & Name (Element));
         when Asis.A_Declaration =>
            Null;
         when Asis.A_Definition =>
            Null;
         when Asis.An_Expression =>
            Put_Expression_Info (Element, State);
         when Asis.An_Association =>
            Null;
         when Asis.A_Statement =>
            Null;
         when Asis.A_Path =>
            Null;
         when Asis.A_Clause =>
            Put_Clause_Info (Element, State);
         when Asis.An_Exception_Handler =>
            null;
      end case;
      State.New_Line;
      State.Indent;
   end Put_Element_Line;


   procedure Finish_Element
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Traversal_State) is
   begin
      State.Dedent;
   end Finish_Element;


   -- Call Pre_Operation on ths element, call Traverse_Element on all children,
   -- then call Post_Operation on this element:
   procedure Traverse_Element is new
     Asis.Iterator.Traverse_Element
       (State_Information => Traversal_State,
        Pre_Operation     => Put_Element_Line,
        Post_Operation    => Finish_Element);

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
