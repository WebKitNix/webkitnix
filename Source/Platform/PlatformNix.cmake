set(Platform_INCLUDES
    nix
    ${WEBCORE_DIR}/platform/graphics/ # For IntRect.h
    ${WTF_DIR} # For config.h
    ${CMAKE_BINARY_DIR} # For cmakeconfig.h
)

set(Platform_HEADERS
    nix/public/Platform.h
    nix/public/WebColor.h
    nix/public/WebFFTFrame.h
    nix/public/WebRect.h
    nix/public/WebSize.h
    nix/public/WebThemeEngine.h
    nix/public/WebAudioBus.h
    nix/public/WebAudioDevice.h
    nix/public/WebCommon.h
    nix/public/WebData.h
    nix/public/WebGamepads.h
    nix/public/WebGamepad.h
    nix/public/WebVector.h
)

set(Platform_SOURCES
    nix/src/Platform.cpp
    nix/src/DefaultWebThemeEngine.cpp
)

install(FILES ${Platform_HEADERS} DESTINATION include/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR}/NixPlatform)
