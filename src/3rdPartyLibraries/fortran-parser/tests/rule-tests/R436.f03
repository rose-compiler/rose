
! Testing type_param_decl_list
type :: foo(xx, yy)
end type
type, extends(foo) :: bar(zz)
end type
end
