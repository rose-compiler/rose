------------------------------------------------------------------------------
--                                                                          --
--                    ASIS APPLICATION TEMPLATE COMPONENTS                  --
--                                                                          --
--                     E L E M E N T _ P R O C E S S I N G                  --
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

--  This package contains routines for ASIS Elements processing.

with Asis;

package Element_Processing is

   procedure Process_Construct (The_Element : Asis.Element);
   --  This is the template for the procedure which is supposed to
   --  perform the analysis of its argument Element (The_Element) based on
   --  the recursive traversing of the Element hierarchy rooted by
   --  The_Element. It calls the instantiation of the ASIS Traverse_Element
   --  generic procedure for The_Element.
   --
   --  This procedure should not be called for Nil_Element;
   --
   --  Note, that the instantiation of Traverse_Element and the way how it is
   --  called is no more then a template. It uses a dummy enumeration type
   --  as the actual type for the state of the traversal, and it uses
   --  dummy procedures which do nothing as actual procedures for Pre- and
   --  Post-operations. The Control parameter of the traversal is set
   --  to Continue and it is not changed by actual Pre- or Post-operations.
   --  All this things will definitely require revising when using this
   --  set of templates to build any real application. (At least you will
   --  have to provide real Pre- and/or Post-operation)
   --
   --  See the package body for more details.

end Element_Processing;