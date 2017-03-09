------------------------------------------------------------------------------
--                                                                          --
--                    ASIS APPLICATION TEMPLATE COMPONENTS                  --
--                                                                          --
--                  A C T U A L S _ F O R _ T R A V E R S I N G             --
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

--  This package contains the definition of the actual types and subprograms
--  to be used to instantiate Asis.Iterator.Traverse_Element.
--  Actually, the declarations given in this package are templates in the
--  sense, that, being formally correct and making the whole code of the
--  template ASIS applications compilable, they do nothing and therefore they
--  have to be replaced in the real ASIS applications.
--
--  The bodies of the subprograms declared in this package are implemented
--  as subunits to localize changes needed to provide real Pre- and
--  Post-operations in real operations

with Asis;

package Actuals_For_Traversing is

   type Traversal_State is (Not_Used);
   --  A placeholder declaration, used as an actual for State_Information in
   --  the template instantiation of Traverse_Element.
   --  If your ASIS application needs some non-trivial state, you should
   --  either change the definition of this type (and of the constant
   --  Initial_Traversal_State) below or use some other type as the actual
   --  for State_Information when instantiating Traverse_Element.

   Initial_Traversal_State : constant Traversal_State := Not_Used;

   procedure Pre_Op
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Traversal_State);
   --  This is the template for the actual Pre-Operation procedure. It does
   --  nothing, and it contains the exception handler which is supposed to
   --  catch all the exception raised in this procedure.
   --  The body of this procedure is implemented as a subunit - in case when
   --  you  would like to provide your own Pre-operation when building your
   --  ASIS tool from the given set of templates (and if you do not need
   --  non-trivial traversal state), you can replace this subunit by your own
   --  code and reuse the rest of the template code.

   procedure Post_Op
     (Element :        Asis.Element;
      Control : in out Asis.Traverse_Control;
      State   : in out Traversal_State);
   --  This is the template for the actual Post-Operation procedure. It does
   --  nothing, and it contains the exception handler which is supposed to
   --  catch all the exception raised in this procedure.
   --  The body of this procedure is implemented as a subunit - in case when
   --  you  would like to provide your own Post-operation when building your
   --  ASIS tool from the given set of templates (and if you do not need
   --  non-trivial traversal state), you can replace this subunit by your own
   --  code and reuse the rest of the template code.

end Actuals_For_Traversing;