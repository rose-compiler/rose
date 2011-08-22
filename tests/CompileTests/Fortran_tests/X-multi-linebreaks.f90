! On the following multiline statement, OFP 0.8.3 failed to handle continuations correctly.

module multi_line_breaks

character, parameter :: p1*(*) = &
&"x", p2*&
&(*) = "y"

end module
