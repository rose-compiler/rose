! This test code demonstrates that the entry statement name is unavailable in OFP.
RECURSIVE FUNCTION FNC() RESULT (RES)

  ENTRY ENTRY_FUNCTION () RESULT (RES) ! The result variable name can be
                                       ! the same as for the function
  RES = 0.0

END FUNCTION

