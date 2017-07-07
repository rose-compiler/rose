with Ada.Characters.Handling;

--GNAT-specific:
with Asis.Set_Get;

package body Asis_Tool_2 is

   package Ach renames Ada.Characters.Handling;

   -----------
   -- PRIVATE:
   -----------
   function To_String (Item : in Wide_String) return String is
   begin
      return Ach.To_String (Item, Substitute => ' ');
   end To_String;

   -----------
   -- PRIVATE:
   -----------
   function To_Wide_String (Item : in String) return Wide_String
     renames Ach.To_Wide_String;

   -----------
   -- PRIVATE:
   -----------
   function To_Chars_Ptr (Item : in Wide_String) return Interfaces.C.Strings.chars_ptr is
   begin
      return Interfaces.C.Strings.New_String (To_String(Item));
   end;

   -----------
   -- PRIVATE:
   -----------
   procedure Trace_Put (Message : in Wide_String) is
   begin
      if Trace_On then
         Awti.Put (Message);
      end if;
   end Trace_Put;

   -----------
   -- PRIVATE:
   -----------
   procedure Trace_Put_Line (Message : in Wide_String) is
   begin
      if Trace_On then
         Awti.Put_Line ("$$$ " & Message);
      end if;
   end Trace_Put_Line;

   -----------
   -- PRIVATE:
   -----------
   procedure Print_Exception_Info (X : in Ada.Exceptions.Exception_Occurrence) is
   begin
      Awti.Put_Line ("EXCEPTION: " & To_Wide_String (Aex.Exception_Name (X)) &
                  " (" & To_Wide_String (Aex.Exception_Information (X)) & ")");
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
   begin
      return "unit_" & Spaceless_Image (Natural (Unit_Id));
   end;

   -----------
   -- PRIVATE:
   -----------
   function To_String (Element_Id : in Types.Node_Id) return String is
   begin
      return "element_" & Spaceless_Image (Natural (Element_Id));
   end;

   -----------
   -- PRIVATE:
   -----------
   function To_Dot_ID_Type (Unit_Id : in A4G.A_Types.Unit_Id) return Dot.ID_Type is
   begin
      return Dot.To_ID_Type (To_String (Unit_Id));
   end;

   -----------
   -- PRIVATE:
   -----------
   function To_Dot_ID_Type (Element_Id : in Types.Node_Id) return Dot.ID_Type is
   begin
      return Dot.To_ID_Type (To_String (Element_Id));
   end;

   -----------
   -- PRIVATE:
   -----------
   package body Indented_Text is

      -- To control where output goes:
      procedure Put (Message : in Wide_String) renames
        Trace_Put;

      procedure Put_Line (Message : in Wide_String) renames
        Trace_Put_Line;

--        procedure Put (Message : in Wide_String) renames
--          Ada.Wide_Text_IO.Put;
--
--        procedure Put_Line (Message : in Wide_String) renames
--          Ada.Wide_Text_IO.Put_Line;

      ------------
      -- EXPORTED:
      ------------
      procedure Indent (This : in out Class) is
      begin
         This.Indent_Level := This.Indent_Level + 1;
      end Indent;

      ------------
      -- EXPORTED:
      ------------
      procedure Dedent (This : in out Class) is
      begin
         if This.Indent_Level = 0 then
            Put_Line ("(Attempted negative indent)");
         else
            This.Indent_Level := This.Indent_Level - 1;
         end if;
      end Dedent;

      ------------
      -- EXPORTED:
      ------------
      procedure New_Line
        (This : in out Class) is
      begin
         Put_Line ("");
         This.Line_In_Progress := False;
      end New_Line;

      ------------
      -- EXPORTED:
      ------------
      procedure End_Line
        (This : in out Class) is
      begin
         if This.Line_In_Progress then
            This.New_Line;
         end if;
      end End_Line;

      ------------
      -- EXPORTED:
      ------------
      procedure Put
        (This    : in out Class;
         Message : in String) is
      begin
         This.Put (To_Wide_String (Message));
      end Put;

      ------------
      -- EXPORTED:
      ------------
      procedure Put
        (This    : in out Class;
         Message : in Wide_String) is
      begin
         This.Put_Indent_If_Needed;
         Put (Message);
      end Put;

      ------------
      -- EXPORTED:
      ------------
      procedure Put_Indented_Line
        (This    : in out Class;
         Message : in String) is
      begin
         This.Put_Indented_Line (To_Wide_String (Message));
      end Put_Indented_Line;

      ------------
      -- EXPORTED:
      ------------
      procedure Put_Indented_Line
        (This    : in out Class;
         Message : in Wide_String) is
      begin
         This.Put_Indent_If_Needed;
         Put_Line (Message);
      end Put_Indented_Line;

      ------------
      -- PRIVATE:
      ------------
      procedure Put_Indent_If_Needed
        (This : in out Class) is
      begin
         if not This.Line_In_Progress then
            Put (This.White_Space);
            This.Line_In_Progress := True;
         end if;
      end Put_Indent_If_Needed;

      ------------
      -- PRIVATE:
      ------------
      function White_Space
        (This : in Class)
      return Wide_String is
      begin
         return (1 .. This.Indent_Level * 2 => ' ');
      end White_Space;

   end Indented_Text;

end Asis_Tool_2;
