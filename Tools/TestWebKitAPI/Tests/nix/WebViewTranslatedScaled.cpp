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

#include "PageLoader.h"
#include "GLUtilities.h"
#include "NIXView.h"
#include "PlatformUtilities.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKRetainPtr.h>

namespace TestWebKitAPI {

static bool scaleChanged = false;

static void didChangeContentScaleFactor(WKViewRef, const void*)
{
    scaleChanged = true;
}

TEST(WebKitNix, WebViewTranslatedScaled)
{
    // This test opens a webpage that contains a white background, no viewport
    // metatag and a red rectangle (20x20)px at (0,0) position. The viewport is
    // then translated to (10,10) position. After that it's applied a scale=2.0
    // At this point we will have a red rectangle of (40x40)px at (10,10).

    const WKSize size = WKSizeMake(100, 100);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);
    forceRepaintClient.viewClient().didChangeContentScaleFactor = didChangeContentScaleFactor;
    WKViewSetViewClient(view.get(), &forceRepaintClient.viewClient().base);

    const int delta = 10;
    WKViewSetUserViewportTranslation(view.get(), delta, delta);

    WKViewInitialize(view.get());
    WKPageSetUseFixedLayout(WKViewGetPage(view.get()), true);
    WKViewSetSize(view.get(), size);

    glViewport(0, 0, size.width, size.height);
    forceRepaintClient.clear();

    Util::PageLoader loader(view.get());

    loader.waitForLoadURLAndRepaint("../nix/red-square");

    for (double scale = 1.0; scale < 3.0; scale++) {
        scaleChanged = false;
        WKViewSetContentScaleFactor(view.get(), scale);
        Util::run(&scaleChanged);
        loader.forceRepaint();

        ToolsNix::RGBAPixel outsideTheContent = offscreenBuffer.readPixelAtPoint(delta - 1, delta - 1);
        EXPECT_EQ(ToolsNix::RGBAPixel::blue(), outsideTheContent);

        ToolsNix::RGBAPixel squareTopLeft = offscreenBuffer.readPixelAtPoint(delta, delta);
        EXPECT_EQ(ToolsNix::RGBAPixel::red(), squareTopLeft);

        const int scaledSize = scale * 20;
        ToolsNix::RGBAPixel squareBottomRight = offscreenBuffer.readPixelAtPoint(delta + scaledSize - 1, delta + scaledSize - 1);
        EXPECT_EQ(ToolsNix::RGBAPixel::red(), squareBottomRight);

        ToolsNix::RGBAPixel outsideSquare = offscreenBuffer.readPixelAtPoint(delta + scaledSize, delta + scaledSize);
        EXPECT_EQ(ToolsNix::RGBAPixel::white(), outsideSquare);
    }
}

} // TestWebKitAPI
