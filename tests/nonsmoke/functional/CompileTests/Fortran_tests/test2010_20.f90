! concat_1 from Rice bug report (8/5/2010)
! This is fixed (suggested fix by Matt: 8/5/2010).

program foo
 character(9) ch1
 character(11) ch2
 character(4) ch3
 character(2) ch4
 character(4) ch5
 character(30) ch6

 ch1 = 'A cat is '
 ch2 = 'definitely '
 ch3 = 'not '
 ch4 = 'a '
 ch5 = 'dog.'

 ch6 = ch1//ch2//ch3//ch4//ch5
 print *,ch6
end
