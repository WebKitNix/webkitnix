/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "PlatformUtilities.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKPage.h>
#include <WebKit2/WKRetainPtr.h>
#include <WebKit2/WKView.h>

namespace TestWebKitAPI {

static bool didWebProcessCrash = false;
static bool didWebProcessRelaunch = false;
static bool didFinishLoad = false;

static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void*)
{
    didFinishLoad = true;
}

static void webProcessCrashed(WKViewRef, WKURLRef, const void*)
{
    didWebProcessCrash = true;
}

static void webProcessRelaunched(WKViewRef, const void*)
{
    didWebProcessRelaunch = true;
}

TEST(WebKit2, WebViewWebProcessCrashed)
{
    WKRetainPtr<WKContextRef> context = adoptWK(Util::createContextForInjectedBundleTest("WebViewWebProcessCrashedTest"));
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    WKViewClientV0 viewClient;
    memset(&viewClient, 0, sizeof(viewClient));
    viewClient.base.version = 0;
    viewClient.webProcessCrashed = webProcessCrashed;
    viewClient.webProcessDidRelaunch = webProcessRelaunched;
    WKViewSetViewClient(view.get(), &viewClient.base);

    WKViewInitialize(view.get());

    WKPageLoaderClientV3 pageLoaderClient;
    memset(&pageLoaderClient, 0, sizeof(pageLoaderClient));
    pageLoaderClient.base.version = 3;
    pageLoaderClient.didFinishLoadForFrame = didFinishLoadForFrame;
    WKPageSetPageLoaderClient(WKViewGetPage(view.get()), &pageLoaderClient.base);

    const WKSize size = WKSizeMake(100, 100);
    WKViewSetSize(view.get(), size);

    for (int i = 0; i < 50; ++i) {
        didFinishLoad = false;
        didWebProcessCrash = false;
        didWebProcessRelaunch = false;

        WKRetainPtr<WKURLRef> redUrl = adoptWK(Util::createURLForResource("../WebKit2/CoordinatedGraphics/red-background", "html"));
        WKPageLoadURL(WKViewGetPage(view.get()), redUrl.get());
        Util::run(&didFinishLoad);
        didFinishLoad = false;

        WKContextPostMessageToInjectedBundle(context.get(), Util::toWK("Crash").get(), 0);
        Util::run(&didWebProcessCrash);
        ASSERT_TRUE(didWebProcessCrash);

        WKRetainPtr<WKURLRef> greenUrl = adoptWK(Util::createURLForResource("../WebKit2/CoordinatedGraphics/green-background", "html"));
        WKPageLoadURL(WKViewGetPage(view.get()), greenUrl.get());
        Util::run(&didFinishLoad);

        ASSERT_TRUE(didWebProcessRelaunch);
        ASSERT_TRUE(didFinishLoad);
    }
}

} // TestWebKitAPI
