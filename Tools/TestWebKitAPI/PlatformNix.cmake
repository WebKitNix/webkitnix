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
    "${PLATFORM_DIR}/nix/"
    ${TESTWEBKITAPI_DIR}/nix
    ${TOOLS_DIR}/Shared/nix
)

set(test_main_SOURCES
    ${TESTWEBKITAPI_DIR}/nix/main.cpp
    ${TESTWEBKITAPI_DIR}/nix/MainLoop.cpp
)

list(APPEND TestWebKitAPIInjectedBundle_SOURCES
    ${TESTWEBKITAPI_DIR}/nix/InjectedBundleControllerNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/PlatformUtilitiesNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/MainLoop.cpp
    ${TESTWEBKITAPI_DIR}/Tests/nix/WebViewWebProcessCrashed_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/nix/WebWorker_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/nix/WebThemeEngine_Bundle.cpp
)

set(TestWebKitAPIBase_SOURCES
    ${test_main_SOURCES}
    ${TESTWEBKITAPI_DIR}/JavaScriptTest.cpp
    ${TESTWEBKITAPI_DIR}/PlatformUtilities.cpp
    ${TESTWEBKITAPI_DIR}/nix/PageLoader.cpp
    ${TESTWEBKITAPI_DIR}/nix/PlatformUtilitiesNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/PlatformWebViewNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/TestsControllerNix.cpp
    ${TOOLS_DIR}/Shared/nix/GLUtilities.cpp
)

set(webkit2Test_LIBRARIES
    TestWebKitAPIBase
    WTF
    WebKit2
    gtest
    ${PNG_LIBRARY}
)

if (WTF_USE_OPENGL_ES_2)
    list(APPEND webkit2Test_LIBRARIES ${OPENGLES2_LIBRARIES})
    include_directories(${OPENGLES2_INCLUDE_DIRS})
else ()
    list(APPEND webkit2Test_LIBRARIES ${OPENGL_LIBRARIES})
    include_directories(${OPENGL_INCLUDE_DIR})
endif ()

if (WTF_USE_EGL)
    list(APPEND TestWebKitAPIBase_SOURCES ${TOOLS_DIR}/Shared/nix/GLUtilitiesEGL.cpp)
    list(APPEND webkit2Test_LIBRARIES ${EGL_LIBRARY})
else ()
    list(APPEND TestWebKitAPIBase_SOURCES ${TOOLS_DIR}/Shared/nix/GLUtilitiesGLX.cpp)
endif ()

set(webcoreTestList
    LayoutUnit
    KURL
)

set(webkit2TestList
    WebKit2/CookieManager
    WebKit2/DOMWindowExtensionNoCache
    WebKit2/DocumentStartUserScriptAlertCrash
    WebKit2/EvaluateJavaScript
    WebKit2/FailedLoad
    WebKit2/Find
    WebKit2/FrameMIMETypeHTML
    WebKit2/FrameMIMETypePNG
    WebKit2/GetInjectedBundleInitializationUserDataCallback
    WebKit2/InjectedBundleBasic
    WebKit2/InjectedBundleInitializationUserDataCallbackWins
    WebKit2/LoadAlternateHTMLStringWithNonDirectoryURL
    WebKit2/LoadCanceledNoServerRedirectCallback
    WebKit2/NewFirstVisuallyNonEmptyLayout
    WebKit2/PageLoadBasic
    WebKit2/PageLoadDidChangeLocationWithinPageForFrame
    WebKit2/ParentFrame
    WebKit2/PreventEmptyUserAgent
    WebKit2/PrivateBrowsingPushStateNoHistoryCallback
    WebKit2/RestoreSessionStateContainingFormData
    WebKit2/WKConnection
    WebKit2/WKString
    WebKit2/WKStringJSString
    WebKit2/WillSendSubmitEvent
    nix/OverflowScroll
    nix/SuspendResume
    nix/WebThemeEngine
    nix/WebViewFindZoomableArea
    nix/WebViewPaintToCurrentGLContext
    nix/WebViewTranslated
    nix/WebViewTranslatedScaled
    nix/WebViewUpdateTextInputState
    nix/WebViewViewport
    nix/WebViewWebProcessCrashed
    nix/WebWorker
)

set(webkit2FailTestList
    WebKit2/CanHandleRequest
    WebKit2/DOMWindowExtensionBasic
    WebKit2/DownloadDecideDestinationCrash
    WebKit2/ForceRepaint
    WebKit2/NewFirstVisuallyNonEmptyLayoutForImages
    WebKit2/NewFirstVisuallyNonEmptyLayoutFrames
    WebKit2/NewFirstVisuallyNonEmptyLayoutFails
    WebKit2/ShouldGoToBackForwardListItem
    WebKit2/WKPageGetScaleFactorNotZero
)

# Tests disabled because of missing features on the test harness:
#
#   WebKit2/AboutBlankLoad
#   WebKit2/HitTestResultNodeHandle
#   WebKit2/MouseMoveAfterCrash
#   WebKit2/ResponsivenessTimerDoesntFireEarly
#   WebKit2/SpacebarScrolling
#   WebKit2/WKPreferences
#
# Flaky test, fails on Release but passes on Debug:
#
#   WebKit2/UserMessage
