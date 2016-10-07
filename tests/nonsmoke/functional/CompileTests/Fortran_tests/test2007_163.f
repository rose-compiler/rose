! Another example program from Liao
      PROGRAM LOAN
      WRITE(*,*) 'Enter amount, percentage rate, and years for the loan'
      READ(*,*) AMOUNT, PCRATE, NYEARS
      RATE = PCRATE /100.0
      REPAY = RATE * AMOUNT /(1.0 - (1.0+RATE)**(-NYEARS))
      MTPAY = REPAY/12.0
      WRITE(*,*) 'Annual repayments are ', REPAY
      WRITE(*,*) 'Monthly payment is ', MTPAY
      WRITE(*,*) 'End of year  Balance'
      DO 15, IYEAR = 1,NYEARS
        AMOUNT = AMOUNT +(AMOUNT * RATE) - REPAY
        WRITE(*,'(1X,I9,F11.2)') IYEAR, AMOUNT
c        WRITE(*,*) IYEAR, AMOUNT
15    CONTINUE
      END
