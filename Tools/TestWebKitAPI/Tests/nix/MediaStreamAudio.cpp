/*
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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

#include "PageLoader.h"
#include "PlatformUtilities.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKRetainPtr.h>

namespace TestWebKitAPI {

static bool loadDone;
static bool audioRenderingDone;
static bool correctDeviceId;
static bool titleChanged;
static bool requestSucceeded;
static bool mediaStreamAttributesOk;

static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void*)
{
    loadDone = true;
}

static void didReceiveMessageFromInjectedBundle(WKContextRef, WKStringRef messageName, WKTypeRef, const void*)
{
    if (WKStringIsEqualToUTF8CString(messageName, "AudioDataVectorRendered"))
        audioRenderingDone = true;
    else if (WKStringIsEqualToUTF8CString(messageName, "DummyAudioDeviceId#0"))
        correctDeviceId = true;
    else if (WKStringIsEqualToUTF8CString(messageName, "MediaStreamAttributesOk"))
        mediaStreamAttributesOk = true;
}

static void didReceiveTitleForFrame(WKPageRef, WKStringRef title, WKFrameRef, WKTypeRef, const void *)
{
    requestSucceeded = WKStringIsEqualToUTF8CString(title, "MediaStreamRequestSuccess");
    titleChanged = true;
}

TEST(WebKitNix, MediaStreamAudio)
{
    loadDone = false;
    correctDeviceId = false;
    audioRenderingDone = false;
    titleChanged = false;
    requestSucceeded = false;

    WKRetainPtr<WKContextRef> context = adoptWK(Util::createContextForInjectedBundleTest("MediaStreamAudioTest"));
    WKRetainPtr<WKViewRef> view = adoptWK(WKViewCreate(context.get(), 0));
    WKViewInitialize(view.get());

    WKPageGroupRef pageGroup = WKPageGetPageGroup(WKViewGetPage(view.get()));
    WKPreferencesRef preferences = WKPageGroupGetPreferences(pageGroup);
    WKPreferencesSetWebAudioEnabled(preferences, true);

    WKContextInjectedBundleClient injectedBundleClient;
    memset(&injectedBundleClient, 0, sizeof(injectedBundleClient));
    injectedBundleClient.didReceiveMessageFromInjectedBundle = didReceiveMessageFromInjectedBundle;
    WKContextSetInjectedBundleClient(context.get(), &injectedBundleClient);

    WKPageLoaderClient loaderClient;
    memset(&loaderClient, 0, sizeof(loaderClient));
    loaderClient.didFinishLoadForFrame = didFinishLoadForFrame;
    loaderClient.didReceiveTitleForFrame = didReceiveTitleForFrame;
    WKPageSetPageLoaderClient(WKViewGetPage(view.get()), &loaderClient);

    WKRetainPtr<WKURLRef> url = adoptWK(Util::createURLForResource("../nix/MediaStreamAudio", "html"));
    WKPageLoadURL(WKViewGetPage(view.get()), url.get());

    Util::run(&loadDone);
    Util::run(&correctDeviceId);
    Util::run(&mediaStreamAttributesOk);
    Util::run(&audioRenderingDone);
    Util::run(&titleChanged);
    ASSERT_TRUE(requestSucceeded);
}

} // TestWebKitAPI
