! Test associate-construct
!      associate-construct  is  associate-stmt
!                                 block
!                               end-associate-stmt
!
!      associate-stmt  is  [ associate-construct-name : ] ASSOCIATE 
!                            ( association-list )
!
!      association  is  associate-name => selector
!
!      selector  is  expr
!                or  variable
!
! Tested here: associate-construct, associate-stmt, end-associate-smt, 
! associate-construct-name, association-list, association, and associate-name.
!
! Not tested here: block, selector, expr, and variable.
associate(a=>b)
  x = 1
end associate

associate(a=>b, c => d)
  x = 1
end associate

my_assoc: associate(a=>b, c => d)
  x = 1
end associate my_assoc

end

