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
    Shared/gtk/ProcessExecutablePathGtk.cpp

    UIProcess/API/nix/NIXView.cpp

    UIProcess/cairo/BackingStoreCairo.cpp

    UIProcess/nix/WebView.cpp
    UIProcess/nix/WebViewClient.cpp
    UIProcess/nix/TextCheckerNix.cpp
    UIProcess/nix/WebContextNix.cpp
    UIProcess/nix/WebInspectorProxyNix.cpp
    UIProcess/nix/WebPageProxyNix.cpp
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
)

list(APPEND WebKit2_INCLUDE_DIRECTORIES
    "${JAVASCRIPTCORE_DIR}/llint"
    "${WEBCORE_DIR}/platform/nix"
    "${WEBCORE_DIR}/platform/graphics/cairo"
    "${WEBCORE_DIR}/platform/graphics/filters/texmap"
    "${WEBCORE_DIR}/svg/graphics"
    "${WEBKIT2_DIR}/Shared/nix"
    "${WEBKIT2_DIR}/UIProcess/nix"
    "${WEBKIT2_DIR}/WebProcess/nix"
    "${WEBKIT2_DIR}/WebProcess/WebCoreSupport/nix"
    "${WTF_DIR}/wtf/gobject"
    ${CAIRO_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${LIBXSLT_INCLUDE_DIRS}
    ${SQLITE_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${WTF_DIR}
    ${HARFBUZZ_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/nix/"
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
       "${WEBKIT2_DIR}/UIProcess/InspectorServer"
    )

    list(APPEND WebKit2_SOURCES
        UIProcess/InspectorServer/nix/WebSocketServerNix.cpp
        UIProcess/InspectorServer/nix/WebInspectorServerNix.cpp
    )
endif ()

list(APPEND WebKit2_LIBRARIES
    ${WebCoreTestSupport_LIBRARY_NAME}
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
set(ForwardingHeaders_NAME forwarding-headerNix)

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
        "${WEBKIT2_DIR}/Shared/soup"
        "${WEBKIT2_DIR}/UIProcess/API/C/soup"
        "${WEBKIT2_DIR}/UIProcess/soup"
        "${WEBKIT2_DIR}/Shared/Downloads/soup"
        "${WEBKIT2_DIR}/WebProcess/soup"
        "${WEBKIT2_DIR}/WebProcess/WebCoreSupport/soup"
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

configure_file(nix/WebKitNix.pc.in ${CMAKE_BINARY_DIR}/WebKit2/nix/WebKitNix.pc @ONLY)
set(WebKitNix_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/nix/NIXEvents.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/nix/NIXView.h"
)

set(WebKitNix_WebKit2_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKArray.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKBase.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKCertificateInfo.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKConnectionRef.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKContextMenuItem.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKContextMenuItemTypes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKData.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKDictionary.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKError.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKEvent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKFindOptions.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKGeometry.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKGraphicsContext.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKImage.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKMutableArray.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKMutableDictionary.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKNumber.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKPageLoadTypes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKPageVisibilityTypes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKPopupItem.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKPopupMenuListener.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKRenderLayer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKRenderObject.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKSecurityOrigin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKSerializedScriptValue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKSerializedScriptValuePrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKString.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKStringPrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKType.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKURL.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKURLRequest.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKURLResponse.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared/API/c/WKUserContentURLPattern.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKApplicationCacheManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKAuthenticationChallenge.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKAuthenticationDecisionListener.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKBackForwardList.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKBackForwardListItem.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKBatteryManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKBatteryStatus.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKContext.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKContextPrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKCookieManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKCredential.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKCredentialTypes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKDatabaseManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKDownload.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKFormSubmissionListener.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKFrame.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKFramePolicyListener.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKGeolocationManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKGeolocationPermissionRequest.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKGeolocationPosition.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKGrammarDetail.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKHitTestResult.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKIconDatabase.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKInspector.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKKeyValueStorageManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKMediaCacheManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKNativeEvent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKNavigationData.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKNetworkInfo.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKNetworkInfoManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKNotification.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKNotificationManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKNotificationPermissionRequest.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKNotificationProvider.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKOpenPanelParameters.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKOpenPanelResultListener.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKPage.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKPageGroup.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKPagePrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKPluginSiteDataManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKPreferences.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKPreferencesPrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKProtectionSpace.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKProtectionSpaceTypes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKResourceCacheManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKTextChecker.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WKVibration.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/C/WebKit2_C.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/cpp/WKRetainPtr.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/UIProcess/API/CoordinatedGraphics/WKCoordinatedScene.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundle.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleBackForwardList.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleBackForwardListItem.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleDOMWindowExtension.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleFrame.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleFramePrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleHitTestResult.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleInitialize.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleInspector.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleNavigationAction.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleNodeHandle.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleNodeHandlePrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundlePage.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundlePageGroup.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundlePageOverlay.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundlePagePrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundlePrivate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleRangeHandle.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/WebProcess/InjectedBundle/API/c/WKBundleScriptWorld.h"
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
install(FILES ${WebKitNix_HEADERS} DESTINATION include/${WebKit2_LIBRARY_NAME}-${PROJECT_VERSION_MAJOR})
install(FILES ${WebKitNix_WebKit2_HEADERS} DESTINATION include/${WebKit2_LIBRARY_NAME}-${PROJECT_VERSION_MAJOR}/WebKit2)
install(FILES ${WebKitNix_JavaScriptCore_HEADERS} DESTINATION include/${WebKit2_LIBRARY_NAME}-${PROJECT_VERSION_MAJOR}/JavaScriptCore)

add_definitions(-DLIBEXECDIR=\"${CMAKE_INSTALL_PREFIX}/${EXEC_INSTALL_DIR}\"
    -DWEBPROCESSNAME=\"${WebProcess_EXECUTABLE_NAME}\"
    -DPLUGINPROCESSNAME=\"${PluginProcess_EXECUTABLE_NAME}\"
)

if (ENABLE_INSPECTOR)
    set(WK2_WEB_INSPECTOR_DIR ${CMAKE_BINARY_DIR}/WebKit2/nix/webinspector)
    set(WK2_WEB_INSPECTOR_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${WebKit2_LIBRARY_NAME}-${PROJECT_VERSION_MAJOR})
    add_definitions(-DWK2_WEB_INSPECTOR_DIR="${WK2_WEB_INSPECTOR_DIR}")
    add_definitions(-DWK2_WEB_INSPECTOR_INSTALL_DIR="${WK2_WEB_INSPECTOR_INSTALL_DIR}/webinspector")
    add_custom_target(
        wk2-web-inspector-resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${WEBCORE_DIR}/inspector/front-end ${WK2_WEB_INSPECTOR_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${WEBCORE_DIR}/English.lproj/localizedStrings.js ${WK2_WEB_INSPECTOR_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${DERIVED_SOURCES_WEBCORE_DIR}/InspectorBackendCommands.js ${WK2_WEB_INSPECTOR_DIR}/InspectorBackendCommands.js
        COMMAND ${CMAKE_COMMAND} -E copy ${WEBKIT2_DIR}/UIProcess/InspectorServer/front-end/inspectorPageIndex.html ${WK2_WEB_INSPECTOR_DIR}
        DEPENDS ${WebCore_LIBRARY_NAME}
    )
    install(DIRECTORY ${WK2_WEB_INSPECTOR_DIR}
        DESTINATION ${WK2_WEB_INSPECTOR_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.js"
                       PATTERN "*.html"
                       PATTERN "*.css"
                       PATTERN "*.gif"
                       PATTERN "*.png")
endif ()
