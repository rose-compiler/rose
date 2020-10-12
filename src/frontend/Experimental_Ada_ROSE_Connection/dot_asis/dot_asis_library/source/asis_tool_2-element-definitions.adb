with Asis.Definitions;
with Asis.Elements;

package body Asis_Tool_2.Element.Definitions is

   ------------
   -- EXPORTED:
   ------------
   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element; State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";

      -----------------------------------------------------------------------
      -- BEGIN field support (Alphabetical order):

      function Component_Definition_View return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Component_Definition_View (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Component_Definition_View", ID);
         return ID;
      end;

      function Corresponding_Parent_Subtype return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Parent_Subtype
                           (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Parent_Subtype", To_String (ID));
         return ID;
      end;

      function Corresponding_Root_Type return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Root_Type
                           (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Root_Type", To_String (ID));
         return ID;
      end;

      function Corresponding_Type_Operators return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Corresponding_Type_Operators (Element),
            Dot_Label_Name => "Corresponding_Type_Operators");
      end;

      function Corresponding_Type_Structure return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Type_Structure
                           (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Type_Structure", To_String (ID));
         return ID;
      end;

      function Definition_Interface_List return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Definition_Interface_List (Element),
            Dot_Label_Name => "Definition_Interface_List");
      end;

      function Delta_Expression return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Delta_Expression (Element));
      begin
         State.Add_To_Dot_Label ("Delta_Expression", To_String (ID));
         return ID;
      end;

      function Digits_Expression return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Digits_Expression (Element));
      begin
         State.Add_To_Dot_Label ("Digits_Expression", To_String (ID));
         return ID;
      end;

      function Discrete_Ranges return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Discrete_Ranges (Element),
            Dot_Label_Name => "Discrete_Ranges");
      end;

      function Discriminant_Associations return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Discriminant_Associations (Element),
            Dot_Label_Name => "Discriminant_Associations");
      end;

      function Has_Abstract return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Abstract (Element);
      begin
         State.Add_To_Dot_Label ("Has_Abstract", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Has_Limited return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Limited (Element);
      begin
         State.Add_To_Dot_Label ("Has_Limited", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Has_Null_Exclusion return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Null_Exclusion (Element);
      begin
         State.Add_To_Dot_Label ("Has_Null_Exclusion", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Has_Private return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Private (Element);
      begin
         State.Add_To_Dot_Label ("Has_Private", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Has_Tagged return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Tagged (Element);
      begin
         State.Add_To_Dot_Label ("Has_Tagged", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Has_Task return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Task (Element);
      begin
         State.Add_To_Dot_Label ("Has_Task", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Implicit_Components return a_nodes_h.Element_ID_List is
         -- Not implemented in ASIS for GNAT GPL 2017 (20170515-63)GNAT GPL 2017 (20170515-63):
         Implemented_In_Asis : constant Boolean := False;
      begin
         if Implemented_In_Asis then
            return To_Element_ID_List
              (This           => State,
               Elements_In    => Asis.Definitions.Implicit_Components (Element),
               Dot_Label_Name => "Implicit_Components");
         else
            return a_nodes_h.Support.Empty_Element_ID_List;
         end if;
      end;

      function Implicit_Inherited_Declarations return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Implicit_Inherited_Declarations (Element),
            Dot_Label_Name => "Implicit_Inherited_Declarations");
      end;

      function Implicit_Inherited_Subprograms return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Implicit_Inherited_Subprograms (Element),
            Dot_Label_Name => "Implicit_Inherited_Subprograms");
      end;

      function Is_Private_Present return ICE.bool is
         Value : constant Boolean := Asis.Definitions.Is_Private_Present (Element);
      begin
         State.Add_To_Dot_Label ("Is_Private_Present", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Lower_Bound return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Lower_Bound (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Lower_Bound", ID);
         return ID;
      end;

      function Parent_Subtype_Indication return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Parent_Subtype_Indication
                           (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Parent_Subtype_Indication", ID);
         return ID;
      end;

      function Private_Part_Items return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Private_Part_Items (Element),
            Dot_Label_Name => "Private_Part_Items");
      end;

      function Range_Attribute return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Range_Attribute (Element));
      begin
         State.Add_To_Dot_Label ("Range_Attribute", To_String (ID));
         return ID;
      end;

      function Real_Range_Constraint return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Real_Range_Constraint (Element));
      begin
         State.Add_To_Dot_Label ("Real_Range_Constraint", To_String (ID));
         return ID;
      end;

      function Record_Components return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Record_Components (Element),
            Dot_Label_Name => "Record_Components",
            Add_Edges      => True);
      end;

      function Record_Definition return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Record_Definition (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Record_Definition", ID);
         return ID;
      end;

      function Subtype_Constraint return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Subtype_Constraint (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Subtype_Constraint", ID);
         return ID;
      end;

      function Subtype_Mark return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Subtype_Mark (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Subtype_Mark", ID);
         return ID;
      end;

      function Upper_Bound return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Upper_Bound (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Upper_Bound", ID);
         return ID;
      end;

      function Visible_Part_Items return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Visible_Part_Items (Element),
            Dot_Label_Name => "Visible_Part_Items",
            Add_Edges      => True);
      end;

      -- END Field support
      -----------------------------------------------------------------------
      -- BEGIN record support:

      function Subtype_Constraint return a_nodes_h.Constraint_Struct is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Subtype_Constraint";

         Result          : a_nodes_h.Constraint_Struct :=
           a_nodes_h.Support.Default_Constraint_Struct;
         Constraint_Kind : constant Asis.Constraint_Kinds :=
           Asis.Elements.Constraint_Kind (Element);

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Constraint_Kind", Constraint_Kind'Image);
            Result.Constraint_Kind := anhS.To_Constraint_Kinds (Constraint_Kind);
         end Add_Common_Items;

         use all type Asis.Constraint_Kinds;
      begin
         If Constraint_Kind /= Not_A_Constraint then
            Add_Common_Items;
         end if;

         case Constraint_Kind is
         when Not_A_Constraint =>
            raise Program_Error with
            Module_Name &   " called with: " & Constraint_Kind'Image;
         when A_Range_Attribute_Reference =>
            Result.Range_Attribute := Range_Attribute;
         when A_Simple_Expression_Range =>
            Result.Lower_Bound := Lower_Bound;
            Result.Upper_Bound := Upper_Bound;
         when A_Digits_Constraint =>
            Result.Digits_Expression := Digits_Expression;
            Result.Real_Range_Constraint := Real_Range_Constraint;
         when A_Delta_Constraint =>
            Result.Delta_Expression := Delta_Expression;
            Result.Real_Range_Constraint := Real_Range_Constraint;
         when An_Index_Constraint =>
            Result.Discrete_Ranges := Discrete_Ranges;
         when A_Discriminant_Constraint =>
            Result.Discriminant_Associations := Discriminant_Associations;
         end case;

         return Result;
      end Subtype_Constraint;

      function Type_Definition
        return a_nodes_h.Type_Definition_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Type_Definition";
         Result    : a_nodes_h.Type_Definition_Struct :=
           a_nodes_h.Support.Default_Type_Definition_Struct;

         Type_Kind : constant Asis.Type_Kinds :=
           Asis.Elements.Type_Kind (Element);

         procedure Add_Common_Items is
         begin -- Add_Common_Items
            State.Add_To_Dot_Label ("Type_Kind", Type_Kind'Image);
            Result.Type_Kind := anhS.To_Type_Kinds (Type_Kind);
            Result.Has_Abstract := Has_Abstract;
            Result.Has_Limited := Has_Limited;
            Result.Has_Private := Has_Private;
            Result.Corresponding_Type_Operators := Corresponding_Type_Operators;
         end Add_Common_Items;

         use all type Asis.Type_Kinds;
      begin -- Process_Type_Definition
         If Type_Kind /= Not_A_Type_Definition then
            Add_Common_Items;
         end if;

         case Type_Kind is
         when Not_A_Type_Definition =>
            raise Program_Error with
            Module_Name & " called with: " & Type_Kind'Image;
         when A_Derived_Type_Definition =>
            Result.Parent_Subtype_Indication := Parent_Subtype_Indication;
            Result.Implicit_Inherited_Declarations := Implicit_Inherited_Declarations;
            Result.Implicit_Inherited_Subprograms := Implicit_Inherited_Subprograms;
            Result.Corresponding_Parent_Subtype := Corresponding_Parent_Subtype;
            Result.Corresponding_Root_Type := Corresponding_Root_Type;
            Result.Corresponding_Type_Structure := Corresponding_Type_Structure;
         when A_Derived_Record_Extension_Definition =>
            Result.Parent_Subtype_Indication := Parent_Subtype_Indication;
            Result.Record_Definition := Record_Definition;
            Result.Implicit_Inherited_Declarations := Implicit_Inherited_Declarations;
            Result.Implicit_Inherited_Subprograms := Implicit_Inherited_Subprograms;
            Result.Corresponding_Parent_Subtype := Corresponding_Parent_Subtype;
            Result.Corresponding_Root_Type := Corresponding_Root_Type;
            Result.Corresponding_Type_Structure := Corresponding_Type_Structure;
            Result.Definition_Interface_List := Definition_Interface_List;
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
         when A_Record_Type_Definition =>
            Result.Record_Definition := Record_Definition;
            Result.Has_Tagged := Has_Tagged; -- Small x in spreadsheet?               := Has_Tagged; -- Small x in spreadsheet?
         when A_Tagged_Record_Type_Definition =>
            Result.Record_Definition := Record_Definition;
         when An_Interface_Type_Definition =>
            State.Add_Not_Implemented;
         when An_Access_Type_Definition =>
            Result.Has_Null_Exclusion := Has_Null_Exclusion;
            State.Add_Not_Implemented;
         end case;

         return Result;
      end Type_Definition;

      function Subtype_Indication
        return a_nodes_h.Subtype_Indication_Struct
      is
         Result : a_nodes_h.Subtype_Indication_Struct :=
           a_nodes_h.Support.Default_Subtype_Indication_Struct;
      begin -- Process_Subtype_Indication
         Result.Has_Null_Exclusion := Has_Null_Exclusion;
         Result.Subtype_Mark       := Subtype_Mark;
         Result.Subtype_Constraint := Subtype_Constraint;
         return Result;
      end Subtype_Indication;

      function Constraint
        return a_nodes_h.Constraint_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Constraint";
         Result : a_nodes_h.Constraint_Struct :=
           a_nodes_h.Support.Default_Constraint_Struct;

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

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Constraint_Kind", Constraint_Kind'Image);
            Result.Constraint_Kind := anhS.To_Constraint_Kinds (Constraint_Kind);
         end Add_Common_Items;

         use all type Asis.Constraint_Kinds;
      begin -- Process_Constraint
         if Constraint_Kind /= Not_A_Constraint then
            Add_Common_Items;
         end if;

         case Constraint_Kind is
         when Not_A_Constraint =>
            raise Program_Error with
              "Element.Pre_Children.Process_Definition.Process_Constraint called with: " &
              Constraint_Kind'Image;
         when A_Range_Attribute_Reference =>
            Result.Range_Attribute := Range_Attribute;
         when A_Simple_Expression_Range =>
            Result.Lower_Bound := Lower_Bound;
            Result.Upper_Bound := Upper_Bound;
         when A_Digits_Constraint =>
            Result.Digits_Expression := Digits_Expression;
            Result.Real_Range_Constraint := Real_Range_Constraint;
         when A_Delta_Constraint =>
            Result.Delta_Expression := Delta_Expression;
            Result.Real_Range_Constraint := Real_Range_Constraint;
         when An_Index_Constraint =>
            Result.Discrete_Ranges := Discrete_Ranges;
         when A_Discriminant_Constraint =>
            Result.Discriminant_Associations := Discriminant_Associations;
         end case;

         return Result;
      end Constraint;

      -----------------------------------------------------------------------

      -- Has side effects:
      function Record_Definition
        return a_nodes_h.Record_Definition_Struct
      is
         Result : a_nodes_h.Record_Definition_Struct :=
           a_nodes_h.Support.Default_Record_Definition_Struct;
      begin
         Result.Record_Components := Record_Components;
         Result.Implicit_Components := Implicit_Components;
         return Result;
      end Record_Definition;

      -----------------------------------------------------------------------

      -- Has side effects:
      function Task_Definition
        return a_nodes_h.Task_Definition_Struct
      is
         Result : a_nodes_h.Task_Definition_Struct :=
           a_nodes_h.Support.Default_Task_Definition_Struct;
      begin
         Result.Has_Task := Has_Task;
         Result.Visible_Part_Items := Visible_Part_Items;
         Result.Private_Part_Items := Private_Part_Items;
         Result.Is_Private_Present := Is_Private_Present;
         return Result;
      end Task_Definition;

      -- END record support
      -----------------------------------------------------------------------

      Result          : a_nodes_h.Definition_Struct :=
        a_nodes_h.Support.Default_Definition_Struct;

      Definition_Kind : constant Asis.Definition_Kinds :=
        Asis.Elements.Definition_Kind (Element);

      procedure Add_Common_Items is
      begin
         State.Add_To_Dot_Label ("Definition_Kind", Definition_Kind'Image);
         Result.Definition_Kind := anhS.To_Definition_Kinds (Definition_Kind);
      end Add_Common_Items;

      use all type Asis.Definition_Kinds;
   begin -- Process_Definition
      If Definition_Kind /= Not_A_Definition then
         Add_Common_Items;
      end if;

      case Definition_Kind is
      when Not_A_Definition =>
         raise Program_Error with
           "Element.Pre_Children.Process_Definition called with: " &
           Definition_Kind'Image;
      when A_Type_Definition =>
         Result.The_Union.The_Type_Definition := Type_Definition;
      when A_Subtype_Indication =>
         Result.The_Union.The_Subtype_Indication := Subtype_Indication;
      when A_Constraint =>
         Result.The_Union.The_Constraint := Constraint;
      when A_Component_Definition =>
         Result.The_Union.The_Component_Definition.
           Component_Definition_View := Component_Definition_View;
      when A_Discrete_Subtype_Definition =>
         State.Add_Not_Implemented;
      when A_Discrete_Range =>
         State.Add_Not_Implemented;
      when An_Unknown_Discriminant_Part =>
         -- No more components:
         null;
      when A_Known_Discriminant_Part =>
         State.Add_Not_Implemented;
      when A_Record_Definition =>
         Result.The_Union.The_Record_Definition := Record_Definition;
      when A_Null_Record_Definition =>
         null; -- No more components
      when A_Null_Component =>
         null; -- No more components
      when A_Variant_Part =>
         State.Add_Not_Implemented;
      when A_Variant =>
         State.Add_Not_Implemented;
      when An_Others_Choice =>
         null; -- No more components
      when An_Access_Definition =>
         -- Access_Definition_Kinds
         State.Add_Not_Implemented;
      when A_Private_Type_Definition =>
         State.Add_Not_Implemented;
      when A_Tagged_Private_Type_Definition =>
         State.Add_Not_Implemented;
      when A_Private_Extension_Definition =>
         State.Add_Not_Implemented;
      when A_Task_Definition =>
         Result.The_Union.The_Task_Definition := Task_Definition;
      when A_Protected_Definition =>
         State.Add_Not_Implemented;
      when A_Formal_Type_Definition =>
         State.Add_Not_Implemented;
      when An_Aspect_Specification =>
         State.Add_Not_Implemented;
      end case;

      State.A_Element.Element_Kind := a_nodes_h.A_Definition;
      State.A_Element.The_Union.Definition := Result;   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Definitions;
