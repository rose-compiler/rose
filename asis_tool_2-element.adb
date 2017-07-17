with Asis.Clauses;
with Asis.Compilation_Units;
with Asis.Declarations;
with Asis.Definitions;
with Asis.Elements;
with Asis.Expressions;
with Asis.Iterator;
with Asis.Statements;
-- GNAT-specific:
with Asis.Set_Get;
with Asis.Text;
with Types;

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


   procedure Add_Element_List
     (This           : in out Class;
      Elements_In    : in     Asis.Element_List;
      Dot_Label_Name : in     String;
      List_Out       :    out a_nodes_h.Element_List)
   is
      Element_Count : constant Natural :=
        Elements_In'Length;
      IDs : anhS.Element_ID_Array_Access := new
        anhS.Element_ID_Array (1 .. Element_Count);
      IDs_Index : Positive := IDs'First;
   begin
      for Element of Elements_In loop
         declare
            Element_ID : constant Types.Node_ID :=
              Asis.Set_Get.Node (Element);
         begin
            IDs (IDs_Index) := Interfaces.C.int (Element_ID);
            This.Add_To_Dot_Label
              (Dot_Label_Name & " (" & IDs_Index'Image & ")",
               To_String (Element_ID));
            IDs_Index := IDs_Index + 1;
         end;
      end loop;
      List_Out :=
        (length => Interfaces.C.int(Element_Count),
         IDs    => anhS.To_Element_ID_Ptr (IDs));
   end Add_Element_List;

   function Add_Trait_Kind
     (Element : in     Asis.Element;
      State   : in out Class)
            return a_nodes_h.Trait_Kinds
   is
      Trait_Kind : Asis.Trait_Kinds := Asis.Elements.Trait_Kind (Element);
   begin
      State.Add_To_Dot_Label
        ("Trait_Kind", Trait_Kind'Image);
      return a_nodes_h.Support.To_Trait_Kinds (Trait_Kind);
   end;

   package Pre_Children is

      procedure Process_Element
        (Element : in     Asis.Element;
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
         State.Add_To_Dot_Label
           (Name => "Pragma_Kind",
            Value => Pragma_Kind'Image);
         State.Add_Not_Implemented;
      end Process_Pragma;

      procedure Process_Defining_Name
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Defining_Name_Kind : Asis.Defining_Name_Kinds :=
           Asis.Elements.Defining_Name_Kind (Element);
         use all type Asis.Defining_Name_Kinds;
      begin
         --        A_Defining_Name       -> Defining_Name_Kinds
         --                                         -> Operator_Kinds
         State.Add_To_Dot_Label (Name => "Defining_Name_Kind",
                              Value => Defining_Name_Kind'Image);
         State.Add_To_Dot_Label (Name => "Name",
                              Value => Name (Element));
         case Defining_Name_Kind is
            when Not_A_Defining_Name =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Defining_Name called with: " &
                 Defining_Name_Kind'Image;
            when A_Defining_Operator_Symbol =>
               State.Add_To_Dot_Label (Name => "Operator_Kind",
                                    Value => Asis.Elements.Operator_Kind (Element)'Image);
            when others =>
               State.Add_Not_Implemented;
         end case;
      end Process_Defining_Name;

      procedure Process_Declaration
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Declaration_Kind : Asis.Declaration_Kinds :=
           Asis.Elements.Declaration_Kind (Element);
          A_Declaration : a_nodes_h.Declaration_Struct :=
           a_nodes_h.Support.Default_Declaration_Struct;
         use all type Asis.Declaration_Kinds;
      begin
         --        A_Declaration         -> Declaration_Kinds
         --                                         -> Declaration_Origin
         --                                         -> Mode_Kinds
         --                                         -> Subprogram_Default_Kinds
         State.Add_To_Dot_Label (Name => "Declaration_Kind",
                              Value => Declaration_Kind'Image);
         State.Add_To_Dot_Label (Name => "Declaration_Origin",
                              Value => Asis.Elements.Declaration_Origin (Element)'Image);
         case Declaration_Kind is
            when A_Parameter_Specification |
                 A_Formal_Object_Declaration =>
               State.Add_To_Dot_Label (Name => "Mode_Kind",
                                    Value => Asis.Elements.Mode_Kind (Element)'Image);
            when others =>
               null;
         end case;
         case Declaration_Kind is
            when A_Formal_Function_Declaration |
                 A_Formal_Procedure_Declaration =>
               State.Add_To_Dot_Label (Name => "Subprogram_Default_Kind",
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
                 A_Function_Declaration |
                 An_Object_Renaming_Declaration |
                 A_Formal_Object_Declaration |
                 A_Formal_Procedure_Declaration |
                 A_Formal_Function_Declaration =>
               State.Add_To_Dot_Label (Name => "Trait_Kind",
                                    Value => Asis.Elements.Trait_Kind (Element)'Image);
               A_Declaration.Trait_Kind := Add_Trait_Kind (Element, State);
            when others =>
               null;
         end case;
      end Process_Declaration;

      -- TODO: Process_Formal_Type_Definition?

      procedure Process_Type_Definition
        (State        : in out Class;
         Element      : in     Asis.Element;
         A_Definition : in out a_nodes_h.Definition_Struct)
      is
         Type_Kind : constant Asis.Type_Kinds :=
           Asis.Elements.Type_Kind (Element);

         procedure Add_Parent_Subtype_Indication is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Parent_Subtype_Indication
                                 (Element));
         begin
            State.Add_To_Dot_Label ("Parent_Subtype_Indication", To_String (ID));
            A_Definition.Parent_Subtype_Indication := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Record_Definition is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Record_Definition (Element));
         begin
            State.Add_To_Dot_Label ("Record_Definition", To_String (ID));
            A_Definition.Record_Definition := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Implicit_Inherited_Declarations is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Definitions.Implicit_Inherited_Declarations (Element),
               Dot_Label_Name => "Implicit_Inherited_Declarations",
               List_Out       => A_Definition.Implicit_Inherited_Declarations);
         end;

         procedure Add_Implicit_Inherited_Subprograms is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Definitions.Implicit_Inherited_Subprograms (Element),
               Dot_Label_Name => "Implicit_Inherited_Subprograms",
               List_Out       => A_Definition.Implicit_Inherited_Subprograms);
         end;

         procedure Add_Corresponding_Parent_Subtype is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Corresponding_Parent_Subtype
                                 (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Parent_Subtype", To_String (ID));
            A_Definition.Corresponding_Parent_Subtype := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Root_Type is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Corresponding_Root_Type
                                 (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Root_Type", To_String (ID));
            A_Definition.Corresponding_Root_Type := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Type_Structure is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Corresponding_Type_Structure
                                 (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Type_Structure", To_String (ID));
            A_Definition.Corresponding_Type_Structure := a_nodes_h.Node_ID (ID);
         end;

         use all type Asis.Type_Kinds;
      begin -- Process_Type_Definition
         State.Add_To_Dot_Label ("Type_Kind", Type_Kind'Image);
         A_Definition.Type_Kind := anhS.To_Type_Kinds (Type_Kind);
         case Type_Kind is
            when Not_A_Type_Definition =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Definition.Process_Type_Definition called with: " &
                 Type_Kind'Image;
            when A_Derived_Type_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               Add_Parent_Subtype_Indication;
               Add_Implicit_Inherited_Declarations;
               Add_Implicit_Inherited_Subprograms;
               Add_Corresponding_Parent_Subtype;
               Add_Corresponding_Root_Type;
               Add_Corresponding_Type_Structure;
            when A_Derived_Record_Extension_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               Add_Parent_Subtype_Indication;
               Add_Record_Definition;
               Add_Implicit_Inherited_Declarations;
               Add_Implicit_Inherited_Subprograms;
               Add_Corresponding_Parent_Subtype;
               Add_Corresponding_Root_Type;
               Add_Corresponding_Type_Structure;
            when An_Enumeration_Type_Definition |
                 A_Signed_Integer_Type_Definition |
                 A_Modular_Type_Definition |
                 A_Root_Type_Definition |
                 A_Floating_Point_Definition |
                 An_Ordinary_Fixed_Point_Definition |
                 A_Decimal_Fixed_Point_Definition |
                 An_Unconstrained_Array_Definition |
                 A_Constrained_Array_Definition =>
               State.Add_Not_Implemented;
            when A_Record_Type_Definition |
                 A_Tagged_Record_Type_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               Add_Record_Definition;
            when An_Interface_Type_Definition =>
               State.Add_Not_Implemented;
            when An_Access_Type_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               State.Add_Not_Implemented;
         end case;
      end Process_Type_Definition;


      procedure Process_Constraint
        (State        : in out Class;
         Element      : in     Asis.Element;
         A_Definition : in out a_nodes_h.Definition_Struct)
      is
         Constraint_Kind : constant Asis.Constraint_Kinds :=
           Asis.Elements.Constraint_Kind (Element);

         procedure Add_Digits_Expression is
         begin
            State.Add_Not_Implemented;
         end;

         procedure Add_Delta_Expression is
         begin
            State.Add_Not_Implemented;
         end;

         procedure Add_Real_Range_Constraint is
         begin
            State.Add_Not_Implemented;
         end;

         procedure Add_Lower_Bound is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Lower_Bound (Element));
         begin
            State.Add_To_Dot_Label ("Lower_Bound", To_String (ID));
            A_Definition.Lower_Bound := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Upper_Bound is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Upper_Bound (Element));
         begin
            State.Add_To_Dot_Label ("Upper_Bound", To_String (ID));
            A_Definition.Upper_Bound := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Range_Attribute is
         begin
            State.Add_Not_Implemented;
         end;

         procedure Add_Discrete_Ranges is
         begin
            State.Add_Not_Implemented;
         end;

         procedure Add_Discriminant_Associations is
         begin
            State.Add_Not_Implemented;
         end;

         use all type Asis.Constraint_Kinds;
      begin
         State.Add_To_Dot_Label ("Constraint_Kind", Constraint_Kind'Image);
         A_Definition.Constraint_Kind := anhS.To_Constraint_Kinds (Constraint_Kind);
         case Constraint_Kind is
            when Not_A_Constraint =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Definition.Process_Constraint called with: " &
                 Constraint_Kind'Image;
            when A_Range_Attribute_Reference =>
               Add_Range_Attribute;
            when A_Simple_Expression_Range =>
               Add_Lower_Bound;
               Add_Upper_Bound;
            when A_Digits_Constraint =>
               Add_Digits_Expression;
               Add_Real_Range_Constraint;
            when A_Delta_Constraint =>
               Add_Delta_Expression;
               Add_Real_Range_Constraint;
            when An_Index_Constraint =>
               Add_Discrete_Ranges;
            when A_Discriminant_Constraint =>
               Add_Discriminant_Associations;
         end case;
      end Process_Constraint;


      procedure Process_Component_Definition
        (State        : in out Class;
         Element      : in     Asis.Element;
         A_Definition : in out a_nodes_h.Definition_Struct)
      is
         Constraint_Kind : constant Asis.Constraint_Kinds :=
           Asis.Elements.Constraint_Kind (Element);
      begin
         State.Add_Not_Implemented;
      end Process_Component_Definition;

      procedure Process_Definition
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Definition_Kind : Asis.Definition_Kinds :=
           Asis.Elements.Definition_Kind (Element);
         A_Definition : a_nodes_h.Definition_Struct :=
           a_nodes_h.Support.Default_Definition_Struct;

         procedure Add_Component_Subtype_Indication is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Component_Subtype_Indication
                                 (Element));
         begin
            State.Add_To_Dot_Label ("Component_Subtype_Indication", To_String (ID));
            A_Definition.Component_Subtype_Indication := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Component_Definition_View is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Definitions.Component_Definition_View (Element));
         begin
            State.Add_To_Dot_Label ("Component_Definition_View", To_String (ID));
            A_Definition.Component_Definition_View := a_nodes_h.Node_ID (ID);
         end;

         use all type Asis.Definition_Kinds;
      begin -- Process_Definition
         State.Add_To_Dot_Label ("Definition_Kind", Definition_Kind'Image);
         A_Definition.Definition_Kind := anhS.To_Definition_Kinds (Definition_Kind);

         case Definition_Kind is
            when Not_A_Definition =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Definition called with: " &
                 Definition_Kind'Image;
            when A_Type_Definition =>
               Process_Type_Definition (State, Element, A_Definition);
            when A_Subtype_Indication =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               State.Add_Not_Implemented;
            when A_Constraint =>
               Process_Constraint (State, Element, A_Definition);
            when A_Component_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               Add_Component_Subtype_Indication;
               Add_Component_Definition_View;
            when A_Discrete_Subtype_Definition =>
               -- Discrete_Range_Kinds
               State.Add_Not_Implemented;
            when A_Discrete_Range =>
               -- Discrete_Range_Kinds
               State.Add_Not_Implemented;
            when An_Unknown_Discriminant_Part =>
               null;
            when A_Known_Discriminant_Part =>
               State.Add_Not_Implemented;
            when A_Record_Definition =>
               State.Add_Not_Implemented;
            when A_Null_Record_Definition =>
               null;
            when A_Null_Component =>
               null;
            when A_Variant_Part =>
               State.Add_Not_Implemented;
            when A_Variant =>
               State.Add_Not_Implemented;
            when An_Others_Choice =>
               State.Add_Not_Implemented;
            when An_Access_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               -- Access_Definition_Kinds
               State.Add_Not_Implemented;
            when A_Private_Type_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               State.Add_Not_Implemented;
            when A_Tagged_Private_Type_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               State.Add_Not_Implemented;
            when A_Private_Extension_Definition =>
               A_Definition.Trait_Kind := Add_Trait_Kind (Element, State);
               State.Add_Not_Implemented;
            when A_Task_Definition =>
               State.Add_Not_Implemented;
            when A_Protected_Definition =>
               State.Add_Not_Implemented;
            when A_Formal_Type_Definition =>
               -- Formal_Type_Kinds
               -- some Trait_Kinds
               State.Add_Not_Implemented;
            when An_Aspect_Specification =>
               State.Add_Not_Implemented;
         end case;

         --        A_Definition          -> Definition_Kinds
         --                                         -> Trait_Kinds
         --                                         -> Type_Kinds
         --                                         -> Formal_Type_Kinds
         --                                         -> Access_Type_Kinds
         --                                         -> Root_Type_Kinds
         --                                         -> Constraint_Kinds
         --                                         -> Discrete_Range_Kinds
      end Process_Definition;

      procedure Process_Expression
        (Element : in     Asis.Element;
         State   : in out Class)
      is
        Expression_Kind : Asis.Expression_Kinds :=
           Asis.Elements.Expression_Kind (Element);
         A_Expression : a_nodes_h.Expression_Struct :=
           a_nodes_h.Support.Default_Expression_Struct;

         -- Supporting procedures are in alphabetical order:
         procedure Add_Converted_Or_Qualified_Expression is
            ID : constant Types.Node_Id := Asis.Set_Get.Node
              (Asis.Expressions.Converted_Or_Qualified_Expression (Element));
         begin
            State.Add_To_Dot_Label
              ("Converted_Or_Qualified_Expression", To_String (ID));
            A_Expression.Converted_Or_Qualified_Expression :=
              a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Converted_Or_Qualified_Subtype_Mark is
            ID : constant Types.Node_Id := Asis.Set_Get.Node
              (Asis.Expressions.Converted_Or_Qualified_Subtype_Mark (Element));
         begin
            State.Add_To_Dot_Label
              ("Converted_Or_Qualified_Subtype_Mark", To_String (ID));
            A_Expression.Converted_Or_Qualified_Subtype_Mark :=
              a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Called_Function is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Corresponding_Called_Function
                                 (Element));
         begin
            State.Add_To_Dot_Label
              ("Corresponding_Called_Function", To_String (ID));
            A_Expression.Corresponding_Called_Function := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Expression_Type is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Corresponding_Expression_Type
                                 (Element));
         begin
            State.Add_To_Dot_Label
              ("Corresponding_Expression_Type", To_String (ID));
            A_Expression.Corresponding_Expression_Type := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Name_Declaration is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Corresponding_Name_Declaration
                                 (Element));
         begin
            State.Add_To_Dot_Label
              ("Corresponding_Name_Declaration", To_String (ID));
            A_Expression.Corresponding_Name_Declaration := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Name_Definition is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Corresponding_Name_Definition
                                 (Element));
         begin
            State.Add_To_Dot_Label
              ("Corresponding_Name_Definition", To_String (ID));
            A_Expression.Corresponding_Name_Definition := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Name_Definition_List is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Expressions.
                 Corresponding_Name_Definition_List (Element),
               Dot_Label_Name => "Corresponding_Name_Definition_List",
               List_Out       => A_Expression.Corresponding_Name_Definition_List);
         end;

         procedure Add_Function_Call_Parameters is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Expressions.
                 Function_Call_Parameters (Element),
               Dot_Label_Name => "Function_Call_Parameters",
               List_Out       => A_Expression.Function_Call_Parameters);
         end;

         procedure Add_Name_Image is
            Name_Image : constant Wide_String :=
              Asis.Expressions.Name_Image (Element);
         begin
            State.Add_To_Dot_Label ("Name_Image", To_String (Name_Image));
            A_Expression.Name_Image := To_Chars_Ptr(Name_Image);
         end;

         procedure Add_Prefix is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Prefix (Element));
         begin
            State.Add_To_Dot_Label ("Prefix", To_String (ID));
            A_Expression.Prefix := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Selector is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Selector (Element));
         begin
            State.Add_To_Dot_Label ("Selector", To_String (ID));
            A_Expression.Selector := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Subpool_Name is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Subpool_Name (Element));
         begin
            State.Add_To_Dot_Label ("Subpool_Name", To_String (ID));
            A_Expression.Subpool_Name := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Value_Image is
            Value_Image : constant Wide_String :=
              Asis.Expressions.Value_Image (Element);
         begin
            State.Add_To_Dot_Label ("Value_Image", To_String (Value_Image));
            A_Expression.Value_Image := To_Chars_Ptr(Value_Image);
         end;

         use all type Asis.Expression_Kinds;
      begin
         State.Add_To_Dot_Label ("Expression_Kind", Expression_Kind'Image);
         A_Expression.Expression_Kind := anhS.To_Expression_Kinds (Expression_Kind);
         --        An_Expression         -> Expression_Kinds
         --                                         -> Operator_Kinds
         --                                         -> Attribute_Kinds
         --
         Add_Corresponding_Expression_Type;

         case Expression_Kind is
            when Not_An_Expression =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Expression called with: " &
                 Expression_Kind'Image;
            when A_Box_Expression =>
               -- No more info:
               null;
            when An_Integer_Literal =>
               Add_Value_Image;
            when A_Real_Literal =>
               Add_Value_Image;
            when A_String_Literal =>
               Add_Value_Image;
            when An_Identifier =>
               Add_Name_Image;
               Add_Corresponding_Name_Definition;
               Add_Corresponding_Name_Definition_List;
               Add_Corresponding_Name_Declaration;
            when An_Operator_Symbol =>
               Add_Name_Image;
               Add_Corresponding_Name_Definition;
               Add_Corresponding_Name_Definition_List;
               Add_Corresponding_Name_Declaration;
                     State.Add_To_Dot_Label ("Operator_Kind",
                                          Asis.Elements.Operator_Kind (Element)'Image);
            when A_Character_Literal =>
               Add_Name_Image;
               Add_Corresponding_Name_Definition;
               Add_Corresponding_Name_Definition_List;
               Add_Corresponding_Name_Declaration;
            when An_Enumeration_Literal =>
               Add_Name_Image;
               Add_Corresponding_Name_Definition;
               Add_Corresponding_Name_Definition_List;
               Add_Corresponding_Name_Declaration;
            when An_Explicit_Dereference =>
               State.Add_Not_Implemented;
            when A_Function_Call =>
               Add_Prefix;
               Add_Corresponding_Called_Function;
               Add_Function_Call_Parameters;
            when An_Indexed_Component =>
               State.Add_Not_Implemented;
            when A_Slice =>
               State.Add_Not_Implemented;
            when A_Selected_Component =>
               Add_Prefix;
               Add_Selector;
            when An_Attribute_Reference =>
               State.Add_To_Dot_Label ("Attribute_Kind",
                                    Asis.Elements.Attribute_Kind (Element)'Image);
               -- Prefix
               -- Attribute_Designator_Identifier
               -- Attribute_Designator_Expressions
               State.Add_Not_Implemented;
            when A_Record_Aggregate =>
               State.Add_Not_Implemented;
            when An_Extension_Aggregate =>
               State.Add_Not_Implemented;
            when A_Positional_Array_Aggregate =>
               State.Add_Not_Implemented;
            when A_Named_Array_Aggregate =>
               State.Add_Not_Implemented;
            when An_And_Then_Short_Circuit =>
               State.Add_Not_Implemented;
            when An_Or_Else_Short_Circuit =>
               State.Add_Not_Implemented;
            when An_In_Membership_Test =>
               State.Add_Not_Implemented;
            when A_Not_In_Membership_Test =>
               State.Add_Not_Implemented;
            when A_Null_Literal =>
               -- No more information:
               null;
               State.Add_Not_Implemented;
            when A_Parenthesized_Expression =>
               State.Add_Not_Implemented;
            when A_Raise_Expression =>
               State.Add_Not_Implemented;
            when A_Type_Conversion =>
               Add_Converted_Or_Qualified_Subtype_Mark;
               Add_Converted_Or_Qualified_Expression;
            when A_Qualified_Expression =>
               State.Add_Not_Implemented;
            when An_Allocation_From_Subtype =>
               Add_Subpool_Name;
               State.Add_Not_Implemented;
            when An_Allocation_From_Qualified_Expression =>
               Add_Subpool_Name;
               State.Add_Not_Implemented;
            when A_Case_Expression =>
               State.Add_Not_Implemented;
            when An_If_Expression =>
               State.Add_Not_Implemented;
            when A_For_All_Quantified_Expression =>
               State.Add_Not_Implemented;
            when A_For_Some_Quantified_Expression =>
               State.Add_Not_Implemented;
         end case;
      end Process_Expression;

      procedure Process_Association
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Association_Kind : Asis.Association_Kinds :=
           Asis.Elements.Association_Kind (Element);
         A_Association : a_nodes_h.Association_Struct :=
           a_nodes_h.Support.Default_Association_Struct;

         procedure Add_Formal_Parameter is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Formal_Parameter (Element));
         begin
            State.Add_To_Dot_Label ("Formal_Parameter", To_String (ID));
            A_Association.Formal_Parameter := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Actual_Parameter is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Expressions.Actual_Parameter (Element));
         begin
            State.Add_To_Dot_Label ("Actual_Parameter", To_String (ID));
            A_Association.Actual_Parameter := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Is_Defaulted_Association is
            Is_Defaulted_Association : Boolean :=
              Asis.Expressions.Is_Defaulted_Association (Element);
         begin
            State.Add_To_Dot_Label
              ("Is_Defaulted_Association", Is_Defaulted_Association'Image);
            A_Association.Is_Defaulted_Association :=
              a_nodes_h.Support.To_bool (Is_Defaulted_Association);
         end;

         procedure Add_Is_Normalized is
            Is_Normalized : Boolean := Asis.Expressions.Is_Normalized (Element);
         begin
            State.Add_To_Dot_Label ("Is_Normalized", Is_Normalized'Image);
            A_Association.Is_Normalized := a_nodes_h.Support.To_bool (Is_Normalized);
         end;

         use all type Asis.Association_Kinds;
      begin
         State.Add_To_Dot_Label ("Association_Kind", Association_Kind'Image);
         A_Association.Association_Kind := anhS.To_Association_Kinds (Association_Kind);
         case Association_Kind is
            when Not_An_Association =>                         -- An unexpected element
               raise Program_Error with
                 "Element.Pre_Children.Process_Association called with: " &
                 Association_Kind'Image;
            when A_Pragma_Argument_Association =>
--                 Add_Formal_Parameter;
--                 Add_Actual_Parameter;
               State.Add_Not_Implemented;
            when A_Discriminant_Association =>
--                 Add_Discriminant_Selector_Names;
--                 Add_Discriminant_Expression;
--                 Add_Is_Normalized;
               State.Add_Not_Implemented;
            when A_Record_Component_Association =>
--                 Add_Record_Component_Choices;
--                 Add_Component_Expression;
--                 Add_Is_Normalized;
--                 Add_Is_Defaulted_Association;
               State.Add_Not_Implemented;
            when An_Array_Component_Association =>
--                 Add_Array_Component_Choices;
--                 Add_Component_Expression;
               State.Add_Not_Implemented;
            when A_Parameter_Association |
                 A_Generic_Association =>
               Add_Formal_Parameter;
               Add_Actual_Parameter;
               Add_Is_Defaulted_Association;
               Add_Is_Normalized;
         end case;


         State.A_Element.Element_Kind := a_nodes_h.An_Association;
         State.A_Element.the_union.association := A_Association;
      end Process_Association;

      procedure Process_Statement
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Statement_Kind : constant Asis.Statement_Kinds :=
           Asis.Elements.Statement_Kind (Element);
         A_Statement : a_nodes_h.Statement_Struct :=
           a_nodes_h.Support.Default_Statement_Struct;

         -- Supporting procedures are in alphabetical order:
         procedure Add_Accept_Body_Exception_Handlers is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Accept_Body_Exception_Handlers (Element),
               Dot_Label_Name => "Accept_Body_Exception_Handlers",
               List_Out       => A_Statement.Accept_Body_Exception_Handlers);
         end;

         procedure Add_Accept_Body_Statements is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Accept_Body_Statements (Element),
               Dot_Label_Name => "Accept_Body_Statements",
               List_Out       => A_Statement.Accept_Body_Statements);
         end;

         procedure Add_Accept_Entry_Direct_Name is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Accept_Entry_Direct_Name (Element));
         begin
            State.Add_To_Dot_Label ("Accept_Entry_Direct_Name", To_String (ID));
            A_Statement.Accept_Entry_Direct_Name := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Accept_Entry_Index is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Accept_Entry_Index (Element));
         begin
            State.Add_To_Dot_Label ("Accept_Entry_Index", To_String (ID));
            A_Statement.Accept_Entry_Index := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Accept_Parameters is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Accept_Parameters (Element),
               Dot_Label_Name => "Accept_Parameters",
               List_Out       => A_Statement.Accept_Parameters);
         end;

         procedure Add_Assignment_Expression is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Assignment_Expression (Element));
         begin
            State.Add_To_Dot_Label ("Assignment_Expression", To_String (ID));
            A_Statement.Assignment_Expression := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Assignment_Variable_Name is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Assignment_Variable_Name (Element));
         begin
            State.Add_To_Dot_Label ("Assignment_Variable_Name", To_String (ID));
            A_Statement.Assignment_Variable_Name := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Associated_Message is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Associated_Message (Element));
         begin
            State.Add_To_Dot_Label ("Associated_Message", To_String (ID));
            A_Statement.Associated_Message := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Call_Statement_Parameters is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Call_Statement_Parameters (Element),
               Dot_Label_Name => "Call_Statement_Parameters",
               List_Out       => A_Statement.Call_Statement_Parameters);
         end;

         procedure Add_Called_Name is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Called_Name (Element));
         begin
            State.Add_To_Dot_Label ("Called_Name", To_String (ID));
            A_Statement.Called_Name := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Called_Entity is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Corresponding_Called_Entity (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Called_Entity", To_String (ID));
            A_Statement.Corresponding_Called_Entity := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Corresponding_Entry is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Corresponding_Entry (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Entry", To_String (ID));
            A_Statement.Corresponding_Entry := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Is_Name_Repeated is
            Value : Boolean := Asis.Statements.Is_Name_Repeated (Element);
         begin
            State.Add_To_Dot_Label ("Is_Name_Repeated", Value'Image);
            A_Statement.Is_Name_Repeated := a_nodes_h.Support.To_bool (Value);
         end;

         procedure Add_Label_Names is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Label_Names (Element),
               Dot_Label_Name => "Label_Names",
               List_Out       => A_Statement.Label_Names);
         end;

         procedure Add_Loop_Statements is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Loop_Statements (Element),
               Dot_Label_Name => "Loop_Statements",
               List_Out       => A_Statement.Loop_Statements);
         end;

         procedure Add_Raised_Exception is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Raised_Exception (Element));
         begin
            State.Add_To_Dot_Label ("Raised_Exception", To_String (ID));
            A_Statement.Raised_Exception := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Return_Expression is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Return_Expression (Element));
         begin
            State.Add_To_Dot_Label ("Return_Expression", To_String (ID));
            A_Statement.Return_Expression := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Statement_Identifier is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Statement_Identifier (Element));
         begin
            State.Add_To_Dot_Label ("Statement_Identifier", To_String (ID));
            A_Statement.Statement_Identifier := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Statement_Paths is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Statement_Paths (Element),
               Dot_Label_Name => "Statement_Paths",
               List_Out       => A_Statement.Statement_Paths);
         end;


         use all type Asis.Statement_Kinds;
      begin
         State.Add_To_Dot_Label ("Statement_Kind", Statement_Kind'Image);
         A_Statement.Statement_Kind := anhS.To_Statement_Kinds (Statement_Kind);
         Add_Label_Names;

         case Statement_Kind is
            when Not_A_Statement =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Statement called with: " &
                 Statement_Kind'Image;

            when A_Null_Statement =>
               null; -- No more info.

            when An_Assignment_Statement =>
               Add_Assignment_Variable_Name;
               Add_Assignment_Expression;

            when An_If_Statement =>
               Add_Statement_Paths;

            when A_Case_Statement =>
               State.Add_Not_Implemented;
               -- Case_Expression

            when A_Loop_Statement =>
               Add_Statement_Identifier;
               Add_Is_Name_Repeated;
               Add_Loop_Statements;

            when A_While_Loop_Statement =>
               State.Add_Not_Implemented;

            when A_For_Loop_Statement =>
               State.Add_Not_Implemented;
               -- Statement_Identifier
               -- For_Loop_Parameter_Specification
               -- Loop_Statements

            when A_Block_Statement =>
               State.Add_Not_Implemented;
               -- Statement_Identifier
               -- Is_Name_Repeated
               -- Is_Declare_Block
               -- Block_Declarative_Items
               -- Block_Statements
               -- Block_Exception_Handlers

            when An_Exit_Statement =>
               State.Add_Not_Implemented;
               -- Exit_Loop_Name
               -- Exit_Condition
               -- Corresponding_Loop_Exited

            when A_Goto_Statement =>
               State.Add_Not_Implemented;
               -- Goto_Label
               -- Corresponding_Destination_Statement

            when A_Procedure_Call_Statement =>
               Add_Called_Name;
               Add_Corresponding_Called_Entity;
               Add_Call_Statement_Parameters;

            when A_Return_Statement =>
               Add_Return_Expression;

            when An_Extended_Return_Statement =>
               State.Add_Not_Implemented;

            when An_Accept_Statement =>
               Add_Accept_Entry_Index;
               Add_Accept_Entry_Direct_Name;
               Add_Accept_Parameters;
               Add_Accept_Body_Statements;
               Add_Accept_Body_Exception_Handlers;
               Add_Corresponding_Entry;

            when An_Entry_Call_Statement =>
               Add_Called_Name;
               Add_Corresponding_Called_Entity;
               Add_Call_Statement_Parameters;

            when A_Requeue_Statement =>
               State.Add_Not_Implemented;
               -- Requeue_Entry_Name
            when A_Requeue_Statement_With_Abort =>
               State.Add_Not_Implemented;
               -- Requeue_Entry_Name

            when A_Delay_Until_Statement =>
               State.Add_Not_Implemented;
               -- Delay_Expression
            when A_Delay_Relative_Statement =>
               State.Add_Not_Implemented;
               -- Delay_Expression

            when A_Terminate_Alternative_Statement =>
               null; -- No more info

            when A_Selective_Accept_Statement =>
               State.Add_Not_Implemented;
               -- Statement_Paths

            when A_Timed_Entry_Call_Statement =>
               State.Add_Not_Implemented;
               -- Statement_Paths

            when A_Conditional_Entry_Call_Statement =>
               State.Add_Not_Implemented;
               -- Statement_Paths

            when An_Asynchronous_Select_Statement =>
               State.Add_Not_Implemented;
               -- Statement_Paths

            when An_Abort_Statement =>
               State.Add_Not_Implemented;
               -- Aborted_Tasks

            when A_Raise_Statement =>
               Add_Raised_Exception;
               Add_Associated_Message;

            when A_Code_Statement =>
               State.Add_Not_Implemented;
               -- Qualified_Expression
         end case;

         State.A_Element.Element_Kind := a_nodes_h.A_Statement;
         State.A_Element.the_union.statement := A_Statement;
      end Process_Statement;

      procedure Process_Path
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Path_Kind : constant Asis.Path_Kinds := Asis.Elements.Path_Kind (Element);
         A_Path : a_nodes_h.Path_Struct :=
           a_nodes_h.Support.Default_Path_Struct;

         procedure Add_Case_Path_Alternative_Choices is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Case_Path_Alternative_Choices (Element),
               Dot_Label_Name => "Case_Path_Alternative_Choices",
               List_Out       => A_Path.Case_Path_Alternative_Choices);
         end;

         procedure Add_Condition_Expression is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Condition_Expression (Element));
         begin
            State.Add_To_Dot_Label ("Condition_Expression", To_String (ID));
            A_Path.Condition_Expression := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Guard is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Guard (Element));
         begin
            State.Add_To_Dot_Label ("Guard", To_String (ID));
            A_Path.Guard := a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Sequence_Of_Statements is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Sequence_Of_Statements (Element),
               Dot_Label_Name => "Sequence_Of_Statements",
               List_Out       => A_Path.Sequence_Of_Statements);
         end;

         use all type Asis.Path_Kinds;
      begin
         State.Add_To_Dot_Label ("Path_Kind", Path_Kind'Image);
         A_Path.Path_Kind := anhS.To_Path_Kinds (Path_Kind);
         Add_Sequence_Of_Statements;
         case Path_Kind is
            when Not_A_Path =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Path called with: " &
                 Path_Kind'Image;
            when An_If_Path =>
               Add_Condition_Expression;
            when An_Elsif_Path =>
               Add_Condition_Expression;
            when An_Else_Path =>
               null; -- No more info
            when A_Case_Path =>
               Add_Case_Path_Alternative_Choices;
            when A_Select_Path =>
               Add_Guard;
            when An_Or_Path =>
               Add_Guard;
            when A_Then_Abort_Path =>
               null; -- No more info
            when A_Case_Expression_Path =>
               Add_Case_Path_Alternative_Choices;
            when An_If_Expression_Path =>
               Add_Condition_Expression;
            when An_Elsif_Expression_Path =>
               Add_Condition_Expression;
            when An_Else_Expression_Path =>
               null; -- No more info
         end case;
         State.A_Element.Element_Kind := a_nodes_h.A_Path;
         State.A_Element.The_Union.Path := A_Path;
      end Process_Path;

      procedure Process_Clause
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Clause_Kind : constant Asis.Clause_Kinds :=
           Asis.Elements.Clause_Kind (Element);
         A_Clause : a_nodes_h.Clause_Struct :=
           a_nodes_h.Support.Default_Clause_Struct;

         procedure Add_Clause_Names is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Clauses.Clause_Names (Element),
               Dot_Label_Name => "Clause_Name",
               List_Out       => A_Clause.Clause_Names);
         end;

         use all type Asis.Clause_Kinds;
      begin
         State.Add_To_Dot_Label ("Clause_Kind", Clause_Kind'Image);
         A_Clause.Clause_Kind := anhS.To_Clause_Kinds (Clause_Kind);

         case Clause_Kind is
            when Not_A_Clause =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Clause called with: " &
                 Clause_Kind'Image;
            when A_Use_Package_Clause =>
               Add_Clause_Names;
            when A_Use_Type_Clause =>
               Add_Clause_Names;
            when A_Use_All_Type_Clause =>
               Add_Clause_Names;
            when A_With_Clause =>
               A_Clause.Trait_Kind := Add_Trait_Kind (Element, State);
               Add_Clause_Names;
            when A_Representation_Clause =>
         --                                         -> Representation_Clause_Kinds
               State.Add_Not_Implemented;
            when A_Component_Clause =>
               State.Add_Not_Implemented;
         end case;

         State.A_Element.Element_Kind := a_nodes_h.A_Clause;
         State.A_Element.the_union.clause := A_Clause;
      end Process_Clause;

      procedure Process_Exception_Handler
        (Element : in     Asis.Element;
         State   : in out Class)
      is
           A_Exception_Handler : a_nodes_h.Exception_Handler_Struct :=
           a_nodes_h.Support.Default_Exception_Handler_Struct;

         procedure Add_Choice_Parameter_Specification is
            ID : constant Types.Node_Id :=
              Asis.Set_Get.Node (Asis.Statements.Choice_Parameter_Specification
                                 (Element));
         begin
            State.Add_To_Dot_Label ("Choice_Parameter_Specification", To_String (ID));
            A_Exception_Handler.Choice_Parameter_Specification :=
              a_nodes_h.Node_ID (ID);
         end;

         procedure Add_Exception_Choices is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Exception_Choices (Element),
               Dot_Label_Name => "Exception_Choices",
               List_Out       => A_Exception_Handler.Exception_Choices);
         end;

         procedure Add_Handler_Statements is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Handler_Statements (Element),
               Dot_Label_Name => "Handler_Statements",
               List_Out       => A_Exception_Handler.Handler_Statements);
         end;

      begin
         Add_Choice_Parameter_Specification;
         Add_Exception_Choices;
         Add_Handler_Statements;
         State.A_Element.Element_Kind := a_nodes_h.An_Exception_Handler;
         State.A_Element.the_union.exception_handler := A_Exception_Handler;
      end Process_Exception_Handler;

      function Get_Enclosing_ID
        (Element : in Asis.Element)
         return Dot.ID_Type
      is
         Result : Dot.ID_Type; -- Initilaized
         Enclosing_Element_Id : constant Types.Node_Id :=
           Asis.Set_Get.Node_Value (Asis.Elements.Enclosing_Element (Element));
         Enclosing_Unit_Id : constant A4G.A_Types.Unit_Id :=
           Asis.Set_Get.Encl_Unit_Id (Element);
         function Enclosing_Is_Element return boolean
           is (Types."/=" (Enclosing_Element_Id, Types.Empty));
      begin
         if Enclosing_Is_Element then
            Result := To_Dot_ID_Type (Enclosing_Element_Id);
         else
            Result := To_Dot_ID_Type (Enclosing_Unit_Id);
         end if;
         return Result;
      end Get_Enclosing_ID;

      function Get_Enclosing_ID
        (Element : in Asis.Element)
         return a_nodes_h.Node_ID is
      begin
         return a_nodes_h.Node_ID
           (Asis.Set_Get.Node_Value
              (Asis.Elements.Enclosing_Element (Element)));
      end Get_Enclosing_ID;

      function Source_Location_Image
        (Element : in Asis.Element)
         return String
      is
         Unit : constant Asis.Compilation_Unit :=
           Asis.Elements.Enclosing_Compilation_Unit (Element);
         Unit_Name : constant String := To_String
           (Asis.Compilation_Units.Unit_Full_Name (Unit));
         Unit_Class : constant Asis.Unit_Classes := Asis.Compilation_Units.Unit_Class (Unit);
         Span : constant Asis.Text.Span := Asis.Text.Element_Span (Element);
         function Spec_Or_Body return String is
            use all type Asis.Unit_Classes;
         begin
            case Unit_Class is
               when Not_A_Class =>
                  return "()";
               when A_Public_Declaration |
                    A_Private_Declaration =>
                  return "(spec)";
               when A_Public_Body |
                    A_Private_Body =>
                  return "(body)";
               when A_Public_Declaration_And_Body =>
                  return "(spec and body)";
               when A_Separate_Body =>
                  return "(separate body)";
            end case;
         end Spec_Or_Body;
      begin
         return Unit_Name & Spec_Or_Body & ":" &
           NLB_Image (Span.First_Line) & ":" & NLB_Image (Span.First_Column) &
           ".." &
           NLB_Image (Span.Last_Line) & ":" & NLB_Image (Span.Last_Column);
      end Source_Location_Image;

      ------------
      -- EXPORTED:
      ------------
      procedure Process_Element
        (Element : in     Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class)
      is
         Element_Kind : constant Asis.Element_Kinds :=
           Asis.Elements.Element_Kind (Element);
         Element_Id   : constant Types.Node_Id := Asis.Set_Get.Node (Element);

         procedure Start_Output
         is
            Default_Node  : Dot.Node_Stmt.Class; -- Initialized
            Default_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         begin
            State.Text.Indent;
            State.Text.End_Line;
            State.Dot_Node := Default_Node;
            State.Dot_Label := Default_Label;
            State.A_Element := a_nodes_h.Support.Default_Element_Struct;

            State.Dot_Node.Node_ID.ID := To_Dot_ID_Type (Element_Id);
            State.A_Element.id := a_nodes_h.Node_ID (Element_Id);

            State.Add_To_Dot_Label ("Element_Kind", Element_Kind'Image);
            State.A_Element.Element_Kind := anhS.To_Element_Kinds (Element_Kind);

            State.Add_To_Dot_Label ("ID", To_String (Element_Id));
            -- ID is in the Dot node twice, but not in the a_node.

            State.Add_To_Dot_Label ("Source", Source_Location_Image (Element));
            State.A_Element.source_location :=
              To_Chars_Ptr (Source_Location_Image (Element));
         end;

         procedure Add_Enclosing_Edge
         is
            Edge_Stmt : Dot.Edges.Stmts.Class; -- Initialized
            Enclosing_Element : constant Asis.Element :=
              Asis.Elements.Enclosing_Element (Element);
            Enclosing_Element_Id : constant Types.Node_Id :=
              Asis.Set_Get.Node (Enclosing_Element);
--    enum Enclosing_Kinds   enclosing_kind;
         begin
            Edge_Stmt.LHS.Node_Id.ID := To_Dot_ID_Type (Enclosing_Element_Id);
            State.A_Element.enclosing_id := a_nodes_h.Node_ID (Enclosing_Element_Id);

            Edge_Stmt.RHS.Node_Id.ID := To_Dot_ID_Type (Element_Id);

            State.Outputs.Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmt));
         end;

         procedure Finish_Output
         is
            A_Node    : a_nodes_h.Node_Struct := anhS.Default_Node_Struct;
         begin
            Add_Enclosing_Edge;
            State.Dot_Node.Add_Label (State.Dot_Label);

            State.Outputs.Graph.Append_Stmt
              (new Dot.Node_Stmt.Class'(State.Dot_Node));

            A_Node.Node_Kind := a_nodes_h.An_Element_Node;
            A_Node.the_union.element := State.A_Element;
            State.Outputs.A_Nodes.Push (A_Node);

            State.Text.End_Line;
            State.Text.Dedent;
         end;

      begin
         Start_Output;
         case Element_Kind is
            when Asis.Not_An_Element =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Element called with: " &
                 Element_Kind'Image;
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
         Finish_Output;
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
         Null;
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
      Outputs : in     Output_Accesses_Record)
   is
      Process_Control : Asis.Traverse_Control := Asis.Continue;
   begin
      This.The_Element := Element;
      -- I like to just pass Outputs through and not store it in the object,
      -- since it is all pointers and we doesn't need to store their values
      -- between calls to Process_Element_Tree. Outputs has to go into
      -- State_Information in the Traverse_Element instatiation, though,
      -- so we'll put it in the object and pass that:
      This.Outputs := Outputs;
      Traverse_Element
        (Element => Element,
         Control => Process_Control,
         State   => This);
   end Process_Element_Tree;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String) is
   begin
-- Instead of this, put the "attribute" in the label:
--        This.Node.Attr_List.Add_Assign_To_First_Attr
--          (Name  => Name,
--           Value => Value);
      This.Dot_Label.Add_Eq_Row(L => Name, R => Value);
      This.Text.Put_Indented_Line (Name & " => """ & Value & """");
   end;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end;

   procedure Add_Not_Implemented
     (This  : in out Class) is
   begin
      This.Add_To_Dot_Label ("ASIS_PROCESSING", String'("NOT_COMPLETELY_IMPLEMENTED"));
   end Add_Not_Implemented;


end Asis_Tool_2.Element;
