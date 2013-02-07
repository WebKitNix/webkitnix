list(APPEND WebCore_INCLUDE_DIRECTORIES
    "${WEBCORE_DIR}/platform/nix"
    "${WEBCORE_DIR}/platform/cairo"
    "${WEBCORE_DIR}/platform/graphics/cairo"
    "${WEBCORE_DIR}/platform/graphics/freetype"
    "${WEBCORE_DIR}/platform/graphics/harfbuzz/"
    "${WEBCORE_DIR}/platform/graphics/harfbuzz/ng"
    "${WEBCORE_DIR}/platform/graphics/opengl"
    "${WEBCORE_DIR}/platform/graphics/surfaces"
    "${WEBCORE_DIR}/platform/graphics/texmap"
    "${WEBCORE_DIR}/platform/linux"
    "${WEBCORE_DIR}/platform/mediastream/gstreamer"
    "${WEBCORE_DIR}/platform/network/soup"
    "${PLATFORM_DIR}/nix/"
    "${PLATFORM_DIR}/chromium/"
)

list(APPEND WebCore_SOURCES
    accessibility/nix/AccessibilityObjectNix.cpp
    page/nix/DragControllerNix.cpp
    page/nix/EventHandlerNix.cpp
    platform/Cursor.cpp
    platform/nix/ClipboardNix.cpp
    platform/nix/ContextMenuNix.cpp
    platform/nix/ContextMenuItemNix.cpp
    platform/nix/CursorNix.cpp
    platform/nix/DragDataNix.cpp
    platform/nix/DragImageNix.cpp
    platform/nix/NixKeyboardUtilities.cpp
    platform/nix/ErrorsNix.cpp
    platform/gtk/EventLoopGtk.cpp
    platform/gtk/FileSystemGtk.cpp
    platform/nix/KURLNix.cpp
    platform/nix/LanguageNix.cpp
    platform/nix/LocalizedStringsNix.cpp
    platform/gtk/LoggingGtk.cpp
    platform/nix/MIMETypeRegistryNix.cpp
    platform/nix/PasteboardNix.cpp
    platform/nix/PlatformScreenNix.cpp
    platform/nix/PlatformKeyboardEventNix.cpp
    platform/nix/RenderThemeNix.cpp
    platform/gtk/RunLoopGtk.cpp
    platform/nix/ScrollbarThemeNix.cpp
    platform/gtk/SharedBufferGtk.cpp
    platform/nix/SoundNix.cpp
    platform/gtk/SharedTimerGtk.cpp
    platform/nix/SystemTimeNix.cpp
    platform/nix/TemporaryLinkStubs.cpp
    platform/nix/WidgetNix.cpp

    platform/cairo/WidgetBackingStoreCairo.cpp
    platform/graphics/cairo/BitmapImageCairo.cpp
    platform/graphics/cairo/CairoUtilities.cpp
    platform/graphics/cairo/FontCairo.cpp
    platform/graphics/cairo/GradientCairo.cpp
    platform/graphics/cairo/GraphicsContextCairo.cpp
    platform/graphics/cairo/ImageBufferCairo.cpp
    platform/graphics/cairo/ImageCairo.cpp
    platform/graphics/cairo/IntRectCairo.cpp
    platform/graphics/cairo/NativeImageCairo.cpp
    platform/graphics/cairo/OwnPtrCairo.cpp
    platform/graphics/cairo/PathCairo.cpp
    platform/graphics/cairo/PatternCairo.cpp
    platform/graphics/cairo/PlatformContextCairo.cpp
    platform/graphics/cairo/PlatformPathCairo.cpp
    platform/graphics/cairo/RefPtrCairo.cpp
    platform/graphics/cairo/TileCairo.cpp
    platform/graphics/cairo/TiledBackingStoreBackendCairo.cpp
    platform/graphics/cairo/TransformationMatrixCairo.cpp

    platform/image-decoders/cairo/ImageDecoderCairo.cpp

    platform/graphics/WOFFFileFormat.cpp
    platform/graphics/cairo/FontCairoHarfbuzzNG.cpp
    platform/graphics/freetype/FontCacheFreeType.cpp
    platform/graphics/freetype/FontCustomPlatformDataFreeType.cpp
    platform/graphics/freetype/FontPlatformDataFreeType.cpp
    platform/graphics/freetype/GlyphPageTreeNodeFreeType.cpp
    platform/graphics/freetype/SimpleFontDataFreeType.cpp
    platform/graphics/harfbuzz/HarfBuzzShaperBase.cpp
    platform/graphics/harfbuzz/HarfBuzzFace.cpp
    platform/graphics/harfbuzz/HarfBuzzFaceCairo.cpp
    platform/graphics/harfbuzz/HarfBuzzShaper.cpp

    platform/graphics/ImageSource.cpp
    platform/graphics/cairo/DrawingBufferCairo.cpp
    platform/graphics/efl/GraphicsContext3DEfl.cpp
    platform/graphics/efl/GraphicsContext3DPrivate.cpp
    platform/graphics/nix/IconNix.cpp
    platform/graphics/nix/ImageNix.cpp
    platform/graphics/OpenGLShims.cpp
    platform/graphics/opengl/Extensions3DOpenGLCommon.cpp
    platform/graphics/opengl/GLPlatformContext.cpp
    platform/graphics/opengl/GLPlatformSurface.cpp
    platform/graphics/opengl/GraphicsContext3DOpenGLCommon.cpp
    platform/graphics/surfaces/GraphicsSurface.cpp
    platform/graphics/texmap/TextureMapperGL.cpp
    platform/graphics/texmap/TextureMapperShaderProgram.cpp
    platform/graphics/texmap/GraphicsLayerTextureMapper.cpp

    platform/image-decoders/ImageDecoder.cpp
    platform/image-decoders/bmp/BMPImageDecoder.cpp
    platform/image-decoders/bmp/BMPImageReader.cpp
    platform/image-decoders/gif/GIFImageDecoder.cpp
    platform/image-decoders/gif/GIFImageReader.cpp
    platform/image-decoders/ico/ICOImageDecoder.cpp
    platform/image-decoders/jpeg/JPEGImageDecoder.cpp
    platform/image-decoders/png/PNGImageDecoder.cpp
    platform/image-decoders/webp/WEBPImageDecoder.cpp
    platform/linux/GamepadDeviceLinux.cpp
    platform/mediastream/gstreamer/MediaStreamCenterGStreamer.cpp
    platform/network/soup/AuthenticationChallengeSoup.cpp
    platform/network/soup/CookieJarSoup.cpp
    platform/network/soup/CookieStorageSoup.cpp
    platform/network/soup/CredentialStorageSoup.cpp
    platform/network/soup/DNSSoup.cpp
    platform/network/soup/GOwnPtrSoup.cpp
    platform/network/soup/NetworkStorageSessionSoup.cpp
    platform/network/soup/ProxyResolverSoup.cpp
    platform/network/soup/ProxyServerSoup.cpp
    platform/network/soup/ResourceErrorSoup.cpp
    platform/network/soup/ResourceHandleSoup.cpp
    platform/network/soup/ResourceRequestSoup.cpp
    platform/network/soup/ResourceResponseSoup.cpp
    platform/network/soup/SocketStreamHandleSoup.cpp
    platform/network/soup/SoupURIUtils.cpp
    platform/PlatformStrategies.cpp
    platform/text/nix/TextBreakIteratorInternalICUNix.cpp

    plugins/PluginDatabase.cpp
    plugins/PluginDebug.cpp
    plugins/PluginPackage.cpp
    plugins/PluginStream.cpp
    plugins/PluginView.cpp
    plugins/PluginPackageNone.cpp
    plugins/PluginViewNone.cpp

    editing/SmartReplaceICU.cpp
    platform/text/TextEncodingDetectorICU.cpp
    platform/text/TextBreakIteratorICU.cpp
    platform/text/TextCodecICU.cpp

    platform/chromium/support/WebAudioBus.cpp
    platform/chromium/support/WebData.cpp
    platform/nix/WebCommon.cpp
)

if (WTF_USE_OPENGL_ES_2)
    list(APPEND WebCore_SOURCES
        platform/graphics/opengl/Extensions3DOpenGLES.cpp
        platform/graphics/opengl/GraphicsContext3DOpenGLES.cpp
    )
    list(APPEND WebCore_LIBRARIES ${OPENGLES2_LIBRARIES})
    add_definitions(-DWTF_USE_OPENGL_ES_2=1)
else ()
    list(APPEND WebCore_SOURCES
        platform/graphics/opengl/Extensions3DOpenGL.cpp
        platform/graphics/opengl/GraphicsContext3DOpenGL.cpp
    )
    list(APPEND WebCore_LIBRARIES
        ${OPENGL_gl_LIBRARY}
    )
endif ()

if (WTF_USE_EGL)
    list(APPEND WebCore_INCLUDE_DIRECTORIES
        platform/graphics/egl
        platform/graphics/surfaces/egl
        ${EGL_INCLUDE_DIR}
    )
    list(APPEND WebCore_SOURCES
        platform/graphics/surfaces/egl/EGLContext.cpp
    )
    list(APPEND WebCore_LIBRARIES ${EGL_LIBRARY})
    add_definitions(-DWTF_USE_EGL=1)
else ()
    list(APPEND WebCore_INCLUDE_DIRECTORIES
        platform/graphics/surfaces/glx
        ${X11_X11_INCLUDE_PATH}
    )
    list(APPEND WebCore_SOURCES
        platform/graphics/surfaces/glx/GraphicsSurfaceGLX.cpp
        platform/graphics/surfaces/glx/X11WindowResources.cpp
        platform/graphics/surfaces/glx/GLXContext.cpp
        platform/graphics/surfaces/glx/GLXSurface.cpp
    )
    list(APPEND WebCore_LIBRARIES ${X11_X11_LIB} ${X11_Xcomposite_LIB} ${X11_Xrender_LIB})
endif ()

if (ENABLE_BATTERY_STATUS)
    list(APPEND WebCore_INCLUDE_DIRECTORIES ${DBUS_INCLUDE_DIRS})
    list(APPEND WebCore_LIBRARIES ${DBUS_LIBRARIES})
endif ()

list(APPEND WebCore_USER_AGENT_STYLE_SHEETS
    ${WEBCORE_DIR}/css/mediaControlsNix.css
    ${WEBCORE_DIR}/css/mediaControlsNixFullscreen.css
)

list(APPEND WebCore_LIBRARIES
    ${CAIRO_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${ICU_LIBRARIES}
    ${JPEG_LIBRARY}
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${PNG_LIBRARY}
    ${SQLITE_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GLIB_GIO_LIBRARIES}
    ${GLIB_GMODULE_LIBRARIES}
    ${GLIB_GOBJECT_LIBRARIES}
    ${LIBSOUP_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${Platform_LIBRARY_NAME}
    ${HARFBUZZ_LIBRARIES}
)

list(APPEND WebCore_INCLUDE_DIRECTORIES
    ${FONTCONFIG_INCLUDE_DIR}
    ${CAIRO_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${ICU_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${LIBXSLT_INCLUDE_DIR}
    ${SQLITE_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${LIBSOUP_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
)

if (ENABLE_VIDEO)
  LIST(APPEND WebCore_INCLUDE_DIRECTORIES
    "${WEBCORE_DIR}/platform/graphics/gstreamer"

        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_BASE_INCLUDE_DIRS}
        ${GSTREAMER_APP_INCLUDE_DIRS}
        ${GSTREAMER_PBUTILS_INCLUDE_DIRS}
    )
    list(APPEND WebCore_SOURCES
        platform/graphics/gstreamer/GRefPtrGStreamer.cpp
        platform/graphics/gstreamer/GStreamerUtilities.cpp
        platform/graphics/gstreamer/GStreamerVersioning.cpp
    )
    list(APPEND WebCore_LIBRARIES
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_BASE_LIBRARIES}
        ${GSTREAMER_APP_LIBRARIES}
        ${GSTREAMER_PBUTILS_LIBRARIES}
    )

    list(APPEND WebCore_INCLUDE_DIRECTORIES
        ${GSTREAMER_VIDEO_INCLUDE_DIRS}
    )
    list(APPEND WebCore_SOURCES
        platform/graphics/gstreamer/GStreamerGWorld.cpp
        platform/graphics/gstreamer/ImageGStreamerCairo.cpp
        platform/graphics/gstreamer/MediaPlayerPrivateGStreamer.cpp
        platform/graphics/gstreamer/PlatformVideoWindowNix.cpp
        platform/graphics/gstreamer/VideoSinkGStreamer.cpp
        platform/graphics/gstreamer/WebKitWebSourceGStreamer.cpp
    )
    list(APPEND WebCore_LIBRARIES
        ${GSTREAMER_VIDEO_LIBRARIES}
    )
endif ()

add_definitions(-DWTF_USE_CROSS_PLATFORM_CONTEXT_MENUS=1
                -DDATA_DIR="${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}")

if (ENABLE_WEB_AUDIO)
  LIST(APPEND WebCore_INCLUDE_DIRECTORIES
    "${WEBCORE_DIR}/platform/audio/chromium"
    ${LIBAVCODEC_INCLUDE_DIRS}
  )
  list(APPEND WebCore_LIBRARIES
    ${LIBAVCODEC_LIBRARIES}
  )
  LIST(APPEND WebCore_SOURCES
    platform/audio/chromium/AudioBusChromium.cpp
    platform/audio/chromium/AudioDestinationChromium.cpp
    platform/audio/ffmpeg/FFTFrameFFMPEG.cpp
  )
  SET(WEB_AUDIO_DIR ${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}/webaudio/resources)
  FILE(GLOB WEB_AUDIO_DATA "${WEBCORE_DIR}/platform/audio/resources/*.wav")
  INSTALL(FILES ${WEB_AUDIO_DATA} DESTINATION ${WEB_AUDIO_DIR})
  ADD_DEFINITIONS(-DUNINSTALLED_AUDIO_RESOURCES_DIR="${WEBCORE_DIR}/platform/audio/resources")
endif ()

if (ENABLE_GAMEPAD)
    list(APPEND WebCore_SOURCES
        platform/chromium/GamepadsChromium.cpp
    )
endif ()
