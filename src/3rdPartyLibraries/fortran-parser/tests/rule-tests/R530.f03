
! Testing data stmt value list. Similar to R526.
! The following doesn't work, though it should.
data truth, beauty, ugly / 3.14159,2.81828,6 /
data foo / 3 /
data bar, this, that, theother / 2*3,2*7.6 /
end
