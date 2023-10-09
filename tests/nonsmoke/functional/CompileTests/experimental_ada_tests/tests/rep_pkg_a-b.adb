with Rep_Pkg_A.V;

package body Rep_Pkg_A.B is
  procedure Run_Tests is
    type SomeEnum is (V);
  begin
    Rep_Pkg_A.V.Run_Tests;
  end Run_Tests;
end ;

