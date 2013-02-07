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
#include "PlatformWebView.h"
#include "GLUtilities.h"

#include "WebKit2/WKAPICast.h"
#include "NIXView.h"

#include <WebKit2/WKImageCairo.h>
#include <cairo/cairo.h>
#include <cassert>

extern "C" {
static gboolean callUpdateDisplay(gpointer);
}

using namespace WebKit;
using namespace WTR;

static void pageDidRequestScroll(NIXView view, WKPoint position, const void* clientInfo)
{
    NIXViewSetScrollPosition(view, position);
}

static void viewNeedsDisplay(NIXView view, WKRect area, const void* clientInfo)
{
    PlatformWebView* platview = static_cast<PlatformWebView*>(const_cast<void*>(clientInfo));
    platview->scheduleDisplayUpdate();
}

static gboolean callUpdateDisplay(gpointer data)
{
    PlatformWebView* platview = reinterpret_cast<PlatformWebView*>(data);
    platview->performDisplayUpdate();
    return 0;
}

namespace WTR {

void PlatformWebView::scheduleDisplayUpdate()
{
    if (m_scheduledDisplayUpdateID)
        return;

    m_scheduledDisplayUpdateID = g_timeout_add(0, callUpdateDisplay, this);
}

void PlatformWebView::performDisplayUpdate()
{
    if (!m_view || !m_scheduledDisplayUpdateID)
        return;

    m_scheduledDisplayUpdateID = 0;
    bool didMakeCurrent = m_offscreenBuffer->makeCurrent();
    assert(didMakeCurrent);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    NIXViewPaintToCurrentGLContext(m_view);
}

PlatformWebView::PlatformWebView(WKContextRef context, WKPageGroupRef pageGroup, WKDictionaryRef options)
{
    m_view = NIXViewCreate(context, pageGroup);
    m_window = 0;
    m_scheduledDisplayUpdateID = 0;

    WKSize size = WKSizeMake(800, 600);

    m_offscreenBuffer = new ToolsNix::GLOffscreenBuffer(size.width, size.height);
    m_offscreenBuffer->makeCurrent();
    glViewport(0, 0, size.width, size.height);

    NIXViewClient viewClient;
    memset(&viewClient, 0, sizeof(NIXViewClient));
    viewClient.version = kNIXViewClientCurrentVersion;
    viewClient.clientInfo = this;
    viewClient.pageDidRequestScroll = pageDidRequestScroll;
    viewClient.viewNeedsDisplay = viewNeedsDisplay;
    NIXViewSetViewClient(m_view, &viewClient);

    NIXViewInitialize(m_view);
    NIXViewSetSize(m_view, size);
}

PlatformWebView::~PlatformWebView()
{
    if (m_scheduledDisplayUpdateID)
        g_source_remove(m_scheduledDisplayUpdateID);
    NIXViewRelease(m_view);
    delete m_offscreenBuffer;
}

void PlatformWebView::resizeTo(unsigned width, unsigned height)
{
    NIXViewSetSize(m_view, WKSizeMake(width, height));
}

WKPageRef PlatformWebView::page()
{
    return NIXViewGetPage(m_view);
}

void PlatformWebView::focus()
{
    NIXViewSetFocused(m_view, true);
}

WKRect PlatformWebView::windowFrame()
{
    return WKRectMake(0, 0, 0, 0);
}

void PlatformWebView::setWindowFrame(WKRect frame)
{
    resizeTo(frame.size.width, frame.size.height);
}

void PlatformWebView::addChromeInputField()
{
}

void PlatformWebView::removeChromeInputField()
{
}

void PlatformWebView::makeWebViewFirstResponder()
{
}

WKRetainPtr<WKImageRef> PlatformWebView::windowSnapshotImage()
{
    int width = NIXViewSize(m_view).width;
    int height = NIXViewSize(m_view).height;
    cairo_format_t format = CAIRO_FORMAT_ARGB32;

    WKImageOptions options;
    int stride = 0;
    stride = cairo_format_stride_for_width(format, width);

    int size = stride * height * 4;
    std::auto_ptr<unsigned char> data(new unsigned char[size]);
    if (!data.get())
        return 0;
    memset(data.get(), 0, size);

    bool didMakeCurrent = m_offscreenBuffer->makeCurrent();
    assert(didMakeCurrent);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

#if USE(OPENGL_ES_2)
    // FIXME: We should swizzle the data to BRGA format.
    GLenum pixelFormat = GL_RGBA;
#else
    GLenum pixelFormat = GL_BGRA;
#endif

    // Read line by line using the Cairo internal format.
    for (int y = 0; y < height; y++)
        glReadPixels(0, (height - (y + 1)), width, 1, pixelFormat, GL_UNSIGNED_BYTE, data.get()+(y*stride));

    cairo_surface_t* surface = cairo_image_surface_create_for_data(data.get(), format, width, height, stride);

    if (!surface)
        return 0;

    WKImageRef resultImage =  WKImageCreateFromCairoSurface(surface, options);
    cairo_surface_destroy(surface);
    return adoptWK(resultImage);
}

} // namespace WTR

