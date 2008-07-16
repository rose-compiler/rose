program a
  type t
     integer :: field ! add a field to it (not in draft)
  end type t

contains
  subroutine b
    implicit type(t) (c) ! Refers to type T declared below
                         ! in subroutine B, not type T
                         ! declared above in program a
    type t
       integer :: this_field ! add a field (not in draft) 
    end type t
  end subroutine b
end program a

