subroutine foo()
  PRINT *, 'Hello World! (single quoted string)'
  PRINT *, "Hello World! (double quoted string)"
  PRINT *, 'He said "Hello World!" (double quoted substring in single quoted string)'
  PRINT *, "He said 'Hello World!' (single quoted substring in double quoted string)"
  PRINT *, "This double quoted string has an unbalanced ' (single quote)"
  PRINT *, 'This single quoted string has an unbalanced " (double quote)'
end subroutine
