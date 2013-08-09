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

#include "GLUtilities.h"
#include "PageLoader.h"
#include "PlatformUtilities.h"
#include "NIXView.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKCoordinatedScene.h>
#include <WebKit2/WKGeometry.h>
#include <WebKit2/WKPreferencesPrivate.h>
#include <WebKit2/WKRetainPtr.h>

namespace TestWebKitAPI {

TEST(WebKitNix, SingleOverflowScroll)
{
    const WKSize size = WKSizeMake(100, 100);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));
    NIXViewSetAutoScaleToFitContents(view.get(), false);

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);

    WKViewInitialize(view.get());
    WKPageSetUseFixedLayout(WKViewGetPage(view.get()), true);
    WKViewSetSize(view.get(), size);
    WKPageGroupRef pageGroup = WKPageGetPageGroup(WKViewGetPage(view.get()));
    WKPreferencesRef preferences = WKPageGroupGetPreferences(pageGroup);
    WKPreferencesSetAcceleratedCompositingForOverflowScrollEnabled(preferences, true);

    glViewport(0, 0, size.width, size.height);
    forceRepaintClient.clear();

    Util::PageLoader loader(view.get());
    loader.waitForLoadURLAndRepaint("../nix/overflow-scroll");
    WKCoordinatedScene scene = WKViewGetCoordinatedScene(view.get());
    ASSERT_TRUE(scene != 0);
    WKCoordinatedSceneLayer layer = WKCoordinatedSceneFindScrollableContentsLayerAt(scene, WKPointMake(30, 30));
    ASSERT_TRUE(layer != 0);
    WKCoordinatedSceneScrollBy(layer, WKSizeMake(0, 100));
    forceRepaintClient.repaint();
    ToolsNix::RGBAPixel greenSample = offscreenBuffer.readPixelAtPoint(25, 25);
    EXPECT_EQ(ToolsNix::RGBAPixel::green(), greenSample);
}

TEST(WebKitNix, MultipleOverflowScroll)
{
    const WKSize size = WKSizeMake(100, 100);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));
    NIXViewSetAutoScaleToFitContents(view.get(), false);

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);

    WKViewInitialize(view.get());
    WKPageSetUseFixedLayout(WKViewGetPage(view.get()), true);
    WKViewSetSize(view.get(), size);

    WKPageGroupRef pageGroup = WKPageGetPageGroup(WKViewGetPage(view.get()));
    WKPreferencesRef preferences = WKPageGroupGetPreferences(pageGroup);
    WKPreferencesSetAcceleratedCompositingForOverflowScrollEnabled(preferences, true);

    glViewport(0, 0, size.width, size.height);
    forceRepaintClient.clear();

    Util::PageLoader loader(view.get());
    loader.waitForLoadURLAndRepaint("../nix/multiple-overflow-scroll-layers");
    WKCoordinatedScene scene = WKViewGetCoordinatedScene(view.get());
    ASSERT_TRUE(scene != 0);
    WKCoordinatedSceneLayer layer = WKCoordinatedSceneFindScrollableContentsLayerAt(scene, WKPointMake(30, 30));
    ASSERT_TRUE(layer != 0);
    WKCoordinatedSceneLayer ancestorScroller = WKCoordinatedSceneGetAncestorScrollableContentsLayer(layer);
    ASSERT_TRUE(ancestorScroller != 0);
    ancestorScroller = WKCoordinatedSceneGetAncestorScrollableContentsLayer(ancestorScroller);
    ASSERT_TRUE(ancestorScroller == 0);
}

} // TestWebKitAPI
