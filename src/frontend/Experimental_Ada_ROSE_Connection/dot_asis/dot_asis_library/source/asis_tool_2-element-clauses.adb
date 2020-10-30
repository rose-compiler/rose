with Asis.Clauses;
with Asis.Elements;

package body Asis_Tool_2.Element.Clauses is

   ------------
   -- EXPORTED:
   ------------
   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element;
      State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
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

      procedure Add_Representation_Clause_Name is
         ID : constant a_nodes_h.Name_ID :=
           Get_Element_ID (Asis.Clauses.Representation_Clause_Name (Element));
      begin
         State.Add_To_Dot_Label ("Representation_Clause_Name", To_String(ID));
         Result.Representation_Clause_Name := ID;
      end;

      procedure Add_Representation_Clause_Expression is
         ID : constant a_nodes_h.Expression_ID :=
           Get_Element_ID (Asis.Clauses.Representation_Clause_Expression (Element));
      begin
         State.Add_To_Dot_Label ("Representation_Clause_Expression", To_String(ID));
         Result.Representation_Clause_Expression := ID;
      end;

      procedure Add_Mod_Clause_Expression is
         ID : constant a_nodes_h.Expression_ID :=
           Get_Element_ID (Asis.Clauses.Mod_Clause_Expression (Element));
      begin
         State.Add_To_Dot_Label ("Mod_Clause_Expression", To_String(ID));
         Result.Mod_Clause_Expression := ID;
      end;

      procedure Add_Component_Clauses is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Clauses.Component_Clauses (Element),
            Dot_Label_Name => "Component_Clauses",
            List_Out       => Result.Component_Clauses,
            Add_Edges      => True);
      end;

      procedure Add_Component_Clause_Position is
         ID : constant a_nodes_h.Expression_ID :=
           Get_Element_ID (Asis.Clauses.Component_Clause_Position (Element));
      begin
         State.Add_To_Dot_Label ("Component_Clause_Position", To_String(ID));
         Result.Component_Clause_Position := ID;
      end;

      procedure Add_Component_Clause_Range is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Clauses.Component_Clause_Range (Element));
      begin
         State.Add_To_Dot_Label ("Component_Clause_Range", To_String(ID));
         Result.Component_Clause_Range := ID;
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
            Add_Representation_Clause_Name;
            Add_Representation_Clause_Expression;
            Add_Mod_Clause_Expression;
            Add_Component_Clauses;
            Add_Component_Clause_Position;
            Add_Component_Clause_Range;
      end case;

      State.A_Element.Element_Kind := a_nodes_h.A_Clause;
      State.A_Element.the_union.clause := Result;
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Clauses;
