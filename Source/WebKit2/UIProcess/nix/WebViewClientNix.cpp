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

#include "config.h"
#include "WebViewClientNix.h"

#include "WebViewNix.h"
#include "EditorState.h"
#include <cstring>

namespace WebKit {

void WebViewClientNix::doneWithTouchEvent(WebView* view, const NIXTouchEvent& event, bool wasEventHandled)
{
    if (!m_client.doneWithTouchEvent)
        return;
    m_client.doneWithTouchEvent(toAPI(view), &event, wasEventHandled, m_client.base.clientInfo);
}

void WebViewClientNix::didFindZoomableArea(WebView* view, WKPoint target, WKRect area)
{
    if (!m_client.didFindZoomableArea)
        return;
    m_client.didFindZoomableArea(toAPI(view), target, area, m_client.base.clientInfo);
}

void WebViewClientNix::setCursor(WebView* view, unsigned shape)
{
    if (!m_client.setCursor)
        return;
    m_client.setCursor(toAPI(view), shape, m_client.base.clientInfo);
}

void WebViewClientNix::updateTextInputState(WebView* view, const EditorState& state)
{
    if (!m_client.updateTextInputState)
        return;

    NIXTextInputState nixState;
    ProxyingRefPtr<API::String> selectedText = toAPI(state.selectedText.impl());
    ProxyingRefPtr<API::String> surroundingText = toAPI(state.surroundingText.impl());
    ProxyingRefPtr<API::String> submitLabel = toAPI(state.submitLabel.impl());

    nixState.selectedText = selectedText;
    nixState.surroundingText = surroundingText;
    nixState.submitLabel = submitLabel;
    nixState.inputMethodHints = state.inputMethodHints;
    nixState.isContentEditable = state.isContentEditable;
    nixState.cursorPosition = state.cursorPosition;
    nixState.anchorPosition = state.anchorPosition;
    nixState.cursorRect = toAPI(state.cursorRect);
    nixState.editorRect = toAPI(state.editorRect);

    m_client.updateTextInputState(toAPI(view), &nixState, m_client.base.clientInfo);
}

void WebViewClientNix::didChangeTooltip(WebView* view, const WKStringRef tooltip)
{
    if (!m_client.didChangeTooltip)
        return;
    m_client.didChangeTooltip(toAPI(view), tooltip, m_client.base.clientInfo);
}

} // namespace WebKit
