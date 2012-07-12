#
# File:  utilities.gnu
#
#----------------------------------------------------------------------------
#
#				UTILITY COMMANDS
#
#  clean_all --
#  clean   -- 
#  clobber_all --
#  clobber -- remove unnecessary garbage plus dependencies, library files,
#             and time-stamps
#
#----------------------------------------------------------------------------

.PHONY: clean clean_all clobber clobber_all

RMOBJS = *% *.o *.l *.lis *.T *.TT *.rif *~ *.lst

clean_all:
	@cd $(TOP); $(MAKE) clobber;\
	
clean:
	@cd $(TOP); $(MAKE) clobber;\
	
clobber_all:
	@cd $(TOP); $(MAKE) clobber;\
	
clobber:
	@$(RM) core pop pop_db compile/$(ObjDepDir)/*.* compile/$(SrcDepDir)/*.*
	@$(RM) compile/*.f compile/*.f90 compile/*.h compile/*.c
	@$(RM) compile/*.o compile/*.mod compile/*.do compile/*.d
	
