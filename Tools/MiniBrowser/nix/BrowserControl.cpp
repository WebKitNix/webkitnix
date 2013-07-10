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

#include "BrowserControl.h"
#include "XlibEventUtils.h"

BrowserControl::BrowserControl(BrowserControlClient * client, int width, int height, std::string url)
    : m_client(client)
    , m_display(XOpenDisplay(0))
    , m_context(XUniqueContext())
    , m_eventSource(0)
{
    if (!m_display)
        fatalError("couldn't connect to X server\n");

    m_eventSource = new XlibEventSource(m_display, this);

    // create the main window
    m_browserWindow = new BrowserWindow(m_display, DefaultRootWindow(m_display), m_context, this, WKRectMake(0, 0, width, height));

    // separate the main window
    m_toolBar = new ToolBar(m_display, m_browserWindow->window(), m_context, this, WKRectMake(0, 0, width, toolBarHeight));
    m_webView = new WebView(m_display, m_browserWindow->window(), m_context, this, WKRectMake(0, toolBarHeight, width, height - toolBarHeight));

    // create navigation buttons
    m_backButton = new Button(m_display, m_toolBar->window(), m_context, this, WKRectMake(backButtonX, toolBarElementY, buttonWidth, buttonHeight), Button::Back);
    m_forwardButton = new Button(m_display, m_toolBar->window(), m_context, this, WKRectMake(forwardButtonX, toolBarElementY, buttonWidth, buttonHeight), Button::Forward);
    m_refreshButton = new Button(m_display, m_toolBar->window(), m_context, this, WKRectMake(refreshButtonX, toolBarElementY, buttonWidth, buttonHeight), Button::Refresh);

    // create a simple urlbar
    m_urlBar = new UrlBar(m_display, m_toolBar->window(), m_context, this, WKRectMake(urlBarX, toolBarElementY, width - urlBarRightOffset, urlBarHeight), url);
}

BrowserControl::~BrowserControl()
{
    delete m_eventSource;

    delete m_backButton;
    delete m_forwardButton;
    delete m_refreshButton;
    delete m_urlBar;
    delete m_toolBar;
    delete m_browserWindow;

    XCloseDisplay(m_display);
}

void BrowserControl::handleXEvent(const XEvent& event)
{
    VisualComponent *component = 0;
    XFindContext(event.xany.display, event.xany.window, m_context, (XPointer*)&component);

    // Send event to the appropriate component that will be handle
    if (component)
        component->handleEvent(event);
}

void BrowserControl::makeCurrent()
{
    m_webView->makeCurrent();
}

void BrowserControl::swapBuffers()
{
    m_webView->swapBuffers();
}

WKSize BrowserControl::windowSize()
{
    return m_browserWindow->size();
}

WKSize BrowserControl::webViewSize()
{
    return m_webView->size();
}

void BrowserControl::setWindowTitle(const char* title)
{
    m_browserWindow->setTitle(title);
}

void BrowserControl::passFocusToWebView()
{
    m_client->addFocus();
    m_urlBar->releaseFocus();
}

void BrowserControl::passFocusToUrlBar()
{
    m_client->releaseFocus();
}

void BrowserControl::updateUrlText(const char* url)
{
    m_urlBar->setText(url);
}

bool BrowserControl::isUrlBarFocused()
{
    return m_urlBar->focused();
}

void BrowserControl::pageGoBack()
{
    m_client->pageGoBack();
    m_urlBar->setText(m_client->activeUrl());
    passFocusToWebView();
}

void BrowserControl::pageGoForward()
{
    m_client->pageGoForward();
    m_urlBar->setText(m_client->activeUrl());
    passFocusToWebView();
}

void BrowserControl::pageReload()
{
    m_client->pageReload();
    m_urlBar->setText(m_client->activeUrl());
    passFocusToWebView();
}

void BrowserControl::loadPage(const char* url)
{
    m_client->loadPage(url);
    m_urlBar->setText(m_client->activeUrl());
    passFocusToWebView();
}

void BrowserControl::setLoadProgress(double progress)
{
    m_urlBar->setLoadProgress(progress);
}

void BrowserControl::handleExposeEvent()
{
    m_client->handleWindowExpose();
}

static inline bool isKeypadKeysym(const KeySym symbol)
{
    // Following keypad symbols are specified on Xlib Programming Manual (section: Keyboard Encoding).
    return (symbol >= 0xFF80 && symbol <= 0xFFBD);
}

static KeySym chooseSymbolForXKeyEvent(const XKeyEvent* event, bool* useUpperCase)
{
    KeySym firstSymbol = XLookupKeysym(const_cast<XKeyEvent*>(event), 0);
    KeySym secondSymbol = XLookupKeysym(const_cast<XKeyEvent*>(event), 1);
    KeySym lowerCaseSymbol, upperCaseSymbol, chosenSymbol;
    XConvertCase(firstSymbol, &lowerCaseSymbol, &upperCaseSymbol);
    bool numLockModifier = event->state & Mod2Mask;
    bool capsLockModifier = event->state & LockMask;
    bool shiftModifier = event->state & ShiftMask;
    if (numLockModifier && isKeypadKeysym(secondSymbol)) {
        chosenSymbol = shiftModifier ? firstSymbol : secondSymbol;
    } else if (lowerCaseSymbol == upperCaseSymbol) {
        chosenSymbol = shiftModifier ? secondSymbol : firstSymbol;
    } else if (shiftModifier == capsLockModifier)
        chosenSymbol = firstSymbol;
    else
        chosenSymbol = secondSymbol;

    *useUpperCase = (lowerCaseSymbol != upperCaseSymbol && chosenSymbol == upperCaseSymbol);
    XConvertCase(chosenSymbol, &lowerCaseSymbol, &upperCaseSymbol);
    return upperCaseSymbol;
}

static NIXKeyEvent convertXKeyEventToNixKeyEvent(const XKeyEvent* event, const KeySym& symbol, bool useUpperCase)
{
    NIXKeyEvent nixEvent;
    nixEvent.type = (event->type == KeyPress) ? kNIXInputEventTypeKeyDown : kNIXInputEventTypeKeyUp;
    nixEvent.modifiers = convertXEventModifiersToNativeModifiers(event->state);
    nixEvent.timestamp = convertXEventTimeToNixTimestamp(event->time);
    nixEvent.shouldUseUpperCase = useUpperCase;
    nixEvent.isKeypad = isKeypadKeysym(symbol);
    nixEvent.key = convertXKeySymToNativeKeycode(symbol);
    return nixEvent;
}

static void XEventToNix(const XEvent& event, NIXKeyEvent* nixEvent)
{
    bool shouldUseUpperCase;
    const XKeyEvent* keyEvent = reinterpret_cast<const XKeyEvent*>(&event);
    KeySym symbol = chooseSymbolForXKeyEvent(keyEvent, &shouldUseUpperCase);
    *nixEvent = convertXKeyEventToNixKeyEvent(keyEvent, symbol, shouldUseUpperCase);
}

void BrowserControl::handleKeyPressEvent(const XEvent& event)
{
    if (!m_urlBar->focused()) {
        NIXKeyEvent ev;
        XEventToNix(event, &ev);
        m_client->handleKeyPress(&ev);
    }
}

void BrowserControl::handleKeyReleaseEvent(const XEvent& event)
{
    if (m_urlBar->focused())
        m_urlBar->handleKeyReleaseEvent(reinterpret_cast<const XKeyReleasedEvent&>(event));
    else {
        NIXKeyEvent ev;
        XEventToNix(event, &ev);
        m_client->handleKeyRelease(&ev);
    }
}

void BrowserControl::updateClickCount(const XButtonPressedEvent& event)
{
    static const double doubleClickInterval = 300;

    if (m_lastClickX != event.x
        || m_lastClickY != event.y
        || m_lastClickButton != event.button
        || event.time - m_lastClickTime >= doubleClickInterval)
        m_clickCount = 1;
    else
        ++m_clickCount;

    m_lastClickX = event.x;
    m_lastClickY = event.y;
    m_lastClickButton = convertXEventButtonToNativeMouseButton(event.button);
    m_lastClickTime = event.time;
}

void BrowserControl::handleButtonPressEvent(const XButtonPressedEvent& event)
{
    if (event.button == 4 || event.button == 5) {
        const float pixelsPerStep = 40.0f;

        NIXWheelEvent ev;
        ev.type = kNIXInputEventTypeWheel;
        ev.modifiers = convertXEventModifiersToNativeModifiers(event.state);
        ev.timestamp = convertXEventTimeToNixTimestamp(event.time);
        ev.x = event.x;
        ev.y = event.y;
        ev.globalX = event.x_root;
        ev.globalY = event.y_root;
        ev.delta = pixelsPerStep * (event.button == 4 ? 1 : -1);
        ev.orientation = event.state & Mod1Mask ? kNIXWheelEventOrientationHorizontal : kNIXWheelEventOrientationVertical;
        m_client->handleMouseWheel(&ev);
        return;
    }
    updateClickCount(event);

    NIXMouseEvent ev;
    ev.type = kNIXInputEventTypeMouseDown;
    ev.button = convertXEventButtonToNativeMouseButton(event.button);
    ev.x = event.x;
    ev.y = event.y;
    ev.globalX = event.x_root;
    ev.globalY = event.y_root;
    ev.clickCount = m_clickCount;
    ev.modifiers = convertXEventModifiersToNativeModifiers(event.state);
    ev.timestamp = convertXEventTimeToNixTimestamp(event.time);
    m_client->handleMousePress(&ev);

    passFocusToWebView();
}

void BrowserControl::handleButtonReleaseEvent(const XButtonReleasedEvent& event)
{
    if (event.button == 4 || event.button == 5)
        return;

    NIXMouseEvent ev;
    ev.type = kNIXInputEventTypeMouseUp;
    ev.button = convertXEventButtonToNativeMouseButton(event.button);
    ev.x = event.x;
    ev.y = event.y;
    ev.globalX = event.x_root;
    ev.globalY = event.y_root;
    ev.clickCount = 0;
    ev.modifiers = convertXEventModifiersToNativeModifiers(event.state);
    ev.timestamp = convertXEventModifiersToNativeModifiers(event.state);

    m_client->handleMouseRelease(&ev);
    passFocusToWebView();
}

void BrowserControl::handlePointerMoveEvent(const XPointerMovedEvent& event)
{
    NIXMouseEvent ev;
    ev.type = kNIXInputEventTypeMouseMove;
    ev.button = kWKEventMouseButtonNoButton;
    ev.x = event.x;
    ev.y = event.y;
    ev.globalX = event.x_root;
    ev.globalY = event.y_root;
    ev.clickCount = 0;
    ev.modifiers = convertXEventModifiersToNativeModifiers(event.state);
    ev.timestamp = convertXEventTimeToNixTimestamp(event.time);
    m_client->handleMouseMove(&ev);
}

void BrowserControl::handleSizeChanged(int width, int height)
{
    m_urlBar->resize(((width - urlBarRightOffset) > minimumUrlBarWidth) ? width - urlBarRightOffset : minimumUrlBarWidth, urlBarHeight);
    m_toolBar->resize(width, toolBarHeight);
    m_webView->resize(width, height);

    m_client->onWindowSizeChange(WKSizeMake(width, height - toolBarHeight));
}

void BrowserControl::handleClose()
{
    m_client->onWindowClose();
}
