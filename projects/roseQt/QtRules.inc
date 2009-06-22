# ------------- MOC ---------------------

if QROSE_USE_WINDOWS
moc_%.cxx: $(srcdir)/%.h 
	$(MOC) $(INCLUDES) -o  $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.cxx: $(srcdir)/%.h 
	@echo "  Qt-MOC $(notdir $< )" && $(MOC) $(QT_INCLUDES) -o  $@ $<
endif 


# ------------- UIC ---------------------

if QROSE_USE_WINDOWS
uic_%.h: $(srcdir)/%.ui 
	$(UIC) -o $@ `cygpath -w $<` -p$(srcdir)
else
ui_%.h: $(srcdir)/%.ui 
	@echo "  Qt-UIC  $(notdir  $< )" && $(UIC) -o $@ $<
endif 




# ------------- RCC --------------------

if QROSE_USE_WINDOWS
qrc_%.cpp: $(srcdir)/%.qrc 
	$(RCC) -name $(patsubst %.qrc, %, $(notdir  $< ) ) -o $@ `cygpath -w $<` -p$(srcdir)
else
qrc_%.cpp: $(srcdir)/%.qrc
	@echo "  Qt-RCC $(RCC) $(notdir  $< ) " && $(RCC) -name $(patsubst %.qrc, %, $(notdir  $< ) ) -o $@ $<
endif 



# ----- Cleanup of generated files -----

CLEANFILES = moc_*.cxx\
             ui_*.h\
             qrc_*.cpp 
            
            
METASOURCES = auto    
