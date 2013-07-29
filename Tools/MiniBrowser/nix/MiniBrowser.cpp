#include "config.h"
#include "MiniBrowser.h"

#include "GLUtilities.h"
#include "TouchMocker.h"
#include "WebKit2/WKArray.h"
#include "WebKit2/WKFrame.h"
#include "WebKit2/WKPreferences.h"
#include "WebKit2/WKPreferencesPrivate.h"
#include "WebKit2/WKString.h"
#include "WKPageNix.h"

#include <GL/gl.h>
#include <cassert>
#include <cmath>
#include <cstring>
#include <glib.h>
#include <iostream>
#include <sstream>
#include <string>

#define NOCOLOR "\e[m"
#define RED     "\e[38;05;9m"
#define YELLOW  "\e[38;05;10m"
#define GREEN   "\e[38;05;11m"

extern void glUseProgram(GLuint);

const double EPSILON = 1e-4;

using namespace std;

MiniBrowser::MiniBrowser(GMainLoop* mainLoop, const Options& options)
    : m_control(new BrowserControl(this, options.width, options.height, options.url))
    , m_view(0)
    , m_mainLoop(mainLoop)
    , m_options(options)
    , m_touchMocker(0)
    , m_displayUpdateScheduled(false)
    , m_gestureRecognizer(GestureRecognizer(this))
    , m_postponeTextInputUpdates(true)
    , m_shouldFocusEditableArea(false)
    , m_shouldRestoreViewportWhenLosingFocus(false)
    , m_viewportMinScale(0.25)
    , m_viewportMaxScale(5)
    , m_viewportUserScalable(true)
    , m_viewportInitScale(1)
{
    WKStringRef bundlePath = WKStringCreateWithUTF8CString(options.injectedBundle.empty() ? MINIBROWSER_INJECTEDBUNDLE_DIR "libMiniBrowserInjectedBundle.so" : options.injectedBundle.c_str());
    m_context = adoptWK(WKContextCreateWithInjectedBundlePath(bundlePath));
    WKRelease(bundlePath);

    WKStringRef groupIdentifier = WKStringCreateWithUTF8CString("MiniBrowser");
    m_pageGroup = adoptWK(WKPageGroupCreateWithIdentifier(groupIdentifier));
    WKRelease(groupIdentifier);

    m_control->setWindowTitle("MiniBrowser");
    g_main_loop_ref(m_mainLoop);

    WKPreferencesRef preferences = WKPageGroupGetPreferences(m_pageGroup.get());
    WKPreferencesSetFrameFlatteningEnabled(preferences, true);
    WKPreferencesSetDeveloperExtrasEnabled(preferences, true);
    WKPreferencesSetWebGLEnabled(preferences, true);
    WKPreferencesSetOfflineWebApplicationCacheEnabled(preferences, true);

    m_view = WKViewCreate(m_context.get(), m_pageGroup.get());

    NIXViewClient nixViewClient;
    memset(&nixViewClient, 0, sizeof(NIXViewClient));
    nixViewClient.version = kNIXViewClientCurrentVersion;
    nixViewClient.clientInfo = this;
    nixViewClient.doneWithTouchEvent = MiniBrowser::doneWithTouchEvent;
    nixViewClient.doneWithGestureEvent = MiniBrowser::doneWithGestureEvent;
    nixViewClient.didFindZoomableArea = MiniBrowser::didFindZoomableArea;
    nixViewClient.updateTextInputState = MiniBrowser::updateTextInputState;
    NIXViewSetNixViewClient(m_view, &nixViewClient);

    WKViewClient viewClient;
    memset(&viewClient, 0, sizeof(WKViewClient));
    viewClient.version = kWKViewClientCurrentVersion;
    viewClient.clientInfo = this;
    viewClient.viewNeedsDisplay = MiniBrowser::viewNeedsDisplay;
    viewClient.webProcessCrashed = MiniBrowser::webProcessCrashed;
    viewClient.webProcessDidRelaunch = MiniBrowser::webProcessRelaunched;
    viewClient.didChangeContentsSize = MiniBrowser::didChangeContentsSize;
    viewClient.didChangeContentsPosition = MiniBrowser::pageDidRequestScroll;
    viewClient.didChangeViewportAttributes = MiniBrowser::didChangeViewportAttributes;
    WKViewSetViewClient(m_view, &viewClient);

    WKViewInitialize(m_view);
    WKPageSetCustomBackingScaleFactor(pageRef(), options.devicePixelRatio);

    if (isMobileMode())
        WKPageSetUseFixedLayout(pageRef(), true);

    WKSize size = m_control->webViewSize();
    m_viewRect = WKRectMake(options.viewportHorizontalDisplacement, options.viewportVerticalDisplacement, size.width - options.viewportHorizontalDisplacement, size.height - options.viewportVerticalDisplacement);
    WKViewSetSize(m_view, m_viewRect.size);

    if (options.viewportHorizontalDisplacement || options.viewportVerticalDisplacement)
        WKViewSetUserViewportTranslation(m_view, options.viewportHorizontalDisplacement, options.viewportVerticalDisplacement);

    if (options.forceTouchEmulationEnabled || isMobileMode()) {
        cout << "Touch Emulation Mode enabled. Hold Control key to build and emit a multi-touch event: each mouse button should be a different touch point. Release Control Key to clear all tracking pressed touches.\n";
        setTouchEmulationMode(true);
    }

    if (!options.userAgent.empty())
        WKPageSetCustomUserAgent(pageRef(), WKStringCreateWithUTF8CString(options.userAgent.c_str()));

    WKViewSetIsFocused(m_view, true);
    WKViewSetIsVisible(m_view, true);

    WKPageContextMenuClient contextMenuClient;
    memset(&contextMenuClient, 0, sizeof(WKPageContextMenuClient));
    contextMenuClient.version = kWKPageContextMenuClientCurrentVersion;
    contextMenuClient.clientInfo = this;
    contextMenuClient.showContextMenu = MiniBrowser::showContextMenu;
    WKPageSetPageContextMenuClient(pageRef(), &contextMenuClient);

    // Popup Menu UI client.
    WKPageUIPopupMenuClient popupMenuClient;
    memset(&popupMenuClient, 0, sizeof(WKPageUIPopupMenuClient));
    popupMenuClient.version = kWKPageUIPopupMenuClientCurrentVersion;
    popupMenuClient.clientInfo = this;
    popupMenuClient.showPopupMenu = MiniBrowser::showPopupMenu;
    WKPageSetUIPopupMenuClient(pageRef(), &popupMenuClient);

    WKPageUIClient uiClient;
    memset(&uiClient, 0, sizeof(WKPageUIClient));
    uiClient.version = kWKPageUIClientCurrentVersion;
    uiClient.clientInfo = this;
    uiClient.runJavaScriptAlert = MiniBrowser::runJavaScriptAlert;
    uiClient.runJavaScriptConfirm = MiniBrowser::runJavaScriptConfirm;
    uiClient.runJavaScriptPrompt = MiniBrowser::runJavaScriptPrompt;
    WKPageSetPageUIClient(pageRef(), &uiClient);

    WKPageLoaderClient loadClient;
    memset(&loadClient, 0, sizeof(WKPageLoaderClient));
    loadClient.version = kWKPageLoaderClientCurrentVersion;
    loadClient.clientInfo = this;
    loadClient.didStartProgress = MiniBrowser::didStartProgress;
    loadClient.didChangeProgress = MiniBrowser::didChangeProgress;
    loadClient.didReceiveTitleForFrame = MiniBrowser::didReceiveTitleForFrame;
    loadClient.didStartProvisionalLoadForFrame = MiniBrowser::didStartProvisionalLoadForFrame;
    loadClient.didFinishDocumentLoadForFrame = MiniBrowser::didFinishDocumentLoadForFrame;
    loadClient.didFailProvisionalLoadWithErrorForFrame = MiniBrowser::didFailProvisionalLoadWithErrorForFrame;
    loadClient.didFirstLayoutForFrame = MiniBrowser::didFirstLayoutForFrame;

    WKPageSetPageLoaderClient(pageRef(), &loadClient);

    WKURLRef wkUrl = WKURLCreateWithUTF8CString(options.url.c_str());
    WKPageLoadURL(pageRef(), wkUrl);
    WKRelease(wkUrl);
}

MiniBrowser::~MiniBrowser()
{
    g_main_loop_unref(m_mainLoop);

    WKRelease(m_view);
    delete m_control;
    delete m_touchMocker;
}

void MiniBrowser::setTouchEmulationMode(bool enabled)
{
    if (enabled && !m_touchMocker) {
        m_touchMocker = new TouchMocker(m_view);
    } else if (!enabled && m_touchMocker) {
        delete m_touchMocker;
        m_touchMocker = 0;
    }
}

static NIXMouseEvent convertToRightButtonClick(double timestamp, const NIXTouchPoint& touch)
{
    NIXMouseEvent nixEvent;
    nixEvent.type = kNIXInputEventTypeMouseDown;
    nixEvent.button = kWKEventMouseButtonRightButton;
    nixEvent.x = touch.x;
    nixEvent.y = touch.y;
    nixEvent.globalX = touch.globalX;
    nixEvent.globalY = touch.globalY;
    nixEvent.clickCount = 1;
    nixEvent.modifiers = 0;
    nixEvent.timestamp = timestamp;
    return nixEvent;
}

void MiniBrowser::saveSnapshot(double timestamp)
{
    ostringstream snapshotFile;
    snapshotFile << "webpage-snapshot-" << timestamp << ".png";
    WKSize size = WKViewGetSize(m_view);
    ToolsNix::dumpGLBufferToPng(snapshotFile.str().c_str(), size.width, size.height);
    cout << "Web page snapshot saved to " << snapshotFile.str() << endl;
}

void MiniBrowser::handleKeyPress(NIXKeyEvent* event)
{
    if (!m_view)
        return;

    switch (event->key) {
    case kNIXKeyEventKey_Print:
        saveSnapshot(event->timestamp);
        return;
    case kNIXKeyEventKey_F5:
        WKPageReload(pageRef());
        return;
    }

    if (event->modifiers & kNIXInputEventModifiersControlKey) {
        switch (event->key) {
        case kNIXKeyEventKey_Left:
            WKPageGoBack(pageRef());
            return;
        case kNIXKeyEventKey_Right:
            WKPageGoForward(pageRef());
            return;
        }
    }
    NIXViewSendKeyEvent(m_view, event);
}

void MiniBrowser::handleKeyRelease(NIXKeyEvent* event)
{
    if (m_touchMocker && m_touchMocker->handleKeyRelease(*event)) {
        scheduleUpdateDisplay();
        return;
    }
    NIXViewSendKeyEvent(m_view, event);
}

void MiniBrowser::handleMousePress(NIXMouseEvent* event)
{
    WKViewRef view = webViewAtX11Position(WKPointMake(event->x, event->y));
    if (!view)
        return;

    WKPoint windowPos = WKPointMake(event->x, event->y);
    WKPoint p = WKViewUserViewportToContents(m_view, WKPointMake(event->x, event->y));
    event->x = p.x;
    event->y = p.y;
    if (m_touchMocker && m_touchMocker->handleMousePress(*event, windowPos)) {
        scheduleUpdateDisplay();
        return;
    }
    NIXViewSendMouseEvent(view, event);
}

void MiniBrowser::handleMouseRelease(NIXMouseEvent* event)
{
    // The mouse release event was allowed to be sent to the TouchMocker because it
    // may be tracking a button press that happened in a valid position.
    WKViewRef view = webViewAtX11Position(WKPointMake(event->x, event->y));
    if (!view)
        return;

    WKPoint p = WKViewUserViewportToContents(m_view, WKPointMake(event->x, event->y));
    event->x = p.x;
    event->y = p.y;
    if (m_touchMocker && m_touchMocker->handleMouseRelease(*event)) {
        scheduleUpdateDisplay();
        return;
    }
    NIXViewSendMouseEvent(view, event);
}

void MiniBrowser::handleMouseMove(NIXMouseEvent* event)
{
    if (m_touchMocker && m_touchMocker->handleMouseMove(*event, WKPointMake(event->x, event->y))) {
        scheduleUpdateDisplay();
        return;
    }
    if (isMobileMode())
        return;

    // The mouse move event was allowed to be sent to the TouchMocker because it
    // may be tracking a button press that happened in a valid position.
    WKViewRef view = webViewAtX11Position(WKPointMake(event->x, event->y));
    if (!view)
        return;
    NIXViewSendMouseEvent(view, event);
}

void MiniBrowser::handleMouseWheel(NIXWheelEvent* event)
{
    NIXViewSendWheelEvent(m_view, event);
}

void MiniBrowser::onWindowSizeChange(WKSize size)
{
    if (!m_view)
        return;

    m_viewRect.size.width = size.width - m_viewRect.origin.x;
    m_viewRect.size.height = size.height - m_viewRect.origin.y;
    WKViewSetSize(m_view, m_viewRect.size);

    if (isMobileMode())
        WKViewSetContentScaleFactor(m_view, scaleToFitContents());
}

void MiniBrowser::onWindowClose()
{
    g_main_loop_quit(m_mainLoop);
}

static const double horizontalMarginForViewportAdjustment = 8.0;
static const double scaleFactorForTextInputFocus = 2.0;

void MiniBrowser::updateDisplay()
{
    if (!m_view || !m_control)
        return;

    m_control->makeCurrent();

    WKSize size = m_control->webViewSize();
    glViewport(0, 0, size.width, size.height);
    glClearColor(0.4, 0.4, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    WKViewPaintToCurrentGLContext(m_view);
    if (m_touchMocker)
        m_touchMocker->paintTouchPoints(size);

    m_control->swapBuffers();

    if (m_options.dumpFrames) {
        static int i = 1;
        saveSnapshot(i++);
    }
}

gboolean MiniBrowser::callUpdateDisplay(gpointer data)
{
    MiniBrowser* browser = reinterpret_cast<MiniBrowser*>(data);

    assert(browser->m_displayUpdateScheduled);
    browser->m_displayUpdateScheduled = false;
    browser->updateDisplay();
    return 0;
}

void MiniBrowser::scheduleUpdateDisplay()
{
    if (m_displayUpdateScheduled)
        return;

    m_displayUpdateScheduled = true;
    g_timeout_add(0, callUpdateDisplay, this);
}

WKPoint MiniBrowser::adjustScrollPositionToBoundaries(WKPoint position)
{
    WKSize visibleContentsSize = NIXViewVisibleContentsSize(m_view);
    double rightBoundary = m_contentsSize.width - visibleContentsSize.width;
    // Contents height may be shorter than the scaled viewport height.
    double bottomBoundary = m_contentsSize.height < visibleContentsSize.height ? 0 : m_contentsSize.height - visibleContentsSize.height;

    if (position.x < 0)
        position.x = 0;
    else if (position.x > rightBoundary)
        position.x = rightBoundary;
    if (position.y < 0)
        position.y = 0;
    else if (position.y > bottomBoundary)
        position.y = bottomBoundary;

    return position;
}

double MiniBrowser::scaleToFitContents()
{
    return m_options.devicePixelRatio * WKViewGetSize(m_view).width / m_contentsSize.width;
}

void MiniBrowser::adjustScrollPosition()
{
    WKPoint position = adjustScrollPositionToBoundaries(WKViewGetContentPosition(m_view));
    if (position == WKViewGetContentPosition(m_view))
        return;
    WKViewSetContentPosition(m_view, position);
}

void MiniBrowser::viewNeedsDisplay(WKViewRef, WKRect area, const void* clientInfo)
{
    UNUSED_PARAM(area);
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    mb->scheduleUpdateDisplay();
}

string createStdStringFromWKString(WKStringRef wkStr)
{
    size_t wkStrSize = WKStringGetMaximumUTF8CStringSize(wkStr);
    string result;
    result.resize(wkStrSize + 1);
    size_t realSize = WKStringGetUTF8CString(wkStr, &result[0], result.length());
    if (realSize > 0)
        result.resize(realSize - 1);
    return result;
}

void MiniBrowser::webProcessCrashed(WKViewRef, WKURLRef url, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    //const string urlString = createStdStringFromWKString(url);
    //cerr << "The web process has crashed on '" << urlString << "', reloading page...\n";
    //WKURLRef wkUrl = WKURLCreateWithUTF8CString(urlString.c_str());
    WKPageLoadURL(mb->pageRef(), url);
}

void MiniBrowser::webProcessRelaunched(WKViewRef, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    cout << "The web process has been restarted.\n";
    mb->scheduleUpdateDisplay();
}

void MiniBrowser::pageDidRequestScroll(WKViewRef, WKPoint position, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    if (!NIXViewIsSuspended(mb->m_view))
        WKViewSetContentPosition(mb->m_view, mb->adjustScrollPositionToBoundaries(position));
    mb->scheduleUpdateDisplay();
}

void MiniBrowser::didChangeContentsSize(WKViewRef, WKSize size, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    mb->m_contentsSize = size;

    if (mb->isMobileMode())
        mb->adjustScrollPosition();
}

void MiniBrowser::didChangeViewportAttributes(WKViewRef view, WKViewportAttributesRef attributes, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));

    WKSize viewportSize = NIXViewportAttributesGetSize(attributes);
    mb->m_viewportWidth = viewportSize.width;
    mb->m_viewportHeight = viewportSize.height;
    mb->m_viewportMinScale = NIXViewportAttributesGetMinimumScale(attributes);
    mb->m_viewportMaxScale = NIXViewportAttributesGetMaximumScale(attributes);
    mb->m_viewportInitScale = NIXViewportAttributesGetInitialScale(attributes);
    mb->m_viewportUserScalable = NIXViewportAttributesGetIsUserScalable(attributes);
}

void MiniBrowser::didFindZoomableArea(WKViewRef, WKPoint target, WKRect area, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));

    // Zoomable area width is the same as web page width, and this is fully visible.
    if (mb->m_contentsSize.width == area.size.width && mb->m_contentsSize.width == NIXViewVisibleContentsSize(mb->m_view).width)
        return;

    // The zoomed area will look nicer with a horizontal margin.
    double scale = WKViewGetSize(mb->m_view).width / (area.size.width + horizontalMarginForViewportAdjustment * 2.0);

    // Trying to zoom to an area with the same scale factor causes a zoom out.
    if (std::fabs(scale - WKViewGetContentScaleFactor(mb->m_view)) < EPSILON)
        scale = mb->scaleToFitContents();
    else {
        // We want the zoomed content area to fit horizontally in the WebView,
        // so let's give the scaleAtPoint method a suitable value.
        target.x = area.origin.x - horizontalMarginForViewportAdjustment;
        WKViewSetContentPosition(mb->m_view, WKPointMake(target.x, WKViewGetContentPosition(mb->m_view).y));
    }

    mb->scaleAtPoint(target, scale);
}

void MiniBrowser::doneWithTouchEvent(WKViewRef, const NIXTouchEvent* event, bool wasEventHandled, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    if (wasEventHandled) {
        mb->m_gestureRecognizer.reset();
        return;
    }

    mb->m_gestureRecognizer.handleTouchEvent(*event);
}

void MiniBrowser::doneWithGestureEvent(WKViewRef, const NIXGestureEvent* event, bool wasEventHandled, const void* clientInfo)
{
    if (wasEventHandled)
        return;

    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));

    if (event->type == kNIXInputEventTypeGestureSingleTap && mb->m_shouldFocusEditableArea) {
        mb->m_shouldFocusEditableArea = false;
        mb->adjustViewportToTextInputArea();
    }

    mb->m_postponeTextInputUpdates = true;
}

double MiniBrowser::scale()
{
    return WKViewGetContentScaleFactor(m_view);
}

void MiniBrowser::handleSingleTap(double timestamp, const NIXTouchPoint& touchPoint)
{
    NIXGestureEvent gestureEvent;
    gestureEvent.type = kNIXInputEventTypeGestureSingleTap;
    gestureEvent.timestamp = timestamp;
    gestureEvent.modifiers = 0;
    gestureEvent.x = touchPoint.x;
    gestureEvent.y = touchPoint.y;
    gestureEvent.globalX = touchPoint.globalX;
    gestureEvent.globalY = touchPoint.globalY;
    gestureEvent.width = 20;
    gestureEvent.height = 20;
    gestureEvent.deltaX = 0.0;
    gestureEvent.deltaY = 0.0;

    m_postponeTextInputUpdates = false;
    NIXViewSendGestureEvent(m_view, &gestureEvent);
}

void MiniBrowser::handleDoubleTap(double timestamp, const NIXTouchPoint& touchPoint)
{
    if (!m_viewportUserScalable)
        return;
    WKPoint contentsPoint = WKPointMake(touchPoint.x, touchPoint.y);
    NIXViewFindZoomableAreaForPoint(m_view, contentsPoint, touchPoint.verticalRadius, touchPoint.horizontalRadius);
}

void MiniBrowser::handleLongTap(double timestamp, const NIXTouchPoint& touch)
{
    NIXMouseEvent event = convertToRightButtonClick(timestamp, touch);
    WKPoint contentsPoint = WKViewUserViewportToContents(m_view, WKPointMake(event.x, event.y));
    event.x = contentsPoint.y;
    event.y = contentsPoint.y;
    NIXViewSendMouseEvent(m_view, &event);
}

void MiniBrowser::handlePanningStarted(double)
{
    NIXViewViewportInteractionStart(m_view);
}

void MiniBrowser::handlePanning(double timestamp, WKPoint delta)
{
    // When the user is panning around the contents we don't force the page scroll position
    // to respect any boundaries other than the physical constraints of the device from where
    // the user input came. This will be adjusted after the user interaction ends.
    WKPoint position = WKViewGetContentPosition(m_view);
    if ((m_contentsSize.width - NIXViewVisibleContentsSize(m_view).width) > 0)
        position.x -= delta.x / m_options.devicePixelRatio;
    position.y -= delta.y / m_options.devicePixelRatio;
    WKViewSetContentPosition(m_view, position);
}

void MiniBrowser::handlePinchStarted(double)
{
    if (m_viewportUserScalable)
        NIXViewViewportInteractionStart(m_view);
}

void MiniBrowser::handlePanningFinished(double timestamp)
{
    adjustScrollPosition();
    NIXViewViewportInteractionStop(m_view);
}

void MiniBrowser::handlePinch(double timestamp, WKPoint delta, double scale, WKPoint contentCenter)
{
    if (!m_viewportUserScalable)
        return;

    // Scaling: The page should be scaled proportionally to the distance of the pinch.
    // Scrolling: If the center of the pinch initially was position (120,120) in content
    //            coordinates, them during the page must be scrolled to keep the pinch center
    //            at the same coordinates.
    WKPoint position = WKPointMake(WKViewGetContentPosition(m_view).x - delta.x, WKViewGetContentPosition(m_view).y - delta.y);

    WKViewSetContentPosition(m_view, position);
    scaleAtPoint(contentCenter, scale, LowerMinimumScale);
}

void MiniBrowser::handlePinchFinished(double)
{
    if (!m_viewportUserScalable)
        return;

    double scale = WKViewGetContentScaleFactor(m_view);
    double minimumScale = double(WKViewGetSize(m_view).width) / m_contentsSize.width;

    bool needsScale = true;
    if (scale > m_viewportMaxScale)
        scale = m_viewportMaxScale;
    else if (scale < m_viewportMinScale)
        scale = m_viewportMinScale;
    else if (scale < minimumScale)
        scale = minimumScale;
    else
        needsScale = false;

    if (needsScale)
        WKViewSetContentScaleFactor(m_view, scale);

    adjustScrollPosition();
    NIXViewViewportInteractionStop(m_view);
}

void MiniBrowser::scaleAtPoint(const WKPoint& point, double scale, ScaleBehavior scaleBehavior)
{
    if (!m_viewportUserScalable)
        return;

    double minimumScale = double(WKViewGetSize(m_view).width) / m_contentsSize.width;

    if (scaleBehavior & LowerMinimumScale)
        minimumScale *= 0.5;

    if (scale < minimumScale)
        scale = minimumScale;

    // Calculate new scroll points that will keep the content
    // approximately at the same visual point.
    double scaleRatio = WKViewGetContentScaleFactor(m_view) / scale;
    WKPoint position = WKPointMake(point.x - (point.x - WKViewGetContentPosition(m_view).x) * scaleRatio,
                                   point.y - (point.y - WKViewGetContentPosition(m_view).y) * scaleRatio);

    WKViewSetContentScaleFactor(m_view, scale);
    if (scaleBehavior & AdjustToBoundaries)
        position = adjustScrollPositionToBoundaries(position);
    WKViewSetContentPosition(m_view, position);
}

static inline bool areaContainsPoint(const WKRect& area, const WKPoint& point)
{
    return !(point.x < area.origin.x || point.y < area.origin.y || point.x >= (area.origin.x + area.size.width) || point.y >= (area.origin.y + area.size.height));
}

WKViewRef MiniBrowser::webViewAtX11Position(const WKPoint& position)
{
    if (areaContainsPoint(m_viewRect, position))
        return m_view;
    return 0;
}

void MiniBrowser::adjustViewportToTextInputArea()
{
    m_shouldRestoreViewportWhenLosingFocus = true;
    m_scaleBeforeFocus = WKViewGetContentScaleFactor(m_view);
    m_scrollPositionBeforeFocus = WKViewGetContentPosition(m_view);

    WKViewSetContentScaleFactor(m_view, scaleFactorForTextInputFocus);

    // After scaling to fit editor rect width, we align vertically based on cursor rect.
    WKPoint scrollPosition;
    scrollPosition.x = m_editorRect.origin.x - scaleFactorForTextInputFocus * horizontalMarginForViewportAdjustment;
    double verticalOffset = (NIXViewVisibleContentsSize(m_view).height - m_cursorRect.size.height) / 2.0;
    scrollPosition.y = m_cursorRect.origin.y - verticalOffset;
    scrollPosition = adjustScrollPositionToBoundaries(scrollPosition);
    WKViewSetContentPosition(m_view, scrollPosition);
}

static inline bool WKRectIsEqual(const WKRect& a, const WKRect& b)
{
    return a.origin == b.origin && a.size.width == b.size.width && a.size.height == b.size.height;
}

void MiniBrowser::updateTextInputState(WKViewRef, const NIXTextInputState* state, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));

    if (mb->m_postponeTextInputUpdates)
        return;

    if (state->isContentEditable) {
        // If we're only moving cursor inside the current editor, then we should not focus it again.
        if (WKRectIsEqual(state->editorRect, mb->m_editorRect))
            return;

        mb->m_shouldFocusEditableArea = true;
        mb->m_cursorRect = state->cursorRect;
        mb->m_editorRect = state->editorRect;
    } else {
        if (mb->m_shouldRestoreViewportWhenLosingFocus) {
            mb->m_shouldRestoreViewportWhenLosingFocus = false;
            WKViewSetContentScaleFactor(mb->m_view, mb->m_scaleBeforeFocus);
            WKViewSetContentPosition(mb->m_view, mb->m_scrollPositionBeforeFocus);
        }
        mb->m_cursorRect = WKRectMake(0, 0, 0, 0);
        mb->m_editorRect = WKRectMake(0, 0, 0, 0);
    }
}

static void printPopupMenuItem(const WKPopupItemRef item, const int optionIndex, const int level)
{
    const WKStringRef title = WKPopupItemCopyText(item);
    if (WKStringIsEmpty(title)) {
        WKRelease(title);
        return;
    }

    // No tabs for level 0.
    for (int i = 0; i < level; ++i)
        cout << "\t";

    cout << optionIndex << "- " << createStdStringFromWKString(title) << endl;
    WKRelease(title);
}

static int renderPopupMenu(const WKArrayRef menuItems, int& optionIndex, const int level = 0)
{
    size_t size = WKArrayGetSize(menuItems);
    for (size_t i = 0; i < size; ++i) {
        const WKPopupItemRef item = static_cast<WKPopupItemRef>(WKArrayGetItemAtIndex(menuItems, i));
        if (WKPopupItemGetType(item) == kWKPopupItemTypeSeparator)
            cout << "--------------------\n";
        else
            printPopupMenuItem(item, optionIndex++, level);
    }
    return (optionIndex - 1);
}

void MiniBrowser::showPopupMenu(WKPageRef page, WKPopupMenuListenerRef menuListenerRef, WKRect rect, WKPopupItemTextDirection textDirection, double pageScaleFactor, WKArrayRef itemsRef, int32_t selectedIndex, const void* clientInfo)
{
    // FIXME: we should have a GUI popup menu at some point.
    cout << "\n# POPUP MENU #\n";

    int optionIndex = 1;
    cout << "--------------------\n";
    const int itemsCounter = renderPopupMenu(itemsRef, optionIndex);
    cout << "--------------------\n";

    int option = 0;
    cout << "Popup Menu option (0 for none): ";
    cin >> option;
    if (option > 0 && option <= itemsCounter) {
        if (WKPopupItemIsLabel(static_cast<WKPopupItemRef>(WKArrayGetItemAtIndex(itemsRef, option - 1))))
            option = selectedIndex + 1;
    } else {
        option = selectedIndex + 1;
    }
    WKPopupMenuListenerSetSelection(menuListenerRef, option - 1);
}

static void printContextMenuItem(const WKContextMenuItemRef item, const int optionIndex, const int level)
{
    const WKStringRef title = WKContextMenuItemCopyTitle(item);
    if (WKStringIsEmpty(title)) {
        WKRelease(title);
        return;
    }

    std::string titleBuffer = createStdStringFromWKString(title);

    // No tabs for level 0.
    for (int i = 0; i < level; ++i)
        cout << "\t";

    cout << optionIndex << "- " << titleBuffer << endl;
    WKRelease(title);
}

static int renderContextMenu(const WKArrayRef menuItems, int& optionIndex, const int level = 0)
{
    size_t size = WKArrayGetSize(menuItems);
    for (size_t i = 0; i < size; ++i) {
        const WKContextMenuItemRef item = static_cast<WKContextMenuItemRef>(WKArrayGetItemAtIndex(menuItems, i));
        if (WKContextMenuItemGetType(item) == kWKContextMenuItemTypeSeparator)
            cout << "--------------------" << endl;
        else if (WKContextMenuItemGetType(item) == kWKContextMenuItemTypeSubmenu) {
            printContextMenuItem(item, optionIndex++, level);
            const WKArrayRef subMenu = WKContextMenuCopySubmenuItems(item);
            renderContextMenu(subMenu, optionIndex, level + 1);
            WKRelease(subMenu);
        } else
            printContextMenuItem(item, optionIndex++, level);
    }
    return (optionIndex - 1);
}

static bool selectContextMenuItemAtIndex(const WKPageRef page, const WKArrayRef menuItems, const int index, int counter = 0)
{
    size_t size = WKArrayGetSize(menuItems);
    for (size_t i = 0; i < size; ++i) {
        const WKContextMenuItemRef item = static_cast<WKContextMenuItemRef>(WKArrayGetItemAtIndex(menuItems, i));
        if (counter == index) {
            WKPageSelectContextMenuItem(page, item);
            return true;
        }
        if (WKContextMenuItemGetType(item) == kWKContextMenuItemTypeSubmenu) {
            const WKArrayRef subMenu = WKContextMenuCopySubmenuItems(item);
            return selectContextMenuItemAtIndex(page, subMenu, index, ++counter);
        }
        counter++;
    }
}

void MiniBrowser::showContextMenu(WKPageRef page, WKPoint menuLocation, WKArrayRef menuItems, const void*)
{
    // FIXME: we should have a GUI context menu at some point.
    cout << "\n# CONTEXT MENU #\n";

    int optionIndex = 1;
    cout << "--------------------\n";
    const int itemsCounter = renderContextMenu(menuItems, optionIndex);
    cout << "--------------------\n";

    int option = 0;
    cout << "Context Menu option (0 for none): ";
    cin >> option;
    if (option > 0 && option <= itemsCounter)
        selectContextMenuItemAtIndex(page, menuItems, option - 1);
}

void MiniBrowser::runJavaScriptAlert(WKPageRef, WKStringRef message, WKFrameRef, const void*)
{
    std::string messageString = createStdStringFromWKString(message);
    cout << RED "[js:alert] " << messageString << NOCOLOR "\n";
}

bool MiniBrowser::runJavaScriptConfirm(WKPageRef, WKStringRef message, WKFrameRef, const void*)
{
    std::string messageString = createStdStringFromWKString(message);
    cout << GREEN "[js:confirm] " << messageString << NOCOLOR "\n> ";

    char option;
    cin >> option;
    return option == 'y' || option == 'Y';
}

WKStringRef MiniBrowser::runJavaScriptPrompt(WKPageRef, WKStringRef message, WKStringRef defaultValue, WKFrameRef, const void*)
{
    std::string messageString = createStdStringFromWKString(message);
    std::string defaultString = createStdStringFromWKString(defaultValue);
    cout << YELLOW "[js:prompt] " << messageString << " [default: '" << defaultString << "']" NOCOLOR "\n> ";

    std::string userInput;
    getline(cin, userInput);
    return WKStringCreateWithUTF8CString(userInput.c_str());
}

void MiniBrowser::didStartProgress(WKPageRef page, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    mb->m_control->setLoadProgress(0.0);
}

void MiniBrowser::didChangeProgress(WKPageRef page, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    mb->m_control->setLoadProgress(WKPageGetEstimatedProgress(page));
}

void MiniBrowser::didReceiveTitleForFrame(WKPageRef, WKStringRef title, WKFrameRef frame, WKTypeRef, const void* clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    std::string titleStr = createStdStringFromWKString(title) + " - MiniBrowser";
    mb->m_control->setWindowTitle(titleStr.c_str());
}

void MiniBrowser::updateActiveUrlText()
{
    std::string url = activeUrl();
    if (m_activeUrlText != url) {
        m_activeUrlText = url;
        m_control->updateUrlText(m_activeUrlText.c_str());
    }
}

void MiniBrowser::didStartProvisionalLoadForFrame(WKPageRef page, WKFrameRef, WKTypeRef, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    mb->updateActiveUrlText();
}

void MiniBrowser::didFinishDocumentLoadForFrame(WKPageRef page, WKFrameRef, WKTypeRef, const void* clientInfo)
{
    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));
    mb->updateActiveUrlText();
}

void MiniBrowser::didFailProvisionalLoadWithErrorForFrame(WKPageRef page, WKFrameRef frame, WKErrorRef error, WKTypeRef, const void *)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    WKStringRef wkErrorDescription = WKErrorCopyLocalizedDescription(error);
    WKPageLoadPlainTextString(page, wkErrorDescription);
    WKRelease(wkErrorDescription);
}

void MiniBrowser::didFirstLayoutForFrame(WKPageRef, WKFrameRef frame, WKTypeRef, const void* clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    MiniBrowser* mb = static_cast<MiniBrowser*>(const_cast<void*>(clientInfo));

    double scale = mb->scaleToFitContents();
    if (scale < mb->m_viewportMinScale)
        scale = mb->m_viewportMinScale;
    else if (scale > mb->m_viewportMaxScale)
        scale = mb->m_viewportMaxScale;
    mb->m_viewportInitScale = scale;

    if (!mb->m_viewportUserScalable)
        mb->m_viewportMaxScale = mb->m_viewportMinScale = scale;

    WKViewSetContentScaleFactor(mb->m_view, scale);
}

std::string MiniBrowser::activeUrl()
{
    WKURLRef urlRef = WKPageCopyActiveURL(pageRef());
    std::string url;
    if (urlRef) {
        WKStringRef urlStr = WKURLCopyString(urlRef);
        url = createStdStringFromWKString(urlStr);
        WKRelease(urlStr);
        WKRelease(urlRef);
    }
    return url;
}
