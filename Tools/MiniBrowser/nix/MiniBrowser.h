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

#ifndef MiniBrowser_h
#define MiniBrowser_h

#include "GestureRecognizer.h"
#include "NIXView.h"
#include "BrowserControl.h"
#include <glib.h>

#include "Options.h"
#include "WebKit2/WKRetainPtr.h"
#include "WebKit2/WKPage.h"
#include "WebKit2/WKURL.h"

class TouchMocker;

class MiniBrowser : public BrowserControlClient, public GestureRecognizerClient {
public:
    MiniBrowser(GMainLoop* mainLoop, const Options& options);
    virtual ~MiniBrowser();

    WKPageRef pageRef() const { return WKViewGetPage(m_view); }

    // WebViewClient.
    virtual void handleWindowExpose() { scheduleUpdateDisplay(); }
    virtual void handleKeyPress(NIXKeyEvent*);
    virtual void handleKeyRelease(NIXKeyEvent*);
    virtual void handleMousePress(NIXMouseEvent*);
    virtual void handleMouseRelease(NIXMouseEvent*);
    virtual void handleMouseMove(NIXMouseEvent*);
    virtual void handleMouseWheel(NIXWheelEvent*);

    virtual void onWindowSizeChange(WKSize);
    virtual void onWindowClose();

    virtual void pageGoBack() { WKPageGoBack(pageRef()); }
    virtual void pageGoForward() { WKPageGoForward(pageRef()); }
    virtual void pageReload() { WKPageReload(pageRef()); }
    virtual void loadPage(const char* url) { WKPageLoadURL(pageRef(), WKURLCreateWithUTF8CString(url)); }
    virtual void addFocus() { WKViewSetIsFocused(m_view, true); }
    virtual void releaseFocus() { WKViewSetIsFocused(m_view, false); }
    virtual std::string activeUrl();

    // ViewsClient.
    static void viewNeedsDisplay(WKViewRef, WKRect area, const void* clientInfo);
    static void webProcessCrashed(WKViewRef, WKURLRef url, const void* clientInfo);
    static void webProcessRelaunched(WKViewRef, const void* clientInfo);
    static void pageDidRequestScroll(WKViewRef, WKPoint position, const void* clientInfo);
    static void didChangeContentsSize(WKViewRef, WKSize size, const void* clientInfo);
    static void didChangeViewportAttributes(WKViewRef view, WKViewportAttributesRef attributes, const void* clientInfo);
    static void didFindZoomableArea(WKViewRef, WKPoint target, WKRect area, const void* clientInfo);
    static void doneWithTouchEvent(WKViewRef, const NIXTouchEvent* event, bool wasEventHandled, const void* clientInfo);
    static void doneWithGestureEvent(WKViewRef, const NIXGestureEvent* event, bool wasEventHandled, const void* clientInfo);
    static void updateTextInputState(WKViewRef, const NIXTextInputState* state, const void* clientInfo);

    // GestureRecognizerClient.
    virtual void handleSingleTap(double timestamp, const NIXTouchPoint&);
    virtual void handleDoubleTap(double timestamp, const NIXTouchPoint&);
    virtual void handleLongTap(double timestamp, const NIXTouchPoint&);
    virtual void handlePanningStarted(double timestamp);
    virtual void handlePanning(double timestamp, WKPoint delta);
    virtual void handlePanningFinished(double timestamp);
    virtual void handlePinchStarted(double timestamp);
    virtual void handlePinch(double timestamp, WKPoint delta, double scale, WKPoint contentCenter);
    virtual void handlePinchFinished(double timestamp);

    // ContextMenuClient.
    static void showContextMenu(WKPageRef page, WKPoint menuLocation, WKArrayRef menuItems, const void* clientInfo);

    // PopupMenuClient.
    static void showPopupMenu(WKPageRef page, WKPopupMenuListenerRef menuListenerRef, WKRect rect, WKPopupItemTextDirection textDirection, double pageScaleFactor, WKArrayRef itemsRef, int32_t selectedIndex, const void* clientInfo);

    // UIClient.
    static void runJavaScriptAlert(WKPageRef page, WKStringRef message, WKFrameRef frame, const void* clientInfo);
    static bool runJavaScriptConfirm(WKPageRef page, WKStringRef message, WKFrameRef frame, const void* clientInfo);
    static WKStringRef runJavaScriptPrompt(WKPageRef page, WKStringRef message, WKStringRef defaultValue, WKFrameRef frame, const void* clientInfo);

    // PageLoaderClient.
    static void didStartProgress(WKPageRef page, const void *clientInfo);
    static void didChangeProgress(WKPageRef page, const void* clientInfo);
    static void didReceiveTitleForFrame(WKPageRef, WKStringRef, WKFrameRef, WKTypeRef, const void*);
    static void didStartProvisionalLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void*);
    static void didFinishDocumentLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void*);
    static void didFailProvisionalLoadWithErrorForFrame(WKPageRef, WKFrameRef, WKErrorRef, WKTypeRef, const void*);
    static void didFirstLayoutForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void*);

    virtual double scale();

    void setTouchEmulationMode(bool enabled);
    bool isMobileMode() const { return !m_options.desktopModeEnabled; }
private:

    enum ScaleBehavior {
        AdjustToBoundaries = 1 << 0,
        LowerMinimumScale = 1 << 1
    };

    void updateDisplay();
    void scheduleUpdateDisplay();
    WKPoint adjustScrollPositionToBoundaries(WKPoint position);
    void adjustScrollPosition();
    void adjustViewportToTextInputArea();
    double scaleToFitContents();

    void scaleAtPoint(const WKPoint& point, double scale, ScaleBehavior scaleBehavior = AdjustToBoundaries);

    WKViewRef webViewAtX11Position(const WKPoint& poisition);
    void saveSnapshot(double timestamp);

    void updateActiveUrlText();

    WKRetainPtr<WKContextRef> m_context;
    WKRetainPtr<WKPageGroupRef> m_pageGroup;
    BrowserControl* m_control;
    WKViewRef m_view;
    WKRect m_viewRect;
    GMainLoop* m_mainLoop;
    const Options& m_options;
    TouchMocker* m_touchMocker;
    bool m_displayUpdateScheduled;
    WKSize m_contentsSize;
    GestureRecognizer m_gestureRecognizer;
    bool m_postponeTextInputUpdates;
    bool m_shouldFocusEditableArea;
    WKRect m_cursorRect;
    WKRect m_editorRect;
    bool m_shouldRestoreViewportWhenLosingFocus;
    double m_scaleBeforeFocus;
    WKPoint m_scrollPositionBeforeFocus;

    float m_viewportWidth;
    float m_viewportHeight;
    float m_viewportMinScale;
    float m_viewportMaxScale;
    float m_viewportInitScale;
    bool m_viewportUserScalable;

    std::string m_activeUrlText;

    static gboolean callUpdateDisplay(gpointer);
};

#endif
