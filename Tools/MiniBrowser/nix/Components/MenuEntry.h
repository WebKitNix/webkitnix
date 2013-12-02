/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Zsolt Borbely <borbezs@inf.u-szeged.hu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MenuEntry_h
#define MenuEntry_h

#include "VisualComponent.h"

#include <cairo-xlib.h>
#include <string>

const int entryBorderSize = 1;

class MenuEntry : public VisualComponent {
public:
    MenuEntry(Display*, Window, XContext, BrowserControl*, WKRect, std::string, int);
    ~MenuEntry();
    virtual void handleEvent(const XEvent&);

private:
    virtual void createXWindow(Window parent, XContext);
    void drawText(std::string);
    void passMenuEntryValue(int);
    void correctWindowSize();

    int m_entryID;
    std::string m_entryName;

    cairo_t* m_cairo;
    cairo_surface_t* m_surface;
};

#endif
