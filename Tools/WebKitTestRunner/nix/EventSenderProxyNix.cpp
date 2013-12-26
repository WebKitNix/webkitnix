/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Zan Dobersek <zandobersek@gmail.com>
 * Copyright (C) 2009 Holger Hans Peter Freyther
 * Copyright (C) 2010 Igalia S.L.
 * Copyright (C) 2011 ProFUSION Embedded Systems
 * Copyright (C) 2012 Samsung Electronics
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// FIXME: Remove this once everything has been moved over to the IPC namespace.
#define CoreIPC IPC

#include "config.h"
#include "EventSenderProxy.h"

#include "NotImplemented.h"
#include "PlatformWebView.h"
#include "TestController.h"

#include <unistd.h>
#include <wtf/CurrentTime.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>
#include <API/nix/NIXView.h>
#include <API/C/WKAPICast.h>

namespace WTR {

// Key event location code defined in DOM Level 3.
enum KeyLocationCode {
    DOMKeyLocationStandard      = 0x00,
    DOMKeyLocationLeft          = 0x01,
    DOMKeyLocationRight         = 0x02,
    DOMKeyLocationNumpad        = 0x03
};

// Unicode values for Apple non-letter keys.
#define KEYCODE_DEL         127
#define KEYCODE_BACKSPACE   8
#define KEYCODE_UPARROW     0xf700
#define KEYCODE_DOWNARROW   0xf701
#define KEYCODE_LEFTARROW   0xf702
#define KEYCODE_RIGHTARROW  0xf703

struct WTREvent {
    NIXInputEvent* event;
    int delay;
};

static WTREvent eventQueue[1024];
static unsigned endOfQueue;
static bool isReplayingEvents;

static std::vector<NIXTouchPoint> touchPoints;
static unsigned touchModifiers;
static int touchVerticalRadius;
static int touchHorizontalRadius;

EventSenderProxy::EventSenderProxy(TestController* testController)
    : m_testController(testController)
    , m_time(0)
    , m_leftMouseButtonDown(false)
    , m_clickCount(0)
    , m_clickTime(0)
    , m_clickButton(0)
{
    memset(eventQueue, 0, sizeof(eventQueue));
    endOfQueue = 0;
    isReplayingEvents = false;
    touchPoints.clear();
    touchModifiers = 0;
    touchVerticalRadius = 0;
    touchHorizontalRadius = 0;
}

EventSenderProxy::~EventSenderProxy()
{
}

static const double doubleClickInterval = 300;

static inline double convertToNixTimestamp(double ms)
{
    return ms / 1000.0;
}

void EventSenderProxy::updateClickCountForButton(int button)
{
    if (m_time - m_clickTime <= doubleClickInterval
        && m_position == m_clickPosition
        && button == m_clickButton) {
        ++m_clickCount;
        m_clickTime = m_time;
        return;
    }

    m_clickCount = 1;
    m_clickTime = m_time;
    m_clickPosition = m_position;
    m_clickButton = button;
}

static unsigned convertToNixModifiers(WKEventModifiers modifiersRef)
{
    unsigned modifiers = 0;

    if (modifiersRef & kWKEventModifiersControlKey)
        modifiers |= kNIXInputEventModifiersControlKey;
    if (modifiersRef & kWKEventModifiersShiftKey)
        modifiers |= kNIXInputEventModifiersShiftKey;
    if (modifiersRef & kWKEventModifiersAltKey)
        modifiers |= kNIXInputEventModifiersAltKey;
    if (modifiersRef & kWKEventModifiersMetaKey)
        modifiers |= kNIXInputEventModifiersMetaKey;

    return modifiers;
}

NIXMouseEvent* EventSenderProxy::createMouseEvent(NIXInputEventType type, unsigned button, WKEventModifiers wkModifiers)
{
    NIXMouseEvent* ev = new NIXMouseEvent;
    memset(ev, 0, sizeof(NIXMouseEvent));
    ev->type = type;
    ev->modifiers = convertToNixModifiers(wkModifiers);
    ev->timestamp = convertToNixTimestamp(m_time);
    ev->button = (WKEventMouseButton) button;
    ev->x = m_position.x;
    ev->y = m_position.y;
    ev->globalX = m_position.x;
    ev->globalY = m_position.y;
    ev->clickCount = m_clickCount;
    return ev;
}

void EventSenderProxy::mouseDown(unsigned button, WKEventModifiers wkModifiers)
{
    updateClickCountForButton(button);

    NIXMouseEvent* ev = createMouseEvent(kNIXInputEventTypeMouseDown, button, wkModifiers);
    sendOrQueueEvent(reinterpret_cast<NIXInputEvent*>(ev));
}

void EventSenderProxy::mouseUp(unsigned button, WKEventModifiers wkModifiers)
{
    NIXMouseEvent* ev = createMouseEvent(kNIXInputEventTypeMouseUp, button, wkModifiers);
    m_clickPosition = m_position;
    m_clickTime = m_time;

    sendOrQueueEvent(reinterpret_cast<NIXInputEvent*>(ev));
}

void EventSenderProxy::mouseMoveTo(double x, double y)
{
    m_position.x = x;
    m_position.y = y;

    NIXMouseEvent* ev = createMouseEvent(kNIXInputEventTypeMouseMove, 0, 0);
    sendOrQueueEvent(reinterpret_cast<NIXInputEvent*>(ev));
}

void EventSenderProxy::mouseScrollBy(int horizontal, int vertical)
{
    notImplemented();
}

void EventSenderProxy::leapForward(int milliseconds)
{
    eventQueue[endOfQueue].delay = milliseconds;
    m_time += milliseconds;
}

NIXKeyEvent* EventSenderProxy::createKeyEvent(NIXInputEventType type, unsigned code, unsigned nixModifiers, bool shouldUseUpperCase, bool isKeypad)
{
    NIXKeyEvent* ev = new NIXKeyEvent;
    memset(ev, 0, sizeof(NIXKeyEvent));
    ev->type = type;
    ev->key = static_cast<NIXKeyEventKey>(code);
    ev->modifiers = nixModifiers;
    ev->timestamp = convertToNixTimestamp(m_time);
    ev->shouldUseUpperCase = shouldUseUpperCase;
    ev->isKeypad = isKeypad;
    return ev;
}

void EventSenderProxy::keyDown(WKStringRef keyRef, WKEventModifiers wkModifiers, unsigned location)
{
    WTF::String key = WebKit::toWTFString(keyRef);
    unsigned modifiers = convertToNixModifiers(wkModifiers);
    bool isKeypad = (location == DOMKeyLocationNumpad);
    bool shouldUseUpperCase = false;

    unsigned code = 0;
    if (key.length() == 1) {
        code = key[0];
        if (code == '\r')
            code = kNIXKeyEventKey_Return;
        else if (code == '\t') {
            code = kNIXKeyEventKey_Tab;
            if (modifiers == kNIXInputEventModifiersShiftKey)
                code = kNIXKeyEventKey_Backtab;
        } else if (code == KEYCODE_DEL || code == KEYCODE_BACKSPACE) {
            code = kNIXKeyEventKey_Backspace;
            if (modifiers == kNIXInputEventModifiersAltKey)
                modifiers = kNIXInputEventModifiersControlKey;
        } else if (code == 'o' && modifiers == kNIXInputEventModifiersControlKey) {
            // Mimic the emacs ctrl-o binding on Mac by inserting a paragraph
            // separator and then putting the cursor back to its original
            // position. Allows us to pass emacs-ctrl-o.html
            code = '\n';
            modifiers = 0;
            sendOrQueueEvent(reinterpret_cast<NIXInputEvent*>(createKeyEvent(kNIXInputEventTypeKeyDown, code, modifiers)));
            sendOrQueueEvent(reinterpret_cast<NIXInputEvent*>(createKeyEvent(kNIXInputEventTypeKeyUp, code, modifiers)));
            code = kNIXKeyEventKey_Left;
        } else if (code == 'y' && modifiers == kNIXInputEventModifiersControlKey)
            code = 'c';
        else if (code == 'k' && modifiers == kNIXInputEventModifiersControlKey)
            code = 'x';
        else if (code == 'a' && modifiers == kNIXInputEventModifiersControlKey) {
            code = kNIXKeyEventKey_Home;
            modifiers = 0;
        } else if (code == KEYCODE_LEFTARROW) {
            code = kNIXKeyEventKey_Left;
            if (modifiers & kNIXInputEventModifiersMetaKey) {
                modifiers -= kNIXInputEventModifiersMetaKey;
                code = kNIXKeyEventKey_Home;
            }
        } else if (code == KEYCODE_RIGHTARROW) {
            code = kNIXKeyEventKey_Right;
            if (modifiers & kNIXInputEventModifiersMetaKey) {
                modifiers -= kNIXInputEventModifiersMetaKey;
                code = kNIXKeyEventKey_End;
            }
        } else if (code == KEYCODE_UPARROW) {
            code = kNIXKeyEventKey_Up;
            if (modifiers & kNIXInputEventModifiersMetaKey) {
                modifiers -= kNIXInputEventModifiersMetaKey;
                code = kNIXKeyEventKey_PageUp;
            }
        } else if (code == KEYCODE_DOWNARROW) {
            code = kNIXKeyEventKey_Down;
            if (modifiers & kNIXInputEventModifiersMetaKey) {
                modifiers -= kNIXInputEventModifiersMetaKey;
                code = kNIXKeyEventKey_PageDown;
            }
        } else {
            shouldUseUpperCase = isASCIIUpper(code);
            code = toASCIIUpper(code);
            if (shouldUseUpperCase)
                modifiers |= kNIXInputEventModifiersShiftKey;
        }
    } else {
        if (key.startsWith('F') && key.length() <= 3) {
            key.remove(0, 1);
            int functionKey = key.toInt();
            ASSERT(functionKey >= 1 && functionKey <= 35);
            code = kNIXKeyEventKey_F1 + (functionKey - 1);
        } else if (key == "leftArrow")
            code = kNIXKeyEventKey_Left;
        else if (key == "rightArrow")
            code = kNIXKeyEventKey_Right;
        else if (key == "upArrow")
            code = kNIXKeyEventKey_Up;
        else if (key == "downArrow")
            code = kNIXKeyEventKey_Down;
        else if (key == "pageUp")
            code = kNIXKeyEventKey_PageUp;
        else if (key == "pageDown")
            code = kNIXKeyEventKey_PageDown;
        else if (key == "home")
            code = kNIXKeyEventKey_Home;
        else if (key == "end")
            code = kNIXKeyEventKey_End;
        else if (key == "insert")
            code = kNIXKeyEventKey_Insert;
        else if (key == "delete")
            code = kNIXKeyEventKey_Delete;
        else if (key == "printScreen")
            code = kNIXKeyEventKey_Print;
        else if (key == "menu")
            code = kNIXKeyEventKey_Menu;
    }

    sendOrQueueEvent(reinterpret_cast<NIXInputEvent*>(createKeyEvent(kNIXInputEventTypeKeyDown, code, modifiers, shouldUseUpperCase, isKeypad)));
    sendOrQueueEvent(reinterpret_cast<NIXInputEvent*>(createKeyEvent(kNIXInputEventTypeKeyUp, code, modifiers, shouldUseUpperCase, isKeypad)));
}

#if ENABLE(TOUCH_EVENTS)
void EventSenderProxy::addTouchPoint(int x, int y)
{
    unsigned lowestId = 0;
    for (size_t i = 0; i < touchPoints.size(); i++) {
        if (touchPoints[i].id == lowestId)
            lowestId++;
    }

    NIXTouchPoint touch;
    touch.id = lowestId;
    touch.state = kNIXTouchPointStateTouchPressed;
    touch.x = x;
    touch.y = y;
    touch.globalX = x;
    touch.globalY = y;
    touch.verticalRadius = touchVerticalRadius;
    touch.horizontalRadius = touchHorizontalRadius;
    touch.rotationAngle = 0.0;
    touch.pressure = 1.0;
    touchPoints.push_back(touch);
}

void EventSenderProxy::updateTouchPoint(int index, int x, int y)
{
    ASSERT(index >= 0 && index < touchPoints.size());

    NIXTouchPoint& touch = touchPoints[index];
    touch.state = kNIXTouchPointStateTouchMoved;
    touch.x = x;
    touch.y = y;
    touch.globalX = x;
    touch.globalY = y;
    touch.verticalRadius = touchVerticalRadius;
    touch.horizontalRadius = touchHorizontalRadius;
}

void EventSenderProxy::releaseTouchPoint(int index)
{
    ASSERT(index >= 0 && index < touchPoints.size());

    NIXTouchPoint& touch = touchPoints[index];
    touch.state = kNIXTouchPointStateTouchReleased;
}

void EventSenderProxy::cancelTouchPoint(int index)
{
    ASSERT(index >= 0 && index < touchPoints.size());

    NIXTouchPoint& touch = touchPoints[index];
    touch.state = kNIXTouchPointStateTouchCancelled;
}

void EventSenderProxy::setTouchModifier(WKEventModifiers wkModifier, bool enable)
{
    unsigned modifier = convertToNixModifiers(wkModifier);
    if (enable)
        touchModifiers |= modifier;
    else
        touchModifiers &= ~modifier;
}

void EventSenderProxy::sendCurrentTouchEvent(NIXInputEventType touchType)
{
    NIXTouchEvent* ev = new NIXTouchEvent;
    ev->modifiers = touchModifiers;
    ev->timestamp = convertToNixTimestamp(m_time);
    ev->type = touchType;
    for (size_t i = 0; i < touchPoints.size(); i++)
        ev->touchPoints[i] = touchPoints[i];
    ev->numTouchPoints = touchPoints.size();
    sendOrQueueEvent(reinterpret_cast<NIXInputEvent*>(ev));

    for (size_t i = 0; i < touchPoints.size(); ++i) {
        NIXTouchPoint& touchPoint = touchPoints[i];
        if (touchPoint.state == kNIXTouchPointStateTouchReleased) {
            touchPoints.erase(touchPoints.begin() + i);
            --i;
        } else
            touchPoint.state = kNIXTouchPointStateTouchStationary;
    }
}

void EventSenderProxy::touchStart()
{
    sendCurrentTouchEvent(kNIXInputEventTypeTouchStart);
}

void EventSenderProxy::touchMove()
{
    sendCurrentTouchEvent(kNIXInputEventTypeTouchMove);
}

void EventSenderProxy::touchEnd()
{
    sendCurrentTouchEvent(kNIXInputEventTypeTouchEnd);
}

void EventSenderProxy::touchCancel()
{
    sendCurrentTouchEvent(kNIXInputEventTypeTouchCancel);
}

void EventSenderProxy::clearTouchPoints()
{
    touchPoints.clear();
}

void EventSenderProxy::setTouchPointRadius(int radiusX, int radiusY)
{
    touchHorizontalRadius = radiusX;
    touchVerticalRadius = radiusY;
}

#endif

static void dispatchAndDeleteEvent(TestController* testController, NIXInputEvent* event)
{
    PlatformWKView view = testController->mainWebView()->platformView();
    switch (event->type) {
        case kNIXInputEventTypeMouseDown:
        case kNIXInputEventTypeMouseUp:
        case kNIXInputEventTypeMouseMove:
            NIXViewSendMouseEvent(view, reinterpret_cast<NIXMouseEvent*>(event));
            break;
        case kNIXInputEventTypeWheel:
            NIXViewSendWheelEvent(view, reinterpret_cast<NIXWheelEvent*>(event));
            break;
        case kNIXInputEventTypeKeyDown:
        case kNIXInputEventTypeKeyUp:
            NIXViewSendKeyEvent(view, reinterpret_cast<NIXKeyEvent*>(event));
            break;
        case kNIXInputEventTypeTouchStart:
        case kNIXInputEventTypeTouchMove:
        case kNIXInputEventTypeTouchEnd:
        case kNIXInputEventTypeTouchCancel:
            NIXViewSendTouchEvent(view, reinterpret_cast<NIXTouchEvent*>(event));
            break;
        case kNIXInputEventTypeGestureSingleTap:
            NIXViewSendGestureEvent(view, reinterpret_cast<NIXGestureEvent*>(event));
            break;
        default:
            notImplemented();
    }
    delete event;
}

void EventSenderProxy::sendOrQueueEvent(NIXInputEvent* event)
{
    if (!endOfQueue && !eventQueue[endOfQueue].delay) {
        dispatchAndDeleteEvent(m_testController, event);
        return;
    }
    eventQueue[endOfQueue++].event = event;
    replaySavedEvents();
}

void EventSenderProxy::replaySavedEvents()
{
    if (isReplayingEvents)
        return;

    isReplayingEvents = true;
    int i = 0;

    while (i < endOfQueue) {
        WTREvent& ev = eventQueue[i];
        if (ev.delay)
            usleep(ev.delay * 1000);
        i++;

        dispatchAndDeleteEvent(m_testController, ev.event);
        ev.event = 0;
        ev.delay = 0;
    }

    endOfQueue = 0;
    isReplayingEvents = false;
}

void EventSenderProxy::continuousMouseScrollBy(int horizontal, int vertical, bool paged)
{
    notImplemented();
}

}
