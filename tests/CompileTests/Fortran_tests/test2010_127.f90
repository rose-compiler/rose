 ! This is converted to an array of character of length 30 (incorrect).
 ! character   fname1*30
   character   fname1*30
   character   fname8*31

 ! This will be an array of characters.
 ! character   fname2(30)

 ! Illegal syntax (and caught as such by the semantic analysis)
 ! integer   fname1*30

 ! These will represent the same type internally (SgTypeString, both are correct) 
   character*30 fname2

 ! Error in internal representation of fname1*30 (should be a string not an array of characters)
   fname2 = fname1
end

