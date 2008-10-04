! Test for how to get att name "i" of return value for "foo"
! Answer: it is in the "result_name" field of SgProcedureHeaderStatement.
integer function foo() RESULT (i)
    i = 1
end function foo

