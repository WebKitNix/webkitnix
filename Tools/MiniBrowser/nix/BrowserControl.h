/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Roland Takacs <rtakacs@inf.u-szeged.hu>
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

#ifndef BrowserControl_h
#define BrowserControl_h

#include "Components/BrowserWindow.h"
#include "Components/Button.h"
#include "Components/PopupMenu.h"
#include "Components/Tooltip.h"
#include "Components/UrlBar.h"
#include "Components/WebView.h"
#include "NIXEvents.h"
#include "XlibEventSource.h"
#include <glib.h>

typedef struct {
    Tooltip* tooltip;
    guint timer;
    int mouseX;
    int mouseY;
    std::string text;
} TooltipInfo;

class BrowserControlClient {
public:
    virtual ~BrowserControlClient() { }

    virtual void handleWindowExpose() = 0;
    virtual void handleKeyPress(NIXKeyEvent*) = 0;
    virtual void handleKeyRelease(NIXKeyEvent*) = 0;
    virtual void handleMousePress(NIXMouseEvent*) = 0;
    virtual void handleMouseRelease(NIXMouseEvent*) = 0;
    virtual void handleMouseMove(NIXMouseEvent*) = 0;
    virtual void handleMouseWheel(NIXWheelEvent*) = 0;

    virtual void onWindowSizeChange(WKSize) = 0;
    virtual void onWindowClose() = 0;

    virtual void pageGoBack() = 0;
    virtual void pageGoForward() = 0;
    virtual void pageReload() = 0;
    virtual void loadPage(const char*) = 0;
    virtual void addFocus() = 0;
    virtual void releaseFocus() = 0;
    virtual std::string activeUrl() = 0;
    virtual void setPopupItem(int) = 0;
};

class BrowserControl : public XlibEventSource::Client {
public:
    static const int toolBarHeight = 34;
    static const int toolBarElementY = 5;

    static const int buttonWidth = 24;
    static const int buttonHeight = 24;
    static const int backButtonX = 3;
    static const int forwardButtonX = 37;
    static const int refreshButtonX = 71;

    static const int urlBarX = 105;
    static const int urlBarHeight = 22;
    static const int minimumUrlBarWidth = 50;
    static const int urlBarRightOffset = urlBarX + 25;

    BrowserControl(BrowserControlClient*, int width, int height, std::string url);
    ~BrowserControl();

    void makeCurrent();
    void swapBuffers();

    WKSize windowSize();
    WKSize webViewSize();

    void setWindowTitle(const char*);

    void updateUrlText(const char*);
    bool isUrlBarFocused();

    void setWebViewCursor(unsigned);

    void passFocusToWebView();
    void passFocusToUrlBar();

    void pageGoBack();
    void pageGoForward();
    void pageReload();
    void loadPage(const char*);
    void setLoadProgress(double);

    void handleExposeEvent();
    void handleKeyPressEvent(const XEvent&);
    void handleKeyReleaseEvent(const XEvent&);
    void handleButtonPressEvent(const XButtonPressedEvent&);
    void handleButtonReleaseEvent(const XButtonReleasedEvent&);
    void handlePointerMoveEvent(const XPointerMovedEvent&);
    void handleSizeChanged(int, int);
    void handleClose();

    void updateClickCount(const XButtonPressedEvent&);
    unsigned clickCount() { return m_clickCount; }

    void defaultEventHandler(const XEvent&);
    void createPopupMenu(WKRect&, std::vector<std::string>*);
    void removePopupMenu(int itemValue = 0);

    void handleTooltipChange(const std::string&);
    void showTooltip();
    void hideTooltip();
    void resetTooltip();

private:
    void init();
    void sendKeyboardEventToNix(const XEvent&);
    void createInputMethodAndInputContext();
    Button* createXButton(int, int, const char*, ButtonFunction);
    // XlibEventSource::Client.
    virtual void handleXEvent(const XEvent&);

    void restartTooltipTimer();
    void stopTooltipTimer();
    void updateTooltipIfNeeded(const XPointerMovedEvent&);

    BrowserControlClient* m_client;

    Display* m_display;
    XContext m_context;
    XIM m_im;
    XIC m_ic;
    XlibEventSource* m_eventSource;

    double m_lastClickTime;
    int m_lastClickX;
    int m_lastClickY;
    WKEventMouseButton m_lastClickButton;
    unsigned m_clickCount;
    Atom m_popupMessage;

    // Visual components
    BrowserWindow* m_browserWindow;
    ToolBar* m_toolBar;
    Button* m_backButton;
    Button* m_forwardButton;
    Button* m_refreshButton;
    PopupMenu* m_popupMenu;
    UrlBar* m_urlBar;
    WebView* m_webView;

    TooltipInfo m_tooltipInfo;
};

#endif
