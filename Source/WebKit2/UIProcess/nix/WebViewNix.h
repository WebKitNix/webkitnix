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

#ifndef WebView_h
#define WebView_h

#include "NIXView.h"
#include "NIXEvents.h"
#include "NotImplemented.h"
#include "PageClient.h"
#include "TextureMapperPlatformLayer.h"
#include "WebContextMenuProxy.h"
#include "WebContextMenuProxyNix.h"
#include "WebViewClientNix.h"
#include "WebPopupItemPlatform.h"
#include "WebPopupMenuListener.h"
#include <WebCore/TransformationMatrix.h>

namespace WebCore {
class CoordinatedGraphicsScene;
}

namespace WebKit {

class WebView : public PageClient {
public:
    WebView(WebContext* context, WebPageGroup* pageGroup);
    virtual ~WebView() { }

    void setViewClient(const NIXViewClient* viewClient);

    void initialize();

    WKSize size() const { return WKSizeMake(m_size.width(), m_size.height()); }
    void setSize(const WKSize& size);

    WKPoint scrollPosition() const { return WKPointMake(m_scrollPosition.x(), m_scrollPosition.y()); }
    void setScrollPosition(const WKPoint& position);

    void setUserViewportTransformation(const WebCore::TransformationMatrix& userViewportTransformation) { m_userViewportTransformation = userViewportTransformation; }
    WKPoint userViewportToContents(WKPoint point);

    bool isFocused() const;
    void setFocused(bool);

    bool isVisible() const;
    void setVisible(bool);

    bool isActive() const;
    void setActive(bool);

    void setTransparentBackground(bool);
    bool transparentBackground() const;

    void setDrawBackground(bool);
    bool drawBackground() const;

    void setScale(float);
    float scale() const { return m_scale; }

    WKSize visibleContentsSize() const { return WKSizeMake(m_size.width() / m_scale, m_size.height() / m_scale); }

    void setOpacity(float opacity) { m_opacity = clampTo(opacity, 0.f, 1.f); }
    float opacity() const { return m_opacity; }

    void paintToCurrentGLContext();

    void commitViewportChanges();

    void findZoomableAreaForPoint(const WKPoint& point, int horizontalRadius, int verticalRadius);

    WKPageRef pageRef();

    void sendMouseEvent(const NIXMouseEvent&);
    void sendWheelEvent(const NIXWheelEvent&);
    void sendKeyEvent(const NIXKeyEvent&);
    void sendTouchEvent(const NIXTouchEvent&);
    void sendGestureEvent(const NIXGestureEvent&);

    // PageClient.
    virtual PassOwnPtr<DrawingAreaProxy> createDrawingAreaProxy();
    virtual void setViewNeedsDisplay(const WebCore::IntRect&);

    virtual bool isViewFocused() { return m_focused; }
    virtual bool isViewVisible() { return m_visible; }
    virtual bool isViewWindowActive() { return m_active; }
    virtual bool isViewInWindow() { return true; } // FIXME
    virtual WebCore::IntSize viewSize() { return m_size; }
    virtual void processDidCrash();
    virtual void didRelaunchProcess();

    virtual void pageDidRequestScroll(const WebCore::IntPoint& point);
    virtual void didChangeContentsSize(const WebCore::IntSize& size);
    virtual void didChangeViewportProperties(const WebCore::ViewportAttributes& viewportAttributes);
    virtual void didFindZoomableArea(const WebCore::IntPoint& target, const WebCore::IntRect& area);

    virtual void pageTransitionViewportReady();

    // PageClient not implemented.
    virtual void displayView() { notImplemented(); }
    virtual bool canScrollView() { notImplemented(); return false; }
    virtual void scrollView(const WebCore::IntRect&, const WebCore::IntSize&) { notImplemented(); }

    virtual void pageClosed() { notImplemented(); }

    virtual void toolTipChanged(const String&, const String&) { notImplemented(); }

    virtual void setCursor(const WebCore::Cursor&) { notImplemented(); }
    virtual void setCursorHiddenUntilMouseMoves(bool) { notImplemented(); }

    virtual void registerEditCommand(PassRefPtr<WebEditCommandProxy>, WebPageProxy::UndoOrRedo) { notImplemented(); }
    virtual void clearAllEditCommands() { notImplemented(); }
    virtual bool canUndoRedo(WebPageProxy::UndoOrRedo) { notImplemented(); return false; }
    virtual void executeUndoRedo(WebPageProxy::UndoOrRedo) { notImplemented(); }

    virtual WebCore::FloatRect convertToDeviceSpace(const WebCore::FloatRect& rect) { notImplemented(); return rect; }
    virtual WebCore::FloatRect convertToUserSpace(const WebCore::FloatRect& rect) { notImplemented(); return rect; }
    virtual WebCore::IntPoint screenToWindow(const WebCore::IntPoint& point) { notImplemented(); return point; }
    virtual WebCore::IntRect windowToScreen(const WebCore::IntRect& rect) { notImplemented(); return rect; }

    virtual void doneWithKeyEvent(const NativeWebKeyboardEvent&, bool) { notImplemented(); }
#if ENABLE(GESTURE_EVENTS)
    virtual void doneWithGestureEvent(const NativeWebGestureEvent&, bool wasEventHandled);
#endif
#if ENABLE(TOUCH_EVENTS)
    virtual void doneWithTouchEvent(const NativeWebTouchEvent&, bool wasEventHandled);
#endif

    virtual PassRefPtr<WebPopupMenuProxy> createPopupMenuProxy(WebPageProxy* page) { return WebPopupMenuListener::create(page); }
    virtual PassRefPtr<WebContextMenuProxy> createContextMenuProxy(WebPageProxy*) { return m_activeContextMenu; }

#if ENABLE(INPUT_TYPE_COLOR)
    virtual PassRefPtr<WebColorChooserProxy> createColorChooserProxy(WebPageProxy*, const Color& initialColor) { notImplemented(); return PassRefPtr<WebColorChooserProxy>(); }
#endif

    virtual void setFindIndicator(PassRefPtr<FindIndicator>, bool, bool)  { notImplemented(); }

#if USE(ACCELERATED_COMPOSITING)
    virtual void enterAcceleratedCompositingMode(const LayerTreeContext&) { notImplemented(); }
    virtual void exitAcceleratedCompositingMode() { notImplemented(); }
    virtual void updateAcceleratedCompositingMode(const LayerTreeContext&) { notImplemented(); }
#endif

    virtual void didChangeScrollbarsForMainFrame() const { notImplemented(); }

    virtual void didCommitLoadForMainFrame(bool) { notImplemented(); }
    virtual void didFinishLoadingDataForCustomRepresentation(const String&, const CoreIPC::DataReference&) { notImplemented(); }
    virtual double customRepresentationZoomFactor() { notImplemented(); return 1.0; }
    virtual void setCustomRepresentationZoomFactor(double) { notImplemented(); }

    virtual void flashBackingStoreUpdates(const Vector<WebCore::IntRect>&) { notImplemented(); }
    virtual void findStringInCustomRepresentation(const String&, WebKit::FindOptions, unsigned) { notImplemented(); }
    virtual void countStringMatchesInCustomRepresentation(const String&, WebKit::FindOptions, unsigned) { notImplemented(); }

    virtual void updateTextInputState();
    virtual void didRenderFrame(const WebCore::IntSize&, const WebCore::IntRect&) { notImplemented(); }

    virtual void suspendActiveDOMObjectsAndAnimations();
    virtual void resumeActiveDOMObjectsAndAnimations();
    virtual bool isSuspended();
    WebCore::CoordinatedGraphicsScene* coordinatedGraphicsScene();

private:
    WebCore::IntPoint roundedViewportPosition() const;
    void updateVisibleContents();

    WebCore::TransformationMatrix userViewportToContentTransformation() const;
    WebCore::TransformationMatrix contentToUserViewportTransformation() const;

    WebCore::FloatRect visibleRect() const;

    WebViewClient m_viewClient;
    WTF::RefPtr<WebPageProxy> m_webPageProxy;
    bool m_focused;
    bool m_visible;
    bool m_active;
    bool m_isSuspended;
    WebCore::IntSize m_size;
    WebCore::IntSize m_contentsSize;
    WebCore::IntPoint m_lastCursorPosition;
    WebCore::FloatPoint m_scrollPosition;
    float m_scale;
    float m_opacity;
    WebCore::TransformationMatrix m_userViewportTransformation;
    WTF::RefPtr<WebContextMenuProxyNix> m_activeContextMenu;
};

inline WebKit::WebView* toImpl(NIXView view)
{
    return reinterpret_cast<WebKit::WebView*>(view);
}

inline NIXView toAPI(WebKit::WebView* view)
{
    return reinterpret_cast<NIXView>(view);
}

} // namespace WebKit

#endif // WebView_h
