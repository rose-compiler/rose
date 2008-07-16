
! Test for case value range list
select case (foo)
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
