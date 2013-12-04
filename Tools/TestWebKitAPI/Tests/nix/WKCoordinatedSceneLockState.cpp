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

#include "GLUtilities.h"
#include "PageLoader.h"
#include "NIXView.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKCoordinatedScene.h>
#include <WebKit2/WKRetainPtr.h>

namespace TestWebKitAPI {

static ToolsNix::GLOffscreenBuffer* glBuffer = 0;
static ToolsNix::RGBAPixel lastPixelSample;

static void didStartProvisionalLoadForFrame_LockAndUnlock(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void* clientInfo)
{
    Util::PageLoader* loader = static_cast<Util::PageLoader*>(const_cast<void*>(clientInfo));
    WKCoordinatedSceneLockState(WKViewGetCoordinatedScene(loader->webView()));
    lastPixelSample = glBuffer->readPixelAtPoint(1, 1);
    WKCoordinatedSceneUnlockState(WKViewGetCoordinatedScene(loader->webView()));
}

static void didStartProvisionalLoadForFrame_LockOnly(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void* clientInfo)
{
    Util::PageLoader* loader = static_cast<Util::PageLoader*>(const_cast<void*>(clientInfo));
    WKCoordinatedSceneLockState(WKViewGetCoordinatedScene(loader->webView()));
    lastPixelSample = glBuffer->readPixelAtPoint(1, 1);
}

TEST(WebKitNix, LockAndUnlockCoordinatedSceneState)
{
    const WKSize size = WKSizeMake(100, 100);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());
    glBuffer = &offscreenBuffer;

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);

    WKViewInitialize(view.get());
    WKPageSetUseFixedLayout(WKViewGetPage(view.get()), true);
    WKViewSetDrawsBackground(view.get(), false);
    WKViewSetSize(view.get(), size);

    glViewport(0, 0, size.width, size.height);
    forceRepaintClient.clear();

    Util::PageLoader loader(view.get());
    loader.loaderClient().didStartProvisionalLoadForFrame = didStartProvisionalLoadForFrame_LockAndUnlock;
    WKPageSetPageLoaderClient(WKViewGetPage(view.get()), &loader.loaderClient().base);

    ToolsNix::RGBAPixel currentPixelSample = offscreenBuffer.readPixelAtPoint(1, 1);
    EXPECT_EQ(ToolsNix::RGBAPixel::blue(), currentPixelSample);

    loader.waitForLoadURLAndRepaint("../WebKit2/CoordinatedGraphics/green-background");

    EXPECT_EQ(ToolsNix::RGBAPixel::blue(), lastPixelSample);
    currentPixelSample = offscreenBuffer.readPixelAtPoint(1, 1);
    EXPECT_EQ(ToolsNix::RGBAPixel::green(), currentPixelSample);

    loader.waitForLoadURLAndRepaint("../WebKit2/CoordinatedGraphics/red-background");

    EXPECT_EQ(ToolsNix::RGBAPixel::green(), lastPixelSample);
    currentPixelSample = offscreenBuffer.readPixelAtPoint(1, 1);
    EXPECT_EQ(ToolsNix::RGBAPixel::red(), currentPixelSample);
}

TEST(WebKitNix, LockCoordinatedSceneState)
{
    const WKSize size = WKSizeMake(100, 100);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());
    glBuffer = &offscreenBuffer;

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);

    WKViewInitialize(view.get());
    WKPageSetUseFixedLayout(WKViewGetPage(view.get()), true);
    WKViewSetDrawsBackground(view.get(), false);
    WKViewSetSize(view.get(), size);

    glViewport(0, 0, size.width, size.height);
    forceRepaintClient.clear();

    Util::PageLoader loader(view.get());
    loader.loaderClient().didStartProvisionalLoadForFrame = didStartProvisionalLoadForFrame_LockOnly;
    WKPageSetPageLoaderClient(WKViewGetPage(view.get()), &loader.loaderClient().base);

    ToolsNix::RGBAPixel currentPixelSample = offscreenBuffer.readPixelAtPoint(1, 1);
    EXPECT_EQ(ToolsNix::RGBAPixel::blue(), currentPixelSample);

    loader.waitForLoadURLAndRepaint("../WebKit2/CoordinatedGraphics/red-background");

    EXPECT_EQ(ToolsNix::RGBAPixel::blue(), lastPixelSample);
    currentPixelSample = offscreenBuffer.readPixelAtPoint(1, 1);
    EXPECT_EQ(ToolsNix::RGBAPixel::blue(), currentPixelSample);

    WKCoordinatedSceneUnlockState(WKViewGetCoordinatedScene(view.get()));
    forceRepaintClient.repaint();

    currentPixelSample = offscreenBuffer.readPixelAtPoint(1, 1);
    EXPECT_EQ(ToolsNix::RGBAPixel::red(), currentPixelSample);
}

} // TestWebKitAPI
