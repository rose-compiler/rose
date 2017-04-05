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


   package Pre_Children is

      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class);

   end Pre_Children;

   package body Pre_Children is

      procedure Process_Pragma
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Pragma_Kind : Asis.Pragma_Kinds :=
           Asis.Elements.Pragma_Kind (Element);
      begin
         --        A_Pragma              -> Pragma_Kinds
         --
         State.Add_Attribute
           (Name => "Pragma_Kind",
            Value => Pragma_Kind'Image);
      end Process_Pragma;

      procedure Process_Defining_Name
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Defining_Name_Kind : Asis.Defining_Name_Kinds :=
           Asis.Elements.Defining_Name_Kind (Element);
      begin
         --        A_Defining_Name       -> Defining_Name_Kinds
         --                                         -> Operator_Kinds
         State.Add_Attribute (Name => "Defining_Name_Kind",
                              Value => Defining_Name_Kind'Image);
         State.Add_Attribute (Name => "Name",
                              Value => Name (Element));
         case Defining_Name_Kind is
            when Asis.A_Defining_Operator_Symbol =>
               State.Add_Attribute (Name => "Operator_Kind",
                                    Value => Asis.Elements.Operator_Kind (Element)'Image);
            when others => null;
         end case;
      end Process_Defining_Name;

      procedure Process_Declaration
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Declaration_Kind : Asis.Declaration_Kinds :=
           Asis.Elements.Declaration_Kind (Element);
         -- Tired of typing "Asis." in front of enum values:
         use Asis;
      begin
         --        A_Declaration         -> Declaration_Kinds
         --                                         -> Declaration_Origin
         --                                         -> Mode_Kinds
         --                                         -> Subprogram_Default_Kinds
         State.Add_Attribute (Name => "Declaration_Kind",
                              Value => Declaration_Kind'Image);
         State.Add_Attribute (Name => "Declaration_Origin",
                              Value => Asis.Elements.Declaration_Origin (Element)'Image);
         case Declaration_Kind is
            when A_Parameter_Specification |
                 A_Formal_Object_Declaration =>
               State.Add_Attribute (Name => "Mode_Kind",
                                    Value => Asis.Elements.Mode_Kind (Element)'Image);
            when others =>
               null;
         end case;
         case Declaration_Kind is
            when A_Formal_Function_Declaration |
                 A_Formal_Procedure_Declaration =>
               State.Add_Attribute (Name => "Subprogram_Default_Kind",
                                    Value => Asis.Elements.Default_Kind (Element)'Image);
            when others =>
               null;
         end case;
         case Declaration_Kind is
            when A_Private_Type_Declaration |
                 A_Private_Extension_Declaration |
                 A_Variable_Declaration |
                 A_Constant_Declaration |
                 A_Deferred_Constant_Declaration |
                 A_Discriminant_Specification |
                 A_Loop_Parameter_Specification |
                 A_Generalized_Iterator_Specification |
                 An_Element_Iterator_Specification |
                 A_Procedure_Declaration |
                 A_Function_Declaration =>
               State.Add_Attribute (Name => "Trait_Kind",
                                    Value => Asis.Elements.Trait_Kind (Element)'Image);
            when others =>
               null;
         end case;
      end Process_Declaration;

      procedure Process_Definition
        (Element : in     Asis.Element;
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
         pragma Compile_Time_Warning (Standard.True, "Asis_Tool_2.Element.Pre_Children.Process_Definition unimplemented");
--           raise Program_Error with "Unimplemented procedure Asis_Tool_2.Element.Pre_Children.Process_Definition";
      end Process_Definition;

      procedure Process_Expression
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Expression_Kind : constant Asis.Expression_Kinds :=
           Asis.Elements.Expression_Kind (Element);
      begin
         --        An_Expression         -> Expression_Kinds
         --                                         -> Operator_Kinds
         --                                         -> Attribute_Kinds
         --
         State.Add_Attribute ("Expression_Kind", Expression_Kind'Image);
         case Expression_Kind is
            when Asis.An_Operator_Symbol =>
               State.Add_Attribute ("Operator_Kind",
                                    Asis.Elements.Operator_Kind (Element)'Image);
            when Asis.An_Attribute_Reference =>
               State.Add_Attribute ("Attribute_Kind",
                                    Asis.Elements.Attribute_Kind (Element)'Image);
            when others =>
               null;
         end case;
         case Expression_Kind is
            when Asis.An_Identifier |
                 Asis.An_Operator_Symbol |
                 Asis.A_Character_Literal |
                 Asis.An_Enumeration_Literal =>
               State.Add_Attribute ("Name_Image",
                                    '"' & Asis.Expressions.Name_Image (Element) & '"');
            when others =>
               null;
         end case;
      end Process_Expression;

      procedure Process_Association
        (Element : in     Asis.Element;
         State   : in out Class)
      is
      begin
         --        An_Association        -> Association_Kinds
         --
         pragma Compile_Time_Warning (Standard.True, "Asis_Tool_2.Element.Pre_Children.Process_Association unimplemented");
--           raise Program_Error with "Unimplemented procedure Asis_Tool_2.Element.Pre_Children.Process_Association";
      end Process_Association;

      procedure Process_Statement
        (Element : in     Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Statement           -> Statement_Kinds
         --
         pragma Compile_Time_Warning (Standard.True, "Asis_Tool_2.Element.Pre_Children.Process_Statement unimplemented");
--           raise Program_Error with "Unimplemented procedure Asis_Tool_2.Element.Pre_Children.Process_Statement";
      end Process_Statement;

      procedure Process_Path
        (Element : in     Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Path                -> Path_Kinds
         --
         pragma Compile_Time_Warning (Standard.True, "Asis_Tool_2.Element.Pre_Children.Process_Path unimplemented");
--           raise Program_Error with "Unimplemented procedure Asis_Tool_2.Element.Pre_Children.Process_Path";
      end Process_Path;

      procedure Process_With_Clause
        (Element : in Asis.Element;
         State   : in out Class)
      is
      begin
         pragma Compile_Time_Warning (Standard.True, "Asis_Tool_2.Element.Pre_Children.Process_With_Clause unimplemented");
--           raise Program_Error with "Unimplemented procedure Asis_Tool_2.Element.Pre_Children.Process_With_Clause";
      end Process_With_Clause;

      procedure Process_Clause
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Clause_Kind : constant Asis.Clause_Kinds :=
           Asis.Elements.Clause_Kind (Element);
      begin
         --        A_Clause              -> Clause_Kinds
         --                                         -> Representation_Clause_Kinds
         State.Add_Attribute ("Clause_Kind", Clause_Kind'Image);
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
            Process_With_Clause (Element, State);
         when Asis.A_Representation_Clause =>
            null;
         when Asis.A_Component_Clause =>
            null;
         end case;
      end Process_Clause;

      procedure Process_Exception_Handler
        (Element : in     Asis.Element;
         State   : in out Class)
      is
      begin
         --        An_Exception_Handler
         pragma Compile_Time_Warning (Standard.True, "Asis_Tool_2.Element.Pre_Children.Process_Pragma unimplemented");
--           raise Program_Error with "Unimplemented procedure Asis_Tool_2.Element.Pre_Children.Process_Pragma";
      end Process_Exception_Handler;

      ------------
      -- EXPORTED:
      ------------
      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class)
      is
         Element_Kind : constant Asis.Element_Kinds :=
           Asis.Elements.Element_Kind (Element);
         New_Node : Dot.Node_Stmt.Class; -- Initialized
      begin
         State.Current_Node := New_Node;
         State.Add_Attribute ("Element_Kind", Element_Kind'Image);
         case Element_Kind is
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
         State.Text.End_Line;
         State.Text.Indent;
         State.Graph.Append_Stmt (new Dot.Node_Stmt.Class'(State.Current_Node));
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

   -----------
   -- PRIVATE:
   -----------
   procedure Add_Attribute
     (This  : in out Class;
      Name  : in     String;
      Value : in     String)
   is
   begin
      This.Text.Put_Indented_Line (Name & " => """ & Value & """");
      This.Current_Node.Attrs.Add_Assign_To_First_Attr
        (Name  => Name,
         Value => Value);
   end;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_Attribute
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String)
   is
   begin
      This.Add_Attribute (Name, To_String (Value));
   end;



end Asis_Tool_2.Element;
