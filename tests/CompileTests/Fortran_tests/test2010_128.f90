 ! This is converted to an array of character of length 30 (incorrect).
 ! character   fname1*30
   character   fname1*30

 ! This is an array of characters (correct)
 ! character, dimension(30) :: fname2

 ! These will represent the same type internally (SgTypeString, both are correct) 
   character*30 fname2
   character(len=30) :: fname3

 ! These pass currently.
   fname1 = fname2
   fname3 = fname2
   fname1 = fname3
   fname2 = fname3

 ! Error in internal representation of fname1*30 (should be a string not an array of characters)
   fname2 = fname1
end
