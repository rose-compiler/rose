AC_DEFUN([ROSE_SUPPORT_VISUALIZATION],
[
# Begin macro ROSE_SUPPORT_VISUALIZATION.

# Inclusion of test for FLTK and GraphViz in support of the visualization 
# provided by dotgl (Nils' work)
#  1) FLTK location path (include and libs path)
#  2) GraphViz location path (include and libs path)

# Note that due to the way in which these libraries are installed in Nils
# directory we have to specify the include and libs paths separately for
# each library.  I don't know if this mechanism has be always be this general.

AC_MSG_CHECKING(for FLTK library include path)
AC_ARG_WITH(FLTK_include,
[  --with-FLTK_include=PATH	Specify the prefix where FLTK include files is installed],
,
if test ! "$with_FLTK_include" ; then
   with_FLTK_include=no
fi
)

AC_MSG_CHECKING(for FLTK library libs path)
AC_ARG_WITH(FLTK_libs,
[  --with-FLTK_libs=PATH	Specify the prefix where FLTK libraries are installed],
,
if test ! "$with_FLTK_libs" ; then
   with_FLTK_libs=no
fi
)

AC_MSG_CHECKING(for GraphViz library include path)
AC_ARG_WITH(GraphViz_include,
[  --with-GraphViz_include=PATH	Specify the prefix where GraphViz include files are installed],
,
if test ! "$with_GraphViz_include" ; then
   with_GraphViz_include=no
fi
)

AC_MSG_CHECKING(for GraphViz library libs path)
AC_ARG_WITH(GraphViz_libs,
[  --with-GraphViz_libs=PATH	Specify the prefix where GraphViz libraries are installed],
,
if test ! "$with_GraphViz_libs" ; then
   with_GraphViz_libs=no
fi
)

# DQ (9/1/2005): Added support for GLUT library (FreeGlut)
AC_MSG_CHECKING(for GLUT library include path)
AC_ARG_WITH(GLUT_include,
[  --with-GLUT_include=PATH	Specify the prefix where GLUT include files are installed])

if test ! "$with_GLUT_include" ; then
   with_GLUT_include=no
fi


# DQ (9/1/2005): Added support for GLUT library (FreeGlut)
AC_MSG_CHECKING(for GLUT library libs path)
AC_ARG_WITH(GLUT_libs,
[  --with-GLUT_libs=PATH	Specify the prefix where GLUT libraries are installed])

if test ! "$with_GLUT_libs" ; then
   with_GLUT_libs=no
fi

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_FLTK_include     = "$with_FLTK_include"])
AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_FLTK_libs        = "$with_FLTK_libs"])
AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_GraphViz_include = "$with_GraphViz_include"])
AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_GraphViz_libs    = "$with_GraphViz_libs"])

if (test "$with_FLTK_include" = no) || (test "$with_FLTK_libs" = no) || 
   (test "$with_GraphViz_include" = no) || (test "$with_GraphViz_libs" = no) ||
   (test "$with_GLUT_include" = no) || (test "$with_GLUT_libs" = no); then
   # If none of these are specified, then don't use visualization (don't build dotGL).
   AC_MSG_NOTICE([skipping use of dotgl visualization])
else
 # optional_Visualization_subdirs="visualization"
   FLTK_include_path=$with_FLTK_include
   FLTK_libs_path=$with_FLTK_libs
   GraphViz_include_path=$with_GraphViz_include
   GraphViz_libs_path=$with_GraphViz_libs
   GLUT_include_path=$with_GLUT_include
   GLUT_libs_path=$with_GLUT_libs

   AC_MSG_NOTICE([setup VIZUALIZATION. FLTK include path     = "$FLTK_include_path"])
   AC_MSG_NOTICE([setup VIZUALIZATION. FLTK libs path        = "$FLTK_libs_path"])
   AC_MSG_NOTICE([setup VIZUALIZATION. GraphViz include path = "$GraphViz_include_path"])
   AC_MSG_NOTICE([setup VIZUALIZATION. GraphViz libs path    = "$GraphViz_libs_path"])
   AC_MSG_NOTICE([setup VIZUALIZATION. GLUT include path     = "$GLUT_include_path"])
   AC_MSG_NOTICE([setup VIZUALIZATION. GLUT libs path        = "$GLUT_libs_path"])

 # echo "Setup VIZUALIZATION! optional_Visualization_subdirs = $optional_Visualization_subdirs"

#  AC_CONFIG_SUBDIRS(src/roseIndependentSupport/visualization)
fi

# These are all used to buld dotGL
AC_SUBST(optional_Visualization_subdirs)
AC_SUBST(FLTK_include_path)
AC_SUBST(FLTK_libs_path)
AC_SUBST(GraphViz_include_path)
AC_SUBST(GraphViz_libs_path)
AC_SUBST(GLUT_include_path)
AC_SUBST(GLUT_libs_path)

# End macro ROSE_SUPPORT_VISUALIZATION.
]
)




