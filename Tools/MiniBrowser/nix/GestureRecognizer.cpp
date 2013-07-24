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

#include "GestureRecognizer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>

static const int PanDistanceThreshold = 10;
static const int MaxDoubleTapInterval = 500;
static const int LongTapInterval = 1000;

static WKPoint computeCenter(WKPoint a, WKPoint b)
{
    return WKPointMake((a.x + b.x) / 2, (a.y + b.y) / 2);
}

static double computeDistance(WKPoint a, WKPoint b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

static bool hasTouchPointPressed(const NIXTouchEvent& event)
{
    for (int i = 0; i < event.numTouchPoints; ++i) {
        if (event.touchPoints[i].state == kNIXTouchPointStateTouchPressed)
            return true;
    }
    return false;
}

GestureRecognizer::GestureRecognizer(GestureRecognizerClient *client)
    : m_client(client)
    , m_state(&GestureRecognizer::noGesture)
    , m_timestamp(0)
    , m_doubleTapTimerId(0)
    , m_longTapTimerId(0)
    , m_initialPinchDistance(0)
    , m_initialPinchScale(0)
{
}

void GestureRecognizer::reset()
{
    cancelLongTapTimerIfNeeded();
    cancelDoubleTapTimerIfNeeded();

    m_state = &GestureRecognizer::noGesture;
    m_timestamp = 0;
    m_initialPinchDistance = 0;
    m_initialPinchScale = 0;
}

void GestureRecognizer::cancelLongTapTimerIfNeeded()
{
    if (!m_longTapTimerId)
        return;

    g_source_remove(m_longTapTimerId);
    m_longTapTimerId = 0;
}

static gboolean longTapTimer(gpointer data)
{
    reinterpret_cast<GestureRecognizer*>(data)->longTapTimerTriggered();
    return false;
}

void GestureRecognizer::startLongTapTimer()
{
    cancelLongTapTimerIfNeeded();
    m_longTapTimerId = g_timeout_add(LongTapInterval, longTapTimer, this);
}

void GestureRecognizer::longTapTimerTriggered()
{
    if (m_state != &GestureRecognizer::singleTapPressed)
        return;

    m_client->handleLongTap(m_timestamp, m_firstTouchPoint);
    reset();
}

void GestureRecognizer::startDoubleTapTimer()
{
    cancelDoubleTapTimerIfNeeded();
    m_doubleTapTimerId = g_timeout_add(MaxDoubleTapInterval, doubleTapTimer, this);
}

void GestureRecognizer::cancelDoubleTapTimerIfNeeded()
{
    if (!m_doubleTapTimerId)
        return;

    g_source_remove(m_doubleTapTimerId);
    m_doubleTapTimerId = 0;
}

static bool exceedsPanThreshold(const NIXTouchPoint& first, const NIXTouchPoint& last)
{
    float deltaX = std::abs(first.globalX - last.globalX);
    float deltaY = std::abs(first.globalY - last.globalY);
    return std::max(deltaX, deltaY) > PanDistanceThreshold;
}

void GestureRecognizer::doubleTapTimerTriggered()
{
    if (m_state == &GestureRecognizer::waitForDoubleTap) {
        m_client->handleSingleTap(m_timestamp, m_firstTouchPoint);
        reset();
    }
}

void GestureRecognizer::fail(const char* reason)
{
    fprintf(stderr, "Gesture recognizer failed: %s\n", reason);
    reset();
}

void GestureRecognizer::noGesture(const NIXTouchEvent& event)
{
    if (event.type != kNIXInputEventTypeTouchStart)
        return;

    m_state = &GestureRecognizer::singleTapPressed;
    NIXTouchPoint touch = event.touchPoints[0];
    m_firstTouchPoint = touch;
    m_previousTouchPoint = touch;
    startLongTapTimer();
}

void GestureRecognizer::setupPinchData(const NIXTouchEvent& event)
{
    const NIXTouchPoint& first = event.touchPoints[0];
    const NIXTouchPoint& second = event.touchPoints[1];

    m_initialPinchDistance = computeDistance(WKPointMake(first.globalX, first.globalY), WKPointMake(second.globalX, second.globalY));
    m_initialPinchScale = m_client->scale();
    m_initialPinchContentCenter = computeCenter(WKPointMake(first.x, first.y), WKPointMake(second.x, second.y));
    m_previousPinchGlobalCenter = computeCenter(WKPointMake(first.globalX, first.globalY), WKPointMake(second.globalX, second.globalY));
}

void GestureRecognizer::singleTapPressed(const NIXTouchEvent& event)
{
    cancelLongTapTimerIfNeeded();
    const NIXTouchPoint& touchPoint = event.touchPoints[0];
    switch (event.type) {
    case kNIXInputEventTypeTouchMove:
        if (exceedsPanThreshold(touchPoint, m_firstTouchPoint))
            setupPanningData(event.timestamp, touchPoint);
        break;
    case kNIXInputEventTypeTouchEnd:
        m_state = &GestureRecognizer::waitForDoubleTap;
        m_timestamp = event.timestamp;
        startDoubleTapTimer();
        break;
    case kNIXInputEventTypeTouchStart:
        m_state = &GestureRecognizer::pinchInProgress;
        setupPinchData(event);
        m_client->handlePinchStarted(event.timestamp);
        break;
    }
}
void GestureRecognizer::waitForDoubleTap(const NIXTouchEvent& event)
{
    cancelDoubleTapTimerIfNeeded();
    switch (event.type) {
    case kNIXInputEventTypeTouchStart:
        m_state = &GestureRecognizer::doubleTapPressed;
        break;
    case kNIXInputEventTypeTouchMove:
    case kNIXInputEventTypeTouchEnd:
        fail("received TouchMove or TouchEnd when in WaitForDoubleTap state.");
        break;
    }
}
void GestureRecognizer::doubleTapPressed(const NIXTouchEvent& event)
{
    const NIXTouchPoint& touchPoint = event.touchPoints[0];
    switch (event.type) {
    case kNIXInputEventTypeTouchMove:
        if (exceedsPanThreshold(touchPoint, m_firstTouchPoint))
            setupPanningData(event.timestamp, touchPoint);
        break;
    case kNIXInputEventTypeTouchEnd:
        m_state = &GestureRecognizer::noGesture;
        m_client->handleDoubleTap(event.timestamp, touchPoint);
        break;
    case kNIXInputEventTypeTouchStart:
        m_state = &GestureRecognizer::pinchInProgress;
        setupPinchData(event);
        m_client->handlePinchStarted(event.timestamp);
        break;
    }
}

void GestureRecognizer::setupPanningData(double timestamp, const NIXTouchPoint& current)
{
    m_client->handlePanningStarted(timestamp);
    m_state = &GestureRecognizer::panningInProgress;
    updatePanningData(timestamp, current);
}

void GestureRecognizer::updatePanningData(double timestamp, const NIXTouchPoint& current)
{
    WKPoint delta = WKPointMake((current.globalX - m_previousTouchPoint.globalX) / m_client->scale(),
                                (current.globalY - m_previousTouchPoint.globalY) / m_client->scale());
    m_client->handlePanning(timestamp, delta);
}

void GestureRecognizer::panningInProgress(const NIXTouchEvent& event)
{
    const NIXTouchPoint& touchPoint = event.touchPoints[0];
    switch (event.type) {
    case kNIXInputEventTypeTouchMove:
        updatePanningData(event.timestamp, touchPoint);
        break;
    case kNIXInputEventTypeTouchEnd:
        m_state = &GestureRecognizer::noGesture;
        m_client->handlePanningFinished(event.timestamp);
        break;
    case kNIXInputEventTypeTouchStart:
        m_state = &GestureRecognizer::pinchInProgress;
        setupPinchData(event);
        m_client->handlePinchStarted(event.timestamp);
        break;
    }
}

void GestureRecognizer::pinchInProgress(const NIXTouchEvent& event)
{
    if (event.numTouchPoints < 2) {
        fail("Received only one touch point while in PinchInProgressState.");
        return;
    }

    switch (event.type) {
    case kNIXInputEventTypeTouchMove:
        updatePinchData(event.timestamp, event);
        break;
    case kNIXInputEventTypeTouchEnd:
        if (hasTouchPointPressed(event))
            m_state = &GestureRecognizer::panningInProgress;
        else {
            m_state == &GestureRecognizer::noGesture;
            reset();
        }
        m_client->handlePinchFinished(event.timestamp);
        break;
    case kNIXInputEventTypeTouchStart:
        // Ignore extra touch points.
        break;
    }
}

void GestureRecognizer::handleTouchEvent(const NIXTouchEvent& event)
{
    (this->*m_state)(event);

    m_previousTouchPoint = event.touchPoints[0];
}

static gboolean doubleTapTimer(gpointer data)
{
    reinterpret_cast<GestureRecognizer*>(data)->doubleTapTimerTriggered();
    return false;
}

void GestureRecognizer::updatePinchData(double timestamp, const NIXTouchEvent& event)
{
    assert(m_initialPinchScale > 0);

    const NIXTouchPoint& first = event.touchPoints[0];
    const NIXTouchPoint& second = event.touchPoints[1];

    WKPoint currentCenter = computeCenter(WKPointMake(first.globalX, first.globalY), WKPointMake(second.globalX, second.globalY));
    WKPoint delta = WKPointMake((currentCenter.x - m_previousPinchGlobalCenter.x) / m_client->scale(),
                                (currentCenter.y - m_previousPinchGlobalCenter.y) / m_client->scale());
    double currentDistance = computeDistance(WKPointMake(first.globalX, first.globalY), WKPointMake(second.globalX, second.globalY));
    double newScale = m_initialPinchScale * (currentDistance / m_initialPinchDistance);

    m_client->handlePinch(timestamp, delta, newScale, m_initialPinchContentCenter);

    m_previousPinchGlobalCenter = currentCenter;
}
