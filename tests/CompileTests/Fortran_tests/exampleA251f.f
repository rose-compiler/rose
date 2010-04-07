      integer function increment_counter()
        common/a25_common/counter
!$OMP THREADPRIVATE(/A25_COMMON/)
        counter = counter +1
        increment_counter = counter
      return
      end function increment_counter
