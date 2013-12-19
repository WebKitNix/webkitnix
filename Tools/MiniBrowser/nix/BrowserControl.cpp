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
#include <string.h>

BrowserControl::BrowserControl(BrowserControlClient * client, int width, int height, std::string url)
    : m_client(client)
    , m_display(0)
    , m_context(0)
    , m_eventSource(0)
    , m_im(0)
    , m_ic(0)
    , m_lastClickTime(0.)
    , m_lastClickX(-1)
    , m_lastClickY(-1)
    , m_lastClickButton(kWKEventMouseButtonNoButton)
    , m_clickCount(0)
    , m_popupMessage(0)
    , m_popupMenu(0)
{
    init();

    m_eventSource = new XlibEventSource(m_display, this);

    // create the main window
    m_browserWindow = new BrowserWindow(m_display, DefaultRootWindow(m_display), m_context, this, WKRectMake(0, 0, width, height));

    // separate the main window
    m_toolBar = new ToolBar(m_display, m_browserWindow->window(), m_context, this, WKRectMake(0, 0, width, toolBarHeight));
    m_webView = new WebView(m_display, m_browserWindow->window(), m_context, this, WKRectMake(0, toolBarHeight, width, height - toolBarHeight));

    // create navigation buttons
    m_backButton = createXButton(backButtonX, toolBarElementY, "previous.png", std::bind(&BrowserControl::pageGoBack, this));
    m_forwardButton = createXButton(forwardButtonX, toolBarElementY, "next.png", std::bind(&BrowserControl::pageGoForward, this));
    m_refreshButton = createXButton(refreshButtonX, toolBarElementY, "refresh.png", std::bind(&BrowserControl::pageReload, this));

    // create a simple urlbar
    m_urlBar = new UrlBar(m_display, m_toolBar->window(), m_context, this, WKRectMake(urlBarX, toolBarElementY, width - urlBarRightOffset, urlBarHeight), url);

    m_popupMessage = XInternAtom(m_display, "MenuEntry", False);

    m_tooltipInfo.tooltip = new Tooltip(m_display, m_webView->window(), m_context, this, WKRectMake(0, 0, 0, 0));

    createInputMethodAndInputContext();
}

Button* BrowserControl::createXButton(int x, int y, const char* imageName, ButtonFunction callback)
{
    WKRect buttonRect = WKRectMake(x, y, buttonWidth, buttonHeight);
    std::string imagePath(MINIBROWSER_ICON_DIR);
    imagePath.append(imageName);

    Button* button = new Button(m_display, m_toolBar->window(), m_context, this, buttonRect, imagePath.c_str(), callback);
    return button;
}

BrowserControl::~BrowserControl()
{
    delete m_eventSource;

    delete m_backButton;
    delete m_forwardButton;
    delete m_refreshButton;
    delete m_urlBar;
    delete m_toolBar;
    delete m_tooltipInfo.tooltip;
    delete m_browserWindow;

    if (m_ic)
        XDestroyIC(m_ic);

    if (m_im)
        XCloseIM(m_im);

    if (m_display)
        XCloseDisplay(m_display);
}

void BrowserControl::init()
{
    char* loc = setlocale(LC_ALL, "");
    if (!loc)
        fprintf(stderr, "Could not use the the default environment locale\n");

    if (!XSupportsLocale())
        fprintf(stderr, "Default locale \"%s\" is no supported\n", loc ? loc : "");

    // When changing the locale being used we must call XSetLocaleModifiers (refer to manpage).
    if (!XSetLocaleModifiers(""))
        fprintf(stderr, "Could not set locale modifiers for locale \"%s\"\n", loc ? loc : "");

    m_display = XOpenDisplay(0);
    if (!m_display)
        fatalError("couldn't connect to X server\n");

    m_context = XUniqueContext();
}

void BrowserControl::createInputMethodAndInputContext()
{
    m_im = XOpenIM(m_display, 0, 0, 0);
    if (!m_im)
        fprintf(stderr, "Could not open input method\n");

    m_ic = XCreateIC(m_im, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, m_browserWindow->window(), 0);
    if (!m_ic)
        fprintf(stderr, "Could not open input context\n");
}

void BrowserControl::handleXEvent(const XEvent& event)
{
    VisualComponent *component = 0;
    XFindContext(event.xany.display, event.xany.window, m_context, (XPointer*)&component);

    // Send event to the appropriate component that will be handle
    if (component)
        component->handleEvent(event);
    else
        defaultEventHandler(event);
}

void BrowserControl::defaultEventHandler(const XEvent& event)
{
    if (event.type == ClientMessage) {
        if (!strcmp(XGetAtomName(m_display, event.xclient.message_type), XGetAtomName(m_display, m_popupMessage)))
            removePopupMenu(event.xclient.data.s[0]);
    }
}

void BrowserControl::removePopupMenu(int itemValue)
{
    if (!m_popupMenu)
        return;

    m_client->setPopupItem(itemValue);
    delete m_popupMenu;
    m_popupMenu = 0;
}

void BrowserControl::createPopupMenu(WKRect& rect, std::vector<std::string> *menuitems)
{
    m_popupMenu = new PopupMenu(m_display, m_webView->window(), m_context, this, WKRectMake(rect.origin.x, rect.origin.y + rect.size.height, rect.size.width, rect.size.height), menuitems);
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
    removePopupMenu();
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
    memset(&nixEvent, 0, sizeof(NIXKeyEvent));
    nixEvent.type = (event->type == KeyPress) ? kNIXInputEventTypeKeyDown : kNIXInputEventTypeKeyUp;
    nixEvent.modifiers = convertXEventModifiersToNativeModifiers(event->state);
    nixEvent.timestamp = convertXEventTimeToNixTimestamp(event->time);
    nixEvent.shouldUseUpperCase = useUpperCase;
    nixEvent.isKeypad = isKeypadKeysym(symbol);
    nixEvent.key = convertXKeySymToNativeKeycode(symbol);
    return nixEvent;
}

void BrowserControl::sendKeyboardEventToNix(const XEvent& event)
{
    if (XFilterEvent(const_cast<XEvent*>(&event), m_browserWindow->window()))
        return;

    NIXKeyEvent ev;
    char buf[20];
    bool shouldUseUpperCase;
    const XKeyEvent* keyEvent = reinterpret_cast<const XKeyEvent*>(&event);
    int count = 0;
    KeySym keySym = 0;
    Status status;

    keySym = chooseSymbolForXKeyEvent(keyEvent, &shouldUseUpperCase);
    ev = convertXKeyEventToNixKeyEvent(keyEvent, keySym, shouldUseUpperCase);

    count = Xutf8LookupString(m_ic, const_cast<XKeyEvent*>(keyEvent), buf, 20, &keySym, &status);
    if (count) {
        buf[count] = '\0';
        ev.text = buf;
    }

    if (ev.type == kNIXInputEventTypeKeyDown)
        m_client->handleKeyPress(&ev);
    else
        m_client->handleKeyRelease(&ev);
}

void BrowserControl::handleKeyPressEvent(const XEvent& event)
{
    if (m_urlBar->focused())
        m_urlBar->handleKeyPressEvent(reinterpret_cast<const XKeyPressedEvent&>(event));
    else
        sendKeyboardEventToNix(event);
}

void BrowserControl::handleKeyReleaseEvent(const XEvent& event)
{
    sendKeyboardEventToNix(event);
}

void BrowserControl::updateClickCount(const XButtonPressedEvent& event)
{
    static const double doubleClickInterval = 300;

    if (m_lastClickX != event.x
        || m_lastClickY != event.y
        || m_lastClickButton != convertXEventButtonToNativeMouseButton(event.button)
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
    hideTooltip();
    removePopupMenu();
    passFocusToWebView();

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
    ev.button = m_lastClickButton;
    ev.x = event.x;
    ev.y = event.y;
    ev.globalX = event.x_root;
    ev.globalY = event.y_root;
    ev.clickCount = 0;
    ev.modifiers = convertXEventModifiersToNativeModifiers(event.state);
    ev.timestamp = convertXEventTimeToNixTimestamp(event.time);
    m_client->handleMouseMove(&ev);

    hideTooltip();
    updateTooltipIfNeeded(event);
}

void BrowserControl::handleSizeChanged(int width, int height)
{
    m_urlBar->resize(((width - urlBarRightOffset) > minimumUrlBarWidth) ? width - urlBarRightOffset : minimumUrlBarWidth, urlBarHeight);
    m_toolBar->resize(width, toolBarHeight);
    m_webView->resize(width, height - toolBarHeight);

    m_client->onWindowSizeChange(WKSizeMake(width, height - toolBarHeight));
}

void BrowserControl::handleClose()
{
    m_client->onWindowClose();
}

void BrowserControl::setWebViewCursor(unsigned shape)
{
    m_webView->setCursor(shape);
}

void BrowserControl::handleTooltipChange(const std::string& text)
{
    m_tooltipInfo.text = text;
    hideTooltip();

    if (!m_tooltipInfo.text.empty())
        restartTooltipTimer();
}

void BrowserControl::updateTooltipIfNeeded(const XPointerMovedEvent& event)
{
    if (m_tooltipInfo.text.empty())
        return;

    m_tooltipInfo.mouseX = event.x;
    m_tooltipInfo.mouseY = event.y;

    restartTooltipTimer();
}

void BrowserControl::showTooltip()
{
    m_tooltipInfo.tooltip->showTooltip(m_tooltipInfo.mouseX, m_tooltipInfo.mouseY, m_tooltipInfo.text);
}

void BrowserControl::resetTooltip()
{
    m_tooltipInfo.text.erase();
    hideTooltip();
}

void BrowserControl::hideTooltip()
{
    m_tooltipInfo.tooltip->hideTooltip();
    stopTooltipTimer();
}

static gboolean timeoutCallback(BrowserControl* control)
{
    control->showTooltip();
    return FALSE;
}

void BrowserControl::restartTooltipTimer()
{
    stopTooltipTimer();
    m_tooltipInfo.timer = g_timeout_add_seconds(1, reinterpret_cast<GSourceFunc>(timeoutCallback), this);
}

void BrowserControl::stopTooltipTimer()
{
    if (!m_tooltipInfo.timer)
        return;

    gboolean removedSource = g_source_remove(m_tooltipInfo.timer);
    m_tooltipInfo.timer = 0;
}
