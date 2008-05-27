all: frontendAndDump spawnFrontend.o

frontendAndDump: frontendAndDump.C
	g++ -o frontendAndDump frontendAndDump.C -I/home/willcock2/rose-install/include -L/home/willcock2/rose-install/lib -lrose $(RT_LIBS)

spawnFrontend.o: spawnFrontend.C spawnFrontend.h
	g++ -c spawnFrontend.C -I/home/willcock2/rose-install/include

identityTranslator: identityTranslator.C spawnFrontend.h spawnFrontend.o frontendAndDump
	g++ -o identityTranslator identityTranslator.C -I/home/willcock2/rose-install/include -L/home/willcock2/rose-install/lib -lrose $(RT_LIBS) spawnFrontend.o

clean:
	-rm -f frontendAndDump spawnFrontend.o identityTranslator
