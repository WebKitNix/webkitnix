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
#include <NIXView.h>
#include "NIXViewAutoPtr.h"

namespace TestWebKitAPI {

static bool didWebProcessCrash = false;
static bool didWebProcessRelaunch = false;
static bool didFinishLoad = false;

static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void*)
{
    didFinishLoad = true;
}

static void webProcessCrashed(NIXView, WKStringRef, const void*)
{
    didWebProcessCrash = true;
}

static void webProcessRelaunched(NIXView, const void*)
{
    didWebProcessRelaunch = true;
}

TEST(WebKitNix, WebViewWebProcessCrashed)
{
    WKRetainPtr<WKContextRef> context = adoptWK(Util::createContextForInjectedBundleTest("WebViewWebProcessCrashedTest"));
    NIXViewAutoPtr view(NIXViewCreate(context.get(), 0));

    NIXViewClient viewClient;
    memset(&viewClient, 0, sizeof(NIXViewClient));
    viewClient.version = kNIXViewClientCurrentVersion;
    viewClient.webProcessCrashed = webProcessCrashed;
    viewClient.webProcessRelaunched = webProcessRelaunched;
    NIXViewSetViewClient(view.get(), &viewClient);

    NIXViewInitialize(view.get());

    WKPageLoaderClient loaderClient;
    memset(&loaderClient, 0, sizeof(loaderClient));

    loaderClient.version = 0;
    loaderClient.didFinishLoadForFrame = didFinishLoadForFrame;
    WKPageSetPageLoaderClient(NIXViewGetPage(view.get()), &loaderClient);

    const WKSize size = WKSizeMake(100, 100);
    NIXViewSetSize(view.get(), size);

    for (int i = 0; i < 50; ++i) {
        didFinishLoad = false;
        didWebProcessCrash = false;
        didWebProcessRelaunch = false;

        WKRetainPtr<WKURLRef> redUrl = adoptWK(Util::createURLForResource("../nix/red-background", "html"));
        WKPageLoadURL(NIXViewGetPage(view.get()), redUrl.get());
        Util::run(&didFinishLoad);
        didFinishLoad = false;

        WKContextPostMessageToInjectedBundle(context.get(), Util::toWK("Crash").get(), 0);
        Util::run(&didWebProcessCrash);
        ASSERT_TRUE(didWebProcessCrash);

        WKRetainPtr<WKURLRef> greenUrl = adoptWK(Util::createURLForResource("../nix/green-background", "html"));
        WKPageLoadURL(NIXViewGetPage(view.get()), greenUrl.get());
        Util::run(&didFinishLoad);

        ASSERT_TRUE(didWebProcessRelaunch);
        ASSERT_TRUE(didFinishLoad);
    }
}

} // TestWebKitAPI
