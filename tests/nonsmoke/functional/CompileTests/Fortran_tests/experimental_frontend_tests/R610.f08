!! R610 substring-range
!    is [ scalar-int-expr ] : [ scalar-int-expr ]
!
a = b(3)(:)
a = b(3)(1:)
a = b(3)(:2)
a = b(3)(1:2)

a = z(3)[13](1:2)

end

