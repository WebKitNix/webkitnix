/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Attila Dusnoki <adusnoki@inf.u-szeged.hu>
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

#ifndef Tooltip_h
#define Tooltip_h

#include "VisualComponent.h"

#include <cairo-xlib.h>
#include <string>
#include <vector>

class Tooltip : public VisualComponent {
public:
    Tooltip(Display*, Window, XContext, BrowserControl*, WKRect);
    ~Tooltip();

    virtual void handleEvent(const XEvent&) { }
    void showTooltip(int, int, std::string);
    void hideTooltip();

private:
    static const int textWidthIndent = 8;
    static const int lineHeightSize = 15;
    static const int mousePositionXOffset = 15;
    static const int mousePositionYOffset = 20;
    static const int maximumCharactersPerLine = 80;

    virtual void createXWindow(Window, XContext);

    void resizeWindow(int, int);
    int windowSlideOffsetX(WKSize, int);
    int windowSlideOffsetY(WKSize, int);

    int widthOfLongestLine();
    int textWidth(const char*);
    void drawText(std::string&, int);
    void wordWrap(std::string);

    std::vector<std::string> m_wrappedText;
    cairo_t* m_cairo;
    cairo_surface_t* m_surface;
};

#endif
