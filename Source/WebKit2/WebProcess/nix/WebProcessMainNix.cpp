/*
 * Copyright (C) 2011 Samsung Electronics. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
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
#include "WebProcessMainNix.h"

#define LIBSOUP_USE_UNSTABLE_REQUEST_API

#include "ProxyResolverSoup.h"
#include "WKBase.h"
#include <WebCore/CoordinatedGraphicsLayer.h>
#include <WebCore/ResourceHandle.h>
#include <WebCore/RunLoop.h>
#include <WebKit2/WebProcess.h>
#include <libsoup/soup-cache.h>
#include <libsoup/soup.h>
#include <runtime/InitializeThreading.h>
#include <unistd.h>
#include <wtf/MainThread.h>
#include <wtf/gobject/GOwnPtr.h>
#include <wtf/gobject/GRefPtr.h>


using namespace WebCore;

namespace WebKit {

WK_EXPORT int WebProcessMainNix(int argc, char* argv[])
{
    // WebProcess should be launched with an option.
    if (argc != 2)
        return 1;

#if !GLIB_CHECK_VERSION(2, 35, 0)
    g_type_init();
#endif

    JSC::initializeThreading();
    WTF::initializeMainThread();

    RunLoop::initializeMainRunLoop();

    SoupSession* session = WebCore::ResourceHandle::defaultSession();
    const char* httpProxy = getenv("http_proxy");
    if (httpProxy) {
        const char* noProxy = getenv("no_proxy");
        SoupProxyURIResolver* resolverNix = soupProxyResolverWkNew(httpProxy, noProxy);
        soup_session_add_feature(session, SOUP_SESSION_FEATURE(resolverNix));
        g_object_unref(resolverNix);
    }

    int socket = atoi(argv[1]);

    ChildProcessInitializationParameters parameters;
    parameters.connectionIdentifier = socket;

    WebProcess::shared().initialize(parameters);

    // Set SOUP cache.
    GOwnPtr<char> soupCacheDirectory(g_build_filename(g_get_user_cache_dir(), "webkitnix", "webprocess", NULL));
    GRefPtr<SoupCache> soupCache = adoptGRef(soup_cache_new(soupCacheDirectory.get(), SOUP_CACHE_SINGLE_USER));
    soup_session_add_feature(session, SOUP_SESSION_FEATURE(soupCache.get()));
    soup_cache_load(soupCache.get());

    RunLoop::run();

    soup_cache_flush(soupCache.get());
    soup_cache_dump(soupCache.get());

    return 0;
}

} // namespace WebKit
