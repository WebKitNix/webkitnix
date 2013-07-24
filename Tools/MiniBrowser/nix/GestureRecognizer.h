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

#ifndef GestureRecognizer_h
#define GestureRecognizer_h

#include "NIXEvents.h"
#include "WebKit2/WKGeometry.h"
#include <glib.h>

extern "C" {
static gboolean doubleTapTimer(gpointer);
static gboolean longTapTimer(gpointer);
}

class GestureRecognizerClient {
public:
    virtual void handleSingleTap(double timestamp, const NIXTouchPoint&) { }
    virtual void handleDoubleTap(double timestamp, const NIXTouchPoint&) { }
    virtual void handleLongTap(double timestamp, const NIXTouchPoint&) { }

    virtual void handlePanningStarted(double timestamp) { }
    virtual void handlePanning(double timestamp, WKPoint delta) { }
    virtual void handlePanningFinished(double timestamp) { }

    virtual double scale() { return 1; }

    virtual void handlePinchStarted(double timestamp) { }
    virtual void handlePinch(double timestamp, WKPoint delta, double distanceRatio, WKPoint contentCenter) { }
    virtual void handlePinchFinished(double timestamp) { }
};

class GestureRecognizer {
public:
    GestureRecognizer(GestureRecognizerClient*);

    void handleTouchEvent(const NIXTouchEvent&);
    void reset();


private:
    // State functions.
    void noGesture(const NIXTouchEvent&);
    void singleTapPressed(const NIXTouchEvent&);
    void waitForDoubleTap(const NIXTouchEvent&);
    void doubleTapPressed(const NIXTouchEvent&);
    void panningInProgress(const NIXTouchEvent&);
    void pinchInProgress(const NIXTouchEvent&);

    void setupPanningData(double, const NIXTouchPoint&);
    void updatePanningData(double, const NIXTouchPoint&);
    void setupPinchData(const NIXTouchEvent&);
    void updatePinchData(double, const NIXTouchEvent&);

    void startLongTapTimer();
    void cancelLongTapTimerIfNeeded();
    void longTapTimerTriggered();

    void startDoubleTapTimer();
    void cancelDoubleTapTimerIfNeeded();
    void doubleTapTimerTriggered();

    typedef void (GestureRecognizer::*StateFunction)(const NIXTouchEvent&);
    StateFunction m_state;

    double m_timestamp;
    NIXTouchPoint m_firstTouchPoint;
    guint m_doubleTapTimerId;
    guint m_longTapTimerId;
    NIXTouchPoint m_previousTouchPoint;
    GestureRecognizerClient* m_client;

    double m_initialPinchDistance;
    double m_initialPinchScale;
    WKPoint m_previousPinchGlobalCenter;
    WKPoint m_initialPinchContentCenter;

    void fail(const char*);
    friend gboolean doubleTapTimer(gpointer);
    friend gboolean longTapTimer(gpointer);
};

#endif
