with Asis.Definitions;
with Asis.Elements;
with A4G.A_Types; use A4G.A_Types;
with Asis.Set_Get;

package body Asis_Adapter.Element.Definitions is

   -- "Add_And_Return_* functions query ASIS for a single value or list of
   -- values, add it to the Dot graph, and return the value.  These functions
   -- usually have more than one caller.
   --
   -- "Add_*" procedures query ASIS for a single value or list of values,
   -- add it to the Dot graph, and set the corresponding value in the local
   -- Result record.  These procedures are only called from the subprogram they
   -- are declared in.
   --
   -- "Create_And_Return_*" functions create a struct, populate it by calling
   -- Add_And_Return_* and Add_* routines based on the Element sub kind, and
   -- return the struct.  These functions are only called from the subprogram
   -- they are declared in.

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

      function Add_And_Return_Access_To_Function_Result_Profile return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Access_To_Function_Result_Profile (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Access_To_Function_Result_Profile", ID);
         return ID;
      end;

      function Add_And_Return_Access_To_Subprogram_Parameter_Profile return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Access_To_Subprogram_Parameter_Profile (Element),
            Dot_Label_Name => "Access_To_Subprogram_Parameter_Profile");
      end;

      function Add_And_Return_Ancestor_Subtype_Indication return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Ancestor_Subtype_Indication (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Ancestor_Subtype_Indication", ID);
         return ID;
      end;

      function Add_And_Return_Anonymous_Access_To_Object_Subtype_Mark return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Anonymous_Access_To_Object_Subtype_Mark (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Anonymous_Access_To_Object_Subtype_Mark", ID);
         return ID;
      end;

      function Add_And_Return_Array_Component_Definition return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Array_Component_Definition (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Array_Component_Definition", ID);
         return ID;
      end;

      function Add_And_Return_Aspect_Mark return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Aspect_Mark (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Aspect_Mark", ID);
         return ID;
      end;

      function Add_And_Return_Aspect_Definition return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Aspect_Definition (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Aspect_Definition", ID);
         return ID;
      end;

      function Add_And_Return_Component_Definition_View return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Component_Definition_View (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Component_Definition_View", ID);
         return ID;
      end;

      function Add_And_Return_Component_Subtype_Indication return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Component_Subtype_Indication (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Component_Subtype_Indication", ID);
         return ID;
      end;

      function Add_And_Return_Corresponding_Parent_Subtype return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Parent_Subtype
                           (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Parent_Subtype", ID);
         return ID;
      end;

      function Add_And_Return_Corresponding_Root_Type return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Root_Type
                           (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Root_Type", ID);
         return ID;
      end;

      function Add_And_Return_Corresponding_Type_Operators return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Corresponding_Type_Operators (Element),
            Dot_Label_Name => "Corresponding_Type_Operators");
      end;

      function Add_And_Return_Corresponding_Type_Structure return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Type_Structure
                           (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Type_Structure", ID);
         return ID;
      end;

      function Add_And_Return_Definition_Interface_List return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Definition_Interface_List (Element),
            Dot_Label_Name => "Definition_Interface_List",
            Add_Edges      => True);
      end;

      function Add_And_Return_Discriminant_Direct_Name return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Discriminant_Direct_Name (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Discriminant_Direct_Name", ID);
         return ID;
      end;

      function Add_And_Return_Delta_Expression return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Delta_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Delta_Expression", ID);
         return ID;
      end;

      function Add_And_Return_Digits_Expression return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Digits_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Digits_Expression", ID);
         return ID;
      end;

      function Add_And_Return_Discrete_Ranges return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Discrete_Ranges (Element),
            Dot_Label_Name => "Discrete_Ranges",
            Add_Edges      => True);
      end;

      function Add_And_Return_Discrete_Subtype_Definitions return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Discrete_Subtype_Definitions (Element),
            Dot_Label_Name => "Discrete_Subtype_Definitions",
            Add_Edges      => True);
      end;

      function Add_And_Return_Discriminant_Associations return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Discriminant_Associations (Element),
            Dot_Label_Name => "Discriminant_Associations",
            Add_Edges      => True);
      end;

      function Add_And_Return_Discriminants return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Discriminants (Element),
            Dot_Label_Name => "Discriminants",
            Add_Edges      => True);
      end;

      function Add_And_Return_Has_Abstract return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Abstract (Element);
      begin
         State.Add_To_Dot_Label ("Has_Abstract", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Has_Aliased return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Aliased (Element);
      begin
         State.Add_To_Dot_Label ("Has_Aliased", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Has_Limited return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Limited (Element);
      begin
         State.Add_To_Dot_Label ("Has_Limited", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Has_Null_Exclusion return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Null_Exclusion (Element);
      begin
         State.Add_To_Dot_Label ("Has_Null_Exclusion", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Has_Private return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Private (Element);
      begin
         State.Add_To_Dot_Label ("Has_Private", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Has_Protected return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Protected (Element);
      begin
         State.Add_To_Dot_Label ("Has_Protected", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Has_Synchronized return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Synchronized (Element);
      begin
         State.Add_To_Dot_Label ("Has_Synchronized", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Has_Tagged return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Tagged (Element);
      begin
         State.Add_To_Dot_Label ("Has_Tagged", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Has_Task return ICE.bool is
         Value : constant Boolean := Asis.Elements.Has_Task (Element);
      begin
         State.Add_To_Dot_Label ("Has_Task", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Implicit_Components return a_nodes_h.Element_ID_List is
         -- Not implemented in ASIS for GNAT GPL 2017 (20170515-63)GNAT GPL 2017 (20170515-63):
         Implemented_In_Asis : constant Boolean := False;
      begin
         if Implemented_In_Asis then
            return To_Element_ID_List
              (This           => State,
               Elements_In    => Asis.Definitions.Implicit_Components (Element),
               Dot_Label_Name => "Implicit_Components",
               Add_Edges      => True);
         else
            return a_nodes_h.Support.Empty_Element_ID_List;
         end if;
      end;

      function Add_And_Return_Implicit_Inherited_Declarations return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Implicit_Inherited_Declarations (Element),
            Dot_Label_Name => "Implicit_Inherited_Declarations",
            Add_Edges      => True);
      end;

      function Add_And_Return_Implicit_Inherited_Subprograms return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Implicit_Inherited_Subprograms (Element),
            Dot_Label_Name => "Implicit_Inherited_Subprograms",
            Add_Edges      => True);
      end;

      function Add_And_Return_Index_Subtype_Definitions return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Index_Subtype_Definitions (Element),
            Dot_Label_Name => "Index_Subtype_Definitions",
            Add_Edges      => True);
      end;

      function Add_And_Return_Is_Not_Null_Return return ICE.bool is
         Value : constant Boolean := Asis.Elements.Is_Not_Null_Return (Element);
      begin
         State.Add_To_Dot_Label ("Is_Not_Null_Return", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Is_Private_Present return ICE.bool is
         Value : constant Boolean := Asis.Definitions.Is_Private_Present (Element);
      begin
         State.Add_To_Dot_Label ("Is_Private_Present", Value);
         return a_nodes_h.Support.To_bool (Value);
      end;

      function Add_And_Return_Lower_Bound return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Lower_Bound (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Lower_Bound", ID);
         return ID;
      end;

      function Add_And_Return_Parent_Subtype_Indication return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Parent_Subtype_Indication
                           (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Parent_Subtype_Indication", ID);
         return ID;
      end;

      function Add_And_Return_Private_Part_Items return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Private_Part_Items (Element, true),
            Dot_Label_Name => "Private_Part_Items",
            Add_Edges      => True);
      end;

      function Add_And_Return_Range_Attribute return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Range_Attribute (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Range_Attribute", ID);
         return ID;
      end;

      function Add_And_Return_Real_Range_Constraint return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Real_Range_Constraint (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Real_Range_Constraint", ID);
         return ID;
      end;

      function Add_And_Return_Record_Components return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Record_Components (Element, true),
            Dot_Label_Name => "Record_Components",
            Add_Edges      => True);
      end;

      function Add_And_Return_Record_Definition return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Record_Definition (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Record_Definition", ID);
         return ID;
      end;

      function Add_And_Return_Subtype_Constraint return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Subtype_Constraint (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Subtype_Constraint", ID);
         return ID;
      end;

      function Add_And_Return_Subtype_Mark return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Subtype_Mark (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Subtype_Mark", ID);
         return ID;
      end;

      function Add_And_Return_Upper_Bound return a_nodes_h.Element_ID is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Upper_Bound (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Upper_Bound", ID);
         return ID;
      end;

      function Add_And_Return_Variant_Choices return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Variant_Choices (Element),
            Dot_Label_Name => "Variant_Choices",
            Add_Edges      => True);
      end;

      function Add_And_Return_Variants return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Variants (Element, true),
            Dot_Label_Name => "Variants",
            Add_Edges      => True);
      end;

      function Add_And_Return_Visible_Part_Items return a_nodes_h.Element_ID_List is
      begin
         return To_Element_ID_List
           (This           => State,
            Elements_In    => Asis.Definitions.Visible_Part_Items (Element, true),
            Dot_Label_Name => "Visible_Part_Items",
            Add_Edges      => True);
      end;

      -- END Field support
      -----------------------------------------------------------------------
      -- BEGIN record support:

      -- Has more than one caller:
      -- Has side effects:
      function Create_And_Return_Access_Definition
        return a_nodes_h.Access_Definition_Struct 
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name &
           ".Create_And_Return_Access_Definition";

         Result          : a_nodes_h.Access_Definition_Struct :=
           a_nodes_h.Support.Default_Access_Definition_Struct;
         Access_Definition_Kind : constant Asis.Access_Definition_Kinds :=
           Asis.Elements.Access_Definition_Kind (Element);

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Access_Definition_Kind", Access_Definition_Kind'Image);
            Result.Access_Definition_Kind := To_Access_Definition_Kinds (Access_Definition_Kind);
            Result.Has_Null_Exclusion := Add_And_Return_Has_Null_Exclusion; 
         end Add_Common_Items;

         use all type Asis.Access_Definition_Kinds;
      begin
         If Access_Definition_Kind /= Not_An_Access_Definition then
            Add_Common_Items;
         end if;

         case Access_Definition_Kind is
         when Not_An_Access_Definition =>
            raise Internal_Error with
            Module_Name & " called with: " & Access_Definition_Kind'Image;
         when An_Anonymous_Access_To_Variable =>
            Result.Anonymous_Access_To_Object_Subtype_Mark := Add_And_Return_Anonymous_Access_To_Object_Subtype_Mark;
         when An_Anonymous_Access_To_Constant =>
            Result.Anonymous_Access_To_Object_Subtype_Mark := Add_And_Return_Anonymous_Access_To_Object_Subtype_Mark;
         when An_Anonymous_Access_To_Procedure =>
            Result.Access_To_Subprogram_Parameter_Profile := Add_And_Return_Access_To_Subprogram_Parameter_Profile;
         when An_Anonymous_Access_To_Protected_Procedure =>
            Result.Access_To_Subprogram_Parameter_Profile := Add_And_Return_Access_To_Subprogram_Parameter_Profile;
         when An_Anonymous_Access_To_Function =>
            Result.Access_To_Subprogram_Parameter_Profile := Add_And_Return_Access_To_Subprogram_Parameter_Profile;
            Result.Is_Not_Null_Return := Add_And_Return_Is_Not_Null_Return;
         when An_Anonymous_Access_To_Protected_Function =>
            Result.Access_To_Subprogram_Parameter_Profile := Add_And_Return_Access_To_Subprogram_Parameter_Profile;
            Result.Is_Not_Null_Return := Add_And_Return_Is_Not_Null_Return;
         end case;

         return Result;
      end Create_And_Return_Access_Definition;


      -- Has side effects:
      function Create_And_Return_Access_Type
        return a_nodes_h.Access_Type_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name &
           ".Create_And_Return_Access_Type";

         Result : a_nodes_h.Access_Type_Struct :=
           a_nodes_h.Support.Default_Access_Type_Struct;
         Access_Type_Kind : constant Asis.Access_Type_Kinds :=
           Asis.Elements.Access_Type_Kind (Element);

         -- Supporting procedures are in alphabetical order:
         procedure Add_Access_To_Object_Definition is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Definitions.Access_To_Object_Definition (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Access_To_Object_Definition", ID);
            Result.Access_To_Object_Definition := ID;
         end;

         procedure Add_Access_To_Function_Result_Profile is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Definitions.Access_To_Function_Result_Profile (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Access_To_Function_Result_Profile", ID);
            Result.Access_To_Function_Result_Profile := ID;
         end;

         procedure Add_Access_To_Subprogram_Parameter_Profile is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Definitions.Access_To_Subprogram_Parameter_Profile (Element),
               Dot_Label_Name => "Access_To_Subprogram_Parameter_Profile",
               List_Out       => Result.Access_To_Subprogram_Parameter_Profile,
               Add_Edges      => True);
         end;

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Access_Type_Kind", Access_Type_Kind'Image);
            Result.Access_Type_Kind := To_Access_Type_Kinds (Access_Type_Kind);
            Result.Has_Null_Exclusion := Add_And_Return_Has_Null_Exclusion; 
         end Add_Common_Items;

         use all type Asis.Access_Type_Kinds;
      begin -- Create_And_Return_Access_Type
         If Access_Type_Kind /= Not_An_Access_Type_Definition then
            Add_Common_Items;
         end if;

         case Access_Type_Kind is
            when Not_An_Access_Type_Definition =>
               raise Internal_Error with
               Module_Name & " called with: " & Access_Type_Kind'Image;
            when A_Pool_Specific_Access_To_Variable =>
               Add_Access_To_Object_Definition;
            when An_Access_To_Variable =>
               Add_Access_To_Object_Definition;
            when An_Access_To_Constant =>
               Add_Access_To_Object_Definition;
            when An_Access_To_Procedure =>
               Add_Access_To_Subprogram_Parameter_Profile;
            when An_Access_To_Protected_Procedure =>
               Add_Access_To_Subprogram_Parameter_Profile;
            when An_Access_To_Function =>
               Add_Access_To_Subprogram_Parameter_Profile;
               Add_Access_To_Function_Result_Profile;
            when An_Access_To_Protected_Function =>
               Add_Access_To_Subprogram_Parameter_Profile;
               Add_Access_To_Function_Result_Profile;
         end case;

         --           when An_Anonymous_Access_To_Variable =>
         --              -- Anonymous_Access_To_Object_Subtype_Mark -- A2005
         --              State.Add_Not_Implemented (Ada_2005);
         --           when An_Anonymous_Access_To_Constant =>
         --              -- Anonymous_Access_To_Object_Subtype_Mark -- A2005
         --              State.Add_Not_Implemented (Ada_2005);
         --           when An_Anonymous_Access_To_Procedure =>
         --              -- Access_To_Subprogram_Parameter_Profile -- A2005
         --              State.Add_Not_Implemented (Ada_2005);
         --           when An_Anonymous_Access_To_Protected_Procedure =>
         --              -- Access_To_Subprogram_Parameter_Profile -- A2005
         --              State.Add_Not_Implemented (Ada_2005);
         --           when An_Anonymous_Access_To_Function =>
         --              -- Access_To_Subprogram_Parameter_Profile -- A2005
         --              State.Add_Not_Implemented (Ada_2005);
         --           when An_Anonymous_Access_To_Protected_Function =>
         --              -- Access_To_Subprogram_Parameter_Profile -- A2005
         --              State.Add_Not_Implemented (Ada_2005);
         -- A2005?
         --           Result.Has_Null_Exclusion := Add_Has_Null_Exclusion;
         --           Result.Is_Not_Null_Return
         return Result;
      end Create_And_Return_Access_Type;

      -- Has side effects:
      function Create_And_Return_Aspect_Specification
        return a_nodes_h.Aspect_Specification_Struct is
      begin
         return
           (Aspect_Mark      => Add_And_Return_Aspect_Mark,
            Aspect_Definition => Add_And_Return_Aspect_Definition);
      end Create_And_Return_Aspect_Specification;

      -- Has side effects:
      function Create_And_Return_Component_Definition
        return a_nodes_h.Component_Definition_Struct
      is
         Result : a_nodes_h.Component_Definition_Struct :=
           a_nodes_h.Support.Default_Component_Definition_Struct;
      begin -- Create_And_Return_Component_Definition
         if Supported_Ada_Version < Ada_2005 then
            -- Component_Subtype_Indication should not be needed as long as
            -- Component_Definition_View is available.  See comment in
            -- Asis.Definitions under function Component_Subtype_Indication.
            Result.Component_Subtype_Indication :=
              Add_And_Return_Component_Subtype_Indication;
            --  Returns Definition_Kinds:
            --       A_Subtype_Indication
         else
            Result.Component_Definition_View :=
              Add_And_Return_Component_Definition_View;
            --  Returns Definition_Kinds:
            --       A_Subtype_Indication
            --       An_Access_Definition
         end if;
         Result.Has_Aliased := Add_And_Return_Has_Aliased;
         return Result;
      end Create_And_Return_Component_Definition;

      -- Has side effects:
      function Create_And_Return_Discrete_Range
        return a_nodes_h.Discrete_Range_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name &
           ".Create_And_Return_Discrete_Range";

         Result          : a_nodes_h.Discrete_Range_Struct :=
           a_nodes_h.Support.Default_Discrete_Range_Struct;
         Discrete_Range_Kind : constant Asis.Discrete_Range_Kinds :=
           Asis.Elements.Discrete_Range_Kind (Element);

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Discrete_Range_Kind", Discrete_Range_Kind'Image);
            Result.Discrete_Range_Kind := To_Discrete_Range_Kinds (Discrete_Range_Kind);
         end Add_Common_Items;

         use all type Asis.Discrete_Range_Kinds;
      begin -- Create_And_Return_Discrete_Range
         If Discrete_Range_Kind /= Not_A_Discrete_Range then
            Add_Common_Items;
         end if;

         case Discrete_Range_Kind is
         when Not_A_Discrete_Range =>
            raise Internal_Error with
            Module_Name & " called with: " & Discrete_Range_Kind'Image;
         when A_Discrete_Subtype_Indication =>
            Result.Subtype_Mark := Add_And_Return_Subtype_Mark;
            Result.Subtype_Constraint := Add_And_Return_Subtype_Constraint;
         when A_Discrete_Range_Attribute_Reference =>
            Result.Range_Attribute := Add_And_Return_Range_Attribute;
         when A_Discrete_Simple_Expression_Range =>
            Result.Lower_Bound := Add_And_Return_Lower_Bound;
            Result.Upper_Bound := Add_And_Return_Upper_Bound;
         end case;

         return Result;
      end Create_And_Return_Discrete_Range;

      -- Has side effects:
      function Create_And_Return_Discrete_Subtype_Definition
        return a_nodes_h.Discrete_Subtype_Definition_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name &
           ".Create_And_Return_Discrete_Discrete_Subtype_Definition";

         Result          : a_nodes_h.Discrete_Subtype_Definition_Struct :=
           a_nodes_h.Support.Default_Discrete_Subtype_Definition_Struct;
         Discrete_Range_Kind : constant Asis.Discrete_Range_Kinds :=
           Asis.Elements.Discrete_Range_Kind (Element);

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Discrete_Range_Kind", Discrete_Range_Kind'Image);
            Result.Discrete_Range_Kind := To_Discrete_Range_Kinds (Discrete_Range_Kind);
         end Add_Common_Items;

         use all type Asis.Discrete_Range_Kinds;
      begin -- Create_And_Return_Discrete_Subtype_Definition
         If Discrete_Range_Kind /= Not_A_Discrete_Range then
            Add_Common_Items;
         end if;

         case Discrete_Range_Kind is
         when Not_A_Discrete_Range =>
            raise Internal_Error with
            Module_Name & " called with: " & Discrete_Range_Kind'Image;
         when A_Discrete_Subtype_Indication =>
            Result.Subtype_Mark := Add_And_Return_Subtype_Mark;
            Result.Subtype_Constraint := Add_And_Return_Subtype_Constraint;
         when A_Discrete_Range_Attribute_Reference =>
            Result.Range_Attribute := Add_And_Return_Range_Attribute;
         when A_Discrete_Simple_Expression_Range =>
            Result.Lower_Bound := Add_And_Return_Lower_Bound;
            Result.Upper_Bound := Add_And_Return_Upper_Bound;
         end case;

         return Result;
      end Create_And_Return_Discrete_Subtype_Definition;

      -- Has side effects:
      function Create_And_Return_Constraint
        return a_nodes_h.Constraint_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name &
           ".Create_And_Return_Constraint";
         Result : a_nodes_h.Constraint_Struct :=
           a_nodes_h.Support.Default_Constraint_Struct;

         Constraint_Kind : constant Asis.Constraint_Kinds :=
           Asis.Elements.Constraint_Kind (Element);

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Constraint_Kind", Constraint_Kind'Image);
            Result.Constraint_Kind := To_Constraint_Kinds (Constraint_Kind);
         end Add_Common_Items;

         use all type Asis.Constraint_Kinds;
      begin -- Create_And_Return_Constraint
         if Constraint_Kind /= Not_A_Constraint then
            Add_Common_Items;
         end if;

         case Constraint_Kind is
         when Not_A_Constraint =>
            raise Internal_Error with
            Module_Name & " called with: " & Constraint_Kind'Image;
         when A_Range_Attribute_Reference =>
            Result.Range_Attribute := Add_And_Return_Range_Attribute;
         when A_Simple_Expression_Range =>
            Result.Lower_Bound := Add_And_Return_Lower_Bound;
            Result.Upper_Bound := Add_And_Return_Upper_Bound;
         when A_Digits_Constraint =>
            Result.Digits_Expression := Add_And_Return_Digits_Expression;
            Result.Real_Range_Constraint := Add_And_Return_Real_Range_Constraint;
         when A_Delta_Constraint =>
            Result.Delta_Expression := Add_And_Return_Delta_Expression;
            Result.Real_Range_Constraint := Add_And_Return_Real_Range_Constraint;
         when An_Index_Constraint =>
            Result.Discrete_Ranges := Add_And_Return_Discrete_Ranges;
         when A_Discriminant_Constraint =>
            Result.Discriminant_Associations := Add_And_Return_Discriminant_Associations;
         end case;

         return Result;
      end Create_And_Return_Constraint;

      -- Has side effects:
      function Create_And_Return_Formal_Type_Definition
        return a_nodes_h.Formal_Type_Definition_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name &
           ".Create_And_Return_Formal_Type_Definition";
         Result : a_nodes_h.Formal_Type_Definition_Struct :=
           a_nodes_h.Support.Default_Formal_Type_Definition_Struct;

         Formal_Type_Kind: constant Asis.Formal_Type_Kinds :=
           Asis.Elements.Formal_Type_Kind (Element);

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Formal_Type_Kind", Formal_Type_Kind'Image);
            Result.Formal_Type_Kind := To_Formal_Type_Kinds (Formal_Type_Kind);
            Result.Corresponding_Type_Operators := Add_And_Return_Corresponding_Type_Operators;
         end Add_Common_Items;

         use all type Asis.Formal_Type_Kinds;
      begin -- Create_And_Return_Formal_Type_Definition
         if Formal_Type_Kind /= Not_A_Formal_Type_Definition then
            Add_Common_Items;
         end if;

         case Formal_Type_Kind is
         when Not_A_Formal_Type_Definition =>
            raise Internal_Error with
            Module_Name & " called with: " & Formal_Type_Kind'Image;
         when A_Formal_Private_Type_Definition =>
            -- Has_Abstract is here for completeness, but it will never be true.
            -- See the comment in Asis.Definitins.Has_Abstract:
            Result.Has_Abstract := Add_And_Return_Has_Abstract;
            Result.Has_Limited := Add_And_Return_Has_Limited;
            Result.Has_Private := Add_And_Return_Has_Private;
             -- No child elements
         when A_Formal_Tagged_Private_Type_Definition =>
            Result.Has_Abstract := Add_And_Return_Has_Abstract;
            Result.Has_Limited := Add_And_Return_Has_Limited;
            Result.Has_Private := Add_And_Return_Has_Private;
            Result.Has_Tagged := Add_And_Return_Has_Tagged;
             -- No child elements
         when A_Formal_Derived_Type_Definition =>
            -- Has_Abstract is here for completeness, but it will never be true.
            -- See the comment in Asis.Definitins.Has_Abstract:
            Result.Has_Abstract := Add_And_Return_Has_Abstract;
            Result.Has_Limited := Add_And_Return_Has_Limited;
            Result.Has_Private := Add_And_Return_Has_Private;
            Result.Implicit_Inherited_Declarations :=
              Add_And_Return_Implicit_Inherited_Declarations;
            Result.Implicit_Inherited_Subprograms :=
              Add_And_Return_Implicit_Inherited_Subprograms;
            Result.Subtype_Mark := Add_And_Return_Subtype_Mark;
--              Result.Definition_Interface_List := -- A2005
--                Add_And_Return_Definition_Interface_List; -- A2005
--              Result.Has_Synchronized := -- A2005
--                Add_And_Return_Has_Synchronized; -- A2005
         when A_Formal_Discrete_Type_Definition |
              A_Formal_Signed_Integer_Type_Definition |
              A_Formal_Modular_Type_Definition |
              A_Formal_Floating_Point_Definition |
              A_Formal_Ordinary_Fixed_Point_Definition |
              A_Formal_Decimal_Fixed_Point_Definition =>
            null; -- No child elements
         when A_Formal_Interface_Type_Definition =>
            State.Add_Not_Implemented (Ada_2005);
         when A_Formal_Unconstrained_Array_Definition =>
            Result.Index_Subtype_Definitions :=
              Add_And_Return_Index_Subtype_Definitions;
            Result.Array_Component_Definition :=
              Add_And_Return_Array_Component_Definition;
         when A_Formal_Constrained_Array_Definition =>
            Result.Discrete_Subtype_Definitions :=
              Add_And_Return_Discrete_Subtype_Definitions;
            Result.Array_Component_Definition :=
              Add_And_Return_Array_Component_Definition;
         when A_Formal_Access_Type_Definition =>
            Result.Access_Type := Create_And_Return_Access_Type;
         end case;

         return Result;
      end Create_And_Return_Formal_Type_Definition;

      -- Has side effects:
      function Create_And_Return_Known_Discriminant_Part
        return a_nodes_h.Known_Discriminant_Part_Struct is
      begin
         return
           (Discriminants => Add_And_Return_Discriminants);
      end Create_And_Return_Known_Discriminant_Part;

      -- Has side effects:
      function Create_And_Return_Private_Extension_Definition
        return a_nodes_h.Private_Extension_Definition_Struct is
      begin
         return
           (Has_Abstract                    => Add_And_Return_Has_Abstract,
            Has_Limited                     => Add_And_Return_Has_Limited,
            Has_Private                     => Add_And_Return_Has_Private,
            Has_Synchronized                => Add_And_Return_Has_Synchronized,
            Implicit_Inherited_Declarations =>
              Add_And_Return_Implicit_Inherited_Declarations,
            Implicit_Inherited_Subprograms  =>
              Add_And_Return_Implicit_Inherited_Subprograms,
            Definition_Interface_List       =>
              Add_And_Return_Definition_Interface_List,
            Ancestor_Subtype_Indication     =>
              Add_And_Return_Ancestor_Subtype_Indication,
            Corresponding_Type_Operators =>
              Add_And_Return_Corresponding_Type_Operators);
      end Create_And_Return_Private_Extension_Definition;

      -- Has side effects:
      function Create_And_Return_Private_Type_Definition
        return a_nodes_h.Private_Type_Definition_Struct is
      begin
         return
           (Has_Abstract => Add_And_Return_Has_Abstract,
            Has_Limited  => Add_And_Return_Has_Limited,
            Has_Private  => Add_And_Return_Has_Private,
            Corresponding_Type_Operators =>
              Add_And_Return_Corresponding_Type_Operators);
      end Create_And_Return_Private_Type_Definition;

      -- Has side effects:
      function Create_And_Return_Protected_Definition
        return a_nodes_h.Protected_Definition_Struct is
      begin
         return
           (Has_Protected      => Add_And_Return_Has_Protected,
            Visible_Part_Items => Add_And_Return_Visible_Part_Items,
            Private_Part_Items => Add_And_Return_Private_Part_Items,
            Is_Private_Present => Add_And_Return_Is_Private_Present,
            Corresponding_Type_Operators =>
              Add_And_Return_Corresponding_Type_Operators);
      end Create_And_Return_Protected_Definition;

      -- Has side effects:
      function Create_And_Return_Subtype_Indication
        return a_nodes_h.Subtype_Indication_Struct is
      begin
         return
           (Has_Null_Exclusion => Add_And_Return_Has_Null_Exclusion,
            Subtype_Mark       => Add_And_Return_Subtype_Mark,
            Subtype_Constraint => Add_And_Return_Subtype_Constraint);
      end Create_And_Return_Subtype_Indication;

      -- Has side effects:
      function Create_And_Return_Subtype_Constraint
        return a_nodes_h.Constraint_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Create_And_Return_Subtype_Constraint";

         Result          : a_nodes_h.Constraint_Struct :=
           a_nodes_h.Support.Default_Constraint_Struct;
         Constraint_Kind : constant Asis.Constraint_Kinds :=
           Asis.Elements.Constraint_Kind (Element);

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Constraint_Kind", Constraint_Kind'Image);
            Result.Constraint_Kind := To_Constraint_Kinds (Constraint_Kind);
         end Add_Common_Items;

         use all type Asis.Constraint_Kinds;
      begin -- Create_And_Return_Subtype_Constraint
         If Constraint_Kind /= Not_A_Constraint then
            Add_Common_Items;
         end if;

         case Constraint_Kind is
         when Not_A_Constraint =>
            raise Internal_Error with
            Module_Name & " called with: " & Constraint_Kind'Image;
         when A_Range_Attribute_Reference =>
            Result.Range_Attribute := Add_And_Return_Range_Attribute;
         when A_Simple_Expression_Range =>
            Result.Lower_Bound := Add_And_Return_Lower_Bound;
            Result.Upper_Bound := Add_And_Return_Upper_Bound;
         when A_Digits_Constraint =>
            Result.Digits_Expression := Add_And_Return_Digits_Expression;
            Result.Real_Range_Constraint := Add_And_Return_Real_Range_Constraint;
         when A_Delta_Constraint =>
            Result.Delta_Expression := Add_And_Return_Delta_Expression;
            Result.Real_Range_Constraint := Add_And_Return_Real_Range_Constraint;
         when An_Index_Constraint =>
            Result.Discrete_Ranges := Add_And_Return_Discrete_Ranges;
         when A_Discriminant_Constraint =>
            Result.Discriminant_Associations := Add_And_Return_Discriminant_Associations;
         end case;

         return Result;
      end Create_And_Return_Subtype_Constraint;

      -- Has side effects:
      function Create_And_Return_Tagged_Private_Type_Definition
        return a_nodes_h.Tagged_Private_Type_Definition_Struct is
      begin
         return
           (Has_Abstract => Add_And_Return_Has_Abstract,
            Has_Limited  => Add_And_Return_Has_Limited,
            Has_Private  => Add_And_Return_Has_Private,
            Has_Tagged   => Add_And_Return_Has_Tagged,
            Corresponding_Type_Operators =>
              Add_And_Return_Corresponding_Type_Operators);
      end Create_And_Return_Tagged_Private_Type_Definition;

      -- Has side effects:
      function Create_And_Return_Type_Definition
        return a_nodes_h.Type_Definition_Struct
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Create_And_Return_Type_Definition";
         Result    : a_nodes_h.Type_Definition_Struct :=
           a_nodes_h.Support.Default_Type_Definition_Struct;

         Type_Kind : constant Asis.Type_Kinds :=
           Asis.Elements.Type_Kind (Element);

         -- Supporting procedures are in alphabetical order:
         procedure Add_Array_Component_Definition is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Definitions.Array_Component_Definition (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Array_Component_Definition", ID);
            Result.Array_Component_Definition := ID;
         end;

         procedure Add_Discrete_Subtype_Definitions is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Definitions.Discrete_Subtype_Definitions (Element),
               Dot_Label_Name => "Discrete_Subtype_Definitions",
               List_Out       => Result.Discrete_Subtype_Definitions,
               Add_Edges      => True);
         end;

         procedure Add_Enumeration_Literal_Declarations is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Definitions.Enumeration_Literal_Declarations (Element),
               Dot_Label_Name => "Enumeration_Literal_Declarations",
               List_Out       => Result.Enumeration_Literal_Declarations,
               Add_Edges      => True);
         end;

         procedure Add_Index_Subtype_Definitions is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Definitions.Index_Subtype_Definitions (Element),
               Dot_Label_Name => "Index_Subtype_Definitions",
               List_Out       => Result.Index_Subtype_Definitions,
               Add_Edges      => True);
         end;

         procedure Add_Integer_Constraint is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Definitions.Integer_Constraint (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Integer_Constraint", ID);
            Result.Integer_Constraint := ID;
         end;

         procedure Add_Mod_Static_Expression is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Definitions.Mod_Static_Expression (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Mod_Static_Expression", ID);
            Result.Mod_Static_Expression := ID;
         end;

         procedure Add_Root_Type_Kind is
            Value : Asis.Root_Type_Kinds := Asis.Elements.Root_Type_Kind (Element);
         begin
            State.Add_To_Dot_Label ("Root_Type_Kind", Value'Image);
            Result.Root_Type_Kind :=
              To_Root_Type_Kinds (Value);
         end;

         procedure Add_Common_Items is
         begin -- Add_Common_Items
            State.Add_To_Dot_Label ("Type_Kind", Type_Kind'Image);
            Result.Type_Kind := To_Type_Kinds (Type_Kind);
            Result.Has_Limited := Add_And_Return_Has_Limited;
            Result.Has_Private := Add_And_Return_Has_Private;
            Result.Corresponding_Type_Operators := Add_And_Return_Corresponding_Type_Operators;
         end Add_Common_Items;

         use all type Asis.Type_Kinds;
      begin -- Create_And_Return_Type_Definition
         If Type_Kind /= Not_A_Type_Definition then
            Add_Common_Items;
         end if;

         case Type_Kind is
         when Not_A_Type_Definition =>
            raise Internal_Error with
            Module_Name & " called with: " & Type_Kind'Image;
         when A_Derived_Type_Definition =>
            Result.Parent_Subtype_Indication := Add_And_Return_Parent_Subtype_Indication;
            Result.Implicit_Inherited_Declarations := Add_And_Return_Implicit_Inherited_Declarations;
            Result.Implicit_Inherited_Subprograms := Add_And_Return_Implicit_Inherited_Subprograms;
            Result.Corresponding_Parent_Subtype := Add_And_Return_Corresponding_Parent_Subtype;
            Result.Corresponding_Root_Type := Add_And_Return_Corresponding_Root_Type;
            Result.Corresponding_Type_Structure := Add_And_Return_Corresponding_Type_Structure;
         when A_Derived_Record_Extension_Definition =>
            Result.Has_Abstract := Add_And_Return_Has_Abstract;
            Result.Parent_Subtype_Indication := Add_And_Return_Parent_Subtype_Indication;
            Result.Record_Definition := Add_And_Return_Record_Definition;
            Result.Implicit_Inherited_Declarations := Add_And_Return_Implicit_Inherited_Declarations;
            Result.Implicit_Inherited_Subprograms := Add_And_Return_Implicit_Inherited_Subprograms;
            Result.Corresponding_Parent_Subtype := Add_And_Return_Corresponding_Parent_Subtype;
            Result.Corresponding_Root_Type := Add_And_Return_Corresponding_Root_Type;
            Result.Corresponding_Type_Structure := Add_And_Return_Corresponding_Type_Structure;
            Result.Definition_Interface_List := Add_And_Return_Definition_Interface_List;
         when An_Enumeration_Type_Definition =>
            Add_Enumeration_Literal_Declarations;
         when A_Signed_Integer_Type_Definition =>
            Add_Integer_Constraint;
         when A_Modular_Type_Definition =>
            Add_Mod_Static_Expression;
         when A_Root_Type_Definition =>
            Add_Root_Type_Kind;
            -- No more info
         when A_Floating_Point_Definition =>
            Result.Digits_Expression := Add_And_Return_Digits_Expression;
            Result.Real_Range_Constraint := Add_And_Return_Real_Range_Constraint;
         when An_Ordinary_Fixed_Point_Definition =>
            Result.Delta_Expression := Add_And_Return_Delta_Expression;
            Result.Real_Range_Constraint := Add_And_Return_Real_Range_Constraint;
         when A_Decimal_Fixed_Point_Definition =>
            Result.Digits_Expression := Add_And_Return_Digits_Expression;
            Result.Delta_Expression := Add_And_Return_Delta_Expression;
            Result.Real_Range_Constraint := Add_And_Return_Real_Range_Constraint;
         when An_Unconstrained_Array_Definition =>
            Add_Index_Subtype_Definitions;
            Add_Array_Component_Definition;
         when A_Constrained_Array_Definition =>
            Add_Discrete_Subtype_Definitions;
            Add_Array_Component_Definition;
         when A_Record_Type_Definition =>
            Result.Record_Definition := Add_And_Return_Record_Definition;
         when A_Tagged_Record_Type_Definition =>
            Result.Has_Tagged := Add_And_Return_Has_Tagged;
            Result.Has_Abstract := Add_And_Return_Has_Abstract;
            Result.Record_Definition := Add_And_Return_Record_Definition;
         when An_Interface_Type_Definition => -- A2005
            State.Add_Not_Implemented (Ada_2005);
         when An_Access_Type_Definition =>
            Result.Access_Type := Create_And_Return_Access_Type;
         end case;

         return Result;
      end Create_And_Return_Type_Definition;

      -- Has side effects:
      function Create_And_Return_Record_Definition
        return a_nodes_h.Record_Definition_Struct is
      begin
         return
           (Record_Components   => Add_And_Return_Record_Components,
            Implicit_Components => Add_And_Return_Implicit_Components);
      end Create_And_Return_Record_Definition;

      -- Has side effects:
      function Create_And_Return_Task_Definition
        return a_nodes_h.Task_Definition_Struct is
      begin
         return
           (Has_Task            => Add_And_Return_Has_Task,
            Visible_Part_Items  => Add_And_Return_Visible_Part_Items,
            Private_Part_Items  => Add_And_Return_Private_Part_Items,
            Is_Private_Present  => Add_And_Return_Is_Private_Present,
            Corresponding_Type_Operators =>
              Add_And_Return_Corresponding_Type_Operators);
      end Create_And_Return_Task_Definition;

      -- Has side effects:
      function Create_And_Return_Variant
        return a_nodes_h.Variant_Struct is
      begin
         return
           (Record_Components   => Add_And_Return_Record_Components,
            Implicit_Components => Add_And_Return_Implicit_Components,
            Variant_Choices     => Add_And_Return_Variant_Choices);
      end Create_And_Return_Variant;

      -- Has side effects:
      function Create_And_Return_Variant_Part
        return a_nodes_h.Variant_Part_Struct is
      begin
         return
           (Discriminant_Direct_Name => Add_And_Return_Discriminant_Direct_Name,
            Variants                 => Add_And_Return_Variants);
      end Create_And_Return_Variant_Part;

      -- END record support
      -----------------------------------------------------------------------

      Result : a_nodes_h.Definition_Struct :=
        a_nodes_h.Support.Default_Definition_Struct;

      Definition_Kind : constant Asis.Definition_Kinds :=
        Asis.Elements.Definition_Kind (Element);

      procedure Add_Common_Items is
      begin
         State.Add_To_Dot_Label ("Definition_Kind", Definition_Kind'Image);
         Result.Definition_Kind := To_Definition_Kinds (Definition_Kind);
      end Add_Common_Items;

      use all type Asis.Definition_Kinds;
   begin -- Do_Pre_Child_Processing
      If Definition_Kind /= Not_A_Definition then
         Add_Common_Items;
      end if;

      case Definition_Kind is
      when Not_A_Definition =>
         raise Internal_Error with
           Module_Name & " called with: " & Definition_Kind'Image;
      when A_Type_Definition =>
         Result.The_Union.The_Type_Definition :=
           Create_And_Return_Type_Definition;
      when A_Subtype_Indication =>
         Result.The_Union.The_Subtype_Indication :=
           Create_And_Return_Subtype_Indication;
      when A_Constraint =>
         Result.The_Union.The_Constraint := Create_And_Return_Constraint;
      when A_Component_Definition =>
         Result.The_Union.The_Component_Definition :=
           Create_And_Return_Component_Definition;
      when A_Discrete_Subtype_Definition =>
         Result.The_Union.The_Discrete_Subtype_Definition :=
           Create_And_Return_Discrete_Subtype_Definition;
      when A_Discrete_Range =>
         Result.The_Union.The_Discrete_Range :=
           Create_And_Return_Discrete_Range;
      when An_Unknown_Discriminant_Part =>
         null; -- No more components:
      when A_Known_Discriminant_Part =>
         Result.The_Union.The_Known_Discriminant_Part :=
           Create_And_Return_Known_Discriminant_Part;
      when A_Record_Definition =>
         Result.The_Union.The_Record_Definition :=
           Create_And_Return_Record_Definition;
      when A_Null_Record_Definition =>
         null; -- No more components
      when A_Null_Component =>
         null; -- No more components
      when A_Variant_Part =>
         Result.The_Union.The_Variant_Part := Create_And_Return_Variant_Part;
      when A_Variant =>
         Result.The_Union.The_Variant := Create_And_Return_Variant;
      when An_Others_Choice =>
         null; -- No more components
      when An_Access_Definition => -- A2005
         Result.The_Union.The_Access_Definition :=
           Create_And_Return_Access_Definition;
      when A_Private_Type_Definition =>
         Result.The_Union.The_Private_Type_Definition :=
           Create_And_Return_Private_Type_Definition;
      when A_Tagged_Private_Type_Definition =>
         Result.The_Union.The_Tagged_Private_Type_Definition :=
           Create_And_Return_Tagged_Private_Type_Definition;
      when A_Private_Extension_Definition =>
         Result.The_Union.The_Private_Extension_Definition :=
           Create_And_Return_Private_Extension_Definition;
      when A_Task_Definition =>
         Result.The_Union.The_Task_Definition :=
           Create_And_Return_Task_Definition;
      when A_Protected_Definition =>
         Result.The_Union.The_Protected_Definition :=
           Create_And_Return_Protected_Definition;
      when A_Formal_Type_Definition =>
         Result.The_Union.The_Formal_Type_Definition :=
           Create_And_Return_Formal_Type_Definition;
      when An_Aspect_Specification => -- A2012
         Result.The_Union.The_Aspect_Specification :=
           Create_And_Return_Aspect_Specification;
      end case;

      State.A_Element.Element_Kind := a_nodes_h.A_Definition;
      State.A_Element.The_Union.Definition := Result;
   end Do_Pre_Child_Processing;


   procedure Do_Post_Child_Processing
     (Element : in Asis.Element; State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Post_Child_Processing";
      Definition_Kind : constant Asis.Definition_Kinds :=
        Asis.Elements.Definition_Kind (Element);
      use all type Asis.Definition_Kinds;

      procedure Append_Implicit_Inherited_Declarations
      is
         elementList : Asis.Element_List := Asis.Definitions.Implicit_Inherited_Declarations (Element);
      begin
         for Elm of elementList loop
            declare
               -- Default_Node  : Dot.Node_Stmt.Class; -- Initialized
               Default_Label : Dot.HTML_Like_Labels.Class; -- Initialized
               Element_ID : constant a_nodes_h.Element_ID :=
                 Get_Element_ID (Elm);
               Element_Kind : constant Asis.Element_Kinds :=
               Asis.Elements.Element_Kind (Elm);
               SpecialCase : A4G.A_Types.Special_Cases := Asis.Set_Get.Special_Case(Elm);
            begin
               -- PUT_LINE(Special_Cases'Image (SpecialCase));
               If SpecialCase /= Stand_Char_Literal then
                 state.Process_Element_Tree(
                  Element => Elm,
                  Outputs => State.Outputs );
               end if;
            end;
         end loop;
      end Append_Implicit_Inherited_Declarations;

      -----------------------------------------------------------------------

   begin -- Do_Post_Child_Processing
     -- Currently this post-processing is only needed to complete
     -- the dot graph and node structure to include implicit elements.
     -- 
     -- The following comment from ASIS document lists the implicit element support,
     -- which should later be included in to the post processing here:
     --
     -- ASIS-for-GNAT supports implicit Elements for the following constructs:
     -- Derived user-defined subprograms
     -- Derived enumeration literals
     -- Derived record components

     If Definition_Kind = A_Type_Definition then
       declare
         Type_Kind : constant Asis.Type_Kinds :=
           Asis.Elements.Type_Kind (Element);
         use all type Asis.Type_Kinds;
       begin
         If Type_Kind = A_Derived_Type_Definition then
           Append_Implicit_Inherited_Declarations;
         end if;
       end;
     end if;
   end Do_Post_Child_Processing;

end Asis_Adapter.Element.Definitions;
