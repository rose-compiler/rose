! This is a bug in OFP, I think!
     integer I,J

     INTEGER array3(2,2)
     INTEGER array4(2,2)

   ! Simple data statement
   ! DATA X, Y, Z / 3., 4., 5. /

   ! data statement using implied do loop
   ! DATA (array(I), array2(I),I=1,10)/ 20 * 42 /

   ! Until I get the index variable information in OFP, I can't handle this
     DATA ((array3(I,J),J=1,2),I=1,2)/1, 2, 3, 4/

   ! This shows the alternative use of the index variables in the implied do loop
     DATA ((array4(I,J),I=1,2),J=1,2)/1, 2, 3, 4/
end
