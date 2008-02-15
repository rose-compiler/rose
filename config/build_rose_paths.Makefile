rose_paths.h: Makefile
	@@true > rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_TOP_SRCDIR \"@top_srcdir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR \"@absolute_path_srcdir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_TOP_BUILDDIR \"@top_pwd@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_PREFIX \"@prefix@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_DATADIR \"@datadir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_BINDIR \"@bindir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_INCLUDEDIR \"@includedir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_INFODIR \"@infodir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_LIBDIR \"@libdir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_LIBEXECDIR \"@libexecdir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_LOCALSTATEDIR \"@localstatedir@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_MANDIR \"@mandir@\"" >> rose_paths.h
	@@echo "" >> rose_paths.h
	@@echo "/* Additional interesting data to provide */" >> rose_paths.h
	@@echo "#define ROSE_CONFIGURE_DATE \"@configure_date@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_BUILD_OS \"@build_os@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_BUILD_CPU \"@build_cpu@\"" >> rose_paths.h
	@@echo "#define ROSE_AUTOMAKE_BUILT_WITH_EDG_SOURCE_CODE @with_edg_source_code@" >> rose_paths.h
	@@echo "" >> rose_paths.h
	@@echo "/* Define the location of the Compass tool within ROSE */" >> rose_paths.h
	@@echo "#define COMPASS_SOURCE_DIRECTORY \"@absolute_path_srcdir@/projects/compass\"" >> rose_paths.h
