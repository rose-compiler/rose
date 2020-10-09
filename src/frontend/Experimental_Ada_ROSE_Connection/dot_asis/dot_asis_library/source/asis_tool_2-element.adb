with Asis.Compilation_Units;
with Asis.Elements;
with Asis.Iterator;
--  -- GNAT-specific:
with Asis.Set_Get;
with Asis.Text;

with Asis_Tool_2.Element.Associations;
with Asis_Tool_2.Element.Clauses;
with Asis_Tool_2.Element.Declarations;
with Asis_Tool_2.Element.Defining_Names;
with Asis_Tool_2.Element.Definitions;
with Asis_Tool_2.Element.Exception_Handlers;
with Asis_Tool_2.Element.Expressions;
with Asis_Tool_2.Element.Paths;
with Asis_Tool_2.Element.Pragmas;
with Asis_Tool_2.Element.Statements;

package body Asis_Tool_2.Element is

   ----------------------
   -- EXPORTED (private):
   ----------------------

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

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end;

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     a_nodes_h.Element_ID) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end;

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

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Value     => Value);
   end;

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

   ------------
   -- EXPORTED:
   ------------
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

   ------------
   -- EXPORTED:
   ------------
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

   ------------
   -- EXPORTED:
   ------------
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
   procedure Do_Pre_Child_Processing
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
            Pragmas.Do_Pre_Child_Processing (Element, State);
         when A_Defining_Name =>
            Defining_Names.Do_Pre_Child_Processing (Element, State);
         when A_Declaration =>
            Declarations.Do_Pre_Child_Processing (Element, State);
         when A_Definition =>
            Definitions.Do_Pre_Child_Processing (Element, State);
         when An_Expression =>
            Expressions.Do_Pre_Child_Processing (Element, State);
         when An_Association =>
            Associations.Do_Pre_Child_Processing (Element, State);
         when A_Statement =>
            Statements.Do_Pre_Child_Processing (Element, State);
         when A_Path =>
            Paths.Do_Pre_Child_Processing (Element, State);
         when A_Clause =>
            Clauses.Do_Pre_Child_Processing (Element, State);
         when An_Exception_Handler =>
            Exception_Handlers.Do_Pre_Child_Processing (Element, State);
      end case;
      --           Add_Enclosing_Element;
      Finish_Output;
   end Do_Pre_Child_Processing;

   procedure Do_Post_Child_Processing
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Class) is
   begin
      State.Outputs.Text.End_Line;
      State.Outputs.Text.Dedent;
      State.Outputs.Text.Put_Indented_Line
        (String'("END " & To_String (State.Element_IDs.First_Element)));
      State.Element_IDs.Delete_First;
   end Do_Post_Child_Processing;

   -- Call Pre_Operation on ths element, call Traverse_Element on all children,
   -- then call Post_Operation on this element:
   procedure Traverse_Element is new
     Asis.Iterator.Traverse_Element
       (State_Information => Class,
        Pre_Operation     => Do_Pre_Child_Processing,
        Post_Operation    => Do_Post_Child_Processing);

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

