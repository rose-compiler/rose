character(len=25) ch2, ch3
open(12, file = 'fred', status = 'old', delim = 'none')
ch2 = '''this string has quotes.'''
             ! Quotes in string ch2
write (12, *) ch2    ! Written with no delimiters
open(12, delim = 'quote') ! Now quote is the delimiter
rewind 12
read(12, *) ch3  ! ch3 now has the value
                 ! 'this string has quotes.  '

end
