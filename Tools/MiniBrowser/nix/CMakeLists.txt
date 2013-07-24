find_package(X11 REQUIRED)

set(MiniBrowser_DIR "${TOOLS_DIR}/MiniBrowser/nix")
    
set(MINIBROWSER_INJECTEDBUNDLE_DIR ${CMAKE_BINARY_DIR}/lib/)
add_definitions(-std=c++0x -DMINIBROWSER_INJECTEDBUNDLE_DIR="${MINIBROWSER_INJECTEDBUNDLE_DIR}")

set(MiniBrowser_SOURCES
    ${MiniBrowser_DIR}/main.cpp
    ${MiniBrowser_DIR}/MiniBrowser.cpp
    ${MiniBrowser_DIR}/BrowserControl.cpp
    ${MiniBrowser_DIR}/GestureRecognizer.cpp
    ${MiniBrowser_DIR}/Options.cpp
    ${MiniBrowser_DIR}/TouchMocker.cpp
    ${MiniBrowser_DIR}/XlibEventSource.cpp
    ${MiniBrowser_DIR}/Components/VisualComponent.cpp
    ${MiniBrowser_DIR}/Components/BrowserWindow.cpp
    ${MiniBrowser_DIR}/Components/WebView.cpp
    ${MiniBrowser_DIR}/Components/Button.cpp
    ${MiniBrowser_DIR}/Components/UrlBar.cpp
    ${TOOLS_DIR}/Shared/nix/GLUtilities.cpp
)

set(MiniBrowser_LIBRARIES
    WebKit2
    ${OPENGL_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GLIB_GTHREAD_LIBRARIES}
    ${CAIRO_LIBRARIES}
    ${X11_LIBRARIES}
    ${PNG_LIBRARY}
)

set(MiniBrowser_INCLUDE_DIRECTORIES
    ${GLIB_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${DERIVED_SOURCES_WEBKIT2_DIR}/include
    ${WEBKIT2_DIR}/UIProcess/API/nix
    ${WEBKIT2_DIR}/UIProcess/API/C/nix
    ${WEBKIT2_DIR}
    ${CMAKE_SOURCE_DIR}/Source
    ${WTF_DIR}
    ${X11_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${TOOLS_DIR}/Shared/nix
    ${CMAKE_BINARY_DIR}
    ${JAVASCRIPTCORE_DIR}
    ${PNG_INCLUDE_DIRS}
)

if (WTF_USE_EGL)
  list(APPEND MiniBrowser_SOURCES ${MiniBrowser_DIR}/Components/EGLWebView.cpp)
  list(APPEND MiniBrowser_LIBRARIES ${EGL_LIBRARY})
  list(APPEND MiniBrowser_INCLUDE_DIRECTORIES ${EGL_INCLUDE_DIR})
else ()
  list(APPEND MiniBrowser_SOURCES ${MiniBrowser_DIR}/Components/GLXWebView.cpp)
endif ()

include_directories(${MiniBrowser_INCLUDE_DIRECTORIES})

add_definitions(-DMINIBROWSER_ICON_DIR="${MiniBrowser_DIR}/Components/icons")
add_executable(MiniBrowser ${MiniBrowser_SOURCES})
target_link_libraries(MiniBrowser ${MiniBrowser_LIBRARIES})
set_target_properties(MiniBrowser PROPERTIES FOLDER "Tools")

add_subdirectory(InjectedBundle)
