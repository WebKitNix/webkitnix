set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Use -femit-struct-debug-baseonly to reduce the size of WebCore static library
    set(CMAKE_CXX_FLAGS_DEBUG "-g -femit-struct-debug-baseonly" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
endif ()

set(SHARED_CORE 0)
set(ENABLE_WEBKIT 0)
set(ENABLE_WEBKIT2 1)

set(JSC_EXECUTABLE_NAME jsc)
set(WTF_LIBRARY_NAME wtf_nix)
set(JavaScriptCore_LIBRARY_NAME javascriptcore_nix)
set(WebCore_LIBRARY_NAME webcore_nix)
set(WebKit2_LIBRARY_NAME WebKitNix)

# Macro used to define flags that make non sense to be undefined, e.g. ENABLE_GLIB_SUPPORT on Nix
set(_WEBKIT_HARDCODED_DEFINES "")
macro(SET_HARDCODED_DEFINE _name)
    list(APPEND _WEBKIT_HARDCODED_DEFINES ${_name})
    set(${_name} ON)
endmacro()

# Set default values for features based on Features${PORT}.config
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/Source/cmake/Features${PORT}.config" featureData NEWLINE_CONSUME)
string(REPLACE ";" "\\\\;" featureData "${featureData}")
string(REPLACE "\n" ";" featureData "${featureData}")

set(_WEBKIT_AVAILABLE_OPTIONS "")
foreach (feature IN ITEMS ${featureData})
    string(REGEX MATCH "^[^\#].*" validRecord ${feature})
    if (validRecord)
        string(REGEX REPLACE "([0-9A-Z_-]+).*" "\\1" defineName "${feature}")
        string(REGEX REPLACE "[0-9A-Z_-]+ *= *(.*)" "\\1" value "${feature}")
        option(${defineName} "Toggle ${defineName}" ${value})
        list(APPEND _WEBKIT_AVAILABLE_OPTIONS ${defineName})
    endif ()
endforeach ()

add_definitions(-DBUILDING_NIX__=1)
add_definitions(-DWTF_PLATFORM_NIX=1)
set(WTF_PLATFORM_NIX 1)

# We set this because we now use Source/Platform/chromium
add_definitions(-DWEBKIT_IMPLEMENTATION=1)
add_definitions(-DWEBKIT_DLL=1)

find_package(Cairo 1.12.8 REQUIRED)
find_package(Fontconfig 2.8.0 REQUIRED)
find_package(Freetype REQUIRED)
find_package(GLIB 2.36.0 REQUIRED COMPONENTS gio gobject gmodule gthread)
find_package(HarfBuzz)
find_package(ICU REQUIRED)
find_package(JPEG REQUIRED)
find_package(LibXml2 2.6 REQUIRED)
find_package(LibXslt 1.1.7 REQUIRED)
find_package(PNG REQUIRED)
find_package(Sqlite REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

if (ENABLE_SOUP)
    find_package(LibSoup 2.42.0 REQUIRED)
    SET_HARDCODED_DEFINE(WTF_USE_SOUP)
else ()
    find_package(CURL REQUIRED)
    SET_HARDCODED_DEFINE(WTF_USE_CURL)
endif ()

if (WTF_USE_OPENGL_ES_2)
    find_package(OpenGLES2 REQUIRED)
    set(WTF_USE_EGL ON)
    set(WTF_USE_OPENGL_ES_2 ON)
else ()
    find_package(OpenGL REQUIRED)
    SET_HARDCODED_DEFINE(WTF_USE_OPENGL)
endif ()

SET_HARDCODED_DEFINE(WTF_USE_GLIB)
SET_HARDCODED_DEFINE(WTF_USE_3D_GRAPHICS)
SET_HARDCODED_DEFINE(WTF_USE_ACCELERATED_COMPOSITING)
SET_HARDCODED_DEFINE(WTF_USE_CAIRO)
SET_HARDCODED_DEFINE(WTF_USE_COORDINATED_GRAPHICS)
SET_HARDCODED_DEFINE(WTF_USE_FREETYPE)
SET_HARDCODED_DEFINE(WTF_USE_HARFBUZZ_NG)
SET_HARDCODED_DEFINE(WTF_USE_ICU_UNICODE)
SET_HARDCODED_DEFINE(WTF_USE_PTHREADS)
SET_HARDCODED_DEFINE(WTF_USE_TEXTURE_MAPPER)
SET_HARDCODED_DEFINE(WTF_USE_TEXTURE_MAPPER_GL)
SET_HARDCODED_DEFINE(WTF_USE_IOSURFACE_CANVAS_BACKING_STORE)
SET_HARDCODED_DEFINE(WTF_USE_TILED_BACKING_STORE)
SET_HARDCODED_DEFINE(WTF_USE_CROSS_PLATFORM_CONTEXT_MENUS)
if (NOT ENABLE_SVG)
    set(ENABLE_SVG_FONTS OFF)
endif ()

if (WTF_USE_EGL)
    find_package(EGL REQUIRED)
    SET_HARDCODED_DEFINE(WTF_USE_EGL)
else ()
    find_package(X11 REQUIRED)
    SET_HARDCODED_DEFINE(HAVE_GLX)
    SET_HARDCODED_DEFINE(WTF_USE_GLX)
    SET_HARDCODED_DEFINE(WTF_PLATFORM_X11)
    SET_HARDCODED_DEFINE(WTF_USE_GRAPHICS_SURFACE)
endif ()


macro(PROCESS_WEBKIT_OPTIONS)
    # Show all options on screen
    message(STATUS "Enabled features:")

    set(_MAX_FEATURE_LENGTH 0)
    foreach (_name ${_WEBKIT_AVAILABLE_OPTIONS})
        string(LENGTH ${_name} _NAME_LENGTH)
        if (_NAME_LENGTH GREATER _MAX_FEATURE_LENGTH)
            set(_MAX_FEATURE_LENGTH ${_NAME_LENGTH})
        endif ()
    endforeach ()

    set(_SHOULD_PRINT_POINTS OFF)
    foreach (_name ${_WEBKIT_AVAILABLE_OPTIONS})
        string(LENGTH ${_name} _NAME_LENGTH)

        set(_MESSAGE " ${_name} ")

        if (_SHOULD_PRINT_POINTS)
            foreach (IGNORE RANGE ${_NAME_LENGTH} ${_MAX_FEATURE_LENGTH})
                set(_MESSAGE "${_MESSAGE} ")
            endforeach ()
            set(_SHOULD_PRINT_POINTS OFF)
        else ()
            foreach (IGNORE RANGE ${_NAME_LENGTH} ${_MAX_FEATURE_LENGTH})
                set(_MESSAGE "${_MESSAGE}.")
            endforeach ()
            set(_SHOULD_PRINT_POINTS ON)
        endif ()

        if (${_name})
            list(APPEND FEATURE_DEFINES ${_name})
            set(FEATURE_DEFINES_WITH_SPACE_SEPARATOR "${FEATURE_DEFINES_WITH_SPACE_SEPARATOR} ${_name}")
        endif ()

        message(STATUS "${_MESSAGE} ${${_name}}")
    endforeach ()

    # Create cmakeconfig.h.in based on the features we have available
    set(CMK_CONFIG_H_IN "${CMAKE_BINARY_DIR}/cmakeconfig.h.in")

    file(WRITE ${CMK_CONFIG_H_IN}
        "#ifndef CMAKECONFIG_H\n"
        "#define CMAKECONFIG_H\n\n")

    file(APPEND ${CMK_CONFIG_H_IN} "\n// Hardcoded defines\n\n")
    foreach (_name ${_WEBKIT_HARDCODED_DEFINES})
        file(APPEND ${CMK_CONFIG_H_IN} "#define ${_name} 1\n")
    endforeach ()

    file(APPEND ${CMK_CONFIG_H_IN} "\n// WebKit options\n\n")
    foreach (_name ${_WEBKIT_AVAILABLE_OPTIONS})
        file(APPEND ${CMK_CONFIG_H_IN} "#cmakedefine01 ${_name}\n")
    endforeach ()

    file(APPEND ${CMK_CONFIG_H_IN} "\n#endif // CMAKECONFIG_H\n")

    file(MD5 ${CMK_CONFIG_H_IN} CMK_CONFIG_H_IN_NEW_MD5)
    if (NOT "${CMK_CONFIG_H_IN_MD5}" STREQUAL "${CMK_CONFIG_H_IN_NEW_MD5}")
        message(STATUS "Creating cmakeconfig.h")
        configure_file(${CMK_CONFIG_H_IN} "${CMAKE_BINARY_DIR}/cmakeconfig.h" @ONLY)
        set(CMK_CONFIG_H_IN_MD5 "${CMK_CONFIG_H_IN_NEW_MD5}" CACHE STRING "Md5 of ${CMK_CONFIG_H_IN}" FORCE)
    endif ()
endmacro()

PROCESS_WEBKIT_OPTIONS()
