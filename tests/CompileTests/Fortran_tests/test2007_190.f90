Program main
   logical i,j,k
   logical a1,a2,a3,b,c,d

 ! These are evaluated similarly (independent of the parenthesis
 ! a1 = i .and. j .and. k
 ! a2 = (i .and. j) .and. k
 ! a3 = i .and. (j .and. k)

   a1 = .not. (.not. (i .and. j))

   b = .not.i .and. .not.j .and. .not.k

   c = .not.i .and. j .and. .not.k

   d = (.not.i) .and. (.not.j) .and. (.not.k)
end
