subroutine variable_qualifiers
   integer :: device, managed, constant, shared, pinned, texture
   attributes(device)   :: device
   attributes(managed)  :: managed
   attributes(constant) :: constant
   attributes(shared)   :: shared
   attributes(pinned)   :: pinned
   attributes(texture)  :: texture
end subroutine
