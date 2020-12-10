with Asis.Elements;

package body Asis_Tool_2.Element.Pragmas is

   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element; State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
      Pragma_Kind : Asis.Pragma_Kinds :=
        Asis.Elements.Pragma_Kind (Element);
   begin
      --        A_Pragma              -> Pragma_Kinds
      --
      State.Add_To_Dot_Label
        (Name => "Pragma_Kind",
         Value => Pragma_Kind'Image);
      State.Add_Not_Implemented;
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Pragmas;
