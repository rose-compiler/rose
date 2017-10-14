with Ada.Characters.Handling;

--GNAT-specific:
with Asis.Set_Get;
with GNAT.Traceback.Symbolic;

package body Asis_Tool_2 is

   package Ach renames Ada.Characters.Handling;

   -----------
   -- PRIVATE:
   -----------
   function To_String (Item : in Wide_String) return String is
     (Ach.To_String (Item, Substitute => ' '));

   -----------
   -- PRIVATE:
   -----------
   function To_Quoted_String (Item : in Wide_String) return String is
      ('"' & To_String (Item) & '"');

   -----------
   -- PRIVATE:
   -----------
   function To_Wide_String (Item : in String) return Wide_String
     renames Ach.To_Wide_String;

   -----------
   -- PRIVATE:
   -----------
   function To_Chars_Ptr (Item : in Wide_String) return Interfaces.C.Strings.chars_ptr is
     (Interfaces.C.Strings.New_String (To_String(Item)));

   -----------
   -- PRIVATE:
   -----------
   procedure Print_Exception_Info (X : in Ada.Exceptions.Exception_Occurrence) is
   begin
      Awti.Put_Line ("EXCEPTION: " & To_Wide_String (Aex.Exception_Name (X)) &
                       " (" & To_Wide_String (Aex.Exception_Information (X)) & ")");
      Awti.Put_Line ("TRACEBACK: ");
      Awti.Put_Line (To_Wide_String (GNAT.Traceback.Symbolic.Symbolic_Traceback (X)));
      Awti.New_Line;
   end Print_Exception_Info;

   -----------
   -- PRIVATE:
   -----------
   function Spaceless_Image (Item : in Natural) return String is
      Leading_Space_Image : constant String := Item'Image;
   begin
      return Leading_Space_Image (2 .. Leading_Space_Image'Last);
   end;

   -----------
   -- PRIVATE:
   -----------
   function To_String (Unit_Id : in A4G.A_Types.Unit_Id) return String is
     ("Unit_" & Spaceless_Image (Natural (Unit_Id)));

   -----------
   -- PRIVATE:
   -----------
   function To_String (Element_Id : in Types.Node_Id) return String is
     ("Element_" & Spaceless_Image (Natural (Element_Id)));

   -----------
   -- PRIVATE:
   -----------
   function To_Dot_ID_Type (Unit_Id : in A4G.A_Types.Unit_Id) return Dot.ID_Type is
     (Dot.To_ID_Type (To_String (Unit_Id)));

   -----------
   -- PRIVATE:
   -----------
   function To_Dot_ID_Type (Element_Id : in Types.Node_Id) return Dot.ID_Type is
     (Dot.To_ID_Type (To_String (Element_Id)));

end Asis_Tool_2;
