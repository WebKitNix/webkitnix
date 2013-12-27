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
#include <WebKit2/WKCoordinatedScene.h>
#include <WebKit2/WKGeometry.h>
#include <WebKit2/WKPreferencesPrivate.h>
#include <WebKit2/WKRetainPtr.h>
#include "Test.h"

namespace TestWebKitAPI {

static bool finishedLoad = false;
static bool scrollChanged = false;
static bool scaleChanged = false;

static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void*)
{
    finishedLoad = true;
}

static void didChangeContentsPosition(WKViewRef, WKPoint p, const void*)
{
    scrollChanged = true;
}

static void didChangeContentScaleFactor(WKViewRef, const void*)
{
    scaleChanged = true;
}

TEST(WebKitNix, BackForward)
{
    const WKSize size = WKSizeMake(800, 600);

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    WKViewInitialize(view.get());
    WKPageSetUseFixedLayout(WKViewGetPage(view.get()), true);
    WKViewSetSize(view.get(), size);
    WKPageGroupRef pageGroup = WKPageGetPageGroup(WKViewGetPage(view.get()));
    WKPreferencesRef preferences = WKPageGroupGetPreferences(pageGroup);

    WKPageRef pageRef = WKViewGetPage(view.get());
    ASSERT_TRUE(pageRef);

    WKPageLoaderClientV3 loaderClient;
    memset(&loaderClient, 0, sizeof(loaderClient));
    loaderClient.base.version = 3;
    loaderClient.didFinishLoadForFrame = didFinishLoadForFrame;
    WKPageSetPageLoaderClient(pageRef, &loaderClient.base);

    WKViewClientV0 viewClient;
    memset(&viewClient, 0, sizeof(viewClient));
    viewClient.base.version = 0;
    viewClient.didChangeContentsPosition = didChangeContentsPosition;
    viewClient.didChangeContentScaleFactor = didChangeContentScaleFactor;
    WKViewSetViewClient(view.get(), &viewClient.base);

    WKRetainPtr<WKURLRef> url = adoptWK(Util::createURLForResource("../nix/backforward1", "html"));
    WKPageLoadURL(pageRef, url.get());
    Util::run(&finishedLoad);

    float newScale = 2.0;
    WKPoint point = WKPointMake(10, 350);

    WKViewSetContentPosition(view.get(), point);
    WKViewSetContentScaleFactor(view.get(), newScale);
    Util::run(&scaleChanged);

    float scale = WKViewGetContentScaleFactor(view.get());
    WKPoint contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(newScale, scale);
    EXPECT_EQ(point.x, contentPosition.x);
    EXPECT_EQ(point.y, contentPosition.y);

    finishedLoad = false;
    scaleChanged = false;
    url = adoptWK(Util::createURLForResource("../nix/backforward2", "html"));
    WKPageLoadURL(pageRef, url.get());
    Util::run(&finishedLoad);
    Util::run(&scaleChanged);

    scale = WKViewGetContentScaleFactor(view.get());
    contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(1, scale);
    EXPECT_EQ(0, contentPosition.x);
    EXPECT_EQ(0, contentPosition.y);

    scaleChanged = false;
    WKPageGoBack(pageRef);
    scrollChanged = false;
    finishedLoad = false;
    Util::run(&finishedLoad);
    Util::run(&scrollChanged);
    Util::run(&scaleChanged);

    scale = WKViewGetContentScaleFactor(view.get());
    contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(newScale, scale);
    EXPECT_EQ(point.x, contentPosition.x);
    EXPECT_EQ(point.y, contentPosition.y);

    scaleChanged = false;
    WKPageGoForward(pageRef);
    scrollChanged = false;
    finishedLoad = false;
    Util::run(&finishedLoad);
    Util::run(&scrollChanged);
    Util::run(&scaleChanged);

    scale = WKViewGetContentScaleFactor(view.get());
    contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(1, scale);
    EXPECT_EQ(0, contentPosition.x);
    EXPECT_EQ(0, contentPosition.y);

    scaleChanged = false;
    WKPageGoBack(pageRef);
    scrollChanged = false;
    finishedLoad = false;
    Util::run(&finishedLoad);
    Util::run(&scrollChanged);
    Util::run(&scaleChanged);

    scale = WKViewGetContentScaleFactor(view.get());
    contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(newScale, scale);
    EXPECT_EQ(point.x, contentPosition.x);
    EXPECT_EQ(point.y, contentPosition.y);

    scaleChanged = false;
    WKPageGoForward(pageRef);
    scrollChanged = false;
    finishedLoad = false;
    Util::run(&finishedLoad);
    Util::run(&scrollChanged);
    Util::run(&scaleChanged);

    scale = WKViewGetContentScaleFactor(view.get());
    contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(1, scale);
    EXPECT_EQ(0, contentPosition.x);
    EXPECT_EQ(0, contentPosition.y);

    scaleChanged = false;
    WKPageGoBack(pageRef);
    scrollChanged = false;
    finishedLoad = false;
    Util::run(&finishedLoad);
    Util::run(&scrollChanged);
    Util::run(&scaleChanged);

    scale = WKViewGetContentScaleFactor(view.get());
    contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(newScale, scale);
    EXPECT_EQ(point.x, contentPosition.x);
    EXPECT_EQ(point.y, contentPosition.y);

    scaleChanged = false;
    WKPageGoForward(pageRef);
    scrollChanged = false;
    finishedLoad = false;
    Util::run(&finishedLoad);
    Util::run(&scrollChanged);
    Util::run(&scaleChanged);

    scale = WKViewGetContentScaleFactor(view.get());
    contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(1, scale);
    EXPECT_EQ(0, contentPosition.x);
    EXPECT_EQ(0, contentPosition.y);

    scaleChanged = false;
    WKPageGoBack(pageRef);
    scrollChanged = false;
    finishedLoad = false;
    Util::run(&finishedLoad);
    Util::run(&scrollChanged);
    Util::run(&scaleChanged);

    scale = WKViewGetContentScaleFactor(view.get());
    contentPosition = WKViewGetContentPosition(view.get());
    EXPECT_EQ(newScale, scale);
    EXPECT_EQ(point.x, contentPosition.x);
    EXPECT_EQ(point.y, contentPosition.y);
}

} // TestWebKitAPI
