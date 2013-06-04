list(APPEND WebKit2_LINK_FLAGS
    ${CAIRO_LDFLAGS}
)

list(APPEND WebKit2_SOURCES
    Platform/gtk/ModuleGtk.cpp
    Platform/efl/WorkQueueEfl.cpp
    Platform/unix/SharedMemoryUnix.cpp

    Platform/CoreIPC/unix/ConnectionUnix.cpp
    Platform/CoreIPC/unix/AttachmentUnix.cpp

    Shared/API/c/cairo/WKImageCairo.cpp

    Shared/API/c/gtk/WKGraphicsContextGtk.cpp

    Shared/cairo/ShareableBitmapCairo.cpp

    Shared/nix/LayerTreeContextNix.cpp
    Shared/nix/NativeWebMouseEventNix.cpp
    Shared/nix/NativeWebWheelEventNix.cpp
    Shared/nix/NativeWebKeyboardEventNix.cpp
    Shared/nix/NativeWebGestureEventNix.cpp
    Shared/nix/NativeWebTouchEventNix.cpp
    Shared/nix/WebEventFactoryNix.cpp

    Shared/efl/ProcessExecutablePathEfl.cpp

    UIProcess/API/C/CoordinatedGraphics/WKView.cpp
    UIProcess/API/nix/NIXView.cpp

    UIProcess/cairo/BackingStoreCairo.cpp

    UIProcess/CoordinatedGraphics/WebView.cpp
    UIProcess/CoordinatedGraphics/WebViewClient.cpp

    UIProcess/nix/WebViewNix.cpp
    UIProcess/nix/WebViewClientNix.cpp
    UIProcess/nix/TextCheckerNix.cpp
    UIProcess/nix/WebContextNix.cpp
    UIProcess/nix/WebInspectorProxyNix.cpp
    UIProcess/nix/WebPageProxyNix.cpp
    UIProcess/nix/WebPopupMenuListenerNix.cpp
    UIProcess/efl/WebPreferencesEfl.cpp

    UIProcess/Launcher/efl/ProcessLauncherEfl.cpp
    UIProcess/efl/WebProcessProxyEfl.cpp

    UIProcess/Plugins/unix/PluginInfoStoreUnix.cpp

    WebProcess/nix/WebProcessMainNix.cpp

    WebProcess/InjectedBundle/gtk/InjectedBundleGtk.cpp

    WebProcess/InjectedBundle/API/nix/NixTestSupport.cpp

    WebProcess/WebCoreSupport/nix/WebErrorsNix.cpp
    WebProcess/WebCoreSupport/nix/WebPopupMenuNix.cpp
    WebProcess/WebCoreSupport/nix/WebContextMenuClientNix.cpp

    WebProcess/WebPage/nix/WebInspectorNix.cpp
    WebProcess/WebPage/nix/WebPageNix.cpp

    UIProcess/Storage/StorageManager.cpp

    UIProcess/DefaultUndoController.cpp
)

list(APPEND WebKit2_INCLUDE_DIRECTORIES
    Shared/nix
    Shared/API/c/nix
    WebProcess/nix
    WebProcess/WebCoreSupport/nix
    UIProcess/nix
    UIProcess/API/nix
    UIProcess/API/C/nix
    UIProcess/API/C/CoordinatedGraphics
    "${JAVASCRIPTCORE_DIR}/llint"
    "${WEBCORE_DIR}/platform/nix"
    "${WEBCORE_DIR}/platform/graphics/cairo"
    "${WEBCORE_DIR}/platform/graphics/filters/texmap"
    "${WEBCORE_DIR}/svg/graphics"
    "${WTF_DIR}/wtf/gobject"
    ${CAIRO_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${LIBXSLT_INCLUDE_DIRS}
    ${SQLITE_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${WTF_DIR}
    ${HARFBUZZ_INCLUDE_DIRS}
    "${THIRDPARTY_DIR}/ANGLE/include/GLSLANG"
    "${THIRDPARTY_DIR}/ANGLE/include/KHR"
    ${ICU_INCLUDE_DIRS}
)

if (WTF_USE_OPENGL_ES_2)
    list(APPEND WebKit2_INCLUDE_DIRECTORIES ${OPENGLES2_INCLUDE_DIRS})
    list(APPEND WebProcess_INCLUDE_DIRECTORIES ${OPENGLES2_INCLUDE_DIRS})
    list(APPEND WebKit2_LIBRARIES ${OPENGLES2_LIBRARIES})
    list(APPEND WebProcess_LIBRARIES ${OPENGLES2_LIBRARIES})
    add_definitions(${OPENGLES2_DEFINITIONS})
else ()
    list(APPEND WebKit2_LIBRARIES ${OPENGL_LIBRARIES})
    list(APPEND WebProcess_LIBRARIES ${OPENGL_LIBRARIES})
    include_directories(${OPENGL_INCLUDE_DIR})
endif ()

if (ENABLE_INSPECTOR_SERVER)
    list(APPEND WebKit2_INCLUDE_DIRECTORIES
       UIProcess/InspectorServer
    )

    list(APPEND WebKit2_SOURCES
        UIProcess/InspectorServer/nix/WebSocketServerNix.cpp
        UIProcess/InspectorServer/nix/WebInspectorServerNix.cpp
    )
endif ()

list(APPEND WebKit2_LIBRARIES
    WebCoreTestSupport
    ${CAIRO_LIBRARIES}
    ${Freetype_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${SQLITE_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${PNG_LIBRARY}
    ${JPEG_LIBRARY}
    ${CMAKE_DL_LIBS}
    ${GLIB_LIBRARIES}
    ${GLIB_GIO_LIBRARIES}
    ${GLIB_GOBJECT_LIBRARIES}
)

list(APPEND WebProcess_SOURCES
    nix/MainNix.cpp
)

list(APPEND WebProcess_LIBRARIES
    ${CAIRO_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${SQLITE_LIBRARIES}
)

add_definitions(-DDEFAULT_THEME_PATH=\"${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}/themes\")

add_custom_target(forwarding-headerNix
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include nix
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include CoordinatedGraphics
)

if (ENABLE_SOUP)
    list(APPEND WebKit2_SOURCES
        Shared/soup/PlatformCertificateInfo.cpp
        Shared/soup/WebCoreArgumentCodersSoup.cpp

        UIProcess/API/C/soup/WKContextSoup.cpp
        UIProcess/API/C/soup/WKSoupRequestManager.cpp

        UIProcess/soup/WebCookieManagerProxySoup.cpp
        UIProcess/soup/WebSoupRequestManagerClient.cpp
        UIProcess/soup/WebSoupRequestManagerProxy.cpp

        WebProcess/Cookies/soup/WebCookieManagerSoup.cpp
        WebProcess/Cookies/soup/WebKitSoupCookieJarSqlite.cpp

        Shared/Downloads/nix/DownloadSoupErrorsNix.cpp
        Shared/Downloads/soup/DownloadSoup.cpp

        WebProcess/soup/WebProcessSoup.cpp
        WebProcess/soup/WebSoupRequestManager.cpp
        WebProcess/soup/WebKitSoupRequestGeneric.cpp
        WebProcess/soup/WebKitSoupRequestInputStream.cpp
        WebProcess/soup/WebSoupRequestManager.cpp
        WebProcess/soup/WebKitSoupRequestGeneric.cpp
        WebProcess/soup/WebKitSoupRequestInputStream.cpp
        WebProcess/WebCoreSupport/soup/WebFrameNetworkingContext.cpp
    )

    list(APPEND WebKit2_MESSAGES_IN_FILES
        UIProcess/soup/WebSoupRequestManagerProxy.messages.in
        WebProcess/soup/WebSoupRequestManager.messages.in
    )

    list(APPEND WebKit2_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/platform/network/soup"
        Shared/soup
        UIProcess/API/C/soup
        UIProcess/soup
        Shared/Downloads/soup
        WebProcess/soup
        WebProcess/WebCoreSupport/soup
        ${LIBSOUP_INCLUDE_DIRS}
    )
    list(APPEND WebKit2_LIBRARIES
        ${LIBSOUP_LIBRARIES}
        rt
    )

    add_custom_target(forwarding-headerSoup
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
    )
    set(ForwardingNetworkHeaders_NAME forwarding-headerSoup)
else ()
    list(APPEND WebKit2_SOURCES
        Shared/curl/WebCoreArgumentCodersCurl.cpp
        WebProcess/curl/WebProcessCurl.cpp
        WebProcess/Cookies/curl/WebCookieManagerCurl.cpp

        Shared/Downloads/curl/DownloadCurl.cpp

        WebProcess/WebCoreSupport/curl/WebFrameNetworkingContext.cpp
    )

    list(APPEND WebKit2_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/platform/network/curl"
        "${WEBKIT2_DIR}/Shared/curl"
        "${WEBKIT2_DIR}/WebProcess/WebCoreSupport/curl"
    )

    list(APPEND WebKit2_LIBRARIES
         ${GLIB_GMODULE_LIBRARIES}
    )

    add_custom_target(forwarding-headercurl
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include curl
    )
    set(ForwardingNetworkHeaders_NAME forwarding-headercurl)
endif ()

set(WEBKIT2_EXTRA_DEPENDENCIES
    forwarding-headerNix
    ${ForwardingNetworkHeaders_NAME}
)

configure_file(nix/WebKitNix.pc.in ${CMAKE_BINARY_DIR}/WebKit2/nix/WebKitNix.pc @ONLY)
set(WebKitNix_HEADERS
    UIProcess/API/nix/NIXEvents.h
    UIProcess/API/nix/NIXView.h
)

macro(PROCESS_HEADER_IFDEFS _file _enable _output)
    get_filename_component(_absolute "${_file}" ABSOLUTE)
    get_filename_component(_filename "${_file}" NAME)
    set(${_output} "${CMAKE_BINARY_DIR}/${_filename}")
    file(READ "${_absolute}" _contents)
    foreach (_define ${_enable})
        string(REGEX REPLACE "${_define}" "TRUE" _contents "${_contents}")
    endforeach ()
    file(WRITE "${${_output}}" "${_contents}")
endmacro()

PROCESS_HEADER_IFDEFS(Shared/API/c/WKBase.h "BUILDING_NIX__" _Shared_API_c_WKBase_h)

set(WebKitNix_WebKit2_HEADERS
    "${_Shared_API_c_WKBase_h}"
    Shared/API/c/WKArray.h
    Shared/API/c/WKCertificateInfo.h
    Shared/API/c/WKConnectionRef.h
    Shared/API/c/WKContextMenuItem.h
    Shared/API/c/WKContextMenuItemTypes.h
    Shared/API/c/WKData.h
    Shared/API/c/WKDictionary.h
    Shared/API/c/WKError.h
    Shared/API/c/WKEvent.h
    Shared/API/c/WKFindOptions.h
    Shared/API/c/WKGeometry.h
    Shared/API/c/WKGraphicsContext.h
    Shared/API/c/WKImage.h
    Shared/API/c/WKMutableArray.h
    Shared/API/c/WKMutableDictionary.h
    Shared/API/c/WKNumber.h
    Shared/API/c/WKPageLoadTypes.h
    Shared/API/c/WKPageVisibilityTypes.h
    Shared/API/c/WKPopupItem.h
    Shared/API/c/WKPopupMenuListener.h
    Shared/API/c/WKRenderLayer.h
    Shared/API/c/WKRenderObject.h
    Shared/API/c/WKSecurityOrigin.h
    Shared/API/c/WKSerializedScriptValue.h
    Shared/API/c/WKSerializedScriptValuePrivate.h
    Shared/API/c/WKString.h
    Shared/API/c/WKStringPrivate.h
    Shared/API/c/WKType.h
    Shared/API/c/WKURL.h
    Shared/API/c/WKURLRequest.h
    Shared/API/c/WKURLResponse.h
    Shared/API/c/WKUserContentURLPattern.h

    Shared/API/c/nix/WKBaseNix.h

    UIProcess/API/C/WKApplicationCacheManager.h
    UIProcess/API/C/WKAuthenticationChallenge.h
    UIProcess/API/C/WKAuthenticationDecisionListener.h
    UIProcess/API/C/WKBackForwardList.h
    UIProcess/API/C/WKBackForwardListItem.h
    UIProcess/API/C/WKBatteryManager.h
    UIProcess/API/C/WKBatteryStatus.h
    UIProcess/API/C/WKContext.h
    UIProcess/API/C/WKContextPrivate.h
    UIProcess/API/C/WKCookieManager.h
    UIProcess/API/C/WKCredential.h
    UIProcess/API/C/WKCredentialTypes.h
    UIProcess/API/C/WKDatabaseManager.h
    UIProcess/API/C/WKDownload.h
    UIProcess/API/C/WKFormSubmissionListener.h
    UIProcess/API/C/WKFrame.h
    UIProcess/API/C/WKFramePolicyListener.h
    UIProcess/API/C/WKGeolocationManager.h
    UIProcess/API/C/WKGeolocationPermissionRequest.h
    UIProcess/API/C/WKGeolocationPosition.h
    UIProcess/API/C/WKGrammarDetail.h
    UIProcess/API/C/WKHitTestResult.h
    UIProcess/API/C/WKIconDatabase.h
    UIProcess/API/C/WKInspector.h
    UIProcess/API/C/WKKeyValueStorageManager.h
    UIProcess/API/C/WKMediaCacheManager.h
    UIProcess/API/C/WKNativeEvent.h
    UIProcess/API/C/WKNavigationData.h
    UIProcess/API/C/WKNetworkInfo.h
    UIProcess/API/C/WKNetworkInfoManager.h
    UIProcess/API/C/WKNotification.h
    UIProcess/API/C/WKNotificationManager.h
    UIProcess/API/C/WKNotificationPermissionRequest.h
    UIProcess/API/C/WKNotificationProvider.h
    UIProcess/API/C/WKOpenPanelParameters.h
    UIProcess/API/C/WKOpenPanelResultListener.h
    UIProcess/API/C/WKPage.h
    UIProcess/API/C/WKPageGroup.h
    UIProcess/API/C/WKPagePrivate.h
    UIProcess/API/C/WKPluginSiteDataManager.h
    UIProcess/API/C/WKPreferences.h
    UIProcess/API/C/WKPreferencesPrivate.h
    UIProcess/API/C/WKProtectionSpace.h
    UIProcess/API/C/WKProtectionSpaceTypes.h
    UIProcess/API/C/WKResourceCacheManager.h
    UIProcess/API/C/WKTextChecker.h
    UIProcess/API/C/WKVibration.h
    UIProcess/API/C/WebKit2_C.h

    UIProcess/API/cpp/WKRetainPtr.h

    UIProcess/API/CoordinatedGraphics/WKCoordinatedScene.h
    UIProcess/API/C/CoordinatedGraphics/WKView.h
    UIProcess/API/C/WKViewportAttributes.h

    WebProcess/InjectedBundle/API/c/WKBundle.h
    WebProcess/InjectedBundle/API/c/WKBundleBackForwardList.h
    WebProcess/InjectedBundle/API/c/WKBundleBackForwardListItem.h
    WebProcess/InjectedBundle/API/c/WKBundleDOMWindowExtension.h
    WebProcess/InjectedBundle/API/c/WKBundleFrame.h
    WebProcess/InjectedBundle/API/c/WKBundleFramePrivate.h
    WebProcess/InjectedBundle/API/c/WKBundleHitTestResult.h
    WebProcess/InjectedBundle/API/c/WKBundleInitialize.h
    WebProcess/InjectedBundle/API/c/WKBundleInspector.h
    WebProcess/InjectedBundle/API/c/WKBundleNavigationAction.h
    WebProcess/InjectedBundle/API/c/WKBundleNodeHandle.h
    WebProcess/InjectedBundle/API/c/WKBundleNodeHandlePrivate.h
    WebProcess/InjectedBundle/API/c/WKBundlePage.h
    WebProcess/InjectedBundle/API/c/WKBundlePageGroup.h
    WebProcess/InjectedBundle/API/c/WKBundlePageOverlay.h
    WebProcess/InjectedBundle/API/c/WKBundlePagePrivate.h
    WebProcess/InjectedBundle/API/c/WKBundlePrivate.h
    WebProcess/InjectedBundle/API/c/WKBundleRangeHandle.h
    WebProcess/InjectedBundle/API/c/WKBundleScriptWorld.h
)

set(WebKitNix_JavaScriptCore_HEADERS
    "${JAVASCRIPTCORE_DIR}/API/APICast.h"
    "${JAVASCRIPTCORE_DIR}/API/APIShims.h"
    "${JAVASCRIPTCORE_DIR}/API/JSBase.h"
    "${JAVASCRIPTCORE_DIR}/API/JSBasePrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JSCallbackConstructor.h"
    "${JAVASCRIPTCORE_DIR}/API/JSCallbackFunction.h"
    "${JAVASCRIPTCORE_DIR}/API/JSCallbackObject.h"
    "${JAVASCRIPTCORE_DIR}/API/JSCallbackObjectFunctions.h"
    "${JAVASCRIPTCORE_DIR}/API/JSClassRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSContextRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSContextRefPrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JSObjectRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSObjectRefPrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JSProfilerPrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JSRetainPtr.h"
    "${JAVASCRIPTCORE_DIR}/API/JSStringRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSStringRefBSTR.h"
    "${JAVASCRIPTCORE_DIR}/API/JSStringRefCF.h"
    "${JAVASCRIPTCORE_DIR}/API/JSValueRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSWeakObjectMapRefInternal.h"
    "${JAVASCRIPTCORE_DIR}/API/JSWeakObjectMapRefPrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JavaScript.h"
    "${JAVASCRIPTCORE_DIR}/API/JavaScriptCore.h"
    "${JAVASCRIPTCORE_DIR}/API/OpaqueJSString.h"
    "${JAVASCRIPTCORE_DIR}/API/WebKitAvailability.h"
)

install(FILES ${CMAKE_BINARY_DIR}/WebKit2/nix/WebKitNix.pc DESTINATION lib/pkgconfig)
install(FILES ${WebKitNix_HEADERS} DESTINATION include/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR})
install(FILES ${WebKitNix_WebKit2_HEADERS} DESTINATION include/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR}/WebKit2)
install(FILES ${WebKitNix_JavaScriptCore_HEADERS} DESTINATION include/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR}/JavaScriptCore)

add_definitions(-DLIBEXECDIR=\"${CMAKE_INSTALL_PREFIX}/${EXEC_INSTALL_DIR}\"
    -DWEBPROCESSNAME=\"WebProcess\"
    -DPLUGINPROCESSNAME=\"PluginProcess\"
)

if (ENABLE_INSPECTOR)
    set(WK2_WEB_INSPECTOR_DIR ${CMAKE_BINARY_DIR}/WebKit2/nix/webinspector)
    set(WK2_WEB_INSPECTOR_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR})
    add_definitions(-DWK2_WEB_INSPECTOR_DIR="${WK2_WEB_INSPECTOR_DIR}")
    add_definitions(-DWK2_WEB_INSPECTOR_INSTALL_DIR="${WK2_WEB_INSPECTOR_INSTALL_DIR}/webinspector")
    add_custom_target(
        wk2-web-inspector-resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${WEBCORE_DIR}/inspector/front-end ${WK2_WEB_INSPECTOR_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${WEBCORE_DIR}/English.lproj/localizedStrings.js ${WK2_WEB_INSPECTOR_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${DERIVED_SOURCES_WEBCORE_DIR}/InspectorBackendCommands.js ${WK2_WEB_INSPECTOR_DIR}/InspectorBackendCommands.js
        COMMAND ${CMAKE_COMMAND} -E copy ${WEBKIT2_DIR}/UIProcess/InspectorServer/front-end/inspectorPageIndex.html ${WK2_WEB_INSPECTOR_DIR}
        DEPENDS WebCore
    )
    install(DIRECTORY ${WK2_WEB_INSPECTOR_DIR}
        DESTINATION ${WK2_WEB_INSPECTOR_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.js"
                       PATTERN "*.html"
                       PATTERN "*.css"
                       PATTERN "*.gif"
                       PATTERN "*.png")
endif ()
