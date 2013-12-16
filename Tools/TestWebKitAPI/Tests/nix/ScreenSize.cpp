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
#include "NIXView.h"
#include "PageLoader.h"
#include "PlatformUtilities.h"
#include <WebKit2/WKRetainPtr.h>
#include <WebKit2/WKString.h>
#include <WebKit2/WKGeometry.h>
#include <cstring>

namespace TestWebKitAPI {

static void alert(WKPageRef, WKStringRef alertText, WKFrameRef, const void* clientInfo)
{
    WKRetain(alertText);
    *(WKStringRef*)clientInfo = alertText;
}

TEST(WebKitNix, ScreenSize)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view = adoptWK(WKViewCreate(context.get(), 0));

    WKViewInitialize(view.get());
    NIXViewSetScreenRect(view.get(), WKRectMake(0, 0, 1234, 4321));

    WKStringRef alertText = 0;
    WKPageUIClientV2 uiClient;
    std::memset(&uiClient, 0, sizeof(uiClient));
    uiClient.base.version = 2;
    uiClient.base.clientInfo = &alertText;
    uiClient.runJavaScriptAlert = &alert;
    WKPageSetPageUIClient(WKViewGetPage(view.get()), &uiClient.base);
    Util::PageLoader loader(view.get());
    loader.waitForLoadURL("../nix/ScreenSize");

    ASSERT_TRUE(alertText);
    EXPECT_WK_STREQ(alertText, "1234x4321");
    WKRelease(alertText);
}

}
