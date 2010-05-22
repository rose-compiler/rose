#if 0
For the attached test code I get  garbled unparsed output for the module
function  "sphere":

CHARACTER(len=6) FUNCTION function(miles)

where the original input is:

  character*6 function sphere ( miles ) result ( zone )

when running it like this:
/disks/utke/Apps/rose_inst/bin/testTranslator -rose:Fortran
-rose:skipfinalCompileStep -rose:output atmosphere.r2f.f95
atmosphere.f95     
line 9:14 missing T_FUNCTION at
'function'                                                                                                

line 9:23 extraneous input 'sphere' expecting
T_LPAREN                                                                                    

line 9:40 no viable alternative at input
'result'                                                                                         

line 10:8 mismatched input ',' expecting
T_FUNCTION                                                                                       

Calling
attributeSpecificationStatement->set_intent(intent)                                                                               

Parser
failed                                                                                                                             

Parser
failed                                                                                                                             

Warning: jserver_callBooleanMethod return non-zero
result.                                                                                

SgAggregateInitializer::get_type(): default
case                                                                                          

SgAggregateInitializer::get_type(): default case         


Same disclaimer as before - please let me know if you need more input or
if I should
make a bug report.

jean

#endif

module sky
  private
  public  :: sphere
  real,        dimension(5) :: bottom
  character*6, dimension(5) :: layer
  data bottom /    0.0,     5.0,    30.0,    50.0,   300.0 /
  data layer  /' Tropo','Strato','  Meso','  Iono','   Exo'/
contains
  character*6 function sphere ( miles ) result ( zone )
    real, intent(IN) :: miles
    do i = 1, size(bottom), 1
       if ( miles .ge. bottom(i) ) zone = layer(i)
    end do
  end function sphere
end module sky

program atmosphere
  use sky
  real, dimension(3) :: altitude = (/ 1.2, 45.6, 789.0 /)
  if(sphere(altitude(1)).EQ." Tropo" .AND. &
       sphere(altitude(2)).EQ."  Meso" .AND. &
       sphere(altitude(3)).EQ."   Exo") then
     print *, "OK"
  else
     print *, "failed"
  end if
end program atmosphere


