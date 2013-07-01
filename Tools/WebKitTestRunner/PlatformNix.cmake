list(APPEND WebKitTestRunner_LINK_FLAGS
)

add_custom_target(forwarding-headersNixForWebKitTestRunner
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT_TESTRUNNER_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include nix
)
set(ForwardingHeadersForWebKitTestRunner_NAME forwarding-headersNixForWebKitTestRunner)

if (WTF_USE_CURL)
    add_custom_target(forwarding-headersNetworkForWebKitTestRunner
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT_TESTRUNNER_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include curl
    )
else ()
    add_custom_target(forwarding-headersNetworkForWebKitTestRunner
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT_TESTRUNNER_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
    )
endif ()
set(ForwardingNetworkHeadersForWebKitTestRunner_NAME forwarding-headersNetworkForWebKitTestRunner)

list(APPEND WebKitTestRunner_SOURCES
    ${WTF_DIR}/wtf/MD5.cpp
    ${WEBKIT_TESTRUNNER_DIR}/cairo/TestInvocationCairo.cpp

    ${WEBKIT_TESTRUNNER_DIR}/nix/EventSenderProxyNix.cpp
    ${WEBKIT_TESTRUNNER_DIR}/nix/PlatformWebViewNix.cpp
    ${WEBKIT_TESTRUNNER_DIR}/nix/TestControllerNix.cpp
    ${TOOLS_DIR}/Shared/nix/GLUtilities.cpp
    ${WEBKIT_TESTRUNNER_DIR}/nix/main.cpp
)

list(APPEND WebKitTestRunner_INCLUDE_DIRECTORIES
    ${WEBKIT2_DIR}/UIProcess/API/nix
    ${WEBKIT2_DIR}/UIProcess/API/C/nix
    ${WEBKIT2_DIR}/UIProcess/API/C/CoordinatedGraphics
    "${WTF_DIR}/wtf/gobject"

    ${CAIRO_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${FONTCONFIG_INCLUDE_DIR}

    "${PLATFORM_DIR}/nix/"
    "${TOOLS_DIR}/Shared/nix"
)

set(WebKitTestRunner_LIBRARIES
    WTF
    WebKit2
    ${CAIRO_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${PNG_LIBRARY}
)

if (WTF_USE_EGL)
    list(APPEND WebKitTestRunner_SOURCES ${TOOLS_DIR}/Shared/nix/GLUtilitiesEGL.cpp)
    list(APPEND WebKitTestRunner_LIBRARIES ${EGL_LIBRARY})
else ()
    list(APPEND WebKitTestRunner_SOURCES ${TOOLS_DIR}/Shared/nix/GLUtilitiesGLX.cpp)
endif ()

if (WTF_USE_OPENGL_ES_2)
    list(APPEND WebKitTestRunner_LIBRARIES ${OPENGLES2_LIBRARIES})
    include_directories(${OPENGLES2_INCLUDE_DIRS})
else ()
    list(APPEND WebKitTestRunner_LIBRARIES ${OPENGL_LIBRARIES})
    include_directories(${OPENGL_INCLUDE_DIR})
endif ()

list(APPEND WebKitTestRunnerInjectedBundle_SOURCES
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/nix/FontManagement.cpp
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/nix/ActivateFontsNix.cpp
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/nix/InjectedBundleNix.cpp
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/nix/TestRunnerNix.cpp
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/nix/PlatformClient.cpp
)

if (ENABLE_GAMEPAD)
    list(APPEND WebKitTestRunnerInjectedBundle_SOURCES ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/nix/GamepadControllerNix.cpp)
endif ()

# FIXME: DOWNLOADED_FONTS_DIR should not hardcode the directory
# structure. See <https://bugs.webkit.org/show_bug.cgi?id=81475>.
add_definitions(-DFONTS_CONF_DIR="${TOOLS_DIR}/DumpRenderTree/gtk/fonts"
                -DDOWNLOADED_FONTS_DIR="${CMAKE_SOURCE_DIR}/WebKitBuild/Dependencies/Root/webkitgtk-test-fonts"
                -DTHEME_DIR="${THEME_BINARY_DIR}"
                -DLOG_DISABLED=1)

set(ImageDiff_SOURCES
    ${WEBKIT_TESTRUNNER_DIR}/nix/ImageDiff.cpp
)
include_directories(${PNG_INCLUDE_DIRS})
add_executable(ImageDiff ${ImageDiff_SOURCES})
target_link_libraries(ImageDiff ${PNG_LIBRARY})
set_target_properties(ImageDiff PROPERTIES FOLDER "Tools")
