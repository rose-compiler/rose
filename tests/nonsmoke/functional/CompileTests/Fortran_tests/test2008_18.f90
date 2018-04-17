subroutine foo
 ! This generates a call to R510 (two calls) so that we know that there a 2D array of undefined size ":,:"
   integer, pointer:: mm(:,:)
end subroutine foo
