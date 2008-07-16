subroutine terminate_iterations
  logical, volatile :: user_hit_any_key

  ! Have the OS start to look for a user keystroke and set the variable
  ! "user_hit_any_key" to true as soon as it detects a keystroke.
  ! This pseudo call is operating system dependent.
  call os_begin_detect_user_keystroke(user_hit_any_key)
  user_hit_any_key = .false.  ! This will ignore any recent keystrokes
  
  print *,  " Hit any key to terminate iterations!"
  
  do i = 1,100
     ! Compute a value for R
     print *, i, r
     if (user_hit_any_key) exit
  enddo

  ! Have the OS stop looking for user keystrokes
  call os_stop_detect_user_keystroke
end subroutine terminate_iterations


