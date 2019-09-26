       recursive subroutine A(input)
        input = input + 1
        if(input < 10) call A(input)
      end  subroutine
