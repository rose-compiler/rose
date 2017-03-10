------------------------------------------------------------------------------
--                                                                          --
--                    ASIS APPLICATION TEMPLATE COMPONENTS                  --
--                                                                          --
--                        U N I T _ P R O C E S S I N G                     --
--                                                                          --
--                                 S p e c                                  --
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

--  This package contains routines for ASIS Compilation Units processing.

with Asis;

package Unit_Processing is

   procedure Process_Unit
     (The_Unit           : Asis.Compilation_Unit;
      Do_Context_Clauses : Boolean := True);
   --  This procedure decomposes its argument unit and calls the element
   --  processing routine for all the top-level components of the unit
   --  element hierarchy. This element processing routine is the instance
   --  of Traverse_Element which performs the recursive traversing of
   --  the unit structural components. In the given set of the ASIS
   --  implementation templates, the element processing routine is used
   --  "empty" actual Pre- and Post-Operations to instantiate
   --  Traverse_Element, they should be replaced by real subprograms
   --  when using this set of templates to build a real ASIS application
   --  from it.
   --
   --  It the argument Unit is Nil or nonexistent unit, this procedure does
   --  nothing.

end Unit_Processing;
