with Asis.Elements;

package body Asis_Tool_2.Element.Pragmas is

   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element; State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
      Pragma_Kind : Asis.Pragma_Kinds :=
        Asis.Elements.Pragma_Kind (Element);
      Result : a_nodes_h.Pragma_Struct :=
        a_nodes_h.Support.Default_Pragma_Struct;

      procedure Add_Name_Image is
         WS : constant Wide_String := Asis.Elements.Pragma_Name_Image (Element);
      begin
         State.Add_To_Dot_Label ("Pragma_Name_Image", To_String(WS));
         Result.Pragma_Name_Image := a_nodes_h.Program_Text(To_Chars_Ptr(WS));
      end;

      procedure Add_Pragma_Argument_Associations is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Elements.Pragma_Argument_Associations (Element),
            Dot_Label_Name => "Pragma_Argument_Associations",
            List_Out       => Result.Pragma_Argument_Associations,
            Add_Edges      => True);
      end;


   begin
      --All Pragmas seem to be identical, so there's no reason to seperate things out

      State.Add_To_Dot_Label ("Pragma_Kind", Pragma_Kind'Image);
      Result.Pragma_Kind := anhS.To_Pragma_Kinds (Pragma_Kind);

      Add_Name_Image;
      Add_Pragma_Argument_Associations;
      -- Done making pragma

      State.A_Element.Element_Kind := a_nodes_h.A_Pragma;
      State.A_Element.the_union.The_Pragma := Result;

   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Pragmas;
