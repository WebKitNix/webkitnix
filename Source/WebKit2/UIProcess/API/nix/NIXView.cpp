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

#include "WebViewNix.h"
#include "WebContext.h"
#include "WebPageGroup.h"
#include "WebViewportAttributes.h"

// Included here to be visible to forwarding headers generation script.
#include <JavaScriptCore/WebKitAvailability.h>
#include <WebKit2/WKBundle.h>

#include <glib-object.h>

using namespace WebKit;

float NIXViewGetScaleToFitContents(WKViewRef view)
{
    return static_cast<WebViewNix*>(toImpl(view))->scaleToFitContents();
}

void NIXViewScaleToFitContents(WKViewRef view)
{
    static_cast<WebViewNix*>(toImpl(view))->adjustScaleToFitContents();
}

void NIXViewSetNixViewClient(WKViewRef view, const NIXViewClientBase* viewClient)
{
    static_cast<WebViewNix*>(toImpl(view))->setNixViewClient(viewClient);
}

WKSize NIXViewVisibleContentsSize(WKViewRef view)
{
    WebCore::FloatSize visibleContentsSize = toImpl(view)->visibleContentsSize();
    return WKSizeMake(visibleContentsSize.width(), visibleContentsSize.height());
}

void NIXViewSetScreenRect(WKViewRef view, WKRect rect)
{
    static_cast<WebViewNix*>(toImpl(view))->setScreenRect(WebCore::FloatRect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));
}

void NIXViewFindZoomableAreaForPoint(WKViewRef view, WKPoint point, int horizontalRadius, int verticalRadius)
{
    static_cast<WebViewNix*>(toImpl(view))->findZoomableAreaForPoint(point, horizontalRadius, verticalRadius);
}

void NIXViewSendMouseEvent(WKViewRef view, const NIXMouseEvent* event)
{
    static_cast<WebViewNix*>(toImpl(view))->sendMouseEvent(*event);
}

void NIXViewSendWheelEvent(WKViewRef view, const NIXWheelEvent* event)
{
    static_cast<WebViewNix*>(toImpl(view))->sendWheelEvent(*event);
}

void NIXViewSendKeyEvent(WKViewRef view, const NIXKeyEvent* event)
{
    static_cast<WebViewNix*>(toImpl(view))->sendKeyEvent(*event);
}

void NIXViewSendTouchEvent(WKViewRef view, const NIXTouchEvent* event)
{
    static_cast<WebViewNix*>(toImpl(view))->sendTouchEvent(*event);
}

void NIXViewSendGestureEvent(WKViewRef view, const NIXGestureEvent* event)
{
    static_cast<WebViewNix*>(toImpl(view))->sendGestureEvent(*event);
}

bool NIXViewIsSuspended(WKViewRef view)
{
    return static_cast<WebViewNix*>(toImpl(view))->isSuspended();
}

WKSize NIXViewportAttributesGetSize(WKViewportAttributesRef attributesRef)
{
    WebCore::ViewportAttributes originalAttributes = toImpl(attributesRef)->originalAttributes();
    return WKSizeMake(originalAttributes.layoutSize.width(), originalAttributes.layoutSize.height());
}

double NIXViewportAttributesGetMinimumScale(WKViewportAttributesRef attributesRef)
{
    return toImpl(attributesRef)->originalAttributes().minimumScale;
}

double NIXViewportAttributesGetMaximumScale(WKViewportAttributesRef attributesRef)
{
    return toImpl(attributesRef)->originalAttributes().maximumScale;
}

double NIXViewportAttributesGetInitialScale(WKViewportAttributesRef attributesRef)
{
    return toImpl(attributesRef)->originalAttributes().initialScale;
}

bool NIXViewportAttributesGetIsUserScalable(WKViewportAttributesRef attributesRef)
{
    return toImpl(attributesRef)->originalAttributes().userScalable;
}

void NIXViewViewportInteractionStart(WKViewRef view)
{
    static_cast<WebViewNix*>(toImpl(view))->viewportInteractionStart();
}

void NIXViewViewportInteractionStop(WKViewRef view)
{
    static_cast<WebViewNix*>(toImpl(view))->viewportInteractionStop();
}
