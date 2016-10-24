! Bug report from Rice: 18-specific-typebound-procedure.f03
! FortranParserActionROSE.C's action 'c_action_type_bound_procedure_part'
! is unimplemented. It fails an assertion.

module m104
  type :: t
  contains
    procedure :: p
  end type
end module
