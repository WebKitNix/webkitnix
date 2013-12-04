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
#include <WebKit2/WKContext.h>
#include <WebKit2/WKPage.h>
#include <WebKit2/WKRetainPtr.h>
#include <WebKit2/WKString.h>
#include "NIXView.h"

static bool operator!=(const WKRect& a, const WKRect& b)
{
    return a.origin.x != b.origin.x || a.origin.y != b.origin.y || a.size.width != b.size.width || a.size.height != b.size.height;
}

// Needed by gtest.
std::ostream& operator<<(std::ostream& stream, const WKRect& rect)
{
    return stream << '[' << rect.origin.x << ", " << rect.origin.y << ' ' << rect.size.width << 'x' << rect.size.height << ']';
}

namespace TestWebKitAPI {

static bool didUpdateTextInputState = false;

static void updateTextInputState(WKViewRef, const NIXTextInputState* state, const void* clientInfo)
{
    didUpdateTextInputState = true;

    memcpy(const_cast<void*>(clientInfo), state, sizeof(NIXTextInputState));
    WKRetain(state->surroundingText);
    WKRetain(state->submitLabel);
}

TEST(WebKitNix, WebViewUpdateTextInputState)
{
    const WKSize size = WKSizeMake(200, 200);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view = adoptWK(WKViewCreate(context.get(), 0));

    NIXTextInputState stateReceived;
    memset(&stateReceived, 0, sizeof(NIXTextInputState));
    NIXViewClientV0 nixViewClient;
    memset(&nixViewClient, 0, sizeof(nixViewClient));
    nixViewClient.base.version = 0;
    nixViewClient.base.clientInfo = &stateReceived;
    nixViewClient.updateTextInputState = updateTextInputState;
    NIXViewSetNixViewClient(view.get(), &nixViewClient.base);

    WKViewInitialize(view.get());
    WKViewSetSize(view.get(), size);

    Util::ForceRepaintClient forceRepaintClient(view.get());

    NIXMouseEvent nixEvent;
    memset(&nixEvent, 0, sizeof(NIXMouseEvent));
    nixEvent.type = kNIXInputEventTypeMouseDown;
    nixEvent.button = kWKEventMouseButtonLeftButton;
    nixEvent.x = 55;
    nixEvent.y = 55;
    nixEvent.globalX = 55;
    nixEvent.globalY = 55;
    nixEvent.clickCount = 1;
    nixEvent.modifiers = static_cast<NIXInputEventModifiers>(0);;
    nixEvent.timestamp = 0;

    // Simple test on content editable.
    Util::PageLoader loader(view.get());
    loader.waitForLoadURLAndRepaint("../nix/single-tap-on-editable-content");
    NIXViewSendMouseEvent(view.get(), &nixEvent);
    nixEvent.type = kNIXInputEventTypeMouseUp;
    NIXViewSendMouseEvent(view.get(), &nixEvent);

    Util::run(&didUpdateTextInputState);

    EXPECT_TRUE(didUpdateTextInputState);
    EXPECT_TRUE(stateReceived.isContentEditable);

    EXPECT_WK_STREQ(stateReceived.surroundingText, "foobar");
    WKRelease(stateReceived.surroundingText);
    EXPECT_WK_STREQ(stateReceived.submitLabel, "");
    WKRelease(stateReceived.submitLabel);
    EXPECT_FALSE(stateReceived.inputMethodHints & NIXImhSensitiveData);
    const WKRect invalidRectState = WKRectMake(0, 0, 0, 0);
    EXPECT_NE(invalidRectState, stateReceived.cursorRect);
    EXPECT_NE(stateReceived.editorRect, invalidRectState);
    EXPECT_NE(stateReceived.cursorRect, stateReceived.editorRect);

    // Test on a form field.
    memset(&stateReceived, 0, sizeof(stateReceived));

    loader.waitForLoadURLAndRepaint("../nix/single-tap-on-form-field");
    nixEvent.type = kNIXInputEventTypeMouseDown;
    NIXViewSendMouseEvent(view.get(), &nixEvent);
    nixEvent.type = kNIXInputEventTypeMouseUp;
    NIXViewSendMouseEvent(view.get(), &nixEvent);

    didUpdateTextInputState = false;
    Util::run(&didUpdateTextInputState);

    WKRelease(stateReceived.surroundingText);
    EXPECT_TRUE(didUpdateTextInputState);
    EXPECT_WK_STREQ(stateReceived.submitLabel, "submitLabelValue");
    WKRelease(stateReceived.submitLabel);
    EXPECT_TRUE(stateReceived.inputMethodHints & NIXImhSensitiveData);
}

} // TestWebKitAPI
