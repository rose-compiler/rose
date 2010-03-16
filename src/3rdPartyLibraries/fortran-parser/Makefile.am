javadir=$(libdir)

OFP_VERSION=@ROSE_OFP_MAJOR_VERSION_NUMBER@.@ROSE_OFP_MINOR_VERSION_NUMBER@.@ROSE_OFP_PATCH_VERSION_NUMBER@
# Thing about if we need the test using an Automake conditional since the jar file always exists.
if ROSE_USE_OPEN_FORTRAN_PARSER
# dist_java_DATA = lib/OpenFortranParser-0.7.2.jar
# dist_java_DATA = lib/OpenFortranParser-${OFP_VERSION}.jar
dist_java_DATA = OpenFortranParser-${OFP_VERSION}.jar
endif

EXTERNAL_OFP_JAR_FILE_URL=http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${OFP_VERSION}
# OFP_JAR_FILE_DISTRIBUTION_PATH=lib

%.jar:
#	test -d ${OFP_JAR_FILE_DISTRIBUTION_PATH} && cp ${OFP_JAR_FILE_DISTRIBUTION_PATH}/$@  . || wget ${EXTERNAL_OFP_JAR_FILE_URL}/$@
#	test -d ${OFP_JAR_FILE_DISTRIBUTION_PATH} && cp ${srcdir}/${OFP_JAR_FILE_DISTRIBUTION_PATH}/$@  ${OFP_JAR_FILE_DISTRIBUTION_PATH} || wget ${EXTERNAL_OFP_JAR_FILE_URL}/$@
#	test -e $@ || cp ${srcdir}/$@ . || wget ${EXTERNAL_OFP_JAR_FILE_URL}/$@


# ${top_builddir}/src/3rdPartyLibraries/fortran-parser/OpenFortranParser-${OFP_VERSION}.jar:
#	test -e $@ || cp ${srcdir}/$@ . || wget ${EXTERNAL_OFP_JAR_FILE_URL}/$@

get_jar_file:
	test -e OpenFortranParser-${OFP_VERSION}.jar || cp ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar . || wget ${EXTERNAL_OFP_JAR_FILE_URL}/OpenFortranParser-${OFP_VERSION}.jar

# TEST_OFP_VERSION=${OFP_VERSION}
TEST_OFP_VERSION=0.8.0
TEST_EXTERNAL_OFP_JAR_FILE_URL=http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${TEST_OFP_VERSION}

get_new_jar_file:
	test -e OpenFortranParser-${TEST_OFP_VERSION}.jar || (test -e ${srcdir}/OpenFortranParser-${TEST_OFP_VERSION}.jar && cp ${srcdir}/OpenFortranParser-${TEST_OFP_VERSION}.jar .) || wget ${TEST_EXTERNAL_OFP_JAR_FILE_URL}/OpenFortranParser-${TEST_OFP_VERSION}.jar

EXTRA_DIST = CMakeLists.txt

clean-local:
	rm -f OpenFortranParser-${OFP_VERSION}.jar

