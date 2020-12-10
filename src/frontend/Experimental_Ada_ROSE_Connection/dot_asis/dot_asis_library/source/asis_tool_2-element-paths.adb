with Asis.Elements;
with Asis.Statements;

package body Asis_Tool_2.Element.Paths is

   ------------
   -- EXPORTED:
   ------------
   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element; State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
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
         if Path_Kind in Asis.A_Statement_Path then
            Add_Element_List
              (This           => State,
               Elements_In    => Asis.Statements.Sequence_Of_Statements (Element),
               Dot_Label_Name => "Sequence_Of_Statements",
               List_Out       => Result.Sequence_Of_Statements,
               Add_Edges      => True);
         else
            -- Asis.Statements.Sequence_Of_Statements doesn't like paths outside
            -- A_Statement_Path:
            State.Add_Not_Implemented;
         end if;
      end;

      procedure Add_Common_Items is
      begin
         State.Add_To_Dot_Label ("Path_Kind", Path_Kind'Image);
         Result.Path_Kind := anhS.To_Path_Kinds (Path_Kind);
         Add_Sequence_Of_Statements; -- Has Add_Not_Implemented
      end Add_Common_Items;

      use all type Asis.Path_Kinds;
   begin
      If Path_Kind /= Not_A_Path then
         Add_Common_Items; -- Has Add_Not_Implemented
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
         when A_Case_Expression_Path => -- A2012
            Add_Case_Path_Alternative_Choices;
         when An_If_Expression_Path => -- A2012
            Add_Condition_Expression;
         when An_Elsif_Expression_Path => -- A2012
            Add_Condition_Expression;
         when An_Else_Expression_Path => -- A2012
            null; -- No more info
      end case;

      State.A_Element.Element_Kind := a_nodes_h.A_Path;
      State.A_Element.The_Union.Path := Result;
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Paths;
