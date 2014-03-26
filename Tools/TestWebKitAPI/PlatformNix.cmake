add_definitions(-DWTF_PLATFORM_NIX)

set(TestWebKitAPI_INCLUDE_DIRS
    ${DERIVED_SOURCES_WEBKIT2_DIR}/include
    ${WEBKIT2_DIR}/UIProcess/API/nix
    ${GLIB_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    "${PLATFORM_DIR}/nix/"
    ${TESTWEBKITAPI_DIR}/nix
    ${TOOLS_DIR}/Shared/nix
)

add_custom_target(forwarding-headersNixForTestWebKitAPI
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include nix
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include CoordinatedGraphics
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${TESTWEBKITAPI_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include nix
)

if (WTF_USE_CURL)
    add_custom_target(forwarding-headersNetworkForTestWebKitAPI
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include curl
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${TESTWEBKITAPI_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include curl
    )
else ()
    list(APPEND TestWebKitAPI_INCLUDE_DIRS
        ${LIBSOUP_INCLUDE_DIRS}
    )
    add_custom_target(forwarding-headersNetworkForTestWebKitAPI
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${TESTWEBKITAPI_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
    )
endif ()
set(ForwardingHeadersForTestWebKitAPI_NAME forwarding-headersNixForTestWebKitAPI)
add_dependencies(forwarding-headersNixForTestWebKitAPI forwarding-headersNetworkForTestWebKitAPI)

include_directories(${TestWebKitAPI_INCLUDE_DIRS})

set(test_main_SOURCES
    ${TESTWEBKITAPI_DIR}/nix/main.cpp
    ${TESTWEBKITAPI_DIR}/nix/MainLoop.cpp
)

set(bundle_harness_SOURCES
    ${TESTWEBKITAPI_DIR}/nix/AudioUtilitiesNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/InjectedBundleControllerNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/PlatformUtilitiesNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/MainLoop.cpp
    ${TESTWEBKITAPI_DIR}/Tests/nix/WebAudio_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/nix/WebThemeEngine_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/DidAssociateFormControls_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/MouseMoveAfterCrash_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WebWorker_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WillLoad_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/CoordinatedGraphics/WebViewWebProcessCrashed_Bundle.cpp
)

if (ENABLE_MEDIA_STREAM)
    list(APPEND bundle_harness_SOURCES ${TESTWEBKITAPI_DIR}/Tests/nix/MediaStreamAudio_Bundle.cpp)
endif ()

set(webkit2_api_harness_SOURCES
    ${test_main_SOURCES}
    ${TESTWEBKITAPI_DIR}/JavaScriptTest.cpp
    ${TESTWEBKITAPI_DIR}/PlatformUtilities.cpp
    ${TESTWEBKITAPI_DIR}/nix/PageLoader.cpp
    ${TESTWEBKITAPI_DIR}/nix/PlatformUtilitiesNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/PlatformWebViewNix.cpp
    ${TESTWEBKITAPI_DIR}/nix/TestsControllerNix.cpp
    ${TOOLS_DIR}/Shared/nix/GLUtilities.cpp
)

list(APPEND TestWebKitAPI_LIBRARIES
    ${CAIRO_LIBRARIES}
)

list(REMOVE_ITEM test_webkit2_api_LIBRARIES JavaScriptCore)

list(APPEND test_webkit2_api_LIBRARIES
    ${PNG_LIBRARY}
    ${CAIRO_LIBRARIES}
)

if (WTF_USE_OPENGL_ES_2)
    list(APPEND test_webkit2_api_LIBRARIES ${OPENGLES2_LIBRARIES})
    include_directories(${OPENGLES2_INCLUDE_DIRS})
else ()
    list(APPEND test_webkit2_api_LIBRARIES ${OPENGL_LIBRARIES})
    include_directories(${OPENGL_INCLUDE_DIR})
endif ()

if (WTF_USE_EGL)
    list(APPEND webkit2_api_harness_SOURCES ${TOOLS_DIR}/Shared/nix/GLUtilitiesEGL.cpp)
    list(APPEND test_webkit2_api_LIBRARIES ${EGL_LIBRARY})
else ()
    list(APPEND webkit2_api_harness_SOURCES ${TOOLS_DIR}/Shared/nix/GLUtilitiesGLX.cpp)
endif ()

set(test_webcore_BINARIES
    LayoutUnit
    URL
)

set(test_webkit2_api_BINARIES
    ../nix/WKViewRestoreZoomAndScrollBackForward
    ../nix/OverflowScroll
    ../nix/ScreenSize
    ../nix/SuspendResume
    ../nix/WKCoordinatedSceneLockState
    ../nix/WebAudio
    ../nix/WebThemeEngine
    ../nix/WebViewFindZoomableArea
    ../nix/WebViewPaintToCurrentGLContext
    ../nix/WebViewTranslated
    ../nix/WebViewTranslatedScaled
    ../nix/WebViewUpdateTextInputState
    ../nix/WebViewViewport

    AboutBlankLoad
    CloseThenTerminate
    CookieManager
    DOMWindowExtensionBasic
    DOMWindowExtensionNoCache
    DidAssociateFormControls
    DocumentStartUserScriptAlertCrash
    DownloadDecideDestinationCrash
    EvaluateJavaScript
    FailedLoad
    Find
    FrameMIMETypeHTML
    FrameMIMETypePNG
    GetInjectedBundleInitializationUserDataCallback
    HitTestResultNodeHandle
    InjectedBundleBasic
    InjectedBundleFrameHitTest
    InjectedBundleInitializationUserDataCallbackWins
    LoadAlternateHTMLStringWithNonDirectoryURL
    LoadCanceledNoServerRedirectCallback
    LoadPageOnCrash
    MouseMoveAfterCrash
    NewFirstVisuallyNonEmptyLayout
    NewFirstVisuallyNonEmptyLayoutForImages
    PageLoadBasic
    PageLoadDidChangeLocationWithinPageForFrame
    ParentFrame
    PreventEmptyUserAgent
    PrivateBrowsingPushStateNoHistoryCallback
    ReloadPageAfterCrash
    ResizeWindowAfterCrash
    ResponsivenessTimerDoesntFireEarly
    RestoreSessionStateContainingFormData
    ShouldGoToBackForwardListItem
    SpacebarScrolling
    TerminateTwice
    UserMessage
    WKPreferences
    WKString
    WKStringJSString
    WKURL
    WebWorker
    WillSendSubmitEvent
    WKPageGetScaleFactorNotZero

    CoordinatedGraphics/WebViewWebProcessCrashed
    CoordinatedGraphics/WKViewUserViewportToContents
)

if (ENABLE_MEDIA_STREAM)
    list(APPEND test_webkit2_api_BINARIES ../nix/MediaStreamAudio)
endif ()

set(test_webkit2_api_fail_BINARIES
    CanHandleRequest
    ForceRepaint
    NewFirstVisuallyNonEmptyLayoutFrames
    NewFirstVisuallyNonEmptyLayoutFails
    WillLoad
)
