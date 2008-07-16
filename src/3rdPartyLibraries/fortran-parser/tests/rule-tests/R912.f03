! Test print-stmt
!      print-stmt  is  PRINT format [, output-item-list ]
!
! Not tested here: format and output-item-list.
print *, "hello"
print 5, a, b, c
print 10, 1
print '(10f8.2)', a, b

end
