! From Craig: rose_bug-3156475.f90
INTEGER, PARAMETER :: MAX_FILE_TYPE_LEN_IS_VERY_LONG = 10
INTEGER, PARAMETER :: NUMBER_FILE_TYPES = 9
character(len=MAX_FILE_TYPE_LEN_IS_VERY_LONG), DIMENSION(NUMBER_FILE_TYPES), PARAMETER :: chararray = &
(/'undefined1','undefined2','undefined3','undefined4', &
'undefined5','undefined6','undefined7','undefined8','undefined9'/)
END 
