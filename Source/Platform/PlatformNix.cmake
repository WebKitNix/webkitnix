set(Platform_INCLUDES
    nix
    ${WEBCORE_DIR}/platform/graphics/ # For IntRect.h
    ${WTF_DIR} # For config.h
    ${CMAKE_BINARY_DIR} # For cmakeconfig.h
)

set(Platform_HEADERS
    nix/public/AudioBus.h
    nix/public/AudioDestinationConsumer.h
    nix/public/AudioDevice.h
    nix/public/Canvas.h
    nix/public/Color.h
    nix/public/Common.h
    nix/public/CString.h
    nix/public/Data.h
    nix/public/FFTFrame.h
    nix/public/Gamepad.h
    nix/public/Gamepads.h
    nix/public/MediaConstraints.h
    nix/public/MediaStream.h
    nix/public/MediaStreamSource.h
    nix/public/MediaStreamTrack.h
    nix/public/Platform.h
    nix/public/PrivatePtr.h
    nix/public/Rect.h
    nix/public/SecurityOrigin.h
    nix/public/Size.h
    nix/public/String.h
    nix/public/ThemeEngine.h
    nix/public/URL.h
    nix/public/UserMediaClient.h
    nix/public/UserMediaRequest.h
    nix/public/Vector.h
)

set(Platform_SOURCES
    nix/src/Platform.cpp
    nix/src/DefaultWebThemeEngine.cpp
)

install(FILES ${Platform_HEADERS} DESTINATION include/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR}/NixPlatform)
