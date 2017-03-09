------------------------------------------------------------------------------
--                                                                          --
--                    ASIS APPLICATION TEMPLATE COMPONENTS                  --
--                                                                          --
--                  A C T U A L S _ F O R _ T R A V E R S I N G             --
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

package body Actuals_For_Traversing is

   -------------
   -- Post_Op --
   -------------

   procedure Post_Op
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Traversal_State)
   is separate;

   ------------
   -- Pre_Op --
   ------------

   procedure Pre_Op
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Traversal_State)
   is separate;

end Actuals_For_Traversing;
