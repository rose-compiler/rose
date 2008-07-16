character (len = 20) ch1
write(10, '(a)') 'this is record 1'
open (unit = 10, status = 'old', pad = 'yes')
rewind 10
read(10, '(a20)') ch1  ! ch1 now has the value
                       ! 'this is record 1    '

end
