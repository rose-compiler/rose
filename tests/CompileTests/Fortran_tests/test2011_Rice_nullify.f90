!
 program test_nullify
   integer, dimension(:), pointer :: p, q(:,:)
   nullify(p, q)
 end 
