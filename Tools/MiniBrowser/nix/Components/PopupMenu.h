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

#ifndef PopupMenu_h
#define PopupMenu_h

#include "Button.h"
#include "MenuEntry.h"

#include <vector>

class PopupMenu : public VisualComponent {
public:
    enum ScrollDirection {
        Backward = -1,
        Forward = 1
    };

    static const int entryHeight = 16;
    static const int borderWidth = 3;

    PopupMenu(Display*, Window, XContext, BrowserControl*, WKRect, std::vector<std::string>*);
    virtual ~PopupMenu();
    virtual void handleEvent(const XEvent&) { };

private:
    virtual void createXWindow(Window parent, XContext);
    void removeMenuEntries();
    void drawMenuEntries(int firstElement = 0);
    void setVisibleSection(ScrollDirection);
    void correctWindowWidth();
    void calculateWindowSize();
    void scrollUp();
    void scrollDown();
    int numberOfMenuItems();
    Button* createScrollButton(int, int, const char*, ButtonFunction);

    std::vector<MenuEntry*> m_menuEntries;
    std::vector<std::string>* m_menuItems;

    int m_countOfEntries;
    int m_countOfVisibleEntries;
    int m_section;
    bool m_tooMuchEntries;
    bool m_hasScrollButtons;
    Button* m_scrollUpButton;
    Button* m_scrollDownButton;
    XContext m_xContext;

    cairo_t* m_cairo;
    cairo_surface_t* m_surface;
};

#endif
