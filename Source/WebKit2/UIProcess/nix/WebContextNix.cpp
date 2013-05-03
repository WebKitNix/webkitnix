/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Portions Copyright (c) 2010 Motorola Mobility, Inc. All rights reserved.
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebContext.h"

#include <WebCore/FileSystem.h>
#include <WebCore/NotImplemented.h>
#include <wtf/gobject/GOwnPtr.h>

#include <glib.h>

#if ENABLE(INSPECTOR_SERVER)
#include "WebInspectorServer.h"
#endif

namespace WebKit {

static void initInspectorServer()
{
#if ENABLE(INSPECTOR_SERVER)
    static bool initialized = false;
    if (initialized)
        return;

    initialized = true;
    String envStr(g_getenv("WEBKIT_INSPECTOR_SERVER"));

    if (!envStr.isNull()) {
        String bindAddress = "127.0.0.1";
        unsigned short port = 25555;

        Vector<String> result;
        envStr.split(":", result);

        if (result.size() == 2) {
            bindAddress = result[0];
            bool ok = false;
            port = result[1].toInt(&ok);
            if (!ok)
                port = 2999;
        }
        WebInspectorServer::shared().listen(bindAddress, port);
    }
#endif
}

String WebContext::applicationCacheDirectory()
{
    GOwnPtr<gchar> cacheDirectory(g_build_filename(g_get_user_cache_dir(), "webkitnix", "applications", NULL));
    return WebCore::filenameToString(cacheDirectory.get());
}

void WebContext::platformInitializeWebProcess(WebProcessCreationParameters&)
{
    initInspectorServer();
}

void WebContext::platformInvalidateContext()
{
    notImplemented();
}

String WebContext::platformDefaultDatabaseDirectory() const
{
    GOwnPtr<gchar> databaseDirectory(g_build_filename(g_get_user_data_dir(), "webkitnix", "databases", NULL));
    return WebCore::filenameToString(databaseDirectory.get());
}

String WebContext::platformDefaultIconDatabasePath() const
{
    notImplemented();
    return String();
}

String WebContext::platformDefaultLocalStorageDirectory() const
{
    GOwnPtr<gchar> storageDirectory(g_build_filename(g_get_user_data_dir(), "webkitnix", "localstorage", NULL));
    return WebCore::filenameToString(storageDirectory.get());
}

String WebContext::platformDefaultDiskCacheDirectory() const
{
    GOwnPtr<gchar> diskCacheDirectory(g_build_filename(g_get_user_cache_dir(), g_get_prgname(), NULL));
    return WebCore::filenameToString(diskCacheDirectory.get());
}

String WebContext::platformDefaultCookieStorageDirectory() const
{
    notImplemented();
    return String();
}

} // namespace WebKit
