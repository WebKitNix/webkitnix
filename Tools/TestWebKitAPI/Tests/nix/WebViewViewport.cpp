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

#include "NIXView.h"
#include "PageLoader.h"
#include "GLUtilities.h"
#include "PlatformUtilities.h"
#include <WebKit2/WKRetainPtr.h>

namespace TestWebKitAPI {

static bool scaleChanged = false;

static void didChangeContentScaleFactor(WKViewRef, const void*)
{
    scaleChanged = true;
}

TEST(WebKitNix, WebViewViewport)
{
    // This test opens a page with contents size of 20000x980 pixels, a white
    // background, no viewport metatag and two black dots located at
    // 10000x400 and 10400x800, then it scales and scroll the page to check if
    // the black pixels are where they are expected to be.

    const unsigned contentsWidth = 980;
    const WKSize size = WKSizeMake(contentsWidth / 2, 200);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());
    glViewport(0, 0, size.width, size.height);

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);
    forceRepaintClient.viewClient().didChangeContentScaleFactor = didChangeContentScaleFactor;
    WKViewSetViewClient(view.get(), &forceRepaintClient.viewClient().base);

    WKViewInitialize(view.get());
    WKPageSetUseFixedLayout(WKViewGetPage(view.get()), true);
    WKViewSetSize(view.get(), size);

    Util::PageLoader loader(view.get());
    loader.waitForLoadURLAndRepaint("../nix/WebViewViewport");

    // Checking that without scrolling, we correctly see the white background.
    ToolsNix::RGBAPixel firstDotBefore = offscreenBuffer.readPixelAtPoint(0, 0);
    EXPECT_EQ(ToolsNix::RGBAPixel::white(), firstDotBefore);
    ToolsNix::RGBAPixel secondDotBefore = offscreenBuffer.readPixelAtPoint(200, size.height - 1);
    EXPECT_EQ(ToolsNix::RGBAPixel::white(), secondDotBefore);

    // Scale and scroll so that the first dot be at the topleft of the view.
    WKViewSetContentScaleFactor(view.get(), 0.5);
    WKViewSetContentPosition(view.get(), WKPointMake(400, 10000));
    Util::run(&scaleChanged);
    loader.forceRepaint();

    // Now check that the black dots are in the expected places.
    // The pixel color of the result could be different depending
    // of the video driver, so the check is made relative to the
    // color at position of the reference pixel. The color in the
    // neighborhood should be numerically higher than the
    // reference color.
    ToolsNix::RGBAPixel firstDotAfter = offscreenBuffer.readPixelAtPoint(0, 0);
    ToolsNix::RGBAPixel sample = offscreenBuffer.readPixelAtPoint(0, 1);
    EXPECT_TRUE(sample > firstDotAfter);
    sample = offscreenBuffer.readPixelAtPoint(1, 0);
    EXPECT_TRUE(sample > firstDotAfter);
    sample = offscreenBuffer.readPixelAtPoint(1, 1);
    EXPECT_TRUE(sample > firstDotAfter);

    ToolsNix::RGBAPixel secondDotAfter = offscreenBuffer.readPixelAtPoint(200, size.height - 1);
    sample = offscreenBuffer.readPixelAtPoint(199, size.height - 2);
    EXPECT_TRUE(sample > secondDotAfter);
    sample = offscreenBuffer.readPixelAtPoint(200, size.height - 2);
    EXPECT_TRUE(sample > secondDotAfter);
    sample = offscreenBuffer.readPixelAtPoint(201, size.height - 2);
    EXPECT_TRUE(sample > secondDotAfter);
    sample = offscreenBuffer.readPixelAtPoint(199, size.height - 1);
    EXPECT_TRUE(sample > secondDotAfter);
    sample = offscreenBuffer.readPixelAtPoint(201, size.height - 1);
    EXPECT_TRUE(sample > secondDotAfter);
}

}
