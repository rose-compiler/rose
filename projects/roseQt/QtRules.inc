# ------------- MOC ---------------------

if QROSE_USE_WINDOWS
moc_%.cpp: $(srcdir)/%.h 
	$(MOC) $(INCLUDES) -o  $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.cpp: $(srcdir)/%.h 
	@$(MOC) $(QT_INCLUDES) -o  $@ $<
	@echo "  Qt-MOC $(notdir  $< )"	
endif 


# ------------- UIC ---------------------

if QROSE_USE_WINDOWS
uic_%.h: $(srcdir)/%.ui 
	$(UIC) -o $@ `cygpath -w $<` -p$(srcdir)
else
ui_%.h: $(srcdir)/%.ui 
	@$(UIC) -o $@ $<
	@echo "  Qt-UIC  $(notdir  $< )"	
endif 




# ------------- RCC --------------------

if QROSE_USE_WINDOWS
qrc_%.cpp: $(srcdir)/%.qrc 
	$(RCC) -name $(patsubst %.qrc, %, $(notdir  $< ) ) -o $@ `cygpath -w $<` -p$(srcdir)
else
qrc_%.cpp: $(srcdir)/%.qrc
	@$(RCC) -name $(patsubst %.qrc, %, $(notdir  $< ) ) -o $@ $<
	@echo "  Qt-RCC $(notdir  $< ) "		
endif 



# ----- Cleanup of generated files -----

CLEANFILES = moc_*.cpp\
             ui_*.h\
             qrc_*.cpp 
            
            
METASOURCES = auto    
