!It seems that the format field of SgReadStatement (probably write and
!print nodes) stores an integer, instead of a label_symbol.
!
!I prefer a label_symbol, which is also used by SgGotoStatement
!currently.  Then the outliner can directly grap the labeled format
!statement rather than searching for it using an integer.
!-----------input------------
      READ 1, N
    1 FORMAT (I3)
      END
