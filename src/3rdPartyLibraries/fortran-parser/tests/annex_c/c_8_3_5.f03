module library_lls
  interface
     subroutine lls(x, a, f, flag)
       real x(:, :)
       ! The SIZE in the next statement is an intrinsic function
       real, dimension (size(x,2)) :: a, f
       integer flag
     end subroutine lls
  end interface
end module library_lls

use library_lls

call lls(x=abc, a=d, f=xx, flag = iflag)

end

