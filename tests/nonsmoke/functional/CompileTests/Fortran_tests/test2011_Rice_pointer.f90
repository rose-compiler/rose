program test_pointer
 integer, pointer, dimension(:):: cp1 
 integer, pointer :: cp2(:) 
  character(len = 10), pointer :: p1 ! length lost in translation
  character, pointer :: p2*(5) ! length lost in translation
  character(len = 10, kind = 1), dimension(:), pointer :: p3 ! length lost in translation
 cp1(3) = 5
 cp2(4) = 6
 allocate(p2)
 p2 = 'abcde'
 allocate(p3(5))
 p3(2) = 'a1b2c3d4e5'
end program

