!! R816 case-value-range
!    is case-value
!    or case-value :
!    or : case-value
!    or case-value : case-value

select case (ifoo)
case (:-1)
	this=that
case (0)
	that=the
case (1:3,5,7)
	the=other
case (4,8:)
	other=bar
end select
end
