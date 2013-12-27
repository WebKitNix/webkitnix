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
#include "PageLoader.h"
#include "PlatformUtilities.h"
#include "NIXView.h"
#include "WebKit2/WKRetainPtr.h"
#include <cstring>

#if USE(OPENGL_ES_2)
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif

using namespace TestWebKitAPI::Util;

PageLoader::PageLoader(WKViewRef view)
    : m_view(view)
    , m_shouldRepaint(false)
    , m_didFinishLoadAndRepaint(false)
{
    std::memset(&m_loaderClient, 0, sizeof(m_loaderClient));

    m_loaderClient.didFinishLoadForFrame = didFinishLoadForFrame;
    m_loaderClient.base.version = 3;
    m_loaderClient.base.clientInfo = this;
    WKPageSetPageLoaderClient(WKViewGetPage(m_view), &m_loaderClient.base);
}

void PageLoader::didForceRepaint(WKErrorRef, void* context)
{
    reinterpret_cast<PageLoader*>(context)->m_didFinishLoadAndRepaint = true;
}

void PageLoader::didFinishLoadForFrame(WKPageRef page, WKFrameRef, WKTypeRef, const void* context)
{
    PageLoader* self = reinterpret_cast<PageLoader*>(const_cast<void*>(context));
    if (self->m_shouldRepaint)
        WKPageForceRepaint(page, const_cast<void*>(context), &PageLoader::didForceRepaint);
    else
        self->m_didFinishLoadAndRepaint = true;
}

void PageLoader::waitForLoadURL(const char* resource)
{
    WKRetainPtr<WKURLRef> urlRef = adoptWK(createURLForResource(resource, "html"));
    WKPageLoadURL(WKViewGetPage(m_view), urlRef.get());
    m_shouldRepaint = false;
    Util::run(&m_didFinishLoadAndRepaint);
    m_didFinishLoadAndRepaint = false;
}

void PageLoader::waitForLoadURLAndRepaint(const char* resource)
{
    WKRetainPtr<WKURLRef> urlRef = adoptWK(createURLForResource(resource, "html"));
    WKPageLoadURL(WKViewGetPage(m_view), urlRef.get());
    m_shouldRepaint = true;
    Util::run(&m_didFinishLoadAndRepaint);
    m_didFinishLoadAndRepaint = false;
}

void PageLoader::forceRepaint()
{
    WKPageForceRepaint(WKViewGetPage(m_view), this, &PageLoader::didForceRepaint);
    Util::run(&m_didFinishLoadAndRepaint);
    m_didFinishLoadAndRepaint = false;
}

ForceRepaintClient::ForceRepaintClient(WKViewRef view)
    : m_view(view)
    , m_clearR(0)
    , m_clearG(0)
    , m_clearB(0)
    , m_clearA(0)
{
    std::memset(&m_viewClient, 0, sizeof(m_viewClient));
    m_viewClient.base.version = 0;
    m_viewClient.base.clientInfo = this;
    m_viewClient.viewNeedsDisplay = viewNeedsDisplay;
    WKViewSetViewClient(m_view, &m_viewClient.base);
}

void ForceRepaintClient::setClearColor(int r, int g, int b, int a)
{
    m_clearR = r;
    m_clearG = g;
    m_clearB = b;
    m_clearA = a;
}

void ForceRepaintClient::clear()
{
    glClearColor(m_clearR, m_clearG, m_clearB, m_clearA);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ForceRepaintClient::repaint()
{
    clear();
    WKViewPaintToCurrentGLContext(m_view);
}

void ForceRepaintClient::viewNeedsDisplay(WKViewRef, WKRect, const void* clientInfo)
{
    ForceRepaintClient* client = static_cast<ForceRepaintClient*>(const_cast<void*>(clientInfo));
    assert(client);
    assert(client->m_view);

    client->repaint();
}
