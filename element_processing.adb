------------------------------------------------------------------------------
--                                                                          --
--                    ASIS APPLICATION TEMPLATE COMPONENTS                  --
--                                                                          --
--                     E L E M E N T _ P R O C E S S I N G                  --
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

with Asis.Iterator;

with Actuals_For_Traversing;

package body Element_Processing is

   -----------------------
   -- Local subprograms --
   -----------------------

   procedure Recursive_Construct_Processing is new
      Asis.Iterator.Traverse_Element
        (State_Information => Actuals_For_Traversing.Traversal_State,
         Pre_Operation     => Actuals_For_Traversing.Pre_Op,
         Post_Operation    => Actuals_For_Traversing.Post_Op);
   --  This instantiation of Traverse_Element actually performs the recursive
   --  analysis of ASIS Elements. In this set of ASIS application templates
   --  it does nothing, but you may get a number of simple, but useful ASIS
   --  applications by providing some real code for Pre_Op and/or Post_Op,
   --  and keeping the rest of the template code unchanged. For example,
   --  see ASIS tutorials included in the ASIS distribution.

   -----------------------
   -- Process_Construct --
   -----------------------

   procedure Process_Construct (The_Element : Asis.Element) is
      Process_Control : Asis.Traverse_Control := Asis.Continue;

      Process_State   : Actuals_For_Traversing.Traversal_State :=
         Actuals_For_Traversing.Initial_Traversal_State;

   begin

      Recursive_Construct_Processing
        (Element => The_Element,
         Control => Process_Control,
         State   => Process_State);

   end Process_Construct;

end Element_Processing;