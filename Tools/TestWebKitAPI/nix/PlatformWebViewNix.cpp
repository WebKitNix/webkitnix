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

// FIXME: Remove this once everything has been moved over to the IPC namespace.
#define CoreIPC IPC

#include "config.h"
#include "PlatformWebView.h"

#include "NIXView.h"
#include "NIXEvents.h"

const double WEBVIEW_WIDTH = 200;
const double WEBVIEW_HEIGHT = 200;
const double SCREEN_WIDTH = 1024;
const double SCREEN_HEIGHT = 768;

namespace TestWebKitAPI {

PlatformWebView::PlatformWebView(WKContextRef context, WKPageGroupRef pageGroup)
{
    m_view = WKViewCreate(context, pageGroup);
    WKViewInitialize(m_view);
    WKViewSetSize(m_view, WKSizeMake(WEBVIEW_WIDTH, WEBVIEW_HEIGHT));
    NIXViewSetScreenRect(m_view, WKRectMake(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
    m_window = 0;
}

PlatformWebView::~PlatformWebView()
{
    WKRelease(m_view);
}

void PlatformWebView::resizeTo(unsigned width, unsigned height)
{
    WKViewSetSize(m_view, WKSizeMake(width, height));
}

WKPageRef PlatformWebView::page() const
{
    return WKViewGetPage(m_view);
}

void PlatformWebView::simulateSpacebarKeyPress()
{
    NIXKeyEvent nixEvent;
    memset(&nixEvent, 0, sizeof(NIXKeyEvent));

    nixEvent.type = kNIXInputEventTypeKeyDown;
    nixEvent.modifiers = 0;
    nixEvent.timestamp = 0;
    nixEvent.shouldUseUpperCase = false;
    nixEvent.isKeypad = false;
    nixEvent.key = kNIXKeyEventKey_Space;
    NIXViewSendKeyEvent(m_view, &nixEvent);
}

void PlatformWebView::simulateRightClick(unsigned x, unsigned y)
{
    NIXMouseEvent nixEvent;
    memset(&nixEvent, 0, sizeof(NIXMouseEvent));

    nixEvent.type = kNIXInputEventTypeMouseDown;
    nixEvent.button = kWKEventMouseButtonRightButton;
    nixEvent.x = x;
    nixEvent.y = y;
    nixEvent.globalX = x;
    nixEvent.globalY = y;
    nixEvent.clickCount = 0;
    nixEvent.modifiers = 0;
    nixEvent.timestamp = 0;
    NIXViewSendMouseEvent(m_view, &nixEvent);
}

void PlatformWebView::simulateMouseMove(unsigned x, unsigned y)
{
    NIXMouseEvent nixEvent;
    memset(&nixEvent, 0, sizeof(NIXMouseEvent));

    nixEvent.type = kNIXInputEventTypeMouseMove;
    nixEvent.button = kWKEventMouseButtonNoButton;
    nixEvent.x = x;
    nixEvent.y = y;
    nixEvent.globalX = x;
    nixEvent.globalY = y;
    nixEvent.clickCount = 0;
    nixEvent.modifiers = 0;
    nixEvent.timestamp = 0;
    NIXViewSendMouseEvent(m_view, &nixEvent);
}

} // namespace TestWebKitAPI
