!! R615 complex-part-designator
!    is designator % RE
!    or designator % IM
!
!  NOTE 6.6
!
complex :: impedance, fft, x

r = impedance%re
i = fft%im
x%im = 0.0

end
