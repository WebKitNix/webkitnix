/*
 * Copyright (C) 2011 ProFUSION Embedded Systems
 * Copyright (C) 2011 Samsung Electronics
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "FontManagement.h"

#include <cstdio>
#include <fontconfig/fontconfig.h>
#include <glib.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>
#include <wtf/gobject/GOwnPtr.h>

static Vector<String> getFontFiles()
{
    Vector<String> fontFilePaths;

    // Ahem is used by many layout tests.
    fontFilePaths.append(String(FONTS_CONF_DIR "/AHEM____.TTF"));
    // A font with no valid Fontconfig encoding to test https://bugs.webkit.org/show_bug.cgi?id=47452
    fontFilePaths.append(String(FONTS_CONF_DIR "/FontWithNoValidEncoding.fon"));

    for (int i = 1; i <= 9; i++) {
        char fontPath[PATH_MAX];
        snprintf(fontPath, PATH_MAX - 1,
                 FONTS_CONF_DIR "/../../fonts/WebKitWeightWatcher%i00.ttf", i);

        fontFilePaths.append(String(fontPath));
    }

    return fontFilePaths;
}

CString getCustomBuildDir()
{
    const char* webkitOutputDir = g_getenv("WEBKIT_OUTPUTDIR");
    if (webkitOutputDir)
        return webkitOutputDir;

    return CString();
}

static CString getFontsPath()
{
    CString webkitOutputDir = getCustomBuildDir();
    GOwnPtr<char> fontsPath(g_build_filename(webkitOutputDir.data(), "Dependencies", "Root", "webkitgtk-test-fonts", NULL));
    if (g_file_test(fontsPath.get(), static_cast<GFileTest>(G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)))
        return fontsPath.get();

    fontsPath.set(g_strdup(DOWNLOADED_FONTS_DIR));
    if (g_file_test(fontsPath.get(), static_cast<GFileTest>(G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)))
        return fontsPath.get();

    fprintf(stderr, "Could not locate tests fonts, try setting WEBKIT_OUTPUTDIR.\n");
    return CString();
}

static bool addFontDirectory(const CString& fontDirectory, FcConfig* config)
{
    if (fontDirectory.isNull())
        return false;

    const char* path = fontDirectory.data();
    if (!FcConfigAppFontAddDir(config, reinterpret_cast<const FcChar8*>(path))) {
        fprintf(stderr, "Could not add font directory %s!\n", path);
        return false;
    }
    return true;
}

static void addFontFiles(const Vector<String>& fontFiles, FcConfig* config)
{
    for (Vector<String>::const_iterator it = fontFiles.begin(); it != fontFiles.end(); ++it) {
        const CString currentFile = (*it).utf8();
        const char* path = currentFile.data();

        if (!FcConfigAppFontAddFile(config, reinterpret_cast<const FcChar8*>(path)))
            fprintf(stderr, "Could not load font at %s!\n", path);
    }
}

void addFontsToEnvironment()
{
    FcInit();

    // Load our configuration file, which sets up proper aliases for family
    // names like sans, serif and monospace.
    FcConfig* config = FcConfigCreate();
    const char* fontConfigFilename = FONTS_CONF_DIR "/fonts.conf";
    if (!FcConfigParseAndLoad(config, reinterpret_cast<const FcChar8*>(fontConfigFilename), true)) {
        fprintf(stderr, "Couldn't load font configuration file from: %s\n", fontConfigFilename);
        exit(1);
    }

    addFontFiles(getFontFiles(), config);
    addFontDirectory(getFontsPath(), config);

    if (!FcConfigSetCurrent(config)) {
        fprintf(stderr, "Could not set the current font configuration!\n");
        exit(1);
    }
}

