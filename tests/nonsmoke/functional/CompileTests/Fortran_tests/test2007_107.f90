   ! This test code demonstrates that we don't get the "I" and "J" 
   ! implicit loop index names (and they are required).
     integer I,J

     INTEGER array(10)
     INTEGER array2(10)
     INTEGER array3(2,2)

   ! Simple data statement
     DATA X, Y, Z / 3., 4., 5. /

   ! data statement using implied do loop
     DATA (array(I), array2(I),I=1,10)/ 20 * 42 /

     DATA ((array3(I,J),J=1,2),I=1,2)/1, 2, 3, 4/

   ! DATA (array(I), array2(I),I=1,10,2)/ 10 * 42 /
   ! DATA (array3(I,J),J=1,2),I=1,2) / 4 * 7 /
   ! DATA ((P(I,L),L=1,8),I=31,37) 
END
