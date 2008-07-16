C this bug is caused by the 'H' edit descriptor, which is a deleted feature
C in the F2003 draft.
 10   FORMAT(5Habc/2)
 100  FORMAT(/5X,50(1H-)/) 

      end
