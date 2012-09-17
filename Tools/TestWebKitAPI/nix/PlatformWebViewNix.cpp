/*
 * Copyright (C) 2012 INdT - Instituto Nokia de Tecnologia
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "PlatformWebView.h"

#include "WebKit2/WKAPICast.h"
#include "WebView.h"

namespace TestWebKitAPI {

class TestWebViewClient : public Nix::WebViewClient {
public:
    void viewNeedsDisplay(int x, int y, int width, int height) {}
    void webProcessCrashed(WKStringRef) {}
    void webProcessRelaunched() {}
};

PlatformWebView::PlatformWebView(WKContextRef context, WKPageGroupRef pageGroup)
{
    m_webViewClient = new TestWebViewClient;
    m_view = Nix::WebView::create(context, pageGroup, m_webViewClient);
    m_view->initialize();
    WKPageSetUseFixedLayout(m_view->pageRef(), true);
    m_window = 0;
}

PlatformWebView::~PlatformWebView()
{
    delete m_webViewClient;
}

void PlatformWebView::resizeTo(unsigned width, unsigned height)
{
    m_view->setSize(width, height);
}

WKPageRef PlatformWebView::page() const
{
    return m_view->pageRef();
}

} // namespace TestWebKitAPI
