	 subroutine ss
	  type mytype
	   integer i
	   character(3) x
	  end type  mytype

 	  type nums
	   integer i(2)
	   real rr(2)
	 end type nums
	   
	  type (mytype),parameter::con11=mytype(1,"abc")
	  type (mytype),parameter::con12=mytype(1,"xxx")

	  type (nums),parameter::con21=nums((/1,2/),(/1.2,2.3/))
	  type (nums),parameter::con22=nums((/3,4/),(/0,3/))
	 end 
 
