add_custom_target(forwarding-headersNixForTestWebKitAPI
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include nix
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include CoordinatedGraphics
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${TESTWEBKITAPI_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include nix
)
set(ForwardingHeadersForTestWebKitAPI_NAME forwarding-headersNixForTestWebKitAPI)

if (ENABLE_SOUP)
    add_custom_target(forwarding-headersSoupForTestWebKitAPI
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${TESTWEBKITAPI_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
    )
    set(ForwardingNetworkHeadersForTestWebKitAPI_NAME forwarding-headersSoupForTestWebKitAPI)
else ()
    add_custom_target(forwarding-headerscurlForTestWebKitAPI
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include curl
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${TESTWEBKITAPI_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include curl
    )
    set(ForwardingNetworkHeadersForTestWebKitAPI_NAME forwarding-headerscurlForTestWebKitAPI)
endif ()

include_directories(
    ${WEBKIT2_DIR}/UIProcess/API/nix
    ${GLIB_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
)

set(test_main_SOURCES
    ${TESTWEBKITAPI_DIR}/nix/main.cpp
    ${TESTWEBKITAPI_DIR}/nix/MainLoop.cpp
)

set(bundle_harness_SOURCES
    ${TESTWEBKITAPI_DIR}/nix/InjectedBundleControllerNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/PlatformUtilitiesNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/MainLoop.cpp
)

set(webkit2_api_harness_SOURCES
    ${TESTWEBKITAPI_DIR}/nix/PlatformUtilitiesNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/PlatformWebViewNix.cpp
)

# The list below works like a test expectation. Tests in the
# test_{webkit2_api|webcore}_BINARIES list are added to the test runner and
# tried on the bots on every build. Tests in test_{webkit2_api|webcore}_fail_BINARIES
# are compiled and suffixed with fail and skipped from the test runner.
#
# Make sure that the tests are passing on both Debug and
# Release builds before adding it to test_{webkit2_api|webcore}_BINARIES.

set(test_webcore_BINARIES
    LayoutUnit
    KURL
)

set(test_webkit2_api_BINARIES
    CookieManager
    DOMWindowExtensionNoCache
    DocumentStartUserScriptAlertCrash
    EvaluateJavaScript
    FailedLoad
    Find
    FrameMIMETypeHTML
    FrameMIMETypePNG
    GetInjectedBundleInitializationUserDataCallback
    InjectedBundleBasic
    InjectedBundleInitializationUserDataCallbackWins
    LoadAlternateHTMLStringWithNonDirectoryURL
    LoadCanceledNoServerRedirectCallback
    NewFirstVisuallyNonEmptyLayout
    PageLoadBasic
    PageLoadDidChangeLocationWithinPageForFrame
    ParentFrame
    PreventEmptyUserAgent
    PrivateBrowsingPushStateNoHistoryCallback
    RestoreSessionStateContainingFormData
    WKConnection
    WKString
    WKStringJSString
    WillSendSubmitEvent
)

set(test_webkit2_api_fail_BINARIES
    CanHandleRequest
    DOMWindowExtensionBasic
    DownloadDecideDestinationCrash
    ForceRepaint
    NewFirstVisuallyNonEmptyLayoutForImages
    NewFirstVisuallyNonEmptyLayoutFrames
    NewFirstVisuallyNonEmptyLayoutFails
    ShouldGoToBackForwardListItem
    WKPageGetScaleFactorNotZero
)

# Tests disabled because of missing features on the test harness:
#
#   AboutBlankLoad
#   HitTestResultNodeHandle
#   MouseMoveAfterCrash
#   ResponsivenessTimerDoesntFireEarly
#   SpacebarScrolling
#   WKPreferences
#
# Flaky test, fails on Release but passes on Debug:
#
#   UserMessage


# WebKitNix API tests
set(TestWebKitNixAPIBase_SOURCES
    ${test_main_SOURCES}
    ${TESTWEBKITAPI_DIR}/nix/PlatformUtilitiesNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/PageLoader.cpp
    ${TOOLS_DIR}/Shared/nix/GLUtilities.cpp
    ${TESTWEBKITAPI_DIR}/nix/TestsControllerNix.cpp
    ${TESTWEBKITAPI_DIR}/PlatformUtilities.cpp
)

set(TestWebKitNixAPIBase_LIBRARIES
    ${PNG_LIBRARY}
    ${GLIB_LIBRARIES}
)

if (WTF_USE_OPENGL_ES_2)
    list(APPEND TestWebKitNixAPIBase_LIBRARIES ${OPENGLES2_LIBRARIES})
    include_directories(${OPENGLES2_INCLUDE_DIRS})
else ()
    list(APPEND TestWebKitNixAPIBase_LIBRARIES ${OPENGL_LIBRARIES})
    include_directories(${OPENGL_INCLUDE_DIR})
endif ()

if (WTF_USE_EGL)
    list(APPEND TestWebKitNixAPIBase_SOURCES ${TOOLS_DIR}/Shared/nix/GLUtilitiesEGL.cpp)
    list(APPEND TestWebKitNixAPIBase_LIBRARIES ${EGL_LIBRARY})
else ()
  list(APPEND TestWebKitNixAPIBase_SOURCES ${TOOLS_DIR}/Shared/nix/GLUtilitiesGLX.cpp)
endif ()

add_library(TestWebKitNixAPIBase ${TestWebKitNixAPIBase_SOURCES})
target_link_libraries(TestWebKitNixAPIBase ${TestWebKitNixAPIBase_LIBRARIES})
add_dependencies(TestWebKitNixAPIBase ${WebKit2_LIBRARY_NAME} ${ForwardingHeadersForTestWebKitAPI_NAME} ${ForwardingNetworkHeadersForTestWebKitAPI_NAME})

list(APPEND bundle_harness_SOURCES
    ${TESTWEBKITAPI_DIR}/Tests/nix/WebViewWebProcessCrashed_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/nix/WebWorker_Bundle.cpp
)

include_directories(
    ${TESTWEBKITAPI_DIR}/nix
    ${TOOLS_DIR}/Shared/nix
)

set(test_webkitnix_api_LIBRARIES
    TestWebKitNixAPIBase
    ${CAIRO_LIBRARIES}
    ${WebKit2_LIBRARY_NAME}
    gtest
)

set(test_webkitnix_api_BINARIES
    SuspendResume
    WebViewPaintToCurrentGLContext
    WebViewTranslated
    WebViewViewport
    WebViewFindZoomableArea
    WebViewTranslatedScaled
    WebViewUpdateTextInputState
    WebWorker
    WebViewWebProcessCrashed
)

set(test_webkitnix_api_fail_BINARIES
)

foreach(testName ${test_webkitnix_api_BINARIES})
    add_executable(test_webkitnix_api_${testName} ${TESTWEBKITAPI_DIR}/Tests/nix/${testName}.cpp)
    add_test(test_webkitnix_api_${testName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_webkitnix_api_${testName})
    set_tests_properties(test_webkitnix_api_${testName} PROPERTIES TIMEOUT 60)
    target_link_libraries(test_webkitnix_api_${testName} ${test_webkitnix_api_LIBRARIES})
endforeach()

# We don't run tests that are expected to fail. We could use the WILL_FAIL
# property, but it reports failure when the test crashes or timeouts and would
# make the bot red.

foreach(testName ${test_webkitnix_api_fail_BINARIES})
    add_executable(test_webkitnix_api_fail_${testName} ${TESTWEBKITAPI_DIR}/Tests/nix/${testName}.cpp)
    target_link_libraries(test_webkitnix_api_fail_${testName} ${test_webkitnix_api_LIBRARIES})
endforeach()
