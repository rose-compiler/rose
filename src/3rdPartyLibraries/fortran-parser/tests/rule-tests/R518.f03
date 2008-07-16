! Testing access-stmt, R518
module ex
10  private
	PUBLIC boo, assignment(=)
	public :: binky, bonky, operator (+) ! see NOTE 5.23, doesn't work
end module ex
