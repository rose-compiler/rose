-- A34017C.ADA

--                             Grant of Unlimited Rights
--
--     Under contracts F33600-87-D-0337, F33600-84-D-0280, MDA903-79-C-0687,
--     F08630-91-C-0015, and DCA100-97-D-0025, the U.S. Government obtained
--     unlimited rights in the software and documentation contained herein.
--     Unlimited rights are defined in DFAR 252.227-7013(a)(19).  By making
--     this public release, the Government intends to confer upon all
--     recipients unlimited rights  equal to those held by the Government.
--     These rights include rights to use, duplicate, release or disclose the
--     released technical data and computer software in whole or in part, in
--     any manner and for any purpose whatsoever, and to have or permit others
--     to do so.
--
--                                    DISCLAIMER
--
--     ALL MATERIALS OR INFORMATION HEREIN RELEASED, MADE AVAILABLE OR
--     DISCLOSED ARE AS IS.  THE GOVERNMENT MAKES NO EXPRESS OR IMPLIED
--     WARRANTY AS TO ANY MATTER WHATSOEVER, INCLUDING THE CONDITIONS OF THE
--     SOFTWARE, DOCUMENTATION OR OTHER INFORMATION RELEASED, MADE AVAILABLE
--     OR DISCLOSED, OR THE OWNERSHIP, MERCHANTABILITY, OR FITNESS FOR A
--     PARTICULAR PURPOSE OF SAID MATERIAL.
--*
-- CHECK THAT IF A DERIVED TYPE DEFINITION IS GIVEN IN THE VISIBLE PART
-- OF A PACKAGE, THE TYPE MAY BE USED AS THE PARENT TYPE IN A DERIVED
-- TYPE DEFINITION IN THE PRIVATE PART OF THE PACKAGE AND IN THE BODY.

-- CHECK THAT IF A TYPE IS DECLARED IN THE VISIBLE PART OF A PACKAGE,
-- AND IS NOT A DERIVED TYPE OR A PRIVATE TYPE, IT MAY BE USED AS THE
-- PARENT TYPE IN A DERIVED TYPE DEFINITION IN THE VISIBLE PART, PRIVATE
-- PART, AND BODY.


-- DSJ 4/27/83


PROCEDURE TYPE_IN_LOCAL_PKG IS
BEGIN
     DECLARE
          PACKAGE PACK1 IS
               TYPE T1 IS RANGE 1 .. 10;
          END PACK1;

          PACKAGE BODY PACK1 IS
               TYPE Q1 IS NEW T1;
          END PACK1;

     BEGIN
          NULL;
     END;

     NULL;
END TYPE_IN_LOCAL_PKG;
