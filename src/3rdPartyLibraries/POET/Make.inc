
from=
to=
file=

copy: 
	for file in $(files); do cp $$file save/$$file; done;

repl1: 
	$(top_builddir)/src/pcg -L$(top_srcdir)/lib -pin=$(file) -pout=$(file) -pfrom=$(from) -pto="$(to)"  $(top_srcdir)/examples/StringTranslator.pt

repl: 
	for file in $(files); do $(top_builddir)/src/pcg -L$(top_srcdir)/lib -pin=$$file -pout=$$file -pfrom=$(from) -pto="$(to)"  $(top_srcdir)/examples/StringTranslator.pt;  done

trans:
	for file in $(files); do $(top_builddir)/src/pcg -L$(top_srcdir)/lib -pin=$$file -pout=$$file -pxformFile="$(top_srcdir)/examples/xform1.pt" $(top_srcdir)/examples/TranslatorDriver.pt; done

dir=to
op=diff

LOCAL_DIFFROSE= if test -n "$(rosedir)"; then \
            for f in $(DIST_SOURCES) $(EXTRA_DIST); do \
                 ff=$${f/__ROSE/}; \
                 if [[ $$ff == "*__*" ]]; then continue; else \
                     if [[ $(dir) == to ]]; then  \
                       command="$(op) $(srcdir)/$$f $(rosedir)/$$ff"; \
                     else \
                       command="$(op) $(rosedir)/$$ff $(srcdir)/$$f"; \
                     fi; \
                     if test -e $(rosedir)/$$ff; then \
                          echo "$$command"; $$command || true; \
                     else echo "new file $(srcdir)/$$f "; \
                     fi; \
                 fi; \
            done; \
          fi


diffROSE:
	make local_diffROSE
	for d in $(SUBDIRS); do cd $$d; make diffROSE; cd ..; done

local_diffROSE:
	$(LOCAL_DIFFROSE)

toROSE:
	make diffROSE op=cp  dir="to"

local_toROSE:
	make local_diffROSE op=cp  dir="to"

fromROSE:
	make diffROSE op=cp dir=from

local_fromROSE:
	make local_diffROSE op=cp dir=from
