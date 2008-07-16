module j; real jx, jy, jz; end module j
module k
  use j, only : kx => jx, ky => jy
  ! kx and ky are local names to module k
  real kz         ! kz is local name to module k
  real jz         ! jz is local name to module k
end module k
program rename
  use j; use k
  ! module j's entity jx is accessible under names jx and kx
  ! module j's entity jy is accessible under names jy and ky
  ! module k's entity kz is accessible under name kz
  ! module j's entity jz and k's entity jz are different entities
  ! and shall not be referenced
end program rename
