module foofoo_mod

  use iso_c_binding

  interface

    function foofoo(n) result(junker) bind(C,name="foo")
      import
      integer(c_int), value :: n
    end function

    function looloo(n) bind(C,name="loo") result(loo)
      import
      integer(c_int), value :: n
    end function

    function googoo(n) bind(C,name="goo")
      import
      integer(c_int), value :: n
    end function

    subroutine hoohoo(n) bind(C,name="hoo")
      import
      integer(c_int), value :: n
    end subroutine

  end interface

end module
