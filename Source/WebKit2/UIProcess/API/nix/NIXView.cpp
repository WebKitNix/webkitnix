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

#include "WKCoordinatedSceneAPICast.h"

// Included here to be visible to forwarding headers generation script.
#include <JavaScriptCore/WebKitAvailability.h>
#include <WebKit2/WKBundle.h>

#include <glib-object.h>

using namespace WebKit;

NIXView NIXViewCreate(WKContextRef context, WKPageGroupRef pageGroup)
{
#if !GLIB_CHECK_VERSION(2, 35, 0)
    g_type_init();
#endif
    return toAPI(new WebKit::WebView(toImpl(context), toImpl(pageGroup)));
}

void NIXViewRelease(NIXView view)
{
    delete toImpl(view);
}

void NIXViewSetViewClient(NIXView view, const NIXViewClient* viewClient)
{
    toImpl(view)->setViewClient(viewClient);
}

void NIXViewInitialize(NIXView view)
{
    toImpl(view)->initialize();
}

WKSize NIXViewSize(NIXView view)
{
    return toImpl(view)->size();
}

void NIXViewSetSize(NIXView view, WKSize size)
{
    toImpl(view)->setSize(size);
}

WKPoint NIXViewScrollPosition(NIXView view)
{
    return toImpl(view)->scrollPosition();
}

void NIXViewSetScrollPosition(NIXView view, WKPoint position)
{
    toImpl(view)->setScrollPosition(position);
}

void NIXViewSetUserViewportTransformation(NIXView view, const NIXMatrix* userViewportTransformation)
{
    WebCore::TransformationMatrix transform(userViewportTransformation->xx, userViewportTransformation->yx,
                                            userViewportTransformation->xy, userViewportTransformation->yy,
                                            userViewportTransformation->x0, userViewportTransformation->y0);
    toImpl(view)->setUserViewportTransformation(transform);
}

WKPoint NIXViewUserViewportToContents(NIXView view, WKPoint point)
{
    return toImpl(view)->userViewportToContents(point);
}

bool NIXViewIsFocused(NIXView view)
{
    return toImpl(view)->isFocused();
}

void NIXViewSetFocused(NIXView view, bool focused)
{
    toImpl(view)->setFocused(focused);
}

bool NIXViewIsVisible(NIXView view)
{
    return toImpl(view)->isVisible();
}

void NIXViewSetVisible(NIXView view, bool visible)
{
    toImpl(view)->setVisible(visible);
}

bool NIXViewIsActive(NIXView view)
{
    return toImpl(view)->isActive();
}

void NIXViewSetActive(NIXView view, bool active)
{
    toImpl(view)->setActive(active);
}

bool NIXViewTransparentBackground(NIXView view)
{
    return toImpl(view)->transparentBackground();
}

void NIXViewSetTransparentBackground(NIXView view, bool transparent)
{
    toImpl(view)->setTransparentBackground(transparent);
}

bool NIXViewDrawBackground(NIXView view)
{
    return toImpl(view)->drawBackground();
}

void NIXViewSetDrawBackground(NIXView view, bool drawBackground)
{
    toImpl(view)->setDrawBackground(drawBackground);
}

float NIXViewScale(NIXView view)
{
    return toImpl(view)->scale();
}

void NIXViewSetScale(NIXView view, float scale)
{
    toImpl(view)->setScale(scale);
}

void NIXViewSetOpacity(NIXView view, float opacity)
{
    toImpl(view)->setOpacity(opacity);
}

float NIXViewOpacity(NIXView view)
{
    return toImpl(view)->opacity();
}

WKSize NIXViewVisibleContentsSize(NIXView view)
{
    return toImpl(view)->visibleContentsSize();
}

void NIXViewPaintToCurrentGLContext(NIXView view)
{
    toImpl(view)->paintToCurrentGLContext();
}

void NIXViewFindZoomableAreaForPoint(NIXView view, WKPoint point, int horizontalRadius, int verticalRadius)
{
    toImpl(view)->findZoomableAreaForPoint(point, horizontalRadius, verticalRadius);
}

WKPageRef NIXViewGetPage(NIXView view)
{
    return toImpl(view)->pageRef();
}

void NIXViewSendMouseEvent(NIXView view, const NIXMouseEvent* event)
{
    toImpl(view)->sendMouseEvent(*event);
}

void NIXViewSendWheelEvent(NIXView view, const NIXWheelEvent* event)
{
    toImpl(view)->sendWheelEvent(*event);
}

void NIXViewSendKeyEvent(NIXView view, const NIXKeyEvent* event)
{
    toImpl(view)->sendKeyEvent(*event);
}

void NIXViewSendTouchEvent(NIXView view, const NIXTouchEvent* event)
{
    toImpl(view)->sendTouchEvent(*event);
}

void NIXViewSendGestureEvent(NIXView view, const NIXGestureEvent* event)
{
    toImpl(view)->sendGestureEvent(*event);
}

void NIXViewSuspendActiveDOMObjectsAndAnimations(NIXView view)
{
    toImpl(view)->suspendActiveDOMObjectsAndAnimations();
}

void NIXViewResumeActiveDOMObjectsAndAnimations(NIXView view)
{
    toImpl(view)->resumeActiveDOMObjectsAndAnimations();
}

bool NIXViewIsSuspended(NIXView view)
{
    return toImpl(view)->isSuspended();
}

WKCoordinatedScene NIXViewGetCoordinatedScene(NIXView view)
{
    return toAPI(toImpl(view)->coordinatedGraphicsScene());
}
