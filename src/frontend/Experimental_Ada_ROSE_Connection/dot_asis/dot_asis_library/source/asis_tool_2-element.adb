with Asis.Clauses;
with Asis.Compilation_Units;
with Asis.Declarations;
with Asis.Definitions;
with Asis.Elements;
with Asis.Expressions;
with Asis.Exceptions;
with Asis.Iterator;
with Asis.Statements;
-- GNAT-specific:
with Asis.Set_Get;
with Asis.Text;

package body Asis_Tool_2.Element is

   Module_Name : constant String := "Asis_Tool_2.Element";

   -- String
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Name      => Name,
                        Value     => Value);
   end;

   -- Wide_String
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end;

   -- Element_ID
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     a_nodes_h.Element_ID) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end;

   -- Boolean
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Boolean) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Name      => Name,
                        Value     => Value);
   end;

   -- String:
   -- Add <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Value     => Value);
   end;

   -- Add to dot label: ASIS_PROCESSING => "NOT_IMPLEMENTED_COMPLETELY"
   procedure Add_Not_Implemented
     (This : in out Class) is
   begin
      This.Add_To_Dot_Label ("ASIS_PROCESSING", String'("NOT_IMPLEMENTED_COMPLETELY"));
      This.Outputs.A_Nodes.Add_Not_Implemented;
   end Add_Not_Implemented;

   procedure Add_Dot_Edge
     (This  : in out Class;
      From  : in     a_nodes_h.Element_ID;
      To    : in     a_nodes_h.Element_ID;
      Label : in     String)
   is
   begin
      Add_Dot_Edge (Outputs   => This.Outputs,
                    From      => From,
                    From_Kind => Element_ID_Kind,
                    To        => To,
                    To_Kind   => Element_ID_Kind,
                    Label     => Label);
   end Add_Dot_Edge;

   -- Add an edge and a dot label:
   procedure Add_To_Dot_Label_And_Edge
     (This  : in out Class;
      Label : in     String;
      To    : in     a_nodes_h.Element_ID) is
   begin
      This.Add_To_Dot_Label (Label, To_String (To));
      This.Add_Dot_Edge (From  => This.Element_IDs.First_Element,
                         To    => To,
                         Label => Label);
   end Add_To_Dot_Label_And_Edge;

   -----------------------------------------------------------------------------
   -- Element_ID support

   ------------
   -- EXPORTED:
   ------------
   function Get_Element_ID
     (Element : in Asis.Element)
      return Element_ID
   is
     ((Node_ID => Asis.Set_Get.Node_Value (Element),
       Kind    => Asis.Set_Get.Int_Kind (Element)));

   ------------
   -- EXPORTED:
   ------------
   function To_Element_ID
     (This : in Element_ID)
      return a_nodes_h.Element_ID
   is
      Result : Integer;
   begin
      Result := Integer (This.Node_ID) * 1000 +
        A4G.Int_Knds.Internal_Element_Kinds'Pos(This.Kind);
      return a_nodes_h.Element_ID (Result);
   end To_Element_ID;

   ------------
   -- EXPORTED:
   ------------
   function Get_Element_ID
     (Element : in Asis.Element)
      return a_nodes_h.Element_ID
   is
      (To_Element_ID (Get_Element_ID (Element)));

   ------------
   -- EXPORTED:
   ------------
   function To_String
     (This : in a_nodes_h.Element_ID)
      return String
   is
     (To_String (This, Element_ID_Kind));

   -- END Element_ID support
   -----------------------------------------------------------------------------

   ------------
   -- EXPORTED:
   ------------
   function To_Element_ID_List
     (Dot_Label       : in out Dot.HTML_Like_Labels.Class;
      Outputs         : in out Outputs_Record;
      This_Element_ID : in     a_nodes_h.Element_ID;
      Elements_In     : in     Asis.Element_List;
      Dot_Label_Name  : in     String;
      Add_Edges       : in     Boolean := False;
      This_Is_Unit    : in     Boolean := False)
     return  a_nodes_h.Element_ID_List
   is
      Element_Count : constant Natural := Elements_In'Length;
      IDs           : anhS.Element_ID_Array_Access := new
        anhS.Element_ID_Array (1 .. Element_Count);
      IDs_Index     : Positive := IDs'First;
   begin
      for Element of Elements_In loop
         declare
            Element_ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Element);
            Label      : constant String :=
              Dot_Label_Name & " (" & IDs_Index'Image & ")";
         begin
            IDs (IDs_Index) := Element_ID;
            Add_To_Dot_Label (Dot_Label => Dot_Label,
                              Outputs   => Outputs,
                              Name      => Label,
                              Value     => To_String (Element_ID));
            if Add_Edges then
               Add_Dot_Edge (Outputs   => Outputs,
                             From      => This_Element_ID,
                             From_Kind => (if This_Is_Unit
                                           then Unit_Id_Kind
                                           else Element_ID_Kind),
                             To        => Element_ID,
                             To_Kind   => Element_ID_Kind,
                             Label     => Label);
            end if;
            IDs_Index := IDs_Index + 1;
         end;
      end loop;
      return
        (length => Interfaces.C.int(Element_Count),
         IDs    => anhS.To_Element_ID_Ptr (IDs));
   end To_Element_ID_List;

   function To_Element_ID_List
     (This           : in out Class;
      Elements_In    : in     Asis.Element_List;
      Dot_Label_Name : in     String;
      Add_Edges      : in     Boolean := False)
     return  a_nodes_h.Element_ID_List is
   begin
      return To_Element_ID_List (Dot_Label       => This.Dot_Label,
                                 Outputs         => This.Outputs,
                                 This_Element_ID => This.Element_IDs.First_Element,
                                 Elements_In     => Elements_In,
                                 Dot_Label_Name  => Dot_Label_Name,
                                 Add_Edges       => Add_Edges);
   end To_Element_ID_List;

   procedure Add_Element_List
     (This           : in out Class;
      Elements_In    : in     Asis.Element_List;
      Dot_Label_Name : in     String;
      List_Out       :    out a_nodes_h.Element_ID_List;
      Add_Edges      : in     Boolean := False) is
   begin
      List_Out := To_Element_ID_List (This           => This,
                                      Elements_In    => Elements_In,
                                      Dot_Label_Name => Dot_Label_Name,
                                      Add_Edges      => Add_Edges);
   end Add_Element_List;

   function Add_Operator_Kind
     (State   : in out Class;
      Element : in     Asis.Element)
      return a_nodes_h.Operator_Kinds
   is
      Operator_Kind : constant Asis.Operator_Kinds :=
        Asis.Elements.Operator_Kind (Element);
   begin
      State.Add_To_Dot_Label ("Operator_Kind", Operator_Kind'Image);
      return anhS.To_Operator_Kinds (Operator_Kind);
   end;

   package Pre_Children is

      procedure Process_Element
        (Element : in     Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class);

   end Pre_Children;

   package body Pre_Children is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Pre_Children";

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Pragma
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Pragma";
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

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Defining_Name
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Defining_Name";
         Result : a_nodes_h.Defining_Name_Struct :=
           a_nodes_h.Support.Default_Defining_Name_Struct;

         Defining_Name_Kind : Asis.Defining_Name_Kinds :=
           Asis.Elements.Defining_Name_Kind (Element);

         -- Supporting procedures are in alphabetical order:
         procedure Add_Defining_Name_Image is
            WS : constant Wide_String := Asis.Declarations.Defining_Name_Image (Element);
         begin
            State.Add_To_Dot_Label ("Defining_Name_Image", To_Quoted_String (WS));
            Result.Defining_Name_Image := To_Chars_Ptr(WS);
         end;

         procedure Add_Defining_Prefix is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Declarations.Defining_Prefix (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Defining_Prefix", ID);
            Result.Defining_Prefix := ID;
         end;

         procedure Add_Defining_Selector is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Declarations.Defining_Selector (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Defining_Selector", ID);
            Result.Defining_Selector := ID;
         end;

         procedure Add_Position_Number_Image is
            WS : constant Wide_String := Asis.Declarations.Position_Number_Image (Element);
         begin
            State.Add_To_Dot_Label ("Position_Number_Image", To_String (WS));
            Result.Position_Number_Image := To_Chars_Ptr(WS);
         end;

         procedure Add_Representation_Value_Image is
            WS : constant Wide_String := Asis.Declarations.Representation_Value_Image (Element);
         begin
            State.Add_To_Dot_Label ("Representation_Value_Image", To_String (WS));
            Result.Representation_Value_Image := To_Chars_Ptr(WS);
         end;

         -- True if this is the name of a constant or a deferred constant.
         -- TODO: Implement
         function Is_Constant return Boolean is
           (False);

         procedure Add_Corresponding_Constant_Declaration is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID
                (Asis.Declarations.Corresponding_Constant_Declaration (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Constant_Declaration", To_String(ID));
            Result.Corresponding_Constant_Declaration := ID;
         end;

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label
              (Name => "Defining_Name_Kind", Value => Defining_Name_Kind'Image);
            Result.Defining_Name_Kind :=
              anhS.To_Defining_Name_Kinds (Defining_Name_Kind);
            Add_Defining_Name_Image;
         end Add_Common_Items;

         use all type Asis.Defining_Name_Kinds;
      begin
         If Defining_Name_Kind /= Not_A_Defining_Name then
            Add_Common_Items;
         end if;

         case Defining_Name_Kind is
            when Not_A_Defining_Name =>
               raise Program_Error with
               Module_Name & " called with: " & Defining_Name_Kind'Image;

            when A_Defining_Identifier =>
               null; -- No more info

            when A_Defining_Character_Literal |
                 A_Defining_Enumeration_Literal =>
               Add_Position_Number_Image;
               Add_Representation_Value_Image;

            when A_Defining_Operator_Symbol =>
               Result.Operator_Kind := Add_Operator_Kind (State, Element);

            when A_Defining_Expanded_Name =>
               Add_Defining_Prefix;
               Add_Defining_Selector;
         end case;

         if Is_Constant then
            Add_Corresponding_Constant_Declaration;
         end if;

         State.A_Element.Element_Kind := a_nodes_h.A_Defining_Name;
         State.A_Element.The_Union.Defining_Name := Result;
      end Process_Defining_Name;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Declaration
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Declaration";
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
      end Process_Declaration;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Definition
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Definition";

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
         State.A_Element.The_Union.Definition := Result;
      end Process_Definition;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Expression
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Expression";
         Result : a_nodes_h.Expression_Struct :=
           a_nodes_h.Support.Default_Expression_Struct;

         Expression_Kind : Asis.Expression_Kinds :=
           Asis.Elements.Expression_Kind (Element);

         -- Supporting procedures are in alphabetical order:
         procedure Add_Converted_Or_Qualified_Expression is
            ID : constant a_nodes_h.Element_ID := Get_Element_ID
              (Asis.Expressions.Converted_Or_Qualified_Expression (Element));
         begin
            State.Add_To_Dot_Label_And_Edge
              ("Converted_Or_Qualified_Expression", ID);
            Result.Converted_Or_Qualified_Expression :=
              ID;
         end;

         procedure Add_Converted_Or_Qualified_Subtype_Mark is
            ID : constant a_nodes_h.Element_ID := Get_Element_ID
              (Asis.Expressions.Converted_Or_Qualified_Subtype_Mark (Element));
         begin
            State.Add_To_Dot_Label_And_Edge
              ("Converted_Or_Qualified_Subtype_Mark", ID);
            Result.Converted_Or_Qualified_Subtype_Mark :=
              ID;
         end;

         procedure Add_Corresponding_Called_Function is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Expressions.Corresponding_Called_Function
                                 (Element));
         begin
            State.Add_To_Dot_Label
              ("Corresponding_Called_Function", To_String (ID));
            Result.Corresponding_Called_Function := ID;
         end;

         procedure Add_Corresponding_Expression_Type is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Expressions.Corresponding_Expression_Type
                                 (Element));
         begin
            State.Add_To_Dot_Label
              ("Corresponding_Expression_Type", To_String (ID));
            Result.Corresponding_Expression_Type := ID;
         end;

         procedure Add_Corresponding_Name_Declaration is
            Parent_Name : constant String := Module_Name;
            Module_Name : constant String := Parent_Name &
              ".Add_Corresponding_Name_Declaration";
            procedure Log (Message : in Wide_String) is
            begin
               Put_Line (Module_Name & ":  " & To_String (Message));
            end;

            ID : a_nodes_h.Element_ID := anhS.Invalid_Element_ID;
         begin
            begin
               ID := Get_Element_ID
                 (Asis.Expressions.Corresponding_Name_Declaration (Element));
            exception
               when X : Asis.Exceptions.Asis_Inappropriate_Element =>
                  Log_Exception (X);
                  Log ("Continuing...");
            end;
            State.Add_To_Dot_Label
              ("Corresponding_Name_Declaration", To_String (ID));
            Result.Corresponding_Name_Declaration := ID;
         end;

         procedure Add_Corresponding_Name_Definition is
            Parent_Name : constant String := Module_Name;
            Module_Name : constant String := Parent_Name &
              ".Add_Corresponding_Name_Definition";
            procedure Log (Message : in Wide_String) is
            begin
               Put_Line (Module_Name & ":  " & To_String (Message));
            end;

            ID : a_nodes_h.Element_ID := anhS.Invalid_Element_ID;
         begin
            begin
               ID := Get_Element_ID
                 (Asis.Expressions.Corresponding_Name_Definition (Element));
            exception
               when X : Asis.Exceptions.Asis_Inappropriate_Element =>
                  Log_Exception (X);
                  Log ("Continuing...");
            end;
            State.Add_To_Dot_Label
              ("Corresponding_Name_Definition", To_String (ID));
            Result.Corresponding_Name_Definition := ID;
         end;

         procedure Add_Corresponding_Name_Definition_List is
            Parent_Name : constant String := Module_Name;
            Module_Name : constant String := Parent_Name &
              ".Add_Corresponding_Name_Definition_List";
            procedure Log (Message : in Wide_String) is
            begin
               Put_Line (Module_Name & ":  " & To_String (Message));
            end;
         begin
            begin
               Add_Element_List
                 (This           => State,
                  Elements_In    => Asis.Expressions.
                    Corresponding_Name_Definition_List (Element),
                  Dot_Label_Name => "Corresponding_Name_Definition_List",
                  List_Out       => Result.Corresponding_Name_Definition_List);
            exception
               when X : Asis.Exceptions.Asis_Inappropriate_Element =>
                  Log_Exception (X);
                  Log ("Continuing...");
            end;
         end;

         procedure Add_Function_Call_Parameters is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Expressions.
                 Function_Call_Parameters (Element),
               Dot_Label_Name => "Function_Call_Parameters",
               List_Out       => Result.Function_Call_Parameters,
               Add_Edges      => True);
         end;

         procedure Add_Name_Image is
            WS : constant Wide_String := Asis.Expressions.Name_Image (Element);
         begin
            State.Add_To_Dot_Label ("Name_Image", To_Quoted_String (WS));
            Result.Name_Image := To_Chars_Ptr (WS);
         end;

         procedure Add_Prefix is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Expressions.Prefix (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Prefix", ID);
            Result.Prefix := ID;
         end;

         procedure Add_Selector is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Expressions.Selector (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Selector", ID);
            Result.Selector := ID;
         end;

         procedure Add_Subpool_Name is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Expressions.Subpool_Name (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Subpool_Name", ID);
            Result.Subpool_Name := ID;
         end;

         procedure Add_Value_Image is
            WS : constant Wide_String := Asis.Expressions.Value_Image (Element);
            use all type Asis.Expression_Kinds;
         begin
            State.Add_To_Dot_Label
              ("Value_Image",
               (if Expression_Kind = A_String_Literal then
                     To_Quoted_String (WS)
                else
                     To_String (WS)));
            Result.Value_Image := To_Chars_Ptr(WS);
         end;

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Expression_Kind", Expression_Kind'Image);
            Result.Expression_Kind := anhS.To_Expression_Kinds (Expression_Kind);
            Add_Corresponding_Expression_Type;
         end Add_Common_Items;

         use all type Asis.Expression_Kinds;
      begin
         If Expression_Kind /= Not_An_Expression then
            Add_Common_Items;
         end if;

         case Expression_Kind is
            when Not_An_Expression =>
               raise Program_Error with
                 Module_Name & " called with: " & Expression_Kind'Image;
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
               Result.Operator_Kind := Add_Operator_Kind (State, Element);
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

         State.A_Element.Element_Kind := a_nodes_h.An_Expression;
         State.A_Element.The_Union.Expression := Result;
      end Process_Expression;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Association
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Association";
         Result : a_nodes_h.Association_Struct :=
           a_nodes_h.Support.Default_Association_Struct;

         Association_Kind : Asis.Association_Kinds :=
           Asis.Elements.Association_Kind (Element);

         procedure Add_Formal_Parameter is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Expressions.Formal_Parameter (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Formal_Parameter", ID);
            Result.Formal_Parameter := ID;
         end;

         procedure Add_Actual_Parameter is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Expressions.Actual_Parameter (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Actual_Parameter", ID);
            Result.Actual_Parameter := ID;
         end;

         procedure Add_Is_Defaulted_Association is
            Value : constant Boolean := Asis.Expressions.Is_Defaulted_Association (Element);
         begin
            State.Add_To_Dot_Label ("Is_Defaulted_Association", Value);
            Result.Is_Defaulted_Association :=
              a_nodes_h.Support.To_bool (Value);
         end;

         procedure Add_Is_Normalized is
            Value : constant Boolean := Asis.Expressions.Is_Normalized (Element);
         begin
            State.Add_To_Dot_Label ("Is_Normalized", Value);
            Result.Is_Normalized := a_nodes_h.Support.To_bool (Value);
         end;

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Association_Kind", Association_Kind'Image);
            Result.Association_Kind := anhS.To_Association_Kinds (Association_Kind);
         end Add_Common_Items;

         use all type Asis.Association_Kinds;
      begin
         If Association_Kind /= Not_An_Association then
            Add_Common_Items;
         end if;

         case Association_Kind is
            when Not_An_Association =>                         -- An unexpected element
               raise Program_Error with
               Module_Name & " called with: " & Association_Kind'Image;
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
         State.A_Element.The_union.association := Result;
      end Process_Association;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Statement
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Statement";
         Result : a_nodes_h.Statement_Struct :=
           a_nodes_h.Support.Default_Statement_Struct;

         Statement_Kind : constant Asis.Statement_Kinds :=
           Asis.Elements.Statement_Kind (Element);

         -- Supporting procedures are in alphabetical order:
         procedure Add_Accept_Body_Exception_Handlers is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Accept_Body_Exception_Handlers (Element),
               Dot_Label_Name => "Accept_Body_Exception_Handlers",
               List_Out       => Result.Accept_Body_Exception_Handlers,
               Add_Edges      => True);
         end;

         procedure Add_Accept_Body_Statements is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Accept_Body_Statements (Element),
               Dot_Label_Name => "Accept_Body_Statements",
               List_Out       => Result.Accept_Body_Statements,
               Add_Edges      => True);
         end;

         procedure Add_Accept_Entry_Direct_Name is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Accept_Entry_Direct_Name (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Accept_Entry_Direct_Name", ID);
            Result.Accept_Entry_Direct_Name := ID;
         end;

         procedure Add_Accept_Entry_Index is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Accept_Entry_Index (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Accept_Entry_Index", ID);
            Result.Accept_Entry_Index := ID;
         end;

         procedure Add_Accept_Parameters is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Accept_Parameters (Element),
               Dot_Label_Name => "Accept_Parameters",
               List_Out       => Result.Accept_Parameters,
               Add_Edges      => True);
         end;

         procedure Add_Assignment_Expression is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Assignment_Expression (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Assignment_Expression", ID);
            Result.Assignment_Expression := ID;
         end;

         procedure Add_Assignment_Variable_Name is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Assignment_Variable_Name (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Assignment_Variable_Name", ID);
            Result.Assignment_Variable_Name := ID;
         end;

         procedure Add_Associated_Message is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Associated_Message (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Associated_Message", ID);
            Result.Associated_Message := ID;
         end;

         procedure Add_Call_Statement_Parameters is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Call_Statement_Parameters (Element),
               Dot_Label_Name => "Call_Statement_Parameters",
               List_Out       => Result.Call_Statement_Parameters,
               Add_Edges      => True);
         end;

         procedure Add_Called_Name is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Called_Name (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Called_Name", ID);
            Result.Called_Name := ID;
         end;

         procedure Add_Case_Expression is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Case_Expression (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Case_Expression", ID);
            Result.Case_Expression := ID;
         end;

         procedure Add_Corresponding_Called_Entity is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Corresponding_Called_Entity (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Called_Entity", To_String (ID));
            Result.Corresponding_Called_Entity := ID;
         end;

         procedure Add_Corresponding_Entry is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Corresponding_Entry (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Entry", To_String (ID));
            Result.Corresponding_Entry := ID;
         end;

         procedure Add_Corresponding_Loop_Exited is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Corresponding_Loop_Exited (Element));
         begin
            State.Add_To_Dot_Label ("Corresponding_Loop_Exited", To_String (ID));
            Result.Corresponding_Loop_Exited := ID;
         end;

         procedure Add_Exit_Condition is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Exit_Condition (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Exit_Condition", ID);
            Result.Exit_Condition := ID;
         end;

         procedure Add_Exit_Loop_Name is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Exit_Loop_Name (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Exit_Loop_Name", ID);
            Result.Exit_Loop_Name := ID;
         end;

         procedure Add_For_Loop_Parameter_Specification is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.For_Loop_Parameter_Specification (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("For_Loop_Parameter_Specification", ID);
            Result.For_Loop_Parameter_Specification := ID;
         end;

         procedure Add_Is_Name_Repeated is
            Value : constant Boolean := Asis.Statements.Is_Name_Repeated (Element);
         begin
            State.Add_To_Dot_Label ("Is_Name_Repeated", Value);
            Result.Is_Name_Repeated := a_nodes_h.Support.To_bool (Value);
         end;

         procedure Add_Label_Names is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Label_Names (Element),
               Dot_Label_Name => "Label_Names",
               List_Out       => Result.Label_Names,
               Add_Edges      => True);
         end;

         procedure Add_Loop_Statements is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Loop_Statements (Element),
               Dot_Label_Name => "Loop_Statements",
               List_Out       => Result.Loop_Statements,
               Add_Edges      => True);
         end;

         procedure Add_Raised_Exception is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Raised_Exception (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Raised_Exception", ID);
            Result.Raised_Exception := ID;
         end;

         procedure Add_Return_Expression is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Return_Expression (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Return_Expression", ID);
            Result.Return_Expression := ID;
         end;

         procedure Add_Statement_Identifier is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Statement_Identifier (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Statement_Identifier", ID);
            Result.Statement_Identifier := ID;
         end;

         procedure Add_Statement_Paths is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Statement_Paths (Element),
               Dot_Label_Name => "Statement_Paths",
               List_Out       => Result.Statement_Paths,
               Add_Edges      => True);
         end;

         procedure Add_While_Condition is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.While_Condition (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("While_Condition", ID);
            Result.While_Condition := ID;
         end;

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Statement_Kind", Statement_Kind'Image);
            Result.Statement_Kind := anhS.To_Statement_Kinds (Statement_Kind);
            Add_Label_Names;
         end Add_Common_Items;

         procedure Add_Common_Loop_Items is
         begin
            Add_Statement_Identifier;
            Add_Is_Name_Repeated;
            Add_Loop_Statements;
         end Add_Common_Loop_Items;


         use all type Asis.Statement_Kinds;
      begin
         If Statement_Kind /= Not_A_Statement then
            Add_Common_Items;
         end if;

         case Statement_Kind is
            when Not_A_Statement =>
               raise Program_Error with
               Module_Name & " called with: " & Statement_Kind'Image;

            when A_Null_Statement =>
               null; -- No more info.

            when An_Assignment_Statement =>
               Add_Assignment_Variable_Name;
               Add_Assignment_Expression;

            when An_If_Statement =>
               Add_Statement_Paths;

            when A_Case_Statement =>
               Add_Case_Expression;
               Add_Statement_Paths;

            when A_Loop_Statement =>
               Add_Common_Loop_Items;

            when A_While_Loop_Statement =>
               Add_While_Condition;
               Add_Common_Loop_Items;

            when A_For_Loop_Statement =>
               Add_For_Loop_Parameter_Specification;
               Add_Common_Loop_Items;

            when A_Block_Statement =>
               State.Add_Not_Implemented;
               -- Statement_Identifier
               -- Is_Name_Repeated
               -- Is_Declare_Block
               -- Block_Declarative_Items
               -- Block_Statements
               -- Block_Exception_Handlers

            when An_Exit_Statement =>
               Add_Exit_Condition;
               Add_Exit_Loop_Name;
               Add_Corresponding_Loop_Exited;

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
         State.A_Element.the_union.statement := Result;
      end Process_Statement;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Path
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Path";
         Result : a_nodes_h.Path_Struct :=
           a_nodes_h.Support.Default_Path_Struct;

         Path_Kind : constant Asis.Path_Kinds := Asis.Elements.Path_Kind (Element);

         procedure Add_Case_Path_Alternative_Choices is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Case_Path_Alternative_Choices (Element),
               Dot_Label_Name => "Case_Path_Alternative_Choices",
               List_Out       => Result.Case_Path_Alternative_Choices,
               Add_Edges      => True);
         end;

         procedure Add_Condition_Expression is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Condition_Expression (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Condition_Expression", ID);
            Result.Condition_Expression := ID;
         end;

         procedure Add_Guard is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Guard (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Guard", ID);
            Result.Guard := ID;
         end;

         procedure Add_Sequence_Of_Statements is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Sequence_Of_Statements (Element),
               Dot_Label_Name => "Sequence_Of_Statements",
               List_Out       => Result.Sequence_Of_Statements,
               Add_Edges      => True);
         end;

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Path_Kind", Path_Kind'Image);
            Result.Path_Kind := anhS.To_Path_Kinds (Path_Kind);
            Add_Sequence_Of_Statements;
         end Add_Common_Items;

         use all type Asis.Path_Kinds;
      begin
         If Path_Kind /= Not_A_Path then
            Add_Common_Items;
         end if;

         case Path_Kind is
            when Not_A_Path =>
               raise Program_Error with
               Module_Name & " called with: " & Path_Kind'Image;
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
         State.A_Element.The_Union.Path := Result;
      end Process_Path;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Clause
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Clause";
         Result : a_nodes_h.Clause_Struct :=
           a_nodes_h.Support.Default_Clause_Struct;

         Clause_Kind : constant Asis.Clause_Kinds :=
           Asis.Elements.Clause_Kind (Element);

         procedure Add_Clause_Names is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Clauses.Clause_Names (Element),
               Dot_Label_Name => "Clause_Name",
               List_Out       => Result.Clause_Names,
               Add_Edges      => True);
         end;

         procedure Add_Has_Limited is
            Value : constant Boolean := Asis.Elements.Has_Limited (Element);
         begin
            State.Add_To_Dot_Label ("Has_Limited", Value);
            Result.Has_Limited := a_nodes_h.Support.To_bool (Value);
         end;

         procedure Add_Common_Items is
         begin
            State.Add_To_Dot_Label ("Clause_Kind", Clause_Kind'Image);
            Result.Clause_Kind := anhS.To_Clause_Kinds (Clause_Kind);
         end Add_Common_Items;

         use all type Asis.Clause_Kinds;
      begin
         If Clause_Kind /= Not_A_Clause then
            Add_Common_Items;
         end if;

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
               Add_Has_Limited;
               Add_Clause_Names;
            when A_Representation_Clause =>
         --                                         -> Representation_Clause_Kinds
               State.Add_Not_Implemented;
            when A_Component_Clause =>
               State.Add_Not_Implemented;
         end case;

         State.A_Element.Element_Kind := a_nodes_h.A_Clause;
         State.A_Element.the_union.clause := Result;
      end Process_Clause;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      procedure Process_Exception_Handler
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Exception_Handler";
         Result : a_nodes_h.Exception_Handler_Struct :=
           a_nodes_h.Support.Default_Exception_Handler_Struct;

         procedure Add_Choice_Parameter_Specification is
            ID : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Statements.Choice_Parameter_Specification
                                 (Element));
         begin
            State.Add_To_Dot_Label_And_Edge ("Choice_Parameter_Specification", ID);
            Result.Choice_Parameter_Specification :=
              ID;
         end;

         procedure Add_Exception_Choices is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Exception_Choices (Element),
               Dot_Label_Name => "Exception_Choices",
               List_Out       => Result.Exception_Choices,
               Add_Edges      => True);
         end;

         procedure Add_Handler_Statements is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Handler_Statements (Element),
               Dot_Label_Name => "Handler_Statements",
               List_Out       => Result.Handler_Statements,
               Add_Edges      => True);
         end;

      begin
         Add_Choice_Parameter_Specification;
         Add_Exception_Choices;
         Add_Handler_Statements;

         State.A_Element.Element_Kind := a_nodes_h.An_Exception_Handler;
         State.A_Element.the_union.exception_handler := Result;
      end Process_Exception_Handler;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

--        function Enclosing_ID
--          (Element : in Asis.Element)
--           return Dot.ID_Type
--        is
--           Result : Dot.ID_Type; -- Initilaized
--           Enclosing_Element_Id : constant a_nodes_h.Element_ID :=
--             Get_Element_ID (Asis.Elements.Enclosing_Element (Element));
--           Enclosing_Unit_Id : constant A4G.A_Types.Unit_Id :=
--             Asis.Set_Get.Encl_Unit_Id (Element);
--           function Enclosing_Is_Element return boolean
--             is (Types."/=" (Enclosing_Element_Id, Types.Empty));
--        begin
--           if Enclosing_Is_Element then
--              Result := To_Dot_ID_Type (Enclosing_Element_Id);
--           else
--              Result := To_Dot_ID_Type (Enclosing_Unit_Id);
--           end if;
--           return Result;
--        end Enclosing_ID;

      function Enclosing_ID
        (Element : in Asis.Element)
         return a_nodes_h.Element_ID is
      begin
         return Get_Element_ID
              (Asis.Elements.Enclosing_Element (Element));
      end Enclosing_ID;

      function Spec_Or_Body_Image
        (Unit_Class : in Asis.Unit_Classes)
         return String
      is
         use all type Asis.Unit_Classes;
      begin
         case Unit_Class is
            when Not_A_Class =>
               return "";
            when A_Public_Declaration |
                 A_Private_Declaration =>
               return ".ads";
            when A_Public_Body |
                 A_Private_Body |
                 A_Public_Declaration_And_Body |
                 A_Separate_Body =>
               return ".adb";
         end case;
      end Spec_Or_Body_Image;

      function Source_Location_Image
        (Unit_Name  : in String;
         Span       : in Asis.Text.Span)
         return String is
      begin
         return Unit_Name & " - " &
           NLB_Image (Span.First_Line) & ":" & NLB_Image (Span.First_Column) &
           " .. " &
           NLB_Image (Span.Last_Line) & ":" & NLB_Image (Span.Last_Column);
      end Source_Location_Image;

      --------------------------------------------------------------------------
      --------------------------------------------------------------------------

      ------------
      -- EXPORTED:
      ------------
      procedure Process_Element
        (Element : in     Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class)
      is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Process_Element";
         Result : a_nodes_h.Element_Struct renames State.A_Element;

         Element_Kind : constant Asis.Element_Kinds :=
           Asis.Elements.Element_Kind (Element);

         procedure Add_Element_ID is begin
            -- ID is in the Dot node twice (once in the Label and once in
            -- Node_ID), but not in the a_node twice.
            State.Add_To_Dot_Label (To_String (State.Element_IDs.First_Element));
            Result.id := State.Element_IDs.First_Element;
         end;

         procedure Add_Element_Kind is begin
            State.Add_To_Dot_Label ("Element_Kind", Element_Kind'Image);
            Result.Element_Kind := anhS.To_Element_Kinds (Element_Kind);
         end;

         procedure Add_Source_Location is
            Unit : constant Asis.Compilation_Unit :=
              Asis.Elements.Enclosing_Compilation_Unit (Element);
            Unit_Class : constant Asis.Unit_Classes :=
              Asis.Compilation_Units.Unit_Class (Unit);
            Unit_Name : constant String := To_String
              (Asis.Compilation_Units.Unit_Full_Name (Unit)) &
              Spec_Or_Body_Image (Unit_Class);
            Span : constant Asis.Text.Span := Asis.Text.Element_Span (Element);

            Image : constant string := Source_Location_Image
              (Unit_Name, Span);
         begin
            State.Add_To_Dot_Label ("Source", Image);
            Result.Source_Location :=
              (Unit_Name    => To_Chars_Ptr (Unit_Name),
               First_Line   => Interfaces.C.int (Span.First_Line),
               First_Column => Interfaces.C.int (Span.First_Column),
               Last_Line    => Interfaces.C.int (Span.Last_Line),
               Last_Column  => Interfaces.C.int (Span.Last_Column));
         end;

         -- Alphabetical order:

         procedure Add_Is_Part_Of_Implicit is
            Value : constant Boolean := Asis.Elements.Is_Part_Of_Implicit (Element);
         begin
            State.Add_To_Dot_Label ("Is_Part_Of_Implicit", Value);
            Result.Is_Part_Of_Implicit := a_nodes_h.Support.To_bool (Value);
         end;

         procedure Add_Is_Part_Of_Inherited is
            Value : constant Boolean := Asis.Elements.Is_Part_Of_Inherited (Element);
         begin
            State.Add_To_Dot_Label ("Is_Part_Of_Inherited", Value);
            Result.Is_Part_Of_Inherited := a_nodes_h.Support.To_bool (Value);
         end;

         procedure Add_Is_Part_Of_Instance is
            Value : constant Boolean := Asis.Elements.Is_Part_Of_Instance (Element);
         begin
            State.Add_To_Dot_Label ("Is_Part_Of_Instance", Value);
            Result.Is_Part_Of_Instance := a_nodes_h.Support.To_bool (Value);
         end;

         procedure Add_Hash is
            Value : constant Asis.ASIS_Integer := Asis.Elements.Hash (Element);
         begin
            State.Add_To_Dot_Label ("Hash", Value'Image);
            Result.Hash := a_nodes_h.ASIS_Integer (Value);
         end;

         procedure Add_Enclosing_Element is
             Value : constant a_nodes_h.Element_ID :=
              Get_Element_ID (Asis.Elements.Enclosing_Element (Element));
         begin
--              State.Add_Dot_Edge (From  => Enclosing_Element_Id,
--                              To    => State.Element_Id,
--                              Label => "Child");
            State.Add_To_Dot_Label ("Enclosing_Element", Value);
            Result.Enclosing_Element_Id := Value;
         end;

         function Debug_Image return ICS.chars_ptr is
            WS : constant Wide_String := Asis.Elements.Debug_Image (Element);
         begin
            State.Add_To_Dot_Label ("Debug_Image", To_Quoted_String (WS));
            return To_Chars_Ptr(WS);
         end;

         procedure Put_Debug is
            procedure Put_Line (Name  : in String;
                                Value : in String) is
            begin
               State.Outputs.Text.Put_Indented_Line ("* " & Name & " => " & Value);
            end;
            procedure Put_Separator is
            begin
               State.Outputs.Text.Put_Indented_Line (String'(1 .. 40 => '*'));
            end;
            use Asis.Set_Get;
         begin
            Put_Separator;
            Put_Line ("Node              ", Node (Element)'Image);
            Put_Line ("R_Node            ", R_Node (Element)'Image);
            Put_Line ("Node_Field_1      ", Node_Field_1 (Element)'Image);
            Put_Line ("Node_Field_2      ", Node_Field_2 (Element)'Image);
            Put_Line ("Node_Value        ", Node_Value (Element)'Image);
            Put_Line ("R_Node_Value      ", R_Node_Value (Element)'Image);
            Put_Line ("Node_Field_1_Value", Node_Field_1_Value (Element)'Image);
            Put_Line ("Node_Field_2_Value", Node_Field_2_Value (Element)'Image);
--              Put_Line ("Encl_Unit         ", Encl_Unit (Element)'Image);
            Put_Line ("Encl_Unit_Id      ", Encl_Unit_Id (Element)'Image);
--              Put_Line ("Encl_Cont         ", Encl_Cont (Element)'Image);
            Put_Line ("Encl_Cont_Id      ", Encl_Cont_Id (Element)'Image);
            Put_Line ("Kind              ", Kind (Element)'Image);
            Put_Line ("Int_Kind          ", Int_Kind (Element)'Image);
            Put_Line ("Is_From_Implicit  ", Is_From_Implicit (Element)'Image);
            Put_Line ("Is_From_Inherited ", Is_From_Inherited (Element)'Image);
            Put_Line ("Is_From_Instance  ", Is_From_Instance (Element)'Image);
            Put_Line ("Special_Case      ", Special_Case (Element)'Image);
            Put_Line ("Normalization_Case", Normalization_Case (Element)'Image);
            Put_Line ("Parenth_Count     ", Parenth_Count (Element)'Image);
            Put_Line ("Encl_Tree         ", Encl_Tree (Element)'Image);
            Put_Line ("Rel_Sloc          ", Rel_Sloc (Element)'Image);
            Put_Line ("Character_Code    ", Character_Code (Element)'Image);
--              Put_Line ("Obtained          ", Obtained (Element)'Image);
            Put_Separator;
         end;

         procedure Start_Output is
            Default_Node  : Dot.Node_Stmt.Class; -- Initialized
            Default_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         begin
            -- Set defaults:
            Result := a_nodes_h.Support.Default_Element_Struct;
            State.Outputs.Text.End_Line;
            -- Element ID comes out on next line via Add_Element_ID:
            State.Outputs.Text.Put_Indented_Line (String'("BEGIN "));
            State.Outputs.Text.Indent;
            State.Dot_Node := Default_Node;
            State.Dot_Label := Default_Label;

            -- Get ID:
            State.Element_IDs.Prepend (Get_Element_ID (Element));
            State.Dot_Node.Node_ID.ID :=
              To_Dot_ID_Type (State.Element_IDs.First_Element, Element_ID_Kind);

            -- Result.Debug_Image := Debug_Image;
            -- Put_Debug;
            Add_Element_ID;
            Add_Element_Kind;
            Add_Is_Part_Of_Implicit;
            Add_Is_Part_Of_Inherited;
            Add_Is_Part_Of_Instance;
            Add_Hash;
            Add_Enclosing_Element;
            Add_Source_Location;
         end;

         procedure Finish_Output is
         begin
            State.Dot_Node.Add_Label (State.Dot_Label);
            State.Outputs.Graph.Append_Stmt
              (new Dot.Node_Stmt.Class'(State.Dot_Node));
            State.Outputs.A_Nodes.Push (Result);
         end;

         use all type Asis.Element_Kinds;
      begin
         If Element_Kind /= Not_An_Element then
            Start_Output;
         end if;

         case Element_Kind is
            when Not_An_Element =>
               raise Program_Error with
               Module_Name & " called with: " & Element_Kind'Image;
            when A_Pragma =>
               Process_Pragma (Element, State);
            when A_Defining_Name =>
               Process_Defining_Name (Element, State);
            when A_Declaration =>
               Process_Declaration (Element, State);
            when A_Definition =>
               Process_Definition (Element, State);
            when An_Expression =>
               Process_Expression (Element, State);
            when An_Association =>
               Process_Association (Element, State);
            when A_Statement =>
               Process_Statement (Element, State);
            when A_Path =>
               Process_Path (Element, State);
            when A_Clause =>
               Process_Clause (Element, State);
            when An_Exception_Handler =>
               Process_Exception_Handler (Element, State);
         end case;
--           Add_Enclosing_Element;
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
         State.Outputs.Text.End_Line;
         State.Outputs.Text.Dedent;
         State.Outputs.Text.Put_Indented_Line
           (String'("END " & To_String (State.Element_IDs.First_Element)));
         State.Element_IDs.Delete_First;
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
      Outputs : in     Outputs_Record)
   is
      Process_Control : Asis.Traverse_Control := Asis.Continue;
   begin
      -- I would like to just pass Outputs through and not store it in the
      -- object, since it is all pointers and we doesn't need to store their
      -- values between calls to Process_Element_Tree. Outputs has to go into
      -- State_Information in the Traverse_Element instatiation, though, so
      -- we'll put it in the object and pass that:
      This.Outputs := Outputs;
      Traverse_Element
        (Element => Element,
         Control => Process_Control,
         State   => This);
   end Process_Element_Tree;

end Asis_Tool_2.Element;

