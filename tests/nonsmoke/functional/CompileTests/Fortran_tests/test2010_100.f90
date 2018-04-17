! Bug report from Rice: 14-entry-statement.f90
! An 'entry' statement causes the front end to subsequently fail an assertion.
! The entry statement leaves an extra name on the stack, but to see a failure
! the entry statement must be followed by a suitable statement which asserts
! that the name stack is empty. A 'function' statement is one.

function f(x)
  entry e(y)  ! incorrectly leaves the name stack nonempty
end function

function g(z)     ! asserts name stack is empty, so fails
end function
