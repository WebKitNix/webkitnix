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

    virtual void setContentScaleFactor(float) override;
    virtual float contentScaleFactor() const override;

    float scaleToFitContents();
    void adjustScaleToFitContents();
    void setScreenRect(const WebCore::FloatRect&);
protected:
    // WebKit::WebView
    virtual void didChangeContentPosition(const WebCore::FloatPoint&) override;

    // PageClient.
    virtual void didFindZoomableArea(const WebCore::IntPoint& target, const WebCore::IntRect& area) override;
    virtual void didCommitLoadForMainFrame(const String& mimeType, bool useCustomContentProvider) override;
    virtual void pageScaleFactorDidChange() override;
    virtual void didFinishLoadingDataForCustomContentProvider(const String& suggestedFilename, const IPC::DataReference&) { UNUSED_PARAM(suggestedFilename); }

#if ENABLE(TOUCH_EVENTS)
    virtual void doneWithTouchEvent(const NativeWebTouchEvent&, bool wasEventHandled) override;
#endif
    virtual void updateTextInputState() override;
    virtual void didRelaunchProcess() override;
    virtual void pageTransitionViewportReady() override;
    virtual PassRefPtr<WebContextMenuProxy> createContextMenuProxy(WebPageProxy*) override { return m_activeContextMenu; }
    virtual PassRefPtr<WebPopupMenuProxy> createPopupMenuProxy(WebPageProxy*) override;

    float deviceScaleFactor() { return m_page->deviceScaleFactor(); }
private:
    WebViewNix(WebContext* context, WebPageGroup* pageGroup);

    void setCursor(const WebCore::Cursor&) override;

    WebViewClientNix m_viewClientNix;
    WebCore::IntPoint m_lastCursorPosition;
    WTF::RefPtr<WebContextMenuProxyNix> m_activeContextMenu;

    float m_scaleFactorToSync;

    friend class WebView;
};

} // namespace WebKit

#endif // WebViewNix_h
