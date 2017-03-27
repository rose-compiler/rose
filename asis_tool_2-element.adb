with Ada.Characters.Handling;
with Ada.Wide_Text_IO;
with Asis.Clauses;
with Asis.Declarations;
with Asis.Elements;
with Asis.Expressions;
with Asis.Iterator;

package body Asis_Tool_2.Element is

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
      State   : in out Class)
   is
      Expression_Kind : constant Asis.Expression_Kinds :=
        Asis.Elements.Expression_Kind (Element);
   begin
      State.Text.Put ("." & Asis.Expression_Kinds'Wide_Image(Expression_Kind));
      case Expression_Kind is
         when Asis.An_Identifier =>
            State.Text.Put (" """ & Asis.Expressions.Name_Image (Element) & '"');
         when Asis.An_Operator_Symbol =>
            State.Text.Put (' ' & Asis.Expressions.Name_Image (Element));
         when others =>
            null;
      end case;
    end Put_Expression_Info;


   procedure Put_With_Clause_Info
     (Element : in Asis.Element;
      State   : in out Class)
   is
   begin
      null;
   end Put_With_Clause_Info;


   package Pre_Children is

      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class);

   end Pre_Children;

   package body Pre_Children is

      procedure Process_Pragma
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Pragma              -> Pragma_Kinds
         --
         Null;
      end Process_Pragma;

      procedure Process_Defining_Name
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Defining_Name       -> Defining_Name_Kinds
         --                                         -> Operator_Kinds
         State.Text.Put (" => " & Name (Element));
      end Process_Defining_Name;

      procedure Process_Declaration
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Declaration         -> Declaration_Kinds
         --                                         -> Declaration_Origin
         --                                         -> Mode_Kinds
         --                                         -> Subprogram_Default_Kinds
         Null;
      end Process_Declaration;

      procedure Process_Definition
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Definition          -> Definition_Kinds
         --                                         -> Trait_Kinds
         --                                         -> Type_Kinds
         --                                         -> Formal_Type_Kinds
         --                                         -> Access_Type_Kinds
         --                                         -> Root_Type_Kinds
         --                                         -> Constraint_Kinds
         --                                         -> Discrete_Range_Kinds
         Null;
      end Process_Definition;

      procedure Process_Expression
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        An_Expression         -> Expression_Kinds
         --                                         -> Operator_Kinds
         --                                         -> Attribute_Kinds
         --
         Put_Expression_Info (Element, State);
      end Process_Expression;

      procedure Process_Association
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        An_Association        -> Association_Kinds
         --
         Null;
      end Process_Association;

      procedure Process_Statement
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Statement           -> Statement_Kinds
         --
         Null;
      end Process_Statement;

      procedure Process_Path
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Path                -> Path_Kinds
         --
         Null;
      end Process_Path;

      procedure Process_With_Clause
        (Element : in Asis.Element;
         State   : in out Class)
      is
      begin
         null;
      end Process_With_Clause;

      procedure Process_Clause
        (Element :        Asis.Element;
         State   : in out Class)
      is
         Clause_Kind : constant Asis.Clause_Kinds :=
           Asis.Elements.Clause_Kind (Element);
      begin
         --        A_Clause              -> Clause_Kinds
         --                                         -> Representation_Clause_Kinds
         State.Text.Put ("." & Asis.Clause_Kinds'Wide_Image (Clause_Kind));
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
      end Process_Clause;

      procedure Process_Exception_Handler
        (Element :        Asis.Element;
         State   : in out Class)
      is
      begin
         --        An_Exception_Handler
         null;
      end Process_Exception_Handler;

      ------------
      -- EXPORTED:
      ------------
      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class)
      is
         Kind : constant Wide_String := Ada.Characters.Handling.To_Wide_String
           (Asis.Elements.Element_Kind (Element)'Image);
      begin
         State.Text.Put_Indent;
         State.Text.Put(Kind);
         case Asis.Elements.Element_Kind (Element) is
         when Asis.Not_An_Element =>
            Null;
         when Asis.A_Pragma =>
            Process_Pragma (Element, State);
         when Asis.A_Defining_Name =>
            Process_Defining_Name (Element, State);
         when Asis.A_Declaration =>
            Process_Declaration (Element, State);
         when Asis.A_Definition =>
            Process_Definition (Element, State);
         when Asis.An_Expression =>
            Process_Expression (Element, State);
         when Asis.An_Association =>
            Process_Association (Element, State);
         when Asis.A_Statement =>
            Process_Statement (Element, State);
         when Asis.A_Path =>
            Process_Path (Element, State);
         when Asis.A_Clause =>
            Process_Clause (Element, State);
         when Asis.An_Exception_Handler =>
            Process_Exception_Handler (Element, State);
         end case;
         State.Text.New_Line;
         State.Text.Indent;
      end Process_Element;

   end Pre_Children;


   package Post_Children is

      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class);

   end Post_Children;

   package body Post_Children is

      ------------
      -- EXPORTED:
      ------------
      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class) is
      begin
         -- TODO: Add edges?
         State.Text.Dedent;
      end Process_Element;

   end Post_Children;


   -- Call Pre_Operation on ths element, call Traverse_Element on all children,
   -- then call Post_Operation on this element:
   procedure Traverse_Element is new
     Asis.Iterator.Traverse_Element
       (State_Information => Class,
        Pre_Operation     => Pre_Children.Process_Element,
        Post_Operation    => Post_Children.Process_Element);

   ------------
   -- EXPORTED:
   ------------
   procedure Process_Element_Tree
     (This    : in out Class;
      Element : in     Asis.Element;
      Graph   : in     Dot.Graphs.Access_Class)
   is
      Process_Control : Asis.Traverse_Control := Asis.Continue;
   begin
      This.The_Element := Element;
      This.Graph := Graph;
      Traverse_Element
        (Element => Element,
         Control => Process_Control,
         State   => This);
   end Process_Element_Tree;

end Asis_Tool_2.Element;
