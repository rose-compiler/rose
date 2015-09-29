AC_DEFUN([AC_FIND_OPENGL],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_PATH_X])

# DQ (9/26/2015): Commented out to avoid aclocal, automake, and autoconf warnings.
# AC_REQUIRE([AC_PATH_XTRA])

  AC_CACHE_CHECK([for OpenGL], mdl_cv_have_OpenGL,
  [
dnl Check for Mesa first, unless we were asked not to.
    AC_ARG_WITH([mesa],
		[AS_HELP_STRING([--with-mesa],
                   [Prefer the Mesa library over a vendors native OpenGL library (default=no)])],
		[],
		[with_mesa=no])

    if test x"$with_mesa" = xyes; then
	GL_search_list="MesaGL   GL"
	GLU_search_list="MesaGLU GLU"
	GLX_search_list="MesaGLX GLX"
    else
	GL_search_list="GL  MesaGL"
	GLU_search_list="GLU MesaGLU"
	GLX_search_list="GLX MesaGLX"
    fi

    AC_LANG_SAVE
    AC_LANG_C

dnl If we are running under X11 then add in the appropriate libraries.
if test x"$no_x" != xyes; then
dnl Add everything we need to compile and link X programs to GL_X_CFLAGS
dnl and GL_X_LIBS.
  GL_CFLAGS="$X_CFLAGS"
#   GL_X_LIBS="$X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
   GL_X_LIBS="$X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt $X_EXTRA_LIBS"
fi
    GL_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$GL_CFLAGS"

    GL_save_LIBS="$LIBS"
    LIBS="$GL_X_LIBS"

    # Save the "AC_MSG_RESULT file descriptor" to FD 8.
    exec 8>&AC_FD_MSG

    # Temporarily turn off AC_MSG_RESULT so that the user gets pretty
    # messages.
    exec AC_FD_MSG>/dev/null

    AC_SEARCH_LIBS(glAccum,         $GL_search_list,  have_GL=yes,   have_GL=no)
    # JAS AC_SEARCH_LIBS(gluBeginCurve,   $GLU_search_list, have_GLU=yes,  have_GLU=no)
    AC_SEARCH_LIBS(glXChooseVisual, $GLX_search_list, have_GLX=yes,  have_GLX=no)

    if test -n "$LIBS"; then
      mdl_cv_have_OpenGL=yes
      GL_LIBS="$LIBS"
    else
      mdl_cv_have_OpenGL=no
      GL_CFLAGS=
    fi

    LIBS="-lglut $LIBS"

    AC_SEARCH_LIBS(glutInit,        glut,             have_glut=yes, have_glut=no)

    if test -n "$LIBS"; then
      mdl_cv_have_glut=yes
      GLUT_LIBS="$LIBS"
    else
      mdl_cv_have_glut=no
      GLUT_CFLAGS=
    fi

    # Restore pretty messages.
    exec AC_FD_MSG>&8

#     if test -n "$LIBS"; then
#       mdl_cv_have_OpenGL=yes
#       GL_LIBS="$LIBS"
#     else
#       mdl_cv_have_OpenGL=no
#       GL_CFLAGS=
#     fi

dnl Reset GL_X_LIBS regardless, since it was just a temporary variable
dnl and we don't want to be global namespace polluters.
    GL_X_LIBS=

    LIBS="$GL_save_LIBS"
    CPPFLAGS="$GL_save_CPPFLAGS"

    AC_LANG_RESTORE

dnl bugfix: dont forget to cache this variables, too
    mdl_cv_GL_CFLAGS="$GL_CFLAGS"
    mdl_cv_GL_LIBS="$GL_LIBS"
    mdl_cv_have_GL="$have_GL"
    mdl_cv_have_GLU="$have_GLU"
    mdl_cv_have_GLX="$have_GLX"
    mdl_cv_have_glut="$have_glut"
  ])
  GL_CFLAGS="$mdl_cv_GL_CFLAGS"
  GL_LIBS="$mdl_cv_GL_LIBS"
  have_GL="$mdl_cv_have_GL"
  have_GLU="$mdl_cv_have_GLU"
  have_GLX="$mdl_cv_have_GLX"
  have_glut="$mdl_cv_have_glut"
  AC_SUBST(GL_CFLAGS)
  AC_SUBST(GL_LIBS)
  AC_SUBST(GLUT_CFLAGS)
  AC_SUBST(GLUT_LIBS)
])
dnl endof bugfix -ainan
