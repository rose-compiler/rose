with Asis.Elements;
with Asis.Exceptions;
with Asis.Expressions;
with Asis.Extensions;
with Asis.Set_Get; use  Asis.Set_Get;
with A4G.Int_Knds; use A4G.Int_Knds;
package body Asis_Adapter.Element.Expressions is

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

      --Designator Expressions only applies to certain kinds of attributes
      procedure Add_Attribute_Designator_Expressions is
         Arg_Kind : constant Internal_Element_Kinds := Int_Kind (Element);
      begin
         case Arg_Kind is 
            when A_First_Attribute  |
                 A_Last_Attribute |
                 A_Length_Attribute |
                 A_Range_Attribute |
                 An_Implementation_Defined_Attribute |
                 An_Unknown_Attribute =>
               Add_Element_List
                 (This           => State,
                  Elements_In    => Asis.Expressions.Attribute_Designator_Expressions (Element),
                  Dot_Label_Name => "Attribute_Designator_Expressions",
                  List_Out       => Result.Attribute_Designator_Expressions,
                  Add_Edges      => True);
            when others => null;
         end case;
      end;
      
      procedure Add_Allocator_Qualified_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Allocator_Qualified_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Allocator_Qualified_Expression", ID);
         Result.Allocator_Qualified_Expression := ID;
      end;
      
      procedure Add_Allocator_Subtype_Indication is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Allocator_Subtype_Indication (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Allocator_Subtype_Indication", ID);
         Result.Allocator_Subtype_Indication := ID;
      end;
      
      procedure Add_Array_Component_Associations is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Expressions.
              Array_Component_Associations (Element),
            Dot_Label_Name => "Array_Component_Associations",
            List_Out       => Result.Array_Component_Associations,
            Add_Edges      => True);
      end;

      procedure Add_Attribute_Kind is
      begin
         State.Add_To_Dot_Label ("Attribute_Kind",
                                 Asis.Elements.Attribute_Kind (Element)'Image);
         Result.Attribute_Kind := To_Attribute_Kinds (Asis.Elements.Attribute_Kind (Element));
      end;
        
      
      procedure Add_Attribute_Designator_Identifier is
         ID : constant a_nodes_h.Element_ID := Get_Element_ID
           (Asis.Expressions.Attribute_Designator_Identifier (Element));
      begin
         State.Add_To_Dot_Label_And_Edge
           ("Attribute_Designator_Identifier", ID);
         Result.Attribute_Designator_Identifier :=
           ID;
      end;

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
         use Asis;
         ID : a_nodes_h.Element_ID := anhS.Invalid_Element_ID;
      begin
         if Asis.Extensions.Is_Uniquely_Defined (Element) then
            ID := Get_Element_ID
              (Asis.Expressions.Corresponding_Name_Declaration (Element));
         end if;
         --May be Invalid/Nil This is so we know if this value is not set
         State.Add_To_Dot_Label  
           ("Corresponding_Name_Declaration", To_String (ID));
         Result.Corresponding_Name_Declaration := ID;
      end;

      procedure Add_Corresponding_Name_Definition is
         ID : a_nodes_h.Element_ID := anhS.Invalid_Element_ID;
      begin
         if Asis.Extensions.Is_Uniquely_Defined (Element) then
            ID := Get_Element_ID
              (Asis.Expressions.Corresponding_Name_Definition (Element));
         end if;
         --May be Invalid/Nil This is so we know if this value is not set
         State.Add_To_Dot_Label
           ("Corresponding_Name_Definition", To_String (ID));
         Result.Corresponding_Name_Definition := ID;
      end;

      procedure Add_Corresponding_Name_Definition_List is
         use Asis;
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name &
           ".Add_Corresponding_Name_Definition_List";
      package Logging is new Generic_Logging (Module_Name); use Logging;

         procedure Add_List (Elements_In : in Asis.Element_List) is
         begin
            Add_Element_List
              (This           => State,
               Elements_In    => Elements_In,
               Dot_Label_Name => "Corresponding_Name_Definition_List",
               List_Out       => Result.Corresponding_Name_Definition_List);
         exception
            when X : Asis.Exceptions.Asis_Inappropriate_Element =>
               Log_Exception (X);
               Log ("Continuing...");
         end Add_List;
      begin
         if Asis.Extensions.Is_Uniquely_Defined (Element) then
            Add_List (Asis.Expressions.Corresponding_Name_Definition_List (Element));
         else
            Add_List (Asis.Nil_Element_List);
         end if;
      end;

      procedure Add_Expression_Paths is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Expressions.
              Expression_Paths (Element),
            Dot_Label_Name => "Expression_Paths",
            List_Out       => Result.Expression_Paths,
            Add_Edges      => True);
      end;
      
      procedure Add_Extension_Aggregate_Expression is
      ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Extension_Aggregate_Expression
                           (Element));
      begin
         State.Add_To_Dot_Label_And_Edge
           ("Extension_Aggregate_Expression", ID);
         Result.Extension_Aggregate_Expression := ID;
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
      
      procedure Add_Index_Expressions is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Expressions.
              Index_Expressions (Element),
            Dot_Label_Name => "Index_Expressions",
            List_Out       => Result.Index_Expressions,
            Add_Edges      => True);
      end;

      procedure Add_Membership_Test_Choices is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Expressions.
              Membership_Test_Choices (Element),
            Dot_Label_Name => "Membership_Test_Choices",
            List_Out       => Result.Membership_Test_Choices,
            Add_Edges      => True);
      end;

      procedure Add_Membership_Test_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Membership_Test_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Membership_Test_Expression", ID);
         Result.Membership_Test_Expression := ID;
      end;

      procedure Add_Name_Image is
         
         function TQS (This : in Wide_String) return String 
                       renames To_Quoted_String;
         
         procedure Add_It_Quoted (It : in Wide_String) is
         begin
            State.Add_To_Dot_Label ("Name_Image", TQS (It));
         end Add_It_Quoted;
         
         WS : constant Wide_String := Asis.Expressions.Name_Image (Element);
         S  : constant String := To_String(WS);
         
         -- If the name image contains < or >, graphviz will barf. Use the
         -- correct special character identifier for those:
         procedure Add_To_Dot_Label is
         begin
            -- Look for ">", "<", ">=", or "<=":
            If S = TQS (">") then
               Add_It_Quoted ("&gt;");
            elsif S = TQS ("<") then
               Add_It_Quoted ("&lt;");
            elsif S = TQS (">=") then
               Add_It_Quoted ("&gt;=");
            elsif S = TQS ("<=") then
               Add_It_Quoted ("&lt;=");           
            else
               -- Not adding quotes in order to reduce changes to dot files 
               -- during RC-511.  Created RC-524 to straighten this out.
               State.Add_To_Dot_Label ("Name_Image", WS);
            end if;
         end Add_To_Dot_Label;
         
      begin -- Add_Name_Image
         Result.Name_Image := To_Chars_Ptr (WS);
         Add_To_Dot_Label;
      end;

      procedure Add_Expression_Parenthesized is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Expression_Parenthesized (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Expression_Parenthesized", ID);
         Result.Expression_Parenthesized := ID;
      end;

      procedure Add_Is_Prefix_Call is
         Value : constant Boolean := Asis.Expressions.Is_Prefix_Call (Element);
      begin
         State.Add_To_Dot_Label ("Is_Prefix_Call", Value);
         Result.Is_Prefix_Call := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Prefix_Notation is
         Value : constant Boolean := Asis.Elements.Is_Prefix_Notation (Element);
      begin
         State.Add_To_Dot_Label ("Is_Prefix_Notation", Value);
         Result.Is_Prefix_Notation := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Prefix is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Prefix (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Prefix", ID);
         Result.Prefix := ID;
      end;
      
      procedure Add_Record_Component_Associations is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Expressions.
              Record_Component_Associations (Element),
            Dot_Label_Name => "Record_Component_Associations",
            List_Out       => Result.Record_Component_Associations,
            Add_Edges      => True);
      end;

      procedure Add_Selector is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Selector (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Selector", ID);
         Result.Selector := ID;
      end;

      procedure Add_Short_Circuit_Operation_Left_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Short_Circuit_Operation_Left_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Short_Circuit_Operation_Left_Expression", ID);
         Result.Short_Circuit_Operation_Left_Expression := ID;
      end;
      
      procedure Add_Short_Circuit_Operation_Right_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Short_Circuit_Operation_Right_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Short_Circuit_Operation_Right_Expression", ID);
         Result.Short_Circuit_Operation_Right_Expression := ID;
      end;
      
      procedure Add_Slice_Range is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Expressions.Slice_Range (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Slice_Range", ID);
         Result.Slice_Range := ID;
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
         Result.Expression_Kind := To_Expression_Kinds (Expression_Kind);
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
         when A_Box_Expression => -- A2005
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
            Add_Prefix; --
         when A_Function_Call =>
            Add_Is_Prefix_Notation;
            Add_Is_Prefix_Call;
            Add_Prefix;
            Add_Corresponding_Called_Function;
            Add_Function_Call_Parameters;
         when An_Indexed_Component =>
            Add_Index_Expressions;--
            Add_Prefix;
            --Corresponding_Called_Function; 2012 only
            --Is_Generatized_Indexing  2012 only
         when A_Slice =>
            Add_Prefix;--
            Add_Slice_Range;
         when A_Selected_Component =>
            Add_Prefix;--selected_component.ads
            Add_Selector;
         when An_Attribute_Reference =>
            Add_Attribute_Kind;
            Add_Prefix;
            Add_Attribute_Designator_Identifier;
            Add_Attribute_Designator_Expressions;
         when A_Record_Aggregate =>
            Add_Record_Component_Associations;
         when An_Extension_Aggregate =>
            Add_Record_Component_Associations;
            Add_Extension_Aggregate_Expression;          
         when A_Positional_Array_Aggregate =>
            Add_Array_Component_Associations;
         when A_Named_Array_Aggregate =>
            Add_Array_Component_Associations;
         when An_And_Then_Short_Circuit =>
            Add_Short_Circuit_Operation_Left_Expression;--short_circuit.adb
            Add_Short_Circuit_Operation_Right_Expression;
         when An_Or_Else_Short_Circuit =>
            Add_Short_Circuit_Operation_Left_Expression;--short_circuit.adb
            Add_Short_Circuit_Operation_Right_Expression;
         when An_In_Membership_Test =>  -- A2012
            Add_Membership_Test_Expression;
            Add_Membership_Test_Choices;
            State.Add_Not_Implemented (Ada_2012);
         when A_Not_In_Membership_Test => -- A2012
            Add_Membership_Test_Expression;
            Add_Membership_Test_Choices;
            State.Add_Not_Implemented (Ada_2012);
         when A_Null_Literal =>
            -- No more information:
            null;
         when A_Parenthesized_Expression =>
              Add_Expression_Parenthesized; --
         when A_Raise_Expression => -- A2012
            -- No more information:
            null;
            State.Add_Not_Implemented (Ada_2012);
         when A_Type_Conversion =>
            Add_Converted_Or_Qualified_Subtype_Mark;
            Add_Converted_Or_Qualified_Expression;
         when A_Qualified_Expression =>
            Add_Converted_Or_Qualified_Subtype_Mark;
            Add_Converted_Or_Qualified_Expression;
         when An_Allocation_From_Subtype =>
            Add_Allocator_Subtype_Indication;
            Add_Subpool_Name;
         when An_Allocation_From_Qualified_Expression =>
            Add_Allocator_Qualified_Expression;
            Add_Subpool_Name;
            State.Add_Not_Implemented;
         when A_Case_Expression => -- A2012
            Add_Expression_Paths;
            State.Add_Not_Implemented (Ada_2012);
         when An_If_Expression => -- A2012
            Add_Expression_Paths;
            State.Add_Not_Implemented (Ada_2012);
         when A_For_All_Quantified_Expression => -- A2012
            -- Iterator_Specification
            -- Predicate
              State.Add_Not_Implemented (Ada_2012);
         when A_For_Some_Quantified_Expression => -- A2012
            -- Iterator_Specification
            -- Predicate
            State.Add_Not_Implemented (Ada_2012);
      end case;

      State.A_Element.Element_Kind := a_nodes_h.An_Expression;
      State.A_Element.The_Union.Expression := Result;
   end Do_Pre_Child_Processing;

end Asis_Adapter.Element.Expressions;
