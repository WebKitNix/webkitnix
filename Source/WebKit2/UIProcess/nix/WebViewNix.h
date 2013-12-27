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

#ifndef WebViewNix_h
#define WebViewNix_h

#include "NIXView.h"
#include "NIXEvents.h"
#include "TextureMapperPlatformLayer.h"
#include "WebContextMenuProxyNix.h"
#include "WebView.h"
#include "WebViewClientNix.h"
#include "WebPopupItemPlatform.h"
#include "WebPopupMenuListener.h"
#include <WebCore/TransformationMatrix.h>
#include <WebCore/Cursor.h>

namespace WebCore {
class CoordinatedGraphicsScene;
}

namespace WebKit {

class WebViewNix : public WebView {
public:
    void setNixViewClient(const NIXViewClientBase* viewClient);

    void findZoomableAreaForPoint(const WKPoint& point, int horizontalRadius, int verticalRadius);

    void sendMouseEvent(const NIXMouseEvent&);
    void sendWheelEvent(const NIXWheelEvent&);
    void sendKeyEvent(const NIXKeyEvent&);
    void sendTouchEvent(const NIXTouchEvent&);
    void sendGestureEvent(const NIXGestureEvent&);

    bool isSuspended() { return page()->areActiveDOMObjectsAndAnimationsSuspended(); }
    void viewportInteractionStart();
    void viewportInteractionStop();

    bool pendingScaleOrPositionChange() const { return m_pendingScaleOrPositionChange; }

    virtual void setContentScaleFactor(float) OVERRIDE;
    virtual float contentScaleFactor() const OVERRIDE;

    float scaleToFitContents();
    void adjustScaleToFitContents();
    void setScreenRect(const WebCore::FloatRect&);
protected:
    // WebKit::WebView
    virtual void didChangeContentPosition(const WebCore::FloatPoint&) OVERRIDE;
    virtual void pageDidRequestScroll(const WebCore::IntPoint&) OVERRIDE;
    virtual void didRenderFrame(const WebCore::IntSize&, const WebCore::IntRect&) OVERRIDE;

    // PageClient.
    virtual void didFindZoomableArea(const WebCore::IntPoint& target, const WebCore::IntRect& area) OVERRIDE;
    virtual void didCommitLoadForMainFrame() OVERRIDE;
    virtual void notifyLoadIsBackForward() OVERRIDE;
    virtual void didStartedMainFrameLayout() OVERRIDE;
    virtual void pageScaleFactorDidChange() OVERRIDE;

#if ENABLE(TOUCH_EVENTS)
    virtual void doneWithTouchEvent(const NativeWebTouchEvent&, bool wasEventHandled) OVERRIDE;
#endif
    virtual void updateTextInputState() OVERRIDE;
    virtual void didRelaunchProcess() OVERRIDE;
    virtual void pageTransitionViewportReady() OVERRIDE;
    virtual PassRefPtr<WebContextMenuProxy> createContextMenuProxy(WebPageProxy*) OVERRIDE { return m_activeContextMenu; }
    virtual PassRefPtr<WebPopupMenuProxy> createPopupMenuProxy(WebPageProxy*) OVERRIDE;

    float deviceScaleFactor() { return m_page->deviceScaleFactor(); }
private:
    WebViewNix(WebContext* context, WebPageGroup* pageGroup);

    void setCursor(const WebCore::Cursor&) OVERRIDE;

    WebViewClientNix m_viewClientNix;
    WebCore::IntPoint m_lastCursorPosition;
    WTF::RefPtr<WebContextMenuProxyNix> m_activeContextMenu;

    float m_scaleFactorToSync;
    bool m_duringFrameRendering;
    bool m_pendingScaleOrPositionChange;
    WebCore::FloatPoint m_contentPositionAfterTransition;
    double m_scaleAfterTransition;
    bool m_loadIsBackForward;

    friend class WebView;
};

} // namespace WebKit

#endif // WebViewNix_h
