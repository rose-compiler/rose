      program cg

      implicit none

    ! Ordering the variable declaration before the common statement 
    ! causes the correct types to be built for the data in the common block.
    ! integer                 	 colidx(7), XXrowstr(7)
    ! common / main_int_mem / 	 colidx,     XXrowstr

    ! But when the common statement preceeds the variable declaration the 
    ! variable declarration does not reuse or reset the data types.
    ! Worse still the types are generated using the implicit type rules 
    ! even through the function is marked as "implicit none"
      common / main_int_mem / 	 colidx,     XXrowstr
      integer                 	 colidx(7), XXrowstr(7)

      integer            j

      j=XXrowstr(j)

      call conj_grad ( colidx, XXrowstr )

      end




