with Asis.Declarations;
with Asis.Definitions;
with Asis.Elements;
with Asis.Expressions;

package body Asis_Tool_2.Element.Declarations is

   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element;
      State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
      Result : a_nodes_h.Declaration_Struct :=
        a_nodes_h.Support.Default_Declaration_Struct;

      Declaration_Kind : Asis.Declaration_Kinds :=
        Asis.Elements.Declaration_Kind (Element);

      -- Supporting procedures are in alphabetical order:
      procedure Add_Aspect_Specifications is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Aspect_Specifications (Element),
            Dot_Label_Name => "Aspect_Specifications",
            List_Out       => Result.Aspect_Specifications,
            Add_Edges      => True);
      end;

      -- This is obsolete a/o Ada95, and should be removed:
      procedure Add_Body_Block_Statement is
      begin
         Result.Body_Block_Statement := anhS.Empty_ID;
      end;

      procedure Add_Body_Declarative_Items is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Body_Declarative_Items (Element),
            Dot_Label_Name => "Body_Declarative_Items",
            List_Out       => Result.Body_Declarative_Items,
            Add_Edges      => True);
      end;

      procedure Add_Body_Exception_Handlers is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Body_Exception_Handlers (Element),
            Dot_Label_Name => "Body_Exception_Handlers",
            List_Out       => Result.Body_Exception_Handlers,
            Add_Edges      => True);
      end;

      procedure Add_Body_Statements is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Body_Statements (Element),
            Dot_Label_Name => "Body_Statements",
            List_Out       => Result.Body_Statements,
            Add_Edges      => True);
      end;

      procedure Add_Corresponding_Base_Entity is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Base_Entity (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Base_Entity", ID);
         Result.Corresponding_Base_Entity := ID;
      end;

      procedure Add_Corresponding_Body is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Body (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Body", ID);
         Result.Corresponding_Body := ID;
      end;

      procedure Add_Corresponding_Body_Stub is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Body_Stub (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Body_Stub", ID);
         Result.Corresponding_Body_Stub := ID;
      end;

      procedure Add_Corresponding_Declaration is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Declaration (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Declaration", ID);
         Result.Corresponding_Declaration := ID;
      end;

      procedure Add_Corresponding_End_Name is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Elements.Corresponding_End_Name (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_End_Name", ID);
         Result.Corresponding_End_Name := ID;
      end;

      procedure Add_Corresponding_Equality_Operator is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Equality_Operator (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Equality_Operator", ID);
         Result.Corresponding_Equality_Operator := ID;
      end;

      procedure Add_Corresponding_First_Subtype is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_First_Subtype (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_First_Subtype", ID);
         Result.Corresponding_First_Subtype := ID;
      end;

      procedure Add_Corresponding_Last_Constraint is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Last_Constraint (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Last_Constraint", ID);
         Result.Corresponding_Last_Constraint := ID;
      end;

      procedure Add_Corresponding_Last_Subtype is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Last_Subtype (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Last_Subtype", ID);
         Result.Corresponding_Last_Subtype := ID;
      end;

      procedure Add_Corresponding_Parent_Subtype is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Parent_Subtype (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Parent_Subtype", ID);
         Result.Corresponding_Last_Subtype := ID;
      end;

      procedure Add_Corresponding_Pragmas is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Elements.Corresponding_Pragmas (Element),
            Dot_Label_Name => "Corresponding_Pragmas",
            List_Out       => Result.Corresponding_Pragmas,
            Add_Edges      => True);
      end;

      procedure Add_Corresponding_Representation_Clauses is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Corresponding_Representation_Clauses (Element),
            Dot_Label_Name => "Corresponding_Representation_Clauses",
            List_Out       => Result.Corresponding_Representation_Clauses);
      end;

      procedure Add_Corresponding_Root_Type is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Root_Type (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Root_Type", ID);
         -- Not in a_nodes?
         --            Result.Corresponding_Root_Type := ID;
         State.Add_Not_Implemented;
      end;

      procedure Add_Corresponding_Subunit is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Subunit (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Subunit", ID);
         Result.Corresponding_Subprogram_Derivation := ID;
      end;

      procedure Add_Corresponding_Subprogram_Derivation is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Subprogram_Derivation (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Subprogram_Derivation", ID);
         Result.Corresponding_Subprogram_Derivation := ID;
      end;

      procedure Add_Corresponding_Type is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Type (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Type", ID);
         Result.Corresponding_Type := ID;
      end;

      procedure Add_Corresponding_Type_Completion is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Type_Completion (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Type_Completion", ID);
         Result.Corresponding_Type_Completion := ID;
      end;

      procedure Add_Corresponding_Type_Declaration is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Type_Declaration (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Type_Declaration", ID);
         Result.Corresponding_Type_Declaration := ID;
      end;

      procedure Add_Corresponding_Type_Operators is begin
         -- No Corresponding_Type_Operators in Declaration_Struct:
         --              Add_Element_List
         --                (This           => State,
         --                 Elements_In    => Asis.Definitions.Corresponding_Type_Operators (Element),
         --                 Dot_Label_Name => "Corresponding_Type_Operators",
         --                 List_Out       => Result.Corresponding_Type_Operators,
         --                 Add_Edges      => True);

         State.Add_Not_Implemented;
      end;

      procedure Add_Corresponding_Type_Partial_View is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Corresponding_Type_Partial_View (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Type_Partial_View", ID);
         Result.Corresponding_Type_Partial_View := ID;
      end;

      procedure Add_Corresponding_Type_Structure is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Definitions.Corresponding_Type_Structure (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Type_Structure", ID);
         -- No Corresponding_Type_Operators in Declaration_Struct:
         --              Result.Corresponding_Type_Structure := ID;
         State.Add_Not_Implemented;
      end;

      procedure Add_Declaration_Interface_List is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Declaration_Interface_List (Element),
            Dot_Label_Name => "Declaration_Interface_List",
            List_Out       => Result.Declaration_Interface_List,
            Add_Edges      => True);
      end;

      procedure Add_Declaration_Kind is
         -- Hides same thing in outer scope:
         Value : Asis.Declaration_Kinds :=  Asis.Elements.Declaration_Kind (Element);
      begin
         State.Add_To_Dot_Label ("Declaration_Kind", Value'Image);
         Result.Declaration_Kind :=
           a_nodes_h.Support.To_Declaration_Kinds (Value);
      end;

      procedure Add_Default_Kind is
         -- Hides same thing in outer scope:
         Value : Asis.Subprogram_Default_Kinds :=  Asis.Elements.Default_Kind (Element);
      begin
         State.Add_To_Dot_Label ("Default_Kind", Value'Image);
         Result.Default_Kind :=
           a_nodes_h.Support.To_Subprogram_Default_Kinds (Value);
      end;

      procedure Add_Declaration_Origin is
         Value : Asis.Declaration_Origins :=  Asis.Elements.Declaration_Origin (Element);
      begin
         State.Add_To_Dot_Label ("Declaration_Origin", Value'Image);
         Result.Declaration_Origin :=
           a_nodes_h.Support.To_Declaration_Origins (Value);
      end;

      procedure Add_Discriminant_Part is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Discriminant_Part (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Discriminant_Part", ID);
         Result.Discriminant_Part := ID;
      end;

      -- Redirecting Add_Declaration_Subtype_Mark to Add_Object_Declaration_View.
      -- See Declaration_Subtype_Mark A2005 comment in asis.declarations.ads in function
      -- Object_Declaration_View.
      procedure Add_Object_Declaration_View;
      procedure Add_Declaration_Subtype_Mark renames Add_Object_Declaration_View;

      procedure Add_Entry_Barrier is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Entry_Barrier (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Entry_Barrier", ID);
         Result.Entry_Family_Definition := ID;
      end;

      procedure Add_Entry_Family_Definition is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Entry_Family_Definition (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Entry_Family_Definition", ID);
         Result.Entry_Family_Definition := ID;
      end;

      procedure Add_Entry_Index_Specification is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Entry_Index_Specification (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Entry_Index_Specification", ID);
         Result.Entry_Index_Specification := ID;
      end;

      procedure Add_Formal_Subprogram_Default is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Formal_Subprogram_Default (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Formal_Subprogram_Default", ID);
         Result.Formal_Subprogram_Default := ID;
      end;

      procedure Add_Generic_Actual_Part is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Generic_Actual_Part (Element),
            Dot_Label_Name => "Generic_Actual_Part",
            List_Out       => Result.Generic_Actual_Part,
            Add_Edges      => True);
      end;

      procedure Add_Generic_Formal_Part is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Generic_Formal_Part (Element),
            Dot_Label_Name => "Generic_Formal_Part",
            List_Out       => Result.Generic_Formal_Part,
            Add_Edges      => True);
      end;

      procedure Add_Generic_Unit_Name is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Generic_Unit_Name (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Generic_Unit_Name", ID);
         Result.Generic_Unit_Name := ID;
      end;

      procedure Add_Has_Abstract is
         Value : constant Boolean := Asis.Elements.Has_Abstract (Element);
      begin
         State.Add_To_Dot_Label ("Has_Abstract", Value);
         Result.Has_Abstract := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Has_Aliased is
         Value : constant Boolean := Asis.Elements.Has_Aliased (Element);
      begin
         State.Add_To_Dot_Label ("Has_Aliased", Value);
         Result.Has_Aliased := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Has_Limited is
         Value : constant Boolean := Asis.Elements.Has_Limited (Element);
      begin
         State.Add_To_Dot_Label ("Has_Limited", Value);
         Result.Has_Limited := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Has_Null_Exclusion is
         Value : constant Boolean := Asis.Elements.Has_Null_Exclusion (Element);
      begin
         State.Add_To_Dot_Label ("Has_Null_Exclusion", Value);
         Result.Has_Null_Exclusion := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Has_Private is
         Value : constant Boolean := Asis.Elements.Has_Private (Element);
      begin
         State.Add_To_Dot_Label ("Has_Private", Value);
         Result.Has_Private := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Has_Protected is
         Value : constant Boolean := Asis.Elements.Has_Protected (Element);
      begin
         State.Add_To_Dot_Label ("Has_Protected", Value);
         Result.Has_Protected := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Has_Reverse is
         Value : constant Boolean := Asis.Elements.Has_Reverse (Element);
      begin
         State.Add_To_Dot_Label ("Has_Reverse", Value);
         Result.Has_Reverse := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Has_Task is
         Value : constant Boolean := Asis.Elements.Has_Task (Element);
      begin
         State.Add_To_Dot_Label ("Has_Task", Value);
         Result.Has_Task := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Has_Tagged is
         Value : constant Boolean := Asis.Elements.Has_Tagged (Element);
      begin
         State.Add_To_Dot_Label ("Has_Tagged", Value);
         Result.Has_Task := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Initialization_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Initialization_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Initialization_Expression", ID);
         Result.Initialization_Expression := ID;
      end;

      procedure Add_Is_Dispatching_Operation is
         Value : constant Boolean := Asis.Declarations.Is_Dispatching_Operation (Element);
      begin
         State.Add_To_Dot_Label ("Is_Dispatching_Operation", Value);
         Result.Is_Dispatching_Operation := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Name_Repeated is
         Value : constant Boolean := Asis.Declarations.Is_Name_Repeated (Element);
      begin
         State.Add_To_Dot_Label ("Is_Name_Repeated", Value);
         Result.Is_Name_Repeated := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Not_Null_Return is
         Value : constant Boolean := Asis.Elements.Is_Not_Null_Return (Element);
      begin
         State.Add_To_Dot_Label ("Is_Not_Null_Return", Value);
         Result.Is_Not_Null_Return := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Not_Overriding_Declaration is
         Value : constant Boolean := Asis.Declarations.Is_Not_Overriding_Declaration (Element);
      begin
         State.Add_To_Dot_Label ("Is_Not_Overriding_Declaration", Value);
         Result.Is_Not_Overriding_Declaration := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Overriding_Declaration is
         Value : constant Boolean := Asis.Declarations.Is_Overriding_Declaration (Element);
      begin
         State.Add_To_Dot_Label ("Is_Overriding_Declaration", Value);
         Result.Is_Overriding_Declaration := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Private_Present is
         Value : constant Boolean := Asis.Declarations.Is_Private_Present (Element);
      begin
         State.Add_To_Dot_Label ("Is_Private_Present", Value);
         Result.Is_Private_Present := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Subunit is
         Value : constant Boolean := Asis.Declarations.Is_Subunit (Element);
      begin
         State.Add_To_Dot_Label ("Is_Subunit", Value);
         Result.Is_Subunit := a_nodes_h.Support.To_bool (Value);
         If Value then
            Add_Corresponding_Body_Stub;
         end if;
      end;

      -- Add_Iteration_Scheme_Name

      procedure Add_Iteration_Scheme_Name is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Iteration_Scheme_Name (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Iteration_Scheme_Name", ID);
         Result.Iteration_Scheme_Name := ID;
      end;

      procedure Add_Iterator_Specification is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Iterator_Specification (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Iterator_Specification", ID);
         -- No Iterator_Specification in a_nodes yet:
         -- TODO: Add:
         -- Result.Iterator_Specification := ID;
         State.Add_Not_Implemented;
      end;

      procedure Add_Mode_Kind is
         Value : constant Asis.Mode_Kinds :=  Asis.Elements.Mode_Kind (Element);
      begin
         State.Add_To_Dot_Label ("Mode_Kind", Value'Image);
         Result.Mode_Kind := a_nodes_h.Support.To_Mode_Kinds (Value);
      end;

      procedure Add_Names is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Names (Element),
            Dot_Label_Name => "Names",
            List_Out       => Result.Names,
            Add_Edges      => True);
      end;

      procedure Add_Object_Declaration_View is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Object_Declaration_View (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Object_Declaration_View", ID);
         Result.Object_Declaration_View := ID;
      end;

      procedure Add_Parameter_Profile is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Parameter_Profile (Element),
            Dot_Label_Name => "Parameter_Profile",
            List_Out       => Result.Parameter_Profile,
            Add_Edges      => True);
      end;

      procedure Add_Pragmas is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Elements.Pragmas (Element),
            Dot_Label_Name => "Pragmas",
            List_Out       => Result.Pragmas,
            Add_Edges      => True);
      end;

      procedure Add_Protected_Operation_Items is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Protected_Operation_Items (Element),
            Dot_Label_Name => "Protected_Operation_Items",
            List_Out       => Result.Protected_Operation_Items,
            Add_Edges      => True);
      end;


      procedure Add_Private_Part_Declarative_Items is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Private_Part_Declarative_Items (Element),
            Dot_Label_Name => "Private_Part_Declarative_Items",
            List_Out       => Result.Private_Part_Declarative_Items,
            Add_Edges      => True);
      end;

      procedure Add_Renamed_Entity is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Renamed_Entity (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Renamed_Entity", ID);
         Result.Renamed_Entity := ID;
      end;

      procedure Add_Result_Profile is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Result_Profile (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Result_Profile", ID);
         Result.Result_Profile := ID;
      end;

      procedure Add_Specification_Subtype_Definition is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Specification_Subtype_Definition (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Specification_Subtype_Definition", ID);
         Result.Specification_Subtype_Definition := ID;
      end;

      procedure Add_Type_Declaration_View is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Declarations.Type_Declaration_View (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Type_Declaration_View", ID);
         Result.Type_Declaration_View := ID;
      end;

      procedure Add_Visible_Part_Declarative_Items is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Declarations.Visible_Part_Declarative_Items (Element),
            Dot_Label_Name => "Visible_Part_Declarative_Items",
            List_Out       => Result.Visible_Part_Declarative_Items,
            Add_Edges      => True);
      end;

      procedure Add_Common_Items is
      begin
         Add_Declaration_Kind;
         Add_Declaration_Origin;
         Add_Corresponding_Pragmas;
         Add_Names;
         Add_Aspect_Specifications;
         Add_Corresponding_Representation_Clauses;
      end Add_Common_Items;

      use all type Asis.Declaration_Kinds;
   begin -- Process_Declaration
      If Declaration_Kind /= Not_A_Declaration then
         Add_Common_Items;
      end if;

      case Declaration_Kind is
      when Not_A_Declaration =>
         raise Program_Error with
         Module_Name & " called with: " & Declaration_Kind'Image;

      when An_Ordinary_Type_Declaration =>
         Add_Has_Abstract;
         Add_Has_Limited;
         Add_Discriminant_Part;
         Add_Type_Declaration_View;
         Add_Corresponding_Type_Declaration;
         Add_Corresponding_Type_Partial_View;
         Add_Corresponding_First_Subtype;
         Add_Corresponding_Last_Constraint;
         Add_Corresponding_Last_Subtype;

      when A_Task_Type_Declaration =>
         Add_Has_Task;
         Add_Corresponding_End_Name;
         Add_Discriminant_Part;
         Add_Type_Declaration_View;
         Add_Corresponding_Type_Declaration;
         Add_Corresponding_Type_Partial_View;
         Add_Corresponding_First_Subtype;
         Add_Corresponding_Last_Constraint;
         Add_Corresponding_Last_Subtype;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Corresponding_Body;
         Add_Declaration_Interface_List;

      when A_Protected_Type_Declaration =>
         Add_Has_Protected;
         Add_Corresponding_End_Name;
         Add_Discriminant_Part;
         Add_Type_Declaration_View;
         Add_Corresponding_Type_Declaration;
         Add_Corresponding_Type_Partial_View;
         Add_Corresponding_First_Subtype;
         Add_Corresponding_Last_Constraint;
         Add_Corresponding_Last_Subtype;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Corresponding_Body;
         Add_Declaration_Interface_List;

      when An_Incomplete_Type_Declaration =>
         Add_Corresponding_End_Name;
         Add_Initialization_Expression;
         Add_Discriminant_Part;
         Add_Corresponding_Type_Declaration;
         Add_Corresponding_Type_Completion;
         Add_Corresponding_Type_Partial_View;
         -- TODO: (2005)
         -- asis.limited_withs.ads (2005)
         --   Is_From_Limited_View

      when A_Tagged_Incomplete_Type_Declaration =>
         Add_Discriminant_Part; -- A2005
         Add_Corresponding_Type_Declaration; -- A2005
         Add_Corresponding_Type_Completion;
         Add_Has_Tagged;

      when A_Private_Type_Declaration =>
         Add_Has_Abstract;
         Add_Has_Limited;
         Add_Has_Private;
         Add_Discriminant_Part;
         Add_Type_Declaration_View;
         Add_Corresponding_Type_Declaration;
         Add_Corresponding_Type_Completion;
         Add_Corresponding_Type_Partial_View;
         Add_Corresponding_First_Subtype;
         Add_Corresponding_Last_Constraint;
         Add_Corresponding_Last_Subtype;

      when A_Private_Extension_Declaration =>
         Add_Has_Abstract;
         Add_Has_Limited;
         Add_Has_Private;
         Add_Discriminant_Part;
         Add_Type_Declaration_View;
         Add_Corresponding_Type_Declaration;
         Add_Corresponding_Type_Completion;
         Add_Corresponding_Type_Partial_View;
         Add_Corresponding_First_Subtype;
         Add_Corresponding_Last_Constraint;
         Add_Corresponding_Last_Subtype;

      when A_Subtype_Declaration =>
         Add_Type_Declaration_View;
         Add_Corresponding_First_Subtype;
         Add_Corresponding_Last_Constraint;
         Add_Corresponding_Last_Subtype;

      when A_Variable_Declaration =>
         Add_Object_Declaration_View;
         Add_Initialization_Expression;

      when A_Constant_Declaration =>
         Add_Object_Declaration_View;
         Add_Initialization_Expression;

      when A_Deferred_Constant_Declaration =>
         Add_Object_Declaration_View;

      when A_Single_Task_Declaration =>
         Add_Object_Declaration_View;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Corresponding_Body;
         Add_Declaration_Interface_List;
         Add_Has_Task;
         Add_Corresponding_End_Name;

      when A_Single_Protected_Declaration =>
         Add_Object_Declaration_View;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Corresponding_Body;
         Add_Declaration_Interface_List;
         Add_Has_Protected;
         Add_Corresponding_End_Name;

      when An_Integer_Number_Declaration =>
         Add_Initialization_Expression;

      when A_Real_Number_Declaration =>
         Add_Initialization_Expression;

      when An_Enumeration_Literal_Specification =>
         null; -- No more info

      when A_Discriminant_Specification =>
         Add_Object_Declaration_View; -- A2005
         Add_Initialization_Expression;
         Add_Declaration_Subtype_Mark;
         Add_Has_Null_Exclusion;

      when A_Component_Declaration =>
         Add_Object_Declaration_View;
         Add_Initialization_Expression;

      when A_Loop_Parameter_Specification =>
         Add_Specification_Subtype_Definition;
         Add_Has_Reverse;

      when A_Generalized_Iterator_Specification =>
         Add_Iterator_Specification;
         Add_Has_Reverse;

      when An_Element_Iterator_Specification =>
         Add_Iteration_Scheme_Name;
         Add_Iterator_Specification;
         Add_Has_Reverse;

      when A_Procedure_Declaration =>
         Add_Has_Abstract;
         Add_Is_Not_Null_Return;
         Add_Parameter_Profile;
         Add_Is_Overriding_Declaration;
         Add_Is_Not_Overriding_Declaration;
         Add_Corresponding_Declaration;
         Add_Corresponding_Body;
         Add_Corresponding_Subprogram_Derivation;
         Add_Corresponding_Type;
         Add_Is_Dispatching_Operation;

      when A_Function_Declaration =>
         Add_Has_Abstract;
         Add_Parameter_Profile;
         Add_Result_Profile;
         Add_Is_Overriding_Declaration;
         Add_Is_Not_Overriding_Declaration;
         Add_Corresponding_Declaration;
         Add_Corresponding_Body;
         Add_Corresponding_Subprogram_Derivation;
         Add_Corresponding_Type;
         Add_Corresponding_Equality_Operator;
         Add_Is_Dispatching_Operation;

      when A_Parameter_Specification =>
         Add_Has_Aliased;
         Add_Has_Null_Exclusion;
         Add_Mode_Kind;
         Add_Object_Declaration_View;
         Add_Initialization_Expression;

      when A_Procedure_Body_Declaration =>
         Add_Pragmas;
         Add_Corresponding_End_Name;
         Add_Parameter_Profile;
         Add_Is_Overriding_Declaration;
         Add_Is_Not_Overriding_Declaration;
         Add_Body_Declarative_Items;
         Add_Body_Statements;
         Add_Body_Exception_Handlers;
         Add_Body_Block_Statement;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Is_Subunit;
         Add_Is_Dispatching_Operation;

      when A_Function_Body_Declaration =>
         Add_Is_Not_Null_Return;
         Add_Pragmas;
         Add_Corresponding_End_Name;
         Add_Parameter_Profile;
         Add_Result_Profile;
         Add_Is_Overriding_Declaration;
         Add_Is_Not_Overriding_Declaration;
         Add_Body_Declarative_Items;
         Add_Body_Statements;
         Add_Body_Exception_Handlers;
         Add_Body_Block_Statement;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Is_Subunit;
         Add_Is_Dispatching_Operation;

      when A_Return_Variable_Specification =>
         State.Add_Not_Implemented; -- A2005

      when A_Return_Constant_Specification =>
         State.Add_Not_Implemented; -- A2005

      when A_Null_Procedure_Declaration =>
         State.Add_Not_Implemented; -- A2005

      when An_Expression_Function_Declaration =>
         State.Add_Not_Implemented; -- A2012

      when A_Package_Declaration =>
         Add_Pragmas;
         Add_Corresponding_End_Name;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Corresponding_Body;
         Add_Visible_Part_Declarative_Items;
         Add_Is_Private_Present;
         Add_Private_Part_Declarative_Items;

      when A_Package_Body_Declaration =>
         Add_Pragmas;
         Add_Corresponding_End_Name;
         Add_Body_Declarative_Items;
         Add_Body_Statements;
         Add_Body_Exception_Handlers;
         Add_Body_Block_Statement;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Is_Subunit;

      when An_Object_Renaming_Declaration =>
         Add_Object_Declaration_View; -- A2005
         Add_Declaration_Subtype_Mark;
         Add_Renamed_Entity;
         Add_Corresponding_Base_Entity;
         Add_Has_Null_Exclusion; -- A2005

      when An_Exception_Renaming_Declaration =>
         Add_Corresponding_Base_Entity;
         Add_Renamed_Entity;

      when A_Package_Renaming_Declaration =>
         Add_Corresponding_Base_Entity;
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Renamed_Entity;

      when A_Procedure_Renaming_Declaration =>
         Add_Corresponding_Base_Entity;
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_Subprogram_Derivation;
         Add_Is_Dispatching_Operation;
         Add_Is_Not_Overriding_Declaration;
         Add_Is_Overriding_Declaration;
         Add_Parameter_Profile;
         Add_Renamed_Entity;

      when A_Function_Renaming_Declaration =>
         Add_Corresponding_Base_Entity;
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_Equality_Operator;
         Add_Corresponding_Subprogram_Derivation;
         Add_Is_Dispatching_Operation;
         Add_Is_Not_Overriding_Declaration;
         Add_Is_Overriding_Declaration;
         Add_Parameter_Profile;
         Add_Renamed_Entity;
         Add_Result_Profile;
         --                 Add_Is_Not_Null_Return -- A2005

      when A_Generic_Package_Renaming_Declaration =>
         Add_Corresponding_Base_Entity;
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Renamed_Entity;

      when A_Generic_Procedure_Renaming_Declaration =>
         Add_Corresponding_Base_Entity;
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Renamed_Entity;

      when A_Generic_Function_Renaming_Declaration =>
         Add_Corresponding_Base_Entity;
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Renamed_Entity;

      when A_Task_Body_Declaration =>
         Add_Has_Task;
         Add_Pragmas;
         Add_Corresponding_End_Name;
         Add_Body_Declarative_Items;
         Add_Body_Statements;
         Add_Body_Exception_Handlers;
         Add_Body_Block_Statement;
         Add_Is_Name_Repeated;
         Add_Corresponding_Declaration;
         Add_Is_Subunit;

      when A_Protected_Body_Declaration =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_End_Name;
         Add_Has_Protected;
         Add_Is_Name_Repeated;
         Add_Is_Subunit;
         Add_Pragmas;
         Add_Protected_Operation_Items;

      when An_Entry_Declaration =>
         Add_Parameter_Profile;
         Add_Is_Overriding_Declaration;
         Add_Is_Not_Overriding_Declaration;
         Add_Corresponding_Body;
         Add_Entry_Family_Definition;

      when An_Entry_Body_Declaration =>
         Add_Body_Block_Statement;
         Add_Body_Declarative_Items;
         Add_Body_Exception_Handlers;
         Add_Body_Statements;
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_End_Name;
         Add_Entry_Barrier;
         Add_Entry_Index_Specification;
         Add_Is_Name_Repeated;
         Add_Parameter_Profile;
         Add_Pragmas;
         Add_Protected_Operation_Items;

      when An_Entry_Index_Specification =>
         Add_Specification_Subtype_Definition;

      when A_Procedure_Body_Stub =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_Subunit;
         Add_Is_Dispatching_Operation;
         Add_Is_Not_Overriding_Declaration;
         Add_Is_Overriding_Declaration;
         Add_Parameter_Profile;

      when A_Function_Body_Stub =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_Subunit;
         Add_Is_Dispatching_Operation;
         Add_Is_Not_Null_Return;
         Add_Is_Not_Overriding_Declaration;
         Add_Is_Overriding_Declaration;
         Add_Parameter_Profile;
         Add_Result_Profile;

      when A_Package_Body_Stub =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_Subunit;

      when A_Task_Body_Stub =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_Subunit;
         Add_Has_Task;

      when A_Protected_Body_Stub =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_Subunit;
         Add_Has_Protected;

      when An_Exception_Declaration =>
         null; -- No more info

      when A_Choice_Parameter_Specification =>
         null; -- No more info

      when A_Generic_Procedure_Declaration =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Generic_Formal_Part;
         Add_Parameter_Profile;
         Add_Pragmas;

      when A_Generic_Function_Declaration =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Generic_Formal_Part;
         Add_Is_Not_Null_Return;
         Add_Parameter_Profile;
         Add_Pragmas;
         Add_Result_Profile;

      when A_Generic_Package_Declaration =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Corresponding_End_Name;
         Add_Generic_Formal_Part;
         Add_Is_Name_Repeated;
         Add_Is_Private_Present;
         Add_Pragmas;
         Add_Private_Part_Declarative_Items;
         Add_Visible_Part_Declarative_Items;

      when A_Package_Instantiation =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Generic_Actual_Part;
         Add_Generic_Unit_Name;

      when A_Procedure_Instantiation =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Generic_Actual_Part;
         Add_Generic_Unit_Name;
         Add_Is_Not_Overriding_Declaration;
         Add_Is_Overriding_Declaration;

      when A_Function_Instantiation =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Generic_Actual_Part;
         Add_Generic_Unit_Name;
         Add_Is_Not_Overriding_Declaration;
         Add_Is_Overriding_Declaration;

      when A_Formal_Object_Declaration =>
         Add_Declaration_Subtype_Mark;
         -- Add_Has_Null_Exclusion; -- A2005
         Add_Initialization_Expression;
         -- Add_Object_Declaration_View; -- A2005
         Add_Mode_Kind;

      when A_Formal_Type_Declaration =>
         Add_Corresponding_First_Subtype;
         Add_Corresponding_Last_Constraint;
         Add_Corresponding_Last_Subtype;
         Add_Corresponding_Parent_Subtype;
         Add_Corresponding_Root_Type;
         Add_Corresponding_Type_Operators;
         Add_Corresponding_Type_Structure;
         Add_Discriminant_Part;
         Add_Type_Declaration_View;

      when A_Formal_Incomplete_Type_Declaration => -- A2012
         State.Add_Not_Implemented;

      when A_Formal_Procedure_Declaration =>
         Add_Default_Kind;
         Add_Formal_Subprogram_Default;
         Add_Has_Abstract;
         Add_Parameter_Profile;

      when A_Formal_Function_Declaration =>
         Add_Default_Kind;
         Add_Formal_Subprogram_Default;
         Add_Has_Abstract;
         Add_Parameter_Profile;
         Add_Result_Profile;
         -- Add_Is_Not_Null_Return -- A2005

      when A_Formal_Package_Declaration =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Generic_Unit_Name;
         Add_Generic_Actual_Part;

      when A_Formal_Package_Declaration_With_Box =>
         Add_Corresponding_Body;
         Add_Corresponding_Declaration;
         Add_Generic_Unit_Name;
         -- Add_Generic_Actual_Part; -- A2005
      end case;

      State.A_Element.Element_Kind := a_nodes_h.A_Declaration;
      State.A_Element.The_Union.Declaration := Result;
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Declarations;
