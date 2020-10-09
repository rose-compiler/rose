with Asis.Declarations;
with Asis.Elements;

package body Asis_Tool_2.Element.Defining_Names is

   ------------
   -- EXPORTED:
   ------------
   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element; State : in out Class)
   is
   Parent_Name : constant String := Module_Name;
   Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
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
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Defining_Names;
