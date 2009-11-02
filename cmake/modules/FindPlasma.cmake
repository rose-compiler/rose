
# FindPlasma.cmake was part of KDE 4.1, but Plasma itself didn't guarantee compatibility before 4.2,
# so anything which relied on Plasma < 4.2 is broken anyway with KDE 4.2.
# So since the package itself didn't keep compatibility, it doesn't make sense to provide
# this file just to keep compatibility for the cmake part, this doesn't help anybody.
# Especially now that plasma is part of kdelibs and as such the variables required 
# for using plasma are set by FindKDE4Internal.cmake.
# This file is not used anywhere in trunk/KDE/ anymore, 3rd party projects get a 
# error message which tells them what to do, so we should be fine.
# Alex
#

message(FATAL_ERROR "FindPlasma.cmake is deprecated. Now with KDE 4.2 Plasma is part of kdelibs and automatically found using find_package(KDE4) instead.
Replace the variables previously coming from FindPlasma.cmake as follows:
PLASMA_OPENGL_FOUND -> KDE4_PLASMA_OPENGL_FOUND
PLASMA_LIBS -> KDE4_PLASMA_LIBS
PLASMA_INCLUDE_DIR -> KDE4_INCLUDE_DIR or KDE4_INCLUDES, should be already set anyway
PLASMA_FOUND -> nothing, it's always there if KDE4, version 4.2 or newer has been found.
If you see this error message in a module within KDE/, update it from svn, it has been fixed already.
")
