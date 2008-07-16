implicit integer(kind=4) (i)
! added to verify that implicit_spec_list was handled by the prepass correctly
implicit integer(kind=4) (i), real(kind=8) (x-z)
end
