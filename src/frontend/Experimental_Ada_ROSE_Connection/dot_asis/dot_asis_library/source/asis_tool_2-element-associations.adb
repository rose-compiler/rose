with Asis.Elements;
with Asis.Expressions;

package body Asis_Tool_2.Element.Associations is

   ------------
   -- EXPORTED:
   ------------
   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element;
      State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
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
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Associations;
