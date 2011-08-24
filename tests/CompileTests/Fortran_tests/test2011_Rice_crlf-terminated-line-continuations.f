! This file contains lines terminated by CR-LF pairs.
! It fails in OFP 0.8.3 with the following message:
!     mismatched input 'f_switch' expecting T_RPAREN
! 
! Apparently OFP's lexer fails to join the two parts of
! the token 'thermdiff_switch' which is (correctly) split
! by a line continuation in the source.
!
! If the line terminations are changed to be just LFs,
! this file parses correctly.

      subroutine diffflux_proc_looptool(nx, ny, nz, 
     *n_spec, baro_switch, thermdif
     *f_switch, diffflux, grad_ys, grad_mixmw, ys, grad_p, press, ds_mix
     *avg, avmolwt, molwt, rs_therm_diff, grad_t, temp)
      end
