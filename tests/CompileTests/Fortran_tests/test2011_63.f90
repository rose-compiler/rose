! From Craig: bug-3156475.f90
integer, parameter :: MAX_FILE_TYPE_LEN_IS_VERY_LONG = 10
integer, parameter :: NUMBER_FILE_TYPES = 9

character(MAX_FILE_TYPE_LEN_IS_VERY_LONG), parameter :: chararray(NUMBER_FILE_TYPES) = (/ &
      'undefined1',   &
      'undefined2',   &
      'undefined3',   &
      'undefined4',   &
      'undefined5',   &
      'undefined6',   &
      'undefined7',   &
      'undefined8',   &
      'undefined9'       /)

end

