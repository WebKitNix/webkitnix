/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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
#include "WebViewNix.h"

#include "DrawingAreaProxyImpl.h"
#include "CoordinatedLayerTreeHostProxy.h"
#include "NativeWebGestureEvent.h"
#include "NativeWebKeyboardEvent.h"
#include "NativeWebMouseEvent.h"
#include "NativeWebWheelEvent.h"
#include "WebContext.h"
#include "WebPageGroup.h"
#include "WebPopupMenuListenerNix.h"
#include "WebPreferences.h"
#include <WebCore/CoordinatedGraphicsScene.h>
#include <WebCore/TextureMapperGL.h>

using namespace WebCore;

namespace WebKit {

PassRefPtr<WebView> WebView::create(WebContext* context, WebPageGroup* pageGroup)
{
#if !GLIB_CHECK_VERSION(2, 35, 0)
    g_type_init();
#endif
    return adoptRef(new WebViewNix(context, pageGroup));
}

WebViewNix::WebViewNix(WebContext* context, WebPageGroup* pageGroup)
    : WebView(context, pageGroup)
    , m_activeContextMenu(WebContextMenuProxyNix::create())
{
}

void WebViewNix::setNixViewClient(const NIXViewClient* viewClient)
{
    m_viewClientNix.initialize(viewClient);
}

void WebViewNix::sendMouseEvent(const NIXMouseEvent& event)
{
    page()->handleMouseEvent(NativeWebMouseEvent(event, &m_lastCursorPosition));
}

void WebViewNix::sendTouchEvent(const NIXTouchEvent& event)
{
    page()->handleTouchEvent(NativeWebTouchEvent(event));
}

void WebViewNix::sendWheelEvent(const NIXWheelEvent& event)
{
    page()->handleWheelEvent(NativeWebWheelEvent(event));
}

void WebViewNix::sendKeyEvent(const NIXKeyEvent& event)
{
    page()->handleKeyboardEvent(NativeWebKeyboardEvent(event));
}

void WebViewNix::sendGestureEvent(const NIXGestureEvent& event)
{
    page()->handleGestureEvent(NativeWebGestureEvent(event));
}

void WebViewNix::findZoomableAreaForPoint(const WKPoint& point, int horizontalRadius, int verticalRadius)
{
    IntPoint contentsPoint(point.x, point.y);
    IntSize touchSize(horizontalRadius * 2, verticalRadius * 2);
    page()->findZoomableAreaForPoint(contentsPoint, touchSize);
}

void WebViewNix::didRelaunchProcess()
{
    coordinatedGraphicsScene()->setActive(true);
    m_client.webProcessDidRelaunch(this);
}

void WebViewNix::didChangeContentScaleFactor(float scaleFactor)
{
    float effectiveScale = scaleFactor * page()->deviceScaleFactor();

    FloatPoint position = contentPosition();
    position.scale(effectiveScale, effectiveScale);
    page()->scalePage(effectiveScale, roundedIntPoint(position));

    updateViewportSize();
}

void WebViewNix::didChangeContentPosition(const WebCore::FloatPoint& trajectoryVector)
{
    DrawingAreaProxy* drawingArea = page()->drawingArea();
    if (!drawingArea)
        return;
    FloatRect visibleContentsRect(contentPosition(), visibleContentsSize());
    visibleContentsRect.intersect(FloatRect(FloatPoint(), contentsSize()));
    drawingArea->setVisibleContentsRect(visibleContentsRect, trajectoryVector);
}

void WebViewNix::didFindZoomableArea(const IntPoint& target, const IntRect& area)
{
    m_viewClientNix.didFindZoomableArea(this, toAPI(target), toAPI(area));
}

void WebViewNix::pageTransitionViewportReady()
{
    page()->commitPageTransitionViewport(); // FIXME: is this correct/needed?
    m_client.didCompletePageTransition(this); // FIXME: is this correct/needed?
}

#if ENABLE(TOUCH_EVENTS)
void WebViewNix::doneWithTouchEvent(const NativeWebTouchEvent& event, bool wasEventHandled)
{
    m_viewClientNix.doneWithTouchEvent(this, *event.nativeEvent(), wasEventHandled);
}
#endif

#if ENABLE(GESTURE_EVENTS)
void WebViewNix::doneWithGestureEvent(const NativeWebGestureEvent& event, bool wasEventHandled)
{
    m_viewClientNix.doneWithGestureEvent(this, *event.nativeEvent(), wasEventHandled);
}
#endif

void WebViewNix::updateTextInputState()
{
    const EditorState& editor = page()->editorState();
    bool isContentEditable = editor.isContentEditable;
    const IntRect& cursorRect = editor.cursorRect;
    const IntRect& editorRect = editor.editorRect;
    m_viewClientNix.updateTextInputState(this, editor.selectedText, editor.surroundingText, editor.inputMethodHints,
                                         editor.isContentEditable, editor.cursorRect, editor.editorRect);
}


PassRefPtr<WebPopupMenuProxy> WebViewNix::createPopupMenuProxy(WebPageProxy* page)
{
    return WebPopupMenuListenerNix::create(page);
}

} // namespace WebKit
