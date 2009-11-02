#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debian".
#----------------------------------------------------------------

# Commands may need to know the format version.
SET(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KDE4__kdefakes" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kdefakes APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kdefakes PROPERTIES
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkdefakes.so.5.2.0"
  IMPORTED_SONAME_DEBIAN "libkdefakes.so.5"
  )

# Import target "KDE4__kdecore" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kdecore APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kdecore PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "/usr/lib/libQtDBus.so;/usr/lib/libQtCore.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkdecore.so.5.2.0"
  IMPORTED_SONAME_DEBIAN "libkdecore.so.5"
  )

# Import target "KDE4__kdeui" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kdeui APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kdeui PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdecore;/usr/lib/libQtSvg.so;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkdeui.so.5.2.0"
  IMPORTED_SONAME_DEBIAN "libkdeui.so.5"
  )

# Import target "KDE4__kpty" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kpty APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kpty PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdecore;/usr/lib/libQtCore.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkpty.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkpty.so.4"
  )

# Import target "KDE4__kdesu" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kdesu APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kdesu PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdecore;KDE4__kpty"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "/usr/lib/libQtCore.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkdesu.so.5.2.0"
  IMPORTED_SONAME_DEBIAN "libkdesu.so.5"
  )

# Import target "KDE4__kjs" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kjs APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kjs PROPERTIES
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkjs.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkjs.so.4"
  )

# Import target "KDE4__kjsapi" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kjsapi APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kjsapi PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kjs;KDE4__kdecore"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkjsapi.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkjsapi.so.4"
  )

# Import target "KDE4__kjsembed" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kjsembed APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kjsembed PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdecore;KDE4__kjs"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkjsembed.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkjsembed.so.4"
  )

# Import target "KDE4__kntlm" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kntlm APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kntlm PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdecore"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkntlm.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkntlm.so.4"
  )

# Import target "KDE4__kio" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kio APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kio PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__solid"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdeui;KDE4__kdecore;/usr/lib/libQtNetwork.so;/usr/lib/libQtXml.so;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkio.so.5.2.0"
  IMPORTED_SONAME_DEBIAN "libkio.so.5"
  )

# Import target "KDE4__solid" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__solid APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__solid PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdecore"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libsolid.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libsolid.so.4"
  )

# Import target "KDE4__kunittest" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kunittest APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kunittest PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdecore;/usr/lib/libQtCore.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkunittest.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkunittest.so.4"
  )

# Import target "KDE4__kde3support" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kde3support APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kde3support PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kparts;KDE4__kpty;KDE4__kfile"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kio;KDE4__kdeui;KDE4__kdecore;/usr/lib/libQtCore.so;/usr/lib/libQtGui.so;/usr/lib/libQt3Support.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkde3support.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkde3support.so.4"
  )

# Import target "KDE4__kfile" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kfile APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kfile PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kio;KDE4__solid"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdeui;KDE4__kdecore;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkfile.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkfile.so.4"
  )

# Import target "KDE4__knewstuff2" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__knewstuff2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__knewstuff2 PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kio"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdeui;KDE4__kdecore;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libknewstuff2.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libknewstuff2.so.4"
  )

# Import target "KDE4__kparts" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kparts APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kparts PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kio;KDE4__kdeui;KDE4__kdecore;/usr/lib/libQtCore.so;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkparts.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkparts.so.4"
  )

# Import target "KDE4__kutils" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kutils APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kutils PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kio;KDE4__kparts"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdecore;KDE4__kdeui;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkutils.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkutils.so.4"
  )

# Import target "KDE4__threadweaver" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__threadweaver APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__threadweaver PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "/usr/lib/libQtCore.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libthreadweaver.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libthreadweaver.so.4"
  )

# Import target "KDE4__khtml" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__khtml APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__khtml PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__ktexteditor"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kparts;KDE4__kjs;KDE4__kio;KDE4__kdeui;KDE4__kdecore;/usr/lib/libQtCore.so;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkhtml.so.5.2.0"
  IMPORTED_SONAME_DEBIAN "libkhtml.so.5"
  )

# Import target "KDE4__ktexteditor" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__ktexteditor APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__ktexteditor PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdecore"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kparts"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libktexteditor.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libktexteditor.so.4"
  )

# Import target "KDE4__kmediaplayer" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kmediaplayer APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kmediaplayer PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "/usr/lib/libQtCore.so;KDE4__kdecore;KDE4__kdeui;KDE4__kio;KDE4__kparts;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkmediaplayer.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkmediaplayer.so.4"
  )

# Import target "KDE4__kimproxy" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kimproxy APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kimproxy PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdecore;KDE4__kdeui;KDE4__kio"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "/usr/lib/libQtCore.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkimproxy.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkimproxy.so.4"
  )

# Import target "KDE4__knotifyconfig" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__knotifyconfig APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__knotifyconfig PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdeui;KDE4__kio"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdecore;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libknotifyconfig.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libknotifyconfig.so.4"
  )

# Import target "KDE4__kdnssd" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kdnssd APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kdnssd PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdeui"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdecore;/usr/lib/libQtCore.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkdnssd.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkdnssd.so.4"
  )

# Import target "KDE4__krosscore" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__krosscore APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__krosscore PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kdeui"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdecore;/usr/lib/libQtScript.so;/usr/lib/libQtXml.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkrosscore.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkrosscore.so.4"
  )

# Import target "KDE4__krossui" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__krossui APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__krossui PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__krosscore;KDE4__kdecore;KDE4__kdeui;KDE4__kio;KDE4__kparts"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libkrossui.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libkrossui.so.4"
  )

# Import target "KDE4__plasma" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__plasma APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__plasma PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBIAN "KDE4__kio;KDE4__kfile;KDE4__knewstuff2;KDE4__threadweaver;KDE4__solid"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "KDE4__kdeui;KDE4__kdecore;/usr/lib/libQtGui.so"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libplasma.so.3.0.0"
  IMPORTED_SONAME_DEBIAN "libplasma.so.3"
  )

# Import target "KDE4__nepomuk" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__nepomuk APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__nepomuk PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBIAN "/usr/lib/libQtCore.so;KDE4__kdecore;KDE4__kdeui"
  IMPORTED_LOCATION_DEBIAN "/usr/lib/libnepomuk.so.4.2.0"
  IMPORTED_SONAME_DEBIAN "libnepomuk.so.4"
  )

# Commands beyond this point should not need to know the version.
SET(CMAKE_IMPORT_FILE_VERSION)
