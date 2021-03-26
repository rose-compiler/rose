module componentAttrSpec_contiguous

  implicit none



  type testtype

     real,dimension(:,:),pointer,contiguous :: curve_X => Null()

  !! Avgerage velocity
     real,dimension(:),pointer,contiguous :: avg_vel_X => Null()


  end type testtype
end module componentAttrSpec_contiguous
