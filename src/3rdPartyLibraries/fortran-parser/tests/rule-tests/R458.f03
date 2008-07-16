
! Testing component_spec_list.
! I haven't been able to trigger this. The following is just the code
! for R456 type_param_spec_list.
type(foo(5)) :: bar
type(foo(dim=a)) :: abar
type(truth(3,4,ugly=5)) :: beauty
end
