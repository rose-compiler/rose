!     C.5.3       Examples of DO constructs

!     The following are all valid examples of nonblock DO constructs:

!     Example 4:

           DO 70
               READ (IUN, '(1X, G14.7)', IOSTAT = IOS) X
               IF (IOS /= 0) EXIT
               IF (X < 0.) GOTO 70
               CALL SUBA (X)
               CALL SUBB (X)
               CALL SUBY (X)
               CYCLE
        70     CALL SUBNEG (X) ! SUBNEG called only when X < 0.
end
