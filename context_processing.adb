with Ada.Characters.Handling;
with Ada.Exceptions;
with Ada.Wide_Text_IO;
with Asis.Compilation_Units;
with Asis.Exceptions;
with Asis.Errors;
with Asis.Implementation;

with Unit_Processing;

package body Context_Processing is

   function Get_Unit_From_File_Name
     (Ada_File_Name : String;
      The_Context   : Asis.Context) return Asis.Compilation_Unit
   is
   begin
      --  To be completed....
      return Asis.Nil_Compilation_Unit;
   end Get_Unit_From_File_Name;

   procedure Process_Context
     (The_Context : Asis.Context;
      Trace       : Boolean := False)
   is
      Units : Asis.Compilation_Unit_List :=
        Asis.Compilation_Units.Compilation_Units (The_Context);
      Next_Unit        : Asis.Compilation_Unit := Asis.Nil_Compilation_Unit;
      Next_Unit_Origin : Asis.Unit_Origins     := Asis.Not_An_Origin;
      Next_Unit_Class  : Asis.Unit_Classes     := Asis.Not_A_Class;
      use Ada.Wide_Text_IO;
      use Asis.Exceptions;

      procedure Trace_Put (Message : in Wide_String)
      is
      begin
         if Trace then
            Put (Message);
         end if;
      end Trace_Put;

   begin
      for Next_Unit of Units loop
         Next_Unit_Class  := Asis.Compilation_Units.Unit_Class (Next_Unit);
         Next_Unit_Origin := Asis.Compilation_Units.Unit_Origin (Next_Unit);

         if Trace then
            Put ("Processing Unit: ");
            Put (Asis.Compilation_Units.Unit_Full_Name (Next_Unit));
            case Next_Unit_Class is
               when Asis.A_Public_Declaration |
                    Asis.A_Private_Declaration =>
                  Put (" (spec)");
               when Asis.A_Separate_Body =>
                  Put (" (subunit)");
               when Asis.A_Public_Body              |
                 Asis.A_Public_Declaration_And_Body |
                 Asis.A_Private_Body                =>
                  Put (" (body)");
               when others =>
                  Put_Line (" (???)");
            end case;
            New_Line;
         end if;

         case Next_Unit_Origin is
            when Asis.An_Application_Unit =>

               -- Do actual work:
               Unit_Processing.Process_Unit (Next_Unit);

                  Trace_Put ("Done");
            when Asis.A_Predefined_Unit =>
                  Trace_Put ("Skipped as a predefined unit");
            when Asis.An_Implementation_Unit =>
                  Trace_Put ("Skipped as an implementation-defined unit");
            when Asis.Not_An_Origin =>
                  Trace_Put ("Skipped as nonexistent unit");
         end case;

         if Trace then
            New_Line;
            New_Line;
         end if;
      end loop;

   exception
      when Ex : ASIS_Inappropriate_Context |
        ASIS_Inappropriate_Container |
        ASIS_Inappropriate_Compilation_Unit |
        ASIS_Inappropriate_Element |
        ASIS_Inappropriate_Line |
        ASIS_Inappropriate_Line_Number |
        ASIS_Failed =>
         declare
            use Ada.Characters.Handling;
            use Ada.Exceptions;
            use Ada.Wide_Text_IO;
         begin
            Put ("Process_Context : ASIS exception (");
            Put (To_Wide_String (Exception_Name (Ex)));
            Put (") is raised when processing unit ");
            Put (Asis.Compilation_Units.Unit_Full_Name (Next_Unit));
            New_Line;
            Put ("ASIS Error Status is ");
            Put
              (Asis.Errors.Error_Kinds'Wide_Image
                 (Asis.Implementation.Status));
            New_Line;
            Put ("ASIS Diagnosis is ");
            New_Line;
            Put (Asis.Implementation.Diagnosis);
            New_Line;
            Asis.Implementation.Set_Status (Asis.Errors.Not_An_Error);
         end;
      when Ex : others =>
         declare
            use Ada.Characters.Handling;
            use Ada.Exceptions;
            use Ada.Wide_Text_IO;
         begin
            Put ("Process_Context : ");
            Put (To_Wide_String (Exception_Name (Ex)));
            Put (" is raised (");
            Put (To_Wide_String (Exception_Information (Ex)));
            Put (")");
            New_Line;
            Put ("when processing unit");
            Put (Asis.Compilation_Units.Unit_Full_Name (Next_Unit));
            New_Line;
         end;
   end Process_Context;

end Context_Processing;
