module iso_fortran_env

 ! Size in bits of the character storage unit.
   INTEGER,PARAMETER :: CHARACTER_STORAGE_SIZE  = 1

 ! Identifies the preconnected unit used for error reporting.
   INTEGER,PARAMETER :: ERROR_UNIT = 2

 ! Size in bits of the file-storage unit.
   INTEGER,PARAMETER :: FILE_STORAGE_SIZE = 3

 ! Identifies the preconnected unit identified by the asterisk (`*') in `READ' statement.
   INTEGER,PARAMETER :: INPUT_UNIT = 4

 ! The value assigned to the variable passed to the IOSTAT= specifier of an input/output statement if an end-of-file condition occurred.
   INTEGER,PARAMETER :: IOSTAT_END = 5

 ! The value assigned to the variable passed to the IOSTAT= specifier
 ! of an input/output statement if an end-of-record condition occurred.
   INTEGER,PARAMETER :: IOSTAT_EOR = 6

 ! The size in bits of the numeric storage unit.
   INTEGER,PARAMETER :: NUMERIC_STORAGE_SIZE = 7

 ! Identifies the preconnected unit identified by the asterisk (`*') in `WRITE' statement.
   INTEGER,PARAMETER :: OUTPUT_UNIT = 8

end module iso_fortran_env
