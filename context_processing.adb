------------------------------------------------------------------------------
--                                                                          --
--                    ASIS APPLICATION TEMPLATE COMPONENTS                  --
--                                                                          --
--                     C O N T E X T _ P R O C E S S I N G                  --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--            Copyright (c) 2000, Free Software Foundation, Inc.            --
--                                                                          --
-- ASIS  Application  Templates are  free software; you can redistribute it --
-- and/or  modify it under  terms  of the  GNU  General  Public  License as --
-- published by the Free Software Foundation; either version 2, or (at your --
-- option) any later version. ASIS Application Templates are distributed in --
-- the hope that they will be useful, but  WITHOUT  ANY  WARRANTY; without  --
-- even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR --
-- PURPOSE. See the GNU General Public License for more details. You should --
-- have  received a copy of the GNU General Public License distributed with --
-- distributed  with  GNAT;  see  file  COPYING. If not, write to the Free  --
-- Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, --
-- USA.                                                                     --
--                                                                          --
-- ASIS Application Templates were developed and are now maintained by Ada  --
-- Core Technologies Inc (http://www.gnat.com).                             --
--                                                                          --
------------------------------------------------------------------------------

with Ada.Wide_Text_IO;
with Ada.Characters.Handling;
with Ada.Exceptions;

with Asis.Compilation_Units;
with Asis.Exceptions;
with Asis.Errors;
with Asis.Implementation;

with Unit_Processing;

package body Context_Processing is

   -----------------------------
   -- Get_Unit_From_File_Name --
   -----------------------------

   function Get_Unit_From_File_Name
     (Ada_File_Name : String;
      The_Context   : Asis.Context)
      return Asis.Compilation_Unit
   is
   begin

      return Asis.Nil_Compilation_Unit;

      --  To be completed....

   end Get_Unit_From_File_Name;

   ---------------------
   -- Process_Context --
   ---------------------

   procedure Process_Context
     (The_Context : Asis.Context;
      Trace       : Boolean := False)
   is
      Units : Asis.Compilation_Unit_List :=
         Asis.Compilation_Units.Compilation_Units (The_Context);

      Next_Unit        : Asis.Compilation_Unit := Asis.Nil_Compilation_Unit;
      Next_Unit_Origin : Asis.Unit_Origins     := Asis.Not_An_Origin;
      Next_Unit_Class  : Asis.Unit_Classes     := Asis.Not_A_Class;
   begin

      for J in Units'Range loop
         Next_Unit        := Units (J);
         Next_Unit_Class  := Asis.Compilation_Units.Unit_Class (Next_Unit);
         Next_Unit_Origin := Asis.Compilation_Units.Unit_Origin (Next_Unit);

         if Trace then
            Ada.Wide_Text_IO.Put ("Processing Unit: ");
            Ada.Wide_Text_IO.Put
              (Asis.Compilation_Units.Unit_Full_Name (Next_Unit));

            case Next_Unit_Class is
               when Asis.A_Public_Declaration |
                    Asis.A_Private_Declaration =>

                  Ada.Wide_Text_IO.Put (" (spec)");

               when Asis.A_Separate_Body =>
                  Ada.Wide_Text_IO.Put (" (subunit)");

               when Asis.A_Public_Body |
                    Asis.A_Public_Declaration_And_Body |
                    Asis.A_Private_Body =>

                  Ada.Wide_Text_IO.Put_Line (" (body)");

               when others =>
                  Ada.Wide_Text_IO.Put_Line (" (???)");
            end case;

            Ada.Wide_Text_IO.New_Line;

         end if;

         case Next_Unit_Origin is
            when Asis.An_Application_Unit =>
               Unit_Processing.Process_Unit (Next_Unit);
               --  This is the call to the procedure which performs the
               --  analysis of a particular unit

               if Trace then
                  Ada.Wide_Text_IO.Put ("Done ...");
               end if;

            when Asis.A_Predefined_Unit =>
               if Trace then
                  Ada.Wide_Text_IO.Put ("Skipped as a predefined unit");
               end if;

            when Asis.An_Implementation_Unit =>

               if Trace then
                  Ada.Wide_Text_IO.Put
                    ("Skipped as an implementation-defined unit");
               end if;

            when Asis.Not_An_Origin =>

               if Trace then
                  Ada.Wide_Text_IO.Put
                    ("Skipped as nonexistent unit");
               end if;

         end case;

         if Trace then
            Ada.Wide_Text_IO.New_Line;
            Ada.Wide_Text_IO.New_Line;
         end if;

      end loop;

   exception
      --  The exception handling in this procedure is somewhat redundant and
      --  may need some reconsidering when using this procedure as a template
      --  for a real ASIS tool

      when Ex : Asis.Exceptions.ASIS_Inappropriate_Context          |
                Asis.Exceptions.ASIS_Inappropriate_Container        |
                Asis.Exceptions.ASIS_Inappropriate_Compilation_Unit |
                Asis.Exceptions.ASIS_Inappropriate_Element          |
                Asis.Exceptions.ASIS_Inappropriate_Line             |
                Asis.Exceptions.ASIS_Inappropriate_Line_Number      |
                Asis.Exceptions.ASIS_Failed                         =>

         Ada.Wide_Text_IO.Put ("Process_Context : ASIS exception (");

         Ada.Wide_Text_IO.Put (Ada.Characters.Handling.To_Wide_String (
                 Ada.Exceptions.Exception_Name (Ex)));

         Ada.Wide_Text_IO.Put (") is raised when processing unit ");

         Ada.Wide_Text_IO.Put
            (Asis.Compilation_Units.Unit_Full_Name (Next_Unit));

         Ada.Wide_Text_IO.New_Line;

         Ada.Wide_Text_IO.Put ("ASIS Error Status is ");

         Ada.Wide_Text_IO.Put
           (Asis.Errors.Error_Kinds'Wide_Image (Asis.Implementation.Status));

         Ada.Wide_Text_IO.New_Line;

         Ada.Wide_Text_IO.Put ("ASIS Diagnosis is ");
         Ada.Wide_Text_IO.New_Line;
         Ada.Wide_Text_IO.Put (Asis.Implementation.Diagnosis);
         Ada.Wide_Text_IO.New_Line;

         Asis.Implementation.Set_Status;

      when Ex : others =>

         Ada.Wide_Text_IO.Put ("Process_Context : ");

         Ada.Wide_Text_IO.Put (Ada.Characters.Handling.To_Wide_String (
                 Ada.Exceptions.Exception_Name (Ex)));

         Ada.Wide_Text_IO.Put (" is raised (");

         Ada.Wide_Text_IO.Put (Ada.Characters.Handling.To_Wide_String (
                 Ada.Exceptions.Exception_Information (Ex)));

         Ada.Wide_Text_IO.Put (")");
         Ada.Wide_Text_IO.New_Line;

         Ada.Wide_Text_IO.Put ("when processing unit");

         Ada.Wide_Text_IO.Put
            (Asis.Compilation_Units.Unit_Full_Name (Next_Unit));

         Ada.Wide_Text_IO.New_Line;

   end Process_Context;

end Context_Processing;