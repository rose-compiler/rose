------------------------------------------------------------------------------
--                                                                          --
--                    ASIS APPLICATION TEMPLATE COMPONENTS                  --
--                                                                          --
--              A S I S _ A P P L I C A T I O N _ D R I V E R _ 1           --
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

with Ada.Exceptions;
with Ada.Wide_Text_IO;
with Ada.Characters.Handling;

with Asis;
with Asis.Ada_Environments;
with Asis.Implementation;
with Asis.Exceptions;
with Asis.Errors;

with Context_Processing;

procedure ASIS_Application_Driver_1 is
   My_Context            : Asis.Context;

   My_Context_Name       : Wide_String := Asis.Ada_Environments.Default_Name;
   --  The default name in case of the GNAT ASIS implementation is empty.
   --  If you would like to have some non-null name for your ASIS Context,
   --  change the initialization expression in this declaration. Note, that
   --  in the GNAT ASIS implementation the name of a Context does not have any
   --  special meaning or semantics associated with particular names.

   My_Context_Parameters : Wide_String :=
      Asis.Ada_Environments.Default_Parameters;
   --  The default COntext parameters in case of the GNAT ASIS implementation
   --  are an empty string. This corresponds to the following Context
   --  definition: "-CA -FT -SA", and has the following meaning:
   --  -CA - a Context is made up by all the tree files in the tree search
   --        path; the tree search path is not set, so the default is used,
   --        and the default is the current directory;
   --  -FT - only pre-created trees are used, no tree file can be created by
   --        ASIS;
   --  -SA - source files for all the Compilation Units belonging to the
   --        Context (except the predefined Standard package) are considered
   --        in the consistency check when opening the Context;
   --
   --  If you would like to use some other Context definition, you have to
   --  replace the initialization expression in this declaration by
   --  the corresponding Parameters string. See the ASIS Reference Manual for
   --  the full details of the Context definition for the GNAT ASIS
   --  implementation.

   Initialization_Parameters : Wide_String := "";
   Finalization_Parameters   : Wide_String := "";
   --  If you would like to use some specific initialization or finalization
   --  parameters, you may set them here as initialization expressions in
   --  the declarations above.

begin
   --  The code below corresponds to the basic sequencing of calls to ASIS
   --  queries which is mandatory for any ASIS application.
   --  First, the ASIS implementation should be initialized
   --  (Asis.Implementation.Initialize), then an application should define
   --  an ASIS Context to process by associating the Context with an external
   --  environment (Asis.Ada_Environments.Associate), then the Context should
   --  be opened (sis.Ada_Environments.Open ). After that all the ASIS queries
   --  may be used (the ASIS processing is encapsulated in
   --  Context_Processing.Process_Context. When the ASIS analysis is over, the
   --  application should release the system resources by closing the ASIS
   --  Context (Asis.Ada_Environments.Close), breaking the association of the
   --  Context with the external world (Asis.Ada_Environments.Dissociate) and
   --  finalizing the ASIS implementation (Asis.Implementation.Finalize).

   Asis.Implementation.Initialize     (Initialization_Parameters);

   Asis.Ada_Environments.Associate
     (The_Context => My_Context,
      Name        => My_Context_Name,
      Parameters  => My_Context_Parameters);

   Asis.Ada_Environments.Open         (My_Context);

   Context_Processing.Process_Context (The_Context => My_Context,
                                       Trace       => True);

   Asis.Ada_Environments.Close        (My_Context);
   Asis.Ada_Environments.Dissociate   (My_Context);
   Asis.Implementation.Finalize       (Finalization_Parameters);

exception
   --  The exception handling in this driver is somewhat redundant and may
   --  need some reconsidering when using this driver in real ASIS tools

   when Ex : Asis.Exceptions.ASIS_Inappropriate_Context          |
             Asis.Exceptions.ASIS_Inappropriate_Container        |
             Asis.Exceptions.ASIS_Inappropriate_Compilation_Unit |
             Asis.Exceptions.ASIS_Inappropriate_Element          |
             Asis.Exceptions.ASIS_Inappropriate_Line             |
             Asis.Exceptions.ASIS_Inappropriate_Line_Number      |
             Asis.Exceptions.ASIS_Failed                         =>

      Ada.Wide_Text_IO.Put ("ASIS exception (");
      Ada.Wide_Text_IO.Put (Ada.Characters.Handling.To_Wide_String (
              Ada.Exceptions.Exception_Name (Ex)));
      Ada.Wide_Text_IO.Put (") is raised");
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

      Ada.Wide_Text_IO.Put (Ada.Characters.Handling.To_Wide_String (
              Ada.Exceptions.Exception_Name (Ex)));
      Ada.Wide_Text_IO.Put (" is raised (");
      Ada.Wide_Text_IO.Put (Ada.Characters.Handling.To_Wide_String (
              Ada.Exceptions.Exception_Information (Ex)));
      Ada.Wide_Text_IO.Put (")");
      Ada.Wide_Text_IO.New_Line;

end ASIS_Application_Driver_1;