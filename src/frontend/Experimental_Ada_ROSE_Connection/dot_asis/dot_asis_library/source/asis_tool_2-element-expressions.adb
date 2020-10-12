with Asis.Elements;
with Asis.Exceptions;
with Asis.Expressions;

package body Asis_Tool_2.Element.Expressions is

   -----------------------------
   -- Do_Pre_Child_Processing --
   -----------------------------

   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element; State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
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
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Expressions;
