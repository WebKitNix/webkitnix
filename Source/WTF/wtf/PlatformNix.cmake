list(APPEND WTF_SOURCES
    gtk/MainThreadGtk.cpp
    gobject/GOwnPtr.cpp
    gobject/GRefPtr.cpp
    gobject/GlibUtilities.cpp
    nix/RunLoopNix.cpp
)

list(APPEND WTF_LIBRARIES
    pthread
    ${ZLIB_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GLIB_GIO_LIBRARIES}
)

list(APPEND WTF_INCLUDE_DIRECTORIES
    ${GLIB_INCLUDE_DIRS}
)
