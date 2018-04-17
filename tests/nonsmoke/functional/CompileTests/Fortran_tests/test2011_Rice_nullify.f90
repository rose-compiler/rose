!
 program test_nullify
   integer, dimension(:), pointer :: p, q(:,:)
   real, dimension(:), pointer:: x
   logical, pointer:: y(:)
   complex, pointer, dimension(:,:):: z
   
   nullify(p, q)
   nullify(x)
   nullify(y,z,x)
 end 
