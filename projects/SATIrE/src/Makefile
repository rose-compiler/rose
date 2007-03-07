
SATIRE_SRC_DIR = .

INSTALL_INCLUDE = $(INSTALL_PREFIX)/include
INSTALL_LIB = $(INSTALL_PREFIX)/lib
INSTALL_BIN = $(INSTALL_PREFIX)/bin

all:
	$(MAKE) -C grato
	$(MAKE) -C pig
	$(MAKE) -C analyzer
	$(MAKE) -C examples
#	$(MAKE) -C experimental

check:
	$(MAKE) check -C grato
#	$(MAKE) check -C experimental

install: all
	mkdir -p $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/commandlineinterface/*.h $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/astaccess/*.h $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/astaccess/syntree.c $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/astaccess/syn $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/astaccess/pagoptions $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/astaccess/edges $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/astaccess/iface.h $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/icfgbuilder/*.h $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/annotation/*.h $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/annotation/StatementAttributeTraversal.C $(INSTALL_INCLUDE)
	cp -p $(SATIRE_SRC_DIR)/analyzer/skeletons/*-template $(INSTALL_INCLUDE)

	mkdir -p $(INSTALL_LIB)
	cp -p $(SATIRE_SRC_DIR)/analyzer/*.a $(INSTALL_LIB)
	cp -p $(SATIRE_SRC_DIR)/analyzer/syntree.o $(INSTALL_LIB)

	mkdir -p $(INSTALL_BIN)
	cp -p $(SATIRE_SRC_DIR)/analyzer/skeletons/newanalysis $(INSTALL_BIN)

clean:
	$(MAKE) clean -C grato
	$(MAKE) clean -C pig
	$(MAKE) clean -C analyzer
	$(MAKE) clean -C examples
	$(MAKE) clean -C experimental
