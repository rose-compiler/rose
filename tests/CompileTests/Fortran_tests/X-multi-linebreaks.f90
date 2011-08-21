! OFP 0.8.3 failed with the following message:
! multi-linebreaks.f90 line 5:53 no viable alternative at input ''
MODULE multi_line_breaks

CHARACTER, PARAMETER :: out_1*(*) = &
&"x", out_2*&
&(*) = "y"

END MODULE

