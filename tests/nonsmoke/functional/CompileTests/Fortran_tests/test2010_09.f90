#if 0
/disks/utke/Apps/rose_inst/bin/testTranslator -rose:Fortran
-rose:skipfinalCompileStep -rose:output charArrSlice.r2f.f95
charArrSlice.f90
/disks/utke/Argonne/Dev/RoseFE/OpenAD/RegressionRose/Simple/TestSources/charArrSlice.f90:1.1:

 program charArrSlice
1
Warning: Nonconforming tab character at (1)
/disks/utke/Argonne/Dev/RoseFE/OpenAD/RegressionRose/Simple/TestSources/charArrSlice.f90:2.1:

 character(10), dimension(2) ::  sa
1
Warning: Nonconforming tab character at (1)
/disks/utke/Argonne/Dev/RoseFE/OpenAD/RegressionRose/Simple/TestSources/charArrSlice.f90:3.1:

 sa(2)="blabla"
1
Warning: Nonconforming tab character at (1)
/disks/utke/Argonne/Dev/RoseFE/OpenAD/RegressionRose/Simple/TestSources/charArrSlice.f90:5.1:

 if (sa(2)(2:4) .eq. "lab") then
1
Warning: Nonconforming tab character at (1)
/disks/utke/Argonne/Dev/RoseFE/OpenAD/RegressionRose/Simple/TestSources/charArrSlice.f90:6.1:

   print *,'OK'
1
Warning: Nonconforming tab character at (1)
/disks/utke/Argonne/Dev/RoseFE/OpenAD/RegressionRose/Simple/TestSources/charArrSlice.f90:7.1:

 endif
1
Warning: Nonconforming tab character at (1)
/disks/utke/Argonne/Dev/RoseFE/OpenAD/RegressionRose/Simple/TestSources/charArrSlice.f90:8.2:

  end
 1
Warning: Nonconforming tab character at (1)
I don't think that this is ever called!
c_action_actual_arg_spec_list__begin()
testTranslator:
/disks/utke/Argonne/svn/CodeReps/anonymous/rose/src/frontend/OpenFortranParser_SAGE_Connection/FortranParserActionROSE.C:16169:
void c_action_actual_arg_spec_list__begin(): Assertion `false' failed.
--

Jean tf

#endif

	program charArrSlice
	character(10), dimension(2) ::  sa
	sa(2)="blabla"
        sa(1)="grigri"
	if (sa(2)(2:4) .eq. "lab") then 
	  print *,'OK'
	endif
 	end 


