! void c_action_intrinsic_type_spec(Token_t*, Token_t*, int, ofp_bool): Assertion `rank == 1' failed.
program p
 character(kind = 1) aK
  character(len = 2)  aL2
  character a, aL3 * 3
  character bL*2, b
  character (len = 3, kind = 1) aLK3 ! kind is lost in translation
  character(kind = 1, len = 3) aKL3 ! kind is lost in translation
  character(kind = 1) aKL4 * 4
  character(kind = 1) akL5 * (5)
  character(kind = 1, len = 7) akL6 * (6)
  character(len = 1, kind = 1) aLK
end program
