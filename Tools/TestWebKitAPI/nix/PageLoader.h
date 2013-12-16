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

#ifndef PageLoader_h
#define PageLoader_h

#include "WebKit2/WKPage.h"
#include "NIXView.h"

namespace TestWebKitAPI {
namespace Util {

class ForceRepaintClient {
public:
    ForceRepaintClient(WKViewRef);
    void setClearColor(int r, int g, int b, int a);
    void clear();
    void repaint();

private:
    static void viewNeedsDisplay(WKViewRef, WKRect, const void*);

    WKViewRef m_view;
    int m_clearR;
    int m_clearG;
    int m_clearB;
    int m_clearA;
};


class PageLoader
{
public:
    PageLoader(WKViewRef);

    void waitForLoadURL(const char* resource);
    void waitForLoadURLAndRepaint(const char* resource);
    void forceRepaint();

    WKPageLoaderClientV3& loaderClient() { return m_loaderClient; }
    WKViewRef webView() { return m_view; }

private:
    WKViewRef m_view;
    bool m_shouldRepaint;
    bool m_didFinishLoadAndRepaint;
    WKPageLoaderClientV3 m_loaderClient;

    static void didForceRepaint(WKErrorRef, void* context);
    static void didFinishLoadForFrame(WKPageRef page, WKFrameRef, WKTypeRef, const void* context);
};

}
}

#endif
